// Fill out your copyright notice in the Description page of Project Settings.

#if (AUTOMATION_TEST)

#include "Tests/Characters/RunnerCharacterTest.h"
#include "CoreMinimal.h"
#include <Tests/AutomationCommon.h>
#include "TimeRunner/Characters/TimeRunner/TimerRunnerCharacter.h"
#include "TimeRunner/Tests/Characters/Class/TestTimerRunnerCharacter.h"
#include <EnhancedInputComponent.h>
#include "TimeRunner/Tests/TestUtils.h"
#include "TimeRunner/Components/TimeDilation/TimeDilationComponent.h"

using namespace AutomationTestUtils;
DECLARE_LOG_CATEGORY_CLASS(RunnerCharacterTest, All, All);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterCanBeInput, "TimeRunner.Characters.TimeRunner.CharacterCanBeInput",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterShouldBeTimeRange, "TimeRunner.Characters.TimeRunner.CharacterShouldBeTimeRange",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterShouldApplyDilation, "TimeRunner.Characters.TimeRunner.CharacterShouldApplyDilation",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterShouldStopDilation, "TimeRunner.Characters.TimeRunner.CharacterShouldApplyDilation",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

bool FCharacterCanBeInput::RunTest(const FString& Parameters)
{
	OpenAutomationMap level(AutomationTestUtils::NameTestMap);

	AddInfo("Open map");

	UWorld* World = AutomationCommon::GetAnyGameWorld();
	if (!TestNotNull("UWorld class dont't exist", World)) return false;
	AddInfo("Received world");

	UObject* LoadObject = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), nullptr, *BPTimeRunnerName));
	UBlueprint* BPLoadClass = Cast<UBlueprint>(LoadObject);
	TestNotNull(FString::Printf(TEXT("Can't load object with path '%s' "), *BPTimeRunnerName), BPLoadClass);
	TestNotNull("Static class is missing", BPLoadClass->StaticClass());

	auto* Runner = World->SpawnActor<ATimerRunnerCharacter>(BPLoadClass->GeneratedClass, FTransform::Identity);
	if (!TestNotNull("ATimerRunnerCharacter class dont't exist", Runner)) return false;
	AddInfo("Spawn ATimerRunnerCharacter");

	World->GetFirstPlayerController()->Possess(Runner);

	auto* EnhuncedInput = Cast<UEnhancedInputComponent>(Runner->InputComponent);
	if (!TestNotNull("UEnhancedInputComponent class dont't exist", EnhuncedInput)) return false;
	AddInfo("Received enhanced input component");

	const TArray<UInputAction*> Inputs = {Runner->GetInputObject().RunAction,	//
										  Runner->GetInputObject().LookActions, //
										  Runner->GetInputObject().JumpActions, //
										  Runner->GetInputObject().SwitchSpeedActions};

	for (int i = 0; i < EnhuncedInput->GetActionEventBindings().Num(); i++)
	{
		const auto& Input = EnhuncedInput->GetActionEventBindings()[i];
		if (!TestTrue("Input action doesn't valid", Input.IsValid())) return false;

		if (!Inputs.Contains(Input->GetAction()))
		{
			AddError(FString::Printf(TEXT("InputComponent of \"ATimerRunnerCharacter\" doesn't have %s"), *Input->GetAction()->GetName()));
			return false;
		}
	}

	return true;
}

