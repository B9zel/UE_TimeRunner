#include "Global/GlobalTimer/GlobalTimer.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/Countermotion/CountermotionComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "Global/GameInstance/MainGameInstance.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "UObject/Class.h"
#include "UObject/StrongObjectPtr.h"
#include <limits>

// Control the clock boundary without sleeping, loading a map, or changing the public timer API.
struct FGlobalTimerTestAccess
{
	static void SetTime(UGlobalTimer& Timer, double Now)
	{
		Timer.TestTime = Now;
	}

	static void TickAt(UGlobalTimer& Timer, double Now)
	{
		SetTime(Timer, Now);
		Timer.Tick(0.0f);
	}

	static int32 GetTimerCount(const UGlobalTimer& Timer)
	{
		return Timer.Timers.Num() + Timer.AddTimer.Num();
	}
};

namespace
{
	constexpr EAutomationTestFlags TimerTestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;

	struct FGlobalTimerFixture
	{
		TStrongObjectPtr<UGlobalTimer> Timer{NewObject<UGlobalTimer>()};

		explicit FGlobalTimerFixture(double StartTime = 0.0)
		{
			FGlobalTimerTestAccess::SetTime(*Timer, StartTime);
			Timer->StartGlobalTimer();
		}

		~FGlobalTimerFixture()
		{
			Timer->StopGlobalTimer();
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerOncePerFrameTest, "TimeRunner.GlobalTimer.OncePerFrame", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerPendingCancellationTest, "TimeRunner.GlobalTimer.PendingCancellation", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerPendingReplacementTest, "TimeRunner.GlobalTimer.PendingReplacement", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerCatchUpTest, "TimeRunner.GlobalTimer.BoundedCatchUp", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerInvalidRatesTest, "TimeRunner.GlobalTimer.InvalidRates", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerStartStopTest, "TimeRunner.GlobalTimer.StartStop", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerCadenceTest, "TimeRunner.GlobalTimer.Cadence", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerHandleComparisonTest, "TimeRunner.GlobalTimer.HandleComparison", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerDeadDelegateTest, "TimeRunner.GlobalTimer.DeadDelegateCleanup", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerReleaseTest, "TimeRunner.GlobalTimer.CancellationReleasesResources", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerCountermotionTest, "TimeRunner.GlobalTimer.CountermotionLifecycle", TimerTestFlags);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGlobalTimerReentrantCleanupTest, "TimeRunner.GlobalTimer.ReentrantCleanup", TimerTestFlags);

bool FGlobalTimerOncePerFrameTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	UGlobalTimer& Timer = *Fixture.Timer;
	FGlobalTimerHandle Direct, Parent, Queued;
	int32 DirectCalls = 0;
	int32 QueuedCalls = 0;
	Timer.SetTimer(Direct, FGobalTimerFunction([&] { ++DirectCalls; }), 0.125f, true, true);
	Timer.SetTimer(Parent, FGobalTimerFunction([&]
	{
		Timer.SetTimer(Queued, FGobalTimerFunction([&] { ++QueuedCalls; }), 0.125f, true, true);
	}), 0.125f);
	FGlobalTimerTestAccess::TickAt(Timer, 0.5);
	TestEqual(TEXT("Direct timer executes once after four elapsed periods"), DirectCalls, 1);
	TestTrue(TEXT("Queued timer is logically valid immediately"), Timer.IsValidTimer(Queued));
	TestEqual(TEXT("New timer is deferred until the next update"), QueuedCalls, 0);

	FGlobalTimerTestAccess::TickAt(Timer, 0.9375);
	TestEqual(TEXT("Once-per-frame survives both pending and active map moves"), QueuedCalls, 1);
	FGlobalTimerTestAccess::TickAt(Timer, 0.96875);
	TestEqual(TEXT("Skipped periods do not leave a backlog"), QueuedCalls, 1);
	FGlobalTimerTestAccess::TickAt(Timer, 1.0);
	TestEqual(TEXT("Once-per-frame retains its fractional interval"), QueuedCalls, 2);

