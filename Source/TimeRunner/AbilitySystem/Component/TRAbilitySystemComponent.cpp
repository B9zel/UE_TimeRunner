// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Component/TRAbilitySystemComponent.h"
#include <GameplayAbilitySpecHandle.h>

bool UTRAbilitySystemComponent::TryActivateAbilityByTagOutHandle(const FGameplayTagContainer& Tags, TArray<FGameplayAbilitySpecHandle>& OutHandle,
																 bool bAllowRemoteActivation)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivatePtrs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(Tags, AbilitiesToActivatePtrs);
	if (AbilitiesToActivatePtrs.Num() < 1)
	{
		return false;
	}

	// Convert from pointers (which can be reallocated, since they point to internal data) to copies of that data
	TArray<FGameplayAbilitySpec> AbilitiesToActivate;
	AbilitiesToActivate.Reserve(AbilitiesToActivatePtrs.Num());
	Algo::Transform(AbilitiesToActivatePtrs, AbilitiesToActivate, [](FGameplayAbilitySpec* SpecPtr) { return *SpecPtr; });

	bool bSuccess = false;
	for (const FGameplayAbilitySpec& GameplayAbilitySpec : AbilitiesToActivate)
	{
		ensure(IsValid(GameplayAbilitySpec.Ability));
		const bool IsActivated = TryActivateAbility(GameplayAbilitySpec.Handle, bAllowRemoteActivation);
		bSuccess |= IsActivated;

		if (IsActivated)
		{
			OutHandle.Add(GameplayAbilitySpec.Handle);
		}
	}
	return bSuccess;
}

void UTRAbilitySystemComponent::CancelAciveAbilities(const FGameplayTagContainer WithTags)
{
	CancelAbilities(&WithTags, nullptr);
}

void UTRAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	OnAbilityEnded.AddUObject(this, &ThisClass::NotifyOnEndAbility);
}

void UTRAbilitySystemComponent::NotifyOnEndAbility(const FAbilityEndedData& Data)
{
	OnEndAbility.Broadcast(Data.AbilitySpecHandle, Data.bWasCancelled);
}