bool FCharacterShouldBeTimeRange::RunTest(const FString& Parameters)
{
	OpenAutomationMap level(AutomationTestUtils::NameTestMap);
	AddInfo("Open map");

	UWorld* World = AutomationCommon::GetAnyGameWorld();
	if (!TestNotNull("UWorld class dont't exist", World)) return false;
	auto* Runner = World->SpawnActor<ATestTimerRunnerCharacter>(ATestTimerRunnerCharacter::StaticClass());
	TestNotNull("ATestTimerRunnerCharacter don't exist", Runner);
	auto* DilationCom = Runner->GetTimeDilationComponent();
	TestNotNull("Time dilation component don't exist", Runner);

	TArray<FVector2f> TestParam = {{0.0f, 0.0f},  //
								   {-5.0f, 0.0f}, //
								   {1.0f, 1.0f},  //
								   {50.0f, 1.0f}, //
								   {0.5f, 0.5f},  //
								   {0.231f, 0.231f}};
	for (auto& i : TestParam)
	{
		DilationCom->SetRunWorldTime(i[0]);
		if (!TestEqual("Run out of range", DilationCom->GetRunWorldTime(), i[1]))
		{
			AddInfo(FString::Printf(TEXT("Input: %f"), i[0]));
			return false;
		}
		DilationCom->SetWalkWorldTime(i[0]);
		if (!TestEqual("Walk out of range", DilationCom->GetWalkWorldTime(), i[1]))
		{
			AddInfo(FString::Printf(TEXT("Input: %f"), i[0]), 1);
			return false;
		}
	}
	Runner->MarkAsGarbage();

	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(FDelayCheckApplyGloblaTime, TObjectPtr<ATestTimerRunnerCharacter>, Runner, const float, Rate, const float,
												 ExpectedDilation);

bool FDelayCheckApplyGloblaTime::Update()
{
	const float Now = FApp::GetCurrentTime();
	if (Now - StartTime >= Rate)
	{
		if (UGameplayStatics::GetGlobalTimeDilation(Runner) != 0.1f)
		{
			UE_LOG(RunnerCharacterTest, Error, TEXT("To be %f, but it was %f"), ExpectedDilation, UGameplayStatics::GetGlobalTimeDilation(Runner));
		}
		ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand);
		return true;
	}
	return false;
}

bool FCharacterShouldApplyDilation::RunTest(const FString& Parameters)
{
	AutomationOpenMap(AutomationTestUtils::NameTestMap);
	AddInfo("Open map");

	UWorld* World = AutomationCommon::GetAnyGameWorld();
	if (!TestNotNull("UWorld class dont't exist", World)) return false;
	auto* Runner = World->SpawnActor<ATestTimerRunnerCharacter>(ATestTimerRunnerCharacter::StaticClass());
	TestNotNull("ATestTimerRunnerCharacter don't exist", Runner);
	auto* DilationComp = Runner->GetTimeDilationComponent();
	TestNotNull("Time dilation component don't exist", Runner);

	DilationComp->SetRunWorldTime(0.1f);
	TestEqual("Different values", DilationComp->GetRunWorldTime(), 0.1f);
	DilationComp->SetWalkWorldTime(1.0f);
	TestEqual("Different values", DilationComp->GetWalkWorldTime(), 1.0f);

	Runner->ApplyDilation();
	ADD_LATENT_AUTOMATION_COMMAND(FDelayCheckApplyGloblaTime(Runner, 0.1f, 0.1));

	return true;
}

DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(FDelayCheckResetGloblaTime, TObjectPtr<ATestTimerRunnerCharacter>, Runner, const float, Rate, const float,
												 ExpectedDilation);

bool FDelayCheckResetGloblaTime::Update()
{
	const float Now = FApp::GetCurrentTime();
	if (Now - StartTime >= Rate)
	{
		Runner->StopDilation();
		if (UGameplayStatics::GetGlobalTimeDilation(Runner) != 0.1f)
		{
			UE_LOG(RunnerCharacterTest, Error, TEXT("To be %f, but it was %f"), ExpectedDilation, UGameplayStatics::GetGlobalTimeDilation(Runner));
		}
		ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand);
		return true;
	}
	return false;
}

bool FCharacterShouldStopDilation::RunTest(const FString& Parameters)
{
	AutomationOpenMap(AutomationTestUtils::NameTestMap);
	AddInfo("Open map");

	UWorld* World = AutomationCommon::GetAnyGameWorld();
	if (!TestNotNull("UWorld class dont't exist", World)) return false;
	auto* Runner = World->SpawnActor<ATestTimerRunnerCharacter>(ATestTimerRunnerCharacter::StaticClass());
	TestNull("ATestTimerRunnerCharacter don't exist", Runner);
	auto* DilationComp = Runner->GetTimeDilationComponent();
	TestNull("Time dilation component don't exist", Runner);

	DilationComp->SetRunWorldTime(0.1f);
	TestEqual("Different values", DilationComp->GetRunWorldTime(), 0.1f);
	DilationComp->SetWalkWorldTime(1.0f);
	TestEqual("Different values", DilationComp->GetWalkWorldTime(), 1.0f);

	Runner->ApplyDilation();
	ADD_LATENT_AUTOMATION_COMMAND(FDelayCheckApplyGloblaTime(Runner, 0.1f, 0.1));

	return true;
}

#endif