	constexpr double StartTime = 17'000'000.0;
	FGlobalTimerFixture LargeClock(StartTime);
	FGlobalTimerHandle SixtyHz;
	int32 Calls = 0;
	LargeClock.Timer->SetTimer(SixtyHz, FGobalTimerFunction([&] { ++Calls; }), 1.0f / 60.0f, true, true);
	FGlobalTimerTestAccess::TickAt(*LargeClock.Timer, StartTime + 0.1);
	TestEqual(TEXT("A rounded deadline cannot dispatch twice in one tick"), Calls, 1);
	FGlobalTimerTestAccess::TickAt(*LargeClock.Timer, StartTime + 0.1);
	TestEqual(TEXT("A rescheduled deadline is strictly in the future"), Calls, 1);
	FGlobalTimerTestAccess::TickAt(*LargeClock.Timer, StartTime + 0.12);
	TestEqual(TEXT("The timer continues on a later tick"), Calls, 2);
	return true;
}

bool FGlobalTimerPendingCancellationTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	UGlobalTimer& Timer = *Fixture.Timer;
	FGlobalTimerHandle Parent, Pending, PendingCopy, Other;
	int32 ParentCalls = 0;
	int32 PendingCalls = 0;
	int32 OtherCalls = 0;
	Timer.SetTimer(Other, FGobalTimerFunction([&] { ++OtherCalls; }), 0.25f);
	Timer.SetTimer(Parent, FGobalTimerFunction([&]
	{
		++ParentCalls;
		Timer.ClearTimer(Other);
		Timer.SetTimer(Pending, FGobalTimerFunction([&] { ++PendingCalls; }), 0.125f, true);
		PendingCopy = Pending;
		TestTrue(TEXT("Pending timer exists before cancellation"), Timer.IsValidTimer(Pending));
		Timer.ClearTimer(Pending);
		TestFalse(TEXT("Cancellation invalidates the caller's handle"), Pending.IsValid());
		TestFalse(TEXT("Copied handle cannot find the canceled pending timer"), Timer.IsValidTimer(PendingCopy));
		Timer.ClearTimer(Parent);
	}), 0.125f, true);
	FGlobalTimerTestAccess::TickAt(Timer, 0.5);
	TestEqual(TEXT("Self-cancellation stops the current catch-up loop"), ParentCalls, 1);
	TestEqual(TEXT("Another due timer canceled by the callback never executes"), OtherCalls, 0);
	Timer.ClearTimer(PendingCopy);
	TestFalse(TEXT("Clearing an already canceled handle invalidates that copy too"), PendingCopy.IsValid());
	FGlobalTimerTestAccess::TickAt(Timer, 1.0);
	TestEqual(TEXT("Canceled pending callback never executes"), PendingCalls, 0);
	return true;
}

bool FGlobalTimerPendingReplacementTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	UGlobalTimer& Timer = *Fixture.Timer;
	FGlobalTimerHandle Parent, Pending, Replaced;
	int32 OldCalls = 0;
	int32 NewCalls = 0;
	Timer.SetTimer(Parent, FGobalTimerFunction([&]
	{
		Timer.SetTimer(Pending, FGobalTimerFunction([&] { ++OldCalls; }), 0.125f, true);
		Replaced = Pending;
		Timer.SetTimer(Pending, FGobalTimerFunction([&] { ++NewCalls; }), 0.125f, true);
		TestFalse(TEXT("Replaced pending timer is immediately inactive"), Timer.IsValidTimer(Replaced));
		TestTrue(TEXT("Replacement is immediately valid"), Timer.IsValidTimer(Pending));
	}), 0.125f);
	FGlobalTimerTestAccess::TickAt(Timer, 0.125);
	FGlobalTimerTestAccess::TickAt(Timer, 0.25);
	TestEqual(TEXT("Replacement does not leave an orphan timer"), OldCalls, 0);
	TestEqual(TEXT("Only the replacement executes"), NewCalls, 1);
	return true;
}

bool FGlobalTimerCatchUpTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	UGlobalTimer& Timer = *Fixture.Timer;
	FGlobalTimerHandle Loop, Tiny, Once;
	int32 LoopCalls = 0;
	int32 TinyCalls = 0;
	int32 OnceCalls = 0;
	Timer.SetTimer(Loop, FGobalTimerFunction([&] { ++LoopCalls; }), 0.125f, true);
	Timer.SetTimer(Tiny, FGobalTimerFunction([&] { ++TinyCalls; }), std::numeric_limits<float>::min(), true);
	Timer.SetTimer(Once, FGobalTimerFunction([&] { ++OnceCalls; }), 0.125f);
	constexpr double HitchTime = 262144.0625;
	FGlobalTimerTestAccess::TickAt(Timer, HitchTime);
	TestEqual(TEXT("A hitch is limited to 256 callbacks per timer"), LoopCalls, 256);
	TestEqual(TEXT("A quotient above int32 range is bounded before conversion"), TinyCalls, 256);
	TestEqual(TEXT("One-shot timer still fires exactly once"), OnceCalls, 1);
	TestFalse(TEXT("One-shot is logically removed immediately"), Timer.IsValidTimer(Once));
	Timer.ClearTimer(Tiny);
	FGlobalTimerTestAccess::TickAt(Timer, HitchTime + 0.03125);
	TestEqual(TEXT("Excess catch-up is dropped, not retried next frame"), LoopCalls, 256);
	FGlobalTimerTestAccess::TickAt(Timer, HitchTime + 0.0625);
	TestEqual(TEXT("Fractional time survives the catch-up cap"), LoopCalls, 257);
	return true;
}

bool FGlobalTimerInvalidRatesTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	UGlobalTimer& Timer = *Fixture.Timer;
	const float InvalidRates[] = {0.0f, -1.0f, std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
	int32 Calls = 0;
	for (const float Rate : InvalidRates)
	{
		FGlobalTimerHandle Handle;
		Timer.SetTimer(Handle, FGobalTimerFunction([&] { ++Calls; }), 0.125f, true);
		const FGlobalTimerHandle OldHandle = Handle;
		Timer.SetTimer(Handle, FGobalTimerFunction([&] { ++Calls; }), Rate, true);
		TestFalse(TEXT("Invalid rate clears the existing handle"), Handle.IsValid());
		TestFalse(TEXT("Invalid rate cancels the previous timer"), Timer.IsValidTimer(OldHandle));
		Timer.SetTimer(Handle, FGobalTimerFunction([&] { ++Calls; }), Rate, true);
		TestFalse(TEXT("Invalid rate cannot create a new timer"), Timer.IsValidTimer(Handle));
	}
	FGlobalTimerTestAccess::TickAt(Timer, 1.0);
	TestEqual(TEXT("Rejected timers never execute"), Calls, 0);
	return true;
}

bool FGlobalTimerStartStopTest::RunTest(const FString& Parameters)
{
	TStrongObjectPtr<UGlobalTimer> Timer(NewObject<UGlobalTimer>());
	FGlobalTimerTestAccess::SetTime(*Timer, 0.0);
	FGlobalTimerHandle Handle, OneShot;
	int32 Calls = 0;
	int32 OneShotCalls = 0;
	Timer->SetTimer(OneShot, FGobalTimerFunction([&] { ++OneShotCalls; }), 0.25f);
	Timer->SetTimer(Handle, FGobalTimerFunction([&]
	{
		++Calls;
		Timer->StopGlobalTimer();
	}), 0.125f, true);
	TestFalse(TEXT("Manager starts disabled"), Timer->IsTickable());
	FGlobalTimerTestAccess::TickAt(*Timer, 0.5);
	TestEqual(TEXT("No dispatch before Start"), Calls, 0);
	Timer->StartGlobalTimer();
	TestTrue(TEXT("Start enables ticking"), Timer->IsTickable());
	FGlobalTimerTestAccess::TickAt(*Timer, 0.5);
	TestEqual(TEXT("Stop inside callback interrupts catch-up immediately"), Calls, 1);
	TestEqual(TEXT("Stop leaves the other due callback undispatched"), OneShotCalls, 0);
	TestTrue(TEXT("Stop keeps the due one-shot active"), Timer->IsValidTimer(OneShot));
	TestFalse(TEXT("Stop disables ticking"), Timer->IsTickable());
	FGlobalTimerTestAccess::TickAt(*Timer, 1.0);
	Timer->Tick(0.0f);
	TestEqual(TEXT("Stopped manager also ignores direct Tick calls"), Calls, 1);
	TestTrue(TEXT("Stop retains timers"), Timer->IsValidTimer(Handle));
	Timer->StartGlobalTimer();
	FGlobalTimerTestAccess::TickAt(*Timer, 1.0);
	TestEqual(TEXT("Restart resumes retained timers"), Calls, 2);
	TestEqual(TEXT("Restart dispatches the retained one-shot"), OneShotCalls, 1);
	Timer->StopGlobalTimer();
	return true;
}

bool FGlobalTimerCadenceTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	UGlobalTimer& Timer = *Fixture.Timer;
	FGlobalTimerHandle Handle;
	int32 Calls = 0;
	Timer.SetTimer(Handle, FGobalTimerFunction([&] { ++Calls; }), 0.125f, true);
	FGlobalTimerTestAccess::TickAt(Timer, 0.1875);
	TestEqual(TEXT("First overdue update fires once"), Calls, 1);
	FGlobalTimerTestAccess::TickAt(Timer, 0.25);
	TestEqual(TEXT("Next deadline remains at 0.25, not 0.3125"), Calls, 2);
	FGlobalTimerTestAccess::TickAt(Timer, 0.6875);
	TestEqual(TEXT("Normal catch-up processes three elapsed periods"), Calls, 5);
	FGlobalTimerTestAccess::TickAt(Timer, 0.75);
	TestEqual(TEXT("Cadence remains stable after catch-up"), Calls, 6);
	return true;
}

bool FGlobalTimerHandleComparisonTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	FGlobalTimerHandle First, Second, Empty;
	Fixture.Timer->SetTimer(First, FGobalTimerFunction([] {}), 1.0f);
	Fixture.Timer->SetTimer(Second, FGobalTimerFunction([] {}), 1.0f);
	const FGlobalTimerHandle Copy = First;
	UScriptStruct* Struct = FGlobalTimerHandle::StaticStruct();
	TestTrue(TEXT("Reflection recognizes a copy of the same handle"), Struct->CompareScriptStruct(&First, &Copy, 0));
	TestFalse(TEXT("Reflection distinguishes two active handles"), Struct->CompareScriptStruct(&First, &Second, 0));
	TestFalse(TEXT("Reflection distinguishes active and empty handles"), Struct->CompareScriptStruct(&First, &Empty, 0));
	Fixture.Timer->ClearTimer(First);
	TestTrue(TEXT("Cleared handle compares equal to an empty handle"), Struct->CompareScriptStruct(&First, &Empty, 0));
	return true;
}

bool FGlobalTimerDeadDelegateTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	UGlobalTimer& Timer = *Fixture.Timer;
	FGlobalTimerHandle Handle;
	TStrongObjectPtr<UCountermotionComponent> Target(NewObject<UCountermotionComponent>());
	TSharedPtr<int32> Resource = MakeShared<int32>(0);
	TWeakPtr<int32> WeakResource = Resource;
	Timer.SetTimer(Handle, TGlobalTimerMethodPtr::CreateWeakLambda(Target.Get(), [Resource] {}), 3600.0f, true);
	Resource.Reset();
	TestTrue(TEXT("Live target keeps its timer valid"), Timer.IsValidTimer(Handle));
	Target->MarkAsGarbage();
	TestFalse(TEXT("Dead target makes its timer logically invalid before its deadline"), Timer.IsValidTimer(Handle));
	FGlobalTimerTestAccess::TickAt(Timer, 0.125);
	TestFalse(TEXT("Dead callback resources are released on the next update, before the deadline"), WeakResource.IsValid());
	Timer.ClearTimer(Handle);
	Timer.SetTimer(Handle, FGlobalUnifieldTimerDelegate(), 3600.0f, true);
	TestFalse(TEXT("An unbound callback cannot create a timer"), Handle.IsValid());
	return true;
}

bool FGlobalTimerReleaseTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	UGlobalTimer& Timer = *Fixture.Timer;
	FGlobalTimerHandle Handle;
	TSharedPtr<int32> Resource = MakeShared<int32>(0);
	TWeakPtr<int32> WeakResource = Resource;
	Timer.SetTimer(Handle, FGobalTimerFunction([Resource] {}), 3600.0f, true);
	Resource.Reset();
	Timer.StopGlobalTimer();
	Timer.ClearTimer(Handle);
	TestFalse(TEXT("Clear releases captures immediately even when dispatch is stopped"), WeakResource.IsValid());

	Timer.StartGlobalTimer();
	Resource = MakeShared<int32>(0);
	WeakResource = Resource;
	FGlobalTimerHandle Pending;
	TWeakPtr<int32> WeakPendingResource;
	Timer.SetTimer(Handle, FGobalTimerFunction([&, Resource]
	{
		TSharedPtr<int32> PendingResource = MakeShared<int32>(0);
		WeakPendingResource = PendingResource;
		Timer.SetTimer(Pending, FGobalTimerFunction([PendingResource] {}), 3600.0f, true);
		Timer.ClearTimer(Pending);
		Timer.ClearTimer(Handle);
		Timer.StopGlobalTimer();
		TestTrue(TEXT("Self-cancellation retains captures until the callback returns"), WeakResource.IsValid());
	}), 0.125f, true);
	Resource.Reset();
	FGlobalTimerTestAccess::TickAt(Timer, 0.5);
	TestFalse(TEXT("Self-canceled captures are released before Update returns despite Stop"), WeakResource.IsValid());
	TestFalse(TEXT("Canceled pending captures are released in the same update"), WeakPendingResource.IsValid());
	return true;
}

bool FGlobalTimerCountermotionTest::RunTest(const FString& Parameters)
{
	FTestWorldWrapper WorldFixture;
	if (!WorldFixture.CreateTestWorld(EWorldType::Game))
	{
		WorldFixture.ForwardErrorMessages(this);
		return false;
	}
	UWorld* World = WorldFixture.GetTestWorld();
	World->GetGameInstance()->Shutdown();
	UMainGameInstance* GameInstance = NewObject<UMainGameInstance>(GEngine);
	World->SetGameInstance(GameInstance);
	GEngine->GetWorldContextFromWorldChecked(World).OwningGameInstance = GameInstance;
	GameInstance->Init();
	World->GetWorldSettings()->DefaultGameMode = AGameModeBase::StaticClass();
	if (!WorldFixture.BeginPlayInTestWorld())
	{
		WorldFixture.ForwardErrorMessages(this);
		return false;
	}
	UGlobalTimer& Timer = *GameInstance->GetGlobalTimer();
	FGlobalTimerTestAccess::SetTime(Timer, 0.0);
	Timer.StartGlobalTimer();
	APawn* Pawn = World->SpawnActor<APawn>();
	APlayerController* Controller = World->SpawnActor<APlayerController>();
	if (!TestNotNull(TEXT("Test pawn spawned"), Pawn) || !TestNotNull(TEXT("Test controller spawned"), Controller))
	{
		Timer.StopGlobalTimer();
		return false;
	}
	UCountermotionComponent* Component = NewObject<UCountermotionComponent>(Pawn);
	Component->RegisterComponent();
	TestEqual(TEXT("No recording before first possession"), FGlobalTimerTestAccess::GetTimerCount(Timer), 0);
	Controller->Possess(Pawn);
	Controller->SetControlRotation(FRotator(0.0, 25.0, 0.0));
	TestEqual(TEXT("First possession starts one timer"), FGlobalTimerTestAccess::GetTimerCount(Timer), 1);
	FGlobalTimerTestAccess::TickAt(Timer, 1.0);
	TestEqual(TEXT("Recording stores the controller rotation"), Component->GetOldData().Rotation.Yaw, 25.0);

	Controller->UnPossess();
	if (!TestEqual(TEXT("Unpossession cancels the recording timer"), FGlobalTimerTestAccess::GetTimerCount(Timer), 0))
	{
		Timer.StopGlobalTimer();
		return false; // Avoid dereferencing the missing controller when this regression fails.
	}
	FGlobalTimerTestAccess::TickAt(Timer, 2.0);
	TestEqual(TEXT("Existing history survives controller loss"), Component->GetOldData().Rotation.Yaw, 25.0);
	Controller->Possess(Pawn);
	Controller->SetControlRotation(FRotator(0.0, 70.0, 0.0));
	TestEqual(TEXT("Re-possession restarts exactly one timer"), FGlobalTimerTestAccess::GetTimerCount(Timer), 1);
	FGlobalTimerTestAccess::TickAt(Timer, 3.0);
	TestEqual(TEXT("Recording resumes with the new control rotation"), Component->GetOldData().Rotation.Yaw, 70.0);

	Component->DestroyComponent();
	TestEqual(TEXT("EndPlay cancels the recording timer immediately"), FGlobalTimerTestAccess::GetTimerCount(Timer), 0);
	Controller->UnPossess();
	Controller->Possess(Pawn);
	TestEqual(TEXT("An ended component cannot restart recording"), FGlobalTimerTestAccess::GetTimerCount(Timer), 0);
	Timer.StopGlobalTimer();
	return true;
}

