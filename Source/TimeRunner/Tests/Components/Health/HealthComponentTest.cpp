// Fill out your copyright notice in the Description page of Project Settings.

#include "Tests/Components/Health/HealthComponentTest.h"
#include "CoreMinimal.h"
#include "TimeRunner/Tests/TestUtils.h"
#include "TimeRunner/Tests/Characters/Class/TestTimerRunnerCharacter.h"
#include "TimeRunner/Components/Health/HealthComponent.h"

#include <Kismet/GameplayStatics.h>

using namespace AutomationTestUtils;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCanBeTakeDamage, "TimeRunner.Components.Health.CanBeTakeDamage",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCheckRangeHP, "TimeRunner.Components.Health.CheckRangeHP",
								 EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::HighPriority | EAutomationTestFlags::ProductFilter);

bool FCanBeTakeDamage::RunTest(const FString& Parameters)
{
	OpenAutomationMap level(NameTestMap);

	UWorld* World = AutomationCommon::GetAnyGameWorld();
	TestNotNull("World doesn't exist", World);

	TObjectPtr<ATestTimerRunnerCharacter> Runner = World->SpawnActor<ATestTimerRunnerCharacter>(ATestTimerRunnerCharacter::StaticClass(), FTransform::Identity);
	if (!TestNotNull("ATestTimerRunnerCharacter class dont't exist", Runner.Get())) return false;
	AddInfo("Spawn ATestTimerRunnerCharacter");

	UHealthComponent* HealthComp = Runner->GetHealthComponent();
	TestNotNull("Health component doesn't exist", HealthComp);

	// HealthComp->SetMaxHP(100.0f);
	// TestEqual("Max HP is different", HealthComp->GetMaxHP(), 100.0f);
	// HealthComp->SetCurrentHP(100.0f);
	// TestEqual("Current HP is different", HealthComp->GetCurrentHP(), 100.0f);

	UGameplayStatics::ApplyDamage(Runner, 40.0f, nullptr, nullptr, TSubclassOf<UDamageType>());

	// TestEqual("Current HP is different after apply damage", HealthComp->GetCurrentHP(), 60.0f);

	return true;
}

bool FCheckRangeHP::RunTest(const FString& Parameters)
{
	OpenAutomationMap level(NameTestMap);

	UWorld* World = AutomationCommon::GetAnyGameWorld();
	TestNotNull("World doesn't exist", World);

	TObjectPtr<ATestTimerRunnerCharacter> Runner = World->SpawnActor<ATestTimerRunnerCharacter>(ATestTimerRunnerCharacter::StaticClass(), FTransform::Identity);
	if (!TestNotNull("ATestTimerRunnerCharacter class dont't exist", Runner.Get())) return false;
	AddInfo("Spawn ATestTimerRunnerCharacter");

	UHealthComponent* HealthComp = Runner->GetHealthComponent();
	TestNotNull("Health component doesn't exist", HealthComp);

	const TArray<FVector2f> TestMaxHPValues = {
		{-55.0f, 0.0f},	   //
		{0.0f, 0.0f},	   //
		{50.0f, 50.0f},	   //
		{5435.4f, 5435.4f} //
	};

	for (auto& i : TestMaxHPValues)
	{
		// HealthComp->SetMaxHP(i[0]);
		// TestEqual("Max HP has different value", HealthComp->GetMaxHP(), i[1]);
	}
	// HealthComp->SetMaxHP(100.0f);
	// TestEqual("Max HP has different value", HealthComp->GetMaxHP(), 100.0f);

	const TArray<FVector2f> TestCurrentHPValues = {
		{-55.0f, 0.0f},	  //
		{0.0f, 0.0f},	  //
		{5.0f, 5.0f},	  //
		{5435.4f, 100.0f} //
	};
	for (auto& i : TestCurrentHPValues)
	{
		///	HealthComp->SetCurrentHP(i[0]);
		//	TestEqual("Current HP has different value", HealthComp->GetCurrentHP(), i[1]);
	}

	// HealthComp->SetMaxHP(100.0f);
	// HealthComp->SetCurrentHP(110.0f);
	// TestEqual("Current HP has different value", HealthComp->GetCurrentHP(), 100.0f);

	// HealthComp->SetMaxHP(50.0f);
	// TestEqual("Max HP has different value", HealthComp->GetMaxHP(), 50.0f);
	// TestEqual("Current HP has different value", HealthComp->GetCurrentHP(), 50.0f);

	return true;
}
