// Fill out your copyright notice in the Description page of Project Settings.

#if (AUTOMATION_TEST)

#include "Tests/Components/Intoxication/IntoxicationComponentTest.h"
#include "CoreMinimal.h"
#include <Tests/AutomationCommon.h>
#include "TimeRunner/Components/Intoxication/IntoxicationComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "TimeRunner/Tests/TestUtils.h"
#include "TimeRunner/Tests/Characters/Class/TestTimerRunnerCharacter.h"
#include "TimeRunner/Tests/Components/Intoxication/Class/TestIntoxicationComponent.h"

using namespace AutomationTestUtils;

DECLARE_LOG_CATEGORY_CLASS(IntoxicationTest, All, All);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShouldBeIncreaseIntoxication, "TimeRunner.Components.Intoxication.ShouldBeIncreaseIntoxication",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShouldBeDecreaseIntoxication, "TimeRunner.Components.Intoxication.ShouldBeDecreaseIntoxication",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShouldBeCheckInputValue, "TimeRunner.Components.Intoxication.ShouldBeCheckInputValue",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShouldBeApplyDamage, "TimeRunner.Components.Intoxication.ShouldBeApplyDamage",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(FCheckIntoxicationWithTimer, TObjectPtr<ATestTimerRunnerCharacter>, Runner, float, Seconds, const float,
												 Expected);

const float Delay = 0.2f;

bool FCheckIntoxicationWithTimer::Update()
{
	const double Time = FPlatformTime::Seconds();

	if (Time - StartTime >= Seconds)
	{
		auto* Intoxication = Runner->GetIntoxicationComponent();

		/*if (Intoxication->GetCurrentIntoxication() != Expected)
		{
			UE_LOG(IntoxicationTest, Error, TEXT("Current intoxication(%f) doesn't equal %f"), Intoxication->GetCurrentIntoxication(), Expected);
		}*/
		Runner->Destroy();

		ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand);

		return true;
	}
	return false;
}

bool FShouldBeIncreaseIntoxication::RunTest(const FString& Parameters)
{
	AutomationOpenMap(NameTestMap);

	UWorld* World = AutomationCommon::GetAnyGameWorld();
	TestNotNull("World doesn't exist", World);

	TObjectPtr<ATestTimerRunnerCharacter> Runner = World->SpawnActor<ATestTimerRunnerCharacter>(ATestTimerRunnerCharacter::StaticClass(), FTransform::Identity);
	if (!TestNotNull("ATestTimerRunnerCharacter class dont't exist", Runner.Get())) return false;
	AddInfo("Spawn ATestTimerRunnerCharacter");

	UIntoxicationComponent* IntoxicationComponent = Runner->GetIntoxicationComponent();

	TestNotNull("Intoxication component doesn't valid", IntoxicationComponent);

	IntoxicationComponent->SetMaxIntoxication(100.0f);
	// IntoxicationComponent->SetCurrentIntoxication(0.0f);
	IntoxicationComponent->SetRateIncrease(1.0f);
	IntoxicationComponent->SetRateDecrease(1.0f);
	IntoxicationComponent->SetSpeedDecreaseIntoxication(10.0f);
	IntoxicationComponent->SetSpeedIncreaseIntoxication(10.0f);

	TestEqual("Different values", IntoxicationComponent->GetMaxIntoxication(), 100.0f);
	// TestEqual("Different values", IntoxicationComponent->GetCurrentIntoxication(), 0.0f);
	TestEqual("Different values", IntoxicationComponent->GetRateIncrease(), 1.0f);
	TestEqual("Different values", IntoxicationComponent->GetRateDecrease(), 1.0f);
	TestEqual("Different values", IntoxicationComponent->GetSpeedIncrease(), 10.0f);
	TestEqual("Different values", IntoxicationComponent->GetSpeedDecrease(), 10.0f);

	IntoxicationComponent->ActivateIntixication();

	ADD_LATENT_AUTOMATION_COMMAND(FCheckIntoxicationWithTimer(Runner, 2.0f, 20.0f));

	return true;
}

