// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Intoxication/IntoxicationAttributeSet.h"

void UIntoxicationAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttributes(Attribute, GetIntoxicationAttribute(), NewValue, 0.0f, GetMaxIntoxication());
}
