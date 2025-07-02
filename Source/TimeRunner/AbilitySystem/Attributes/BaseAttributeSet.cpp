// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/BaseAttributeSet.h"

void UBaseAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	ChangeAttribute.Broadcast(Attribute, NewValue);
}

void UBaseAttributeSet::ClampAttributes(const FGameplayAttribute& InAttributes, const FGameplayAttribute& SelfAttributes, float& Current, const float Min,
										const float Max)
{
	if (InAttributes == SelfAttributes)
	{
		Current = FMath::Clamp<float>(Current, Min, Max);
	}
}