bool FShouldBeDecreaseIntoxication::RunTest(const FString& Parameters)
{
	AutomationOpenMap(NameTestMap);

	UWorld* World = AutomationCommon::GetAnyGameWorld();
	TestNotNull("World doesn't exist", World);

	TObjectPtr<ATestTimerRunnerCharacter> Runner = World->SpawnActor<ATestTimerRunnerCharacter>(ATestTimerRunnerCharacter::StaticClass(), FTransform::Identity);
	if (!TestNotNull("ATestTimerRunnerCharacter class dont't exist", Runner.Get())) return false;
	AddInfo("Spawn ATestTimerRunnerCharacter");

	UIntoxicationComponent* IntoxicationComponent = Runner->GetIntoxicationComponent();

	TestNotNull("Intoxication component doesn't valid", IntoxicationComponent);

	IntoxicationComponent->SetMaxIntoxication(100.0f);
	// IntoxicationComponent->SetCurrentIntoxication(100.0f);
	IntoxicationComponent->SetRateIncrease(1.0f);
	IntoxicationComponent->SetRateDecrease(1.0f);
	IntoxicationComponent->SetSpeedDecreaseIntoxication(10.0f);
	IntoxicationComponent->SetSpeedIncreaseIntoxication(10.0f);

	TestEqual("Different values", IntoxicationComponent->GetMaxIntoxication(), 100.0f);
	// TestEqual("Different values", IntoxicationComponent->GetCurrentIntoxication(), 100.0f);
	TestEqual("Different values", IntoxicationComponent->GetRateIncrease(), 1.0f);
	TestEqual("Different values", IntoxicationComponent->GetRateDecrease(), 1.0f);
	TestEqual("Different values", IntoxicationComponent->GetSpeedIncrease(), 10.0f);
	TestEqual("Different values", IntoxicationComponent->GetSpeedDecrease(), 10.0f);

	IntoxicationComponent->DeactivateIntixication();

	ADD_LATENT_AUTOMATION_COMMAND(FCheckIntoxicationWithTimer(Runner, 2.0f + Delay, 80.0f));

	return true;
}

bool FShouldBeCheckInputValue::RunTest(const FString& Parameters)
{
	auto* IntoxicationComp = NewObject<UIntoxicationComponent>();
	TestNotNull("Intoxication component doesn't exist", IntoxicationComp);

	const TArray<FVector2f> TestMaxIntoxicatin = {
		{-7.0f, 1.0f},	   //
		{0.0f, 1.0f},	   //
		{55.0f, 55.0f},	   //
		{1034.5f, 1034.5f} //
	};

	for (auto& i : TestMaxIntoxicatin)
	{
		IntoxicationComp->SetMaxIntoxication(i[0]);
		TestEqual("Max intoxication has different value", IntoxicationComp->GetMaxIntoxication(), i[1]);
	}

	const TArray<FVector2f> TesCurrentIntoxicatin = {
		{-7.0f, 0.0f},	//
		{0.0f, 0.0f},	//
		{55.0f, 55.0f}, //
		{34.5f, 34.5f}	//
	};

	for (auto& i : TesCurrentIntoxicatin)
	{
		// IntoxicationComp->SetCurrentIntoxication(i[0]);
		// TestEqual("Current intoxication has different value", IntoxicationComp->GetCurrentIntoxication(), i[1]);
	}

	const TArray<FVector2f> TesRateIntoxicatin = {
		{-7.0f, UE_KINDA_SMALL_NUMBER}, //
		{0.0f, UE_KINDA_SMALL_NUMBER},	//
		{66.0f, 66.0f},					//
		{304.5f, 304.5f}				//
	};

	for (auto& i : TesRateIntoxicatin)
	{
		IntoxicationComp->SetRateIncrease(i[0]);
		TestEqual("Rate increase has different value", IntoxicationComp->GetRateIncrease(), i[1]);
		IntoxicationComp->SetRateDecrease(i[0]);
		TestEqual("Rate decrease has different value", IntoxicationComp->GetRateDecrease(), i[1]);
	}

	const TArray<FVector2f> TesSpeedIntoxicatin = {
		{-7.0f, 0.0f},	 //
		{-70.0f, 0.0f},	 //
		{0.0f, 0.0f},	 //
		{66.0f, 66.0f},	 //
		{304.5f, 304.5f} //
	};

	for (auto& i : TesRateIntoxicatin)
	{
		IntoxicationComp->SetSpeedIncreaseIntoxication(i[0]);
		TestEqual("Speed increase has different value", IntoxicationComp->GetSpeedIncrease(), i[1]);
		IntoxicationComp->SetSpeedDecreaseIntoxication(i[0]);
		TestEqual("Speed decrease has different value", IntoxicationComp->GetSpeedDecrease(), i[1]);
	}

	const TArray<FVector2f>& TesDamageIntoxicatin = TesSpeedIntoxicatin;
	for (auto& i : TesDamageIntoxicatin)
	{
		IntoxicationComp->SetIntoxicationDamage(i[0]);
		TestEqual("Intoxication damage has different value", IntoxicationComp->GetIntoxicationDamage(), i[1]);
	}

	const TArray<FVector2f>& TestRateApplyDamage = TesRateIntoxicatin;
	for (auto& i : TestRateApplyDamage)
	{
		IntoxicationComp->SetRateApplyDamage(i[0]);
		TestEqual("Rate apply damage has different value", IntoxicationComp->GetRateApplyDamage(), i[1]);
	}

	const TArray<FVector2f>& TestSafeBorder = {{-7.0f, 0.0f},  //
											   {-70.0f, 0.0f}, //
											   {0.0f, 0.0f},   //
											   {66.0f, 1.0f},  //
											   {304.5f, 1.0f}, //
											   {0.35f, 0.35},  //
											   {0.876f, 0.876f}};
	for (auto& i : TestSafeBorder)
	{
		IntoxicationComp->SetSafeBorder(i[0]);
		TestEqual("Safe border has different value", IntoxicationComp->GetSafeBorder(), i[1]);
	}

	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(FShouldBeTakentoxicationDamage, TObjectPtr<ATestTimerRunnerCharacter>, Runner, TObjectPtr<UTestCharacter>,
												 OtherCharacter, const float, Wait);

bool FShouldBeTakentoxicationDamage::Update()
{
	const double Time = FPlatformTime::Seconds();

	if (OtherCharacter->WasApplyDamage)
	{
		UE_LOG(IntoxicationTest, Display, TEXT("Was apply damage"));
		return true;
	}

	if (Time - StartTime >= Wait)
	{
		Runner->Destroy();

		ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand);

		return true;
	}
	return false;
}

