// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/DataAssets/ConfigAbilityDataAsset.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

void UConfigAbilityDataAsset::GiveToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent, FAbilitySetGrantedHandles* GrantedHandles) const
{
	if (!AbilitySystemComponent) return;

	// Give abilities to the ability system component
	for (const FGameplayAbilitySpecData& AbilityData : Abilities)
	{
		if (!AbilityData.Ability) continue;
		FGameplayAbilitySpec Spec(AbilityData.Ability);
		if (AbilityData.IsAutoActivate)
		{
			FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbilityAndActivateOnce(Spec);
			if (GrantedHandles)
			{
				GrantedHandles->AddAbilityHandle(Handle);
			}
		}
		else
		{
			FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
			if (GrantedHandles)
			{
				GrantedHandles->AddAbilityHandle(Handle);
			}
		}
	}

	// Give effects to the ability system component
	for (const FGameplayEffectSpecData& EffectData : Effects)
	{
		if (!EffectData.Effect) continue;
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectData.Effect, EffectData.Level, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (GrantedHandles)
			{
				GrantedHandles->AddEffectHandle(ActiveGEHandle);
			}
		}
	}
}

void FAbilitySetGrantedHandles::ClearHandles(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!AbilitySystemComponent || (AbilityHandles.IsEmpty() && EffectHandles.IsEmpty())) return;

	for (const FGameplayAbilitySpecHandle Handle : AbilityHandles)
	{
		AbilitySystemComponent->ClearAbility(Handle);
	}
	for (FActiveGameplayEffectHandle& Handle : EffectHandles)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(Handle);
	}
	AbilityHandles.Empty();
	EffectHandles.Empty();
}
