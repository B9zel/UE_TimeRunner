// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Health/HealthAttributeSet.h"
#include <GameplayEffectExtension.h>

UHealthAttributeSet::UHealthAttributeSet() : AppliedDamage(0.0f), Healing(0.0f)
{
}

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// ClampAttributes(Attribute, GetHealthAttribute(), NewValue, 0.0f, GetMaxHealth());
	ClampAttributes(Attribute, GetHealthAttribute(), NewValue, 0.0f, GetMaxHealth());
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	AActor* InstigatActor = Data.EffectSpec.GetEffectContext().GetInstigatorAbilitySystemComponent()->GetOwner();
	if (Data.EvaluatedData.Attribute == GetAppliedDamageAttribute())
	{
		const float OldValue = GetHealth();
		const float NewValue = FMath::Clamp(GetHealth() - GetAppliedDamage(), 0.0f, GetMaxHealth());

		SetHealth(NewValue);
		SetAppliedDamage(0.0f);
		ChangeHealth.Broadcast(InstigatActor, OldValue, NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		const float OldValue = GetHealth();
		const float NewValue = FMath::Clamp(GetHealth() + GetHealing(), 0.0f, GetMaxHealth());

		SetHealth(NewValue);
		SetHealing(0.0f);
		ChangeHealth.Broadcast(InstigatActor, OldValue, NewValue);
	}
}