bool FGlobalTimerReentrantCleanupTest::RunTest(const FString& Parameters)
{
	FGlobalTimerFixture Fixture;
	UGlobalTimer& Timer = *Fixture.Timer;
	FGlobalTimerHandle First, Second, SecondCopy, Added, Parent;
	int32 Calls = 0;
	bool SawCanceledTimerAsValid = false;
	struct FCapture
	{
		TFunction<void()> OnDestroy;
		~FCapture() { OnDestroy(); }
	};
	TSharedPtr<FCapture> Resource = MakeShared<FCapture>();
	Resource->OnDestroy = [&]
	{
		SawCanceledTimerAsValid = Timer.IsValidTimer(SecondCopy);
		Timer.ClearTimer(Second);
		Timer.SetTimer(Added, FGobalTimerFunction([&] { ++Calls; }), 0.125f);
		Timer.Tick(0.0f); // Reentrant dispatch must not run while entries are being removed.
	};
	Timer.SetTimer(First, FGobalTimerFunction([Resource] {}), 1.0f);
	Resource.Reset();
	TSharedPtr<int32> SecondResource = MakeShared<int32>(0);
	TWeakPtr<int32> WeakSecondResource = SecondResource;
	Timer.SetTimer(Second, FGobalTimerFunction([SecondResource] {}), 1.0f);
	SecondCopy = Second;
	SecondResource.Reset();
	Timer.SetTimer(Parent, FGobalTimerFunction([&]
	{
		Timer.ClearTimer(First);
		Timer.ClearTimer(Second);
	}), 0.125f);
	FGlobalTimerTestAccess::TickAt(Timer, 0.125);
	TestFalse(TEXT("Canceled timers stay logically invalid throughout capture destruction"), SawCanceledTimerAsValid);
	TestFalse(TEXT("Capture destructor can cancel another timer and release its resources"), WeakSecondResource.IsValid());
	TestTrue(TEXT("Capture destructor can safely create a replacement timer"), Timer.IsValidTimer(Added));
	TestEqual(TEXT("Cleanup prevents reentrant dispatch"), Calls, 0);
	FGlobalTimerTestAccess::TickAt(Timer, 0.25);
	TestEqual(TEXT("Timer created during cleanup executes on the next update"), Calls, 1);
	return true;
}

#endif
