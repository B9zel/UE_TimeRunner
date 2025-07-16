// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Health/HealthAttributeSet.h"
#include <GameplayEffectExtension.h>

UHealthAttributeSet::UHealthAttributeSet() : Damage(0.0f), Healing(0.0f)
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

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), 0.0f, GetMaxHealth()));
		SetDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), 0.0f, GetMaxHealth()));
		SetHealing(0.0f);
	}
}