bool FShouldBeApplyDamage::RunTest(const FString& Parameters)
{
	AutomationOpenMap(NameTestMap);

	UWorld* World = AutomationCommon::GetAnyGameWorld();
	TestNotNull("World doesn't exist", World);

	TObjectPtr<ATestTimerRunnerCharacter> Runner = World->SpawnActor<ATestTimerRunnerCharacter>(ATestTimerRunnerCharacter::StaticClass(), FTransform::Identity);
	if (!TestNotNull("ATestTimerRunnerCharacter class dont't exist", Runner.Get())) return false;
	AddInfo("Spawn ATestTimerRunnerCharacter");

	TObjectPtr<UTestCharacter> testCharacter = NewObject<UTestCharacter>();
	Runner->OnTakeAnyDamage.AddDynamic(testCharacter.Get(), &UTestCharacter::TakeDamage);

	UIntoxicationComponent* IntoxicationComponent = Runner->GetIntoxicationComponent();

	TestNotNull("Intoxication component doesn't valid", IntoxicationComponent);

	IntoxicationComponent->SetMaxIntoxication(100.0f);
	// IntoxicationComponent->SetCurrentIntoxication(100.0f);
	IntoxicationComponent->SetRateIncrease(1.0f);
	IntoxicationComponent->SetRateDecrease(1.0f);
	IntoxicationComponent->SetSpeedDecreaseIntoxication(10.0f);
	IntoxicationComponent->SetSpeedIncreaseIntoxication(10.0f);
	IntoxicationComponent->SetSafeBorder(0.5f);
	IntoxicationComponent->SetIntoxicationDamage(10.0f);
	IntoxicationComponent->SetRateApplyDamage(0.1f);

	IntoxicationComponent->ActivateIntixication();

	ADD_LATENT_AUTOMATION_COMMAND(FShouldBeTakentoxicationDamage(Runner, testCharacter, 0.1f + Delay));

	return true;
}

#endif