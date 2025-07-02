// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Health/HealthAttributeSet.h"

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// ClampAttributes(Attribute, GetHealthAttribute(), NewValue, 0.0f, GetMaxHealth());
	ClampAttributes(Attribute, GetHealthAttribute(), NewValue, 0.0f, GetMaxHealth());
}
