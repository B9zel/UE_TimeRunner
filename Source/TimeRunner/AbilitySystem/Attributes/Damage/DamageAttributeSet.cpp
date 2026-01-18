// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/Damage/DamageAttributeSet.h"

UDamageAttributeSet::UDamageAttributeSet() : Damage(0.0f)
{
}

void UDamageAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (NewValue < 0.0f)
	{
		NewValue = 0.0f;
	}
}
