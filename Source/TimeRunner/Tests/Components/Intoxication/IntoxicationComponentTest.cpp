// Fill out your copyright notice in the Description page of Project Settings.

#if (AUTOMATION_TEST)

#include "Tests/Components/Intoxication/IntoxicationComponentTest.h"
#include "CoreMinimal.h"
#include <Tests/AutomationCommon.h>
#include "TimeRunner/Components/Intoxication/IntoxicationComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "TimeRunner/Tests/TestUtils.h"
#include "TimeRunner/Tests/Characters/Class/TestTimerRunnerCharacter.h"

using namespace AutomationTestUtils;

DECLARE_LOG_CATEGORY_CLASS(IntoxicationTest, All, All);

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShouldBeIncreaseIntoxication, "TimeRunner.Components.Intoxication.ShouldBeIncreaseIntoxication",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FShouldBeDecreaseIntoxication, "TimeRunner.Components.Intoxication.ShouldBeDecreaseIntoxication",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(FCheckIntoxicationWithTimer, TObjectPtr<ATestTimerRunnerCharacter>, Runner, float, Seconds, const float,
												 Expected);

bool FCheckIntoxicationWithTimer::Update()
{
	const double Time = FPlatformTime::Seconds();

	if (Time - StartTime >= Seconds)
	{
		auto* Intoxication = Runner->GetIntoxicationComponent();

		if (Intoxication->GetCurrentIntoxication() != Expected)
		{
			UE_LOG(IntoxicationTest, Error, TEXT("Current intoxication(%f) doesn't equal %f"), Intoxication->GetCurrentIntoxication(), Expected);
		}
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
	IntoxicationComponent->SetCurrentIntoxication(0.0f);
	IntoxicationComponent->SetRateIncrease(1.0f);
	IntoxicationComponent->SetRateDecrease(1.0f);
	IntoxicationComponent->SetSpeedDecreaseIntoxication(10.0f);
	IntoxicationComponent->SetSpeedIncreaseIntoxication(10.0f);

	TestEqual("Different values", IntoxicationComponent->GetMaxIntoxication(), 100.0f);
	TestEqual("Different values", IntoxicationComponent->GetCurrentIntoxication(), 0.0f);
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
	IntoxicationComponent->SetCurrentIntoxication(100.0f);
	IntoxicationComponent->SetRateIncrease(1.0f);
	IntoxicationComponent->SetRateDecrease(1.0f);
	IntoxicationComponent->SetSpeedDecreaseIntoxication(10.0f);
	IntoxicationComponent->SetSpeedIncreaseIntoxication(10.0f);

	TestEqual("Different values", IntoxicationComponent->GetMaxIntoxication(), 100.0f);
	TestEqual("Different values", IntoxicationComponent->GetCurrentIntoxication(), 100.0f);
	TestEqual("Different values", IntoxicationComponent->GetRateIncrease(), 1.0f);
	TestEqual("Different values", IntoxicationComponent->GetRateDecrease(), 1.0f);
	TestEqual("Different values", IntoxicationComponent->GetSpeedIncrease(), 10.0f);
	TestEqual("Different values", IntoxicationComponent->GetSpeedDecrease(), 10.0f);

	IntoxicationComponent->DeactivateIntixication();

	ADD_LATENT_AUTOMATION_COMMAND(FCheckIntoxicationWithTimer(Runner, 2.0f, 80.0f));

	return true;
}

#endif