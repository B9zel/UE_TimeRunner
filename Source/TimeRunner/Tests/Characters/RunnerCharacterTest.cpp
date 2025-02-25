// Fill out your copyright notice in the Description page of Project Settings.

#if (AUTOMATION_TEST)

#include "Tests/Characters/RunnerCharacterTest.h"
#include "CoreMinimal.h"
#include <Tests/AutomationCommon.h>
#include "TimeRunner/Characters/TimeRunner/TimerRunnerCharacter.h"
#include <EnhancedInputComponent.h>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterCanBeInput, "TimeRunner.Characters.TimeRunner.CharacterCanBeInput",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterShouldBeTimeRange, "TimeRunner.Characters.TimeRunner.CharacterShouldBeTimeRange",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterShouldApplyDilation, "TimeRunner.Characters.TimeRunner.CharacterShouldApplyDilation",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

const FString NameMap = "/Game/Tests/Level/L_EmptyTest";
const FString BPTimeRunnerName = "/Game/Game/Blueprints/Characters/TimeRunner/BP_TimeRunnerCharacter.BP_TimeRunnerCharacter";

bool FCharacterCanBeInput::RunTest(const FString& Parameters)
{

	TestTrue("Can't open map", AutomationOpenMap(FString(NameMap)));
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

	const TArray<UInputAction*> Inputs = {Runner->GetInputObject().RunAction, Runner->GetInputObject().LookActions};

	for (int i = 0; i < EnhuncedInput->GetActionEventBindings().Num(); i++)
	{
		const auto& Input = EnhuncedInput->GetActionEventBindings()[i];
		if (!TestTrue("Input action doesn't valid", Input.IsValid())) return false;

		if (!Inputs.Contains(Input->GetAction()))
		{
			AddError(FString::Printf(TEXT("InputComponent of \"ATimerRunnerCharacter\" doesn't have %s"), *Input->GetAction()->GetName()), 1);
			return false;
		}
	}

	ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand);
	return true;
}

bool FCharacterShouldBeTimeRange::RunTest(const FString& Parameters)
{
	auto* Runner = NewObject<ATimerRunnerCharacter>();

	TArray<FVector2f> TestParam = {{0.0f, 0.0f},  //
								   {-5.0f, 0.0f}, //
								   {1.0f, 1.0f},  //
								   {50.0f, 1.0f}, //
								   {0.5f, 0.5f},  //
								   {0.231f, 0.231f}};

	for (auto& i : TestParam)
	{
		Runner->SetRunWorldTime(i[0]);
		if (!TestEqual("Run out of range", Runner->GetRunWorldTime(), i[1]))
		{
			AddInfo(FString::Printf(TEXT("Input: %f"), i[0]));
			return false;
		}
		Runner->SetWalkWorldTime(i[0]);
		if (!TestEqual("Walk out of range", Runner->GetWalkWorldTime(), i[1]))
		{
			AddInfo(FString::Printf(TEXT("Input: %f"), i[0]), 1);
			return false;
		}
	}
	Runner->MarkAsGarbage();

	return true;
}

bool FCharacterShouldApplyDilation::RunTest(const FString& Parameters)
{
	auto* Runner = NewObject<ATimerRunnerCharacter>();

	return true;
}

#endif
