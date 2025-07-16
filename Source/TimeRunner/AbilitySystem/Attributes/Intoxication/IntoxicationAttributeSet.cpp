// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Intoxication/IntoxicationAttributeSet.h"
#include <GameplayEffectExtension.h>

void UIntoxicationAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttributes(Attribute, GetIntoxicationAttribute(), NewValue, 0.0f, GetMaxIntoxication());
}

void UIntoxicationAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetIncreaseIntoxicationAttribute())
	{
		SetIntoxication(FMath::Clamp(GetIntoxication() + GetIncreaseIntoxication(), 0.0f, GetMaxIntoxication()));
		SetIncreaseIntoxication(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetDecreaseIntoxicationAttribute())
	{
		SetIntoxication(FMath::Clamp(GetIntoxication() - GetDecreaseIntoxication(), 0.0f, GetMaxIntoxication()));
		SetDecreaseIntoxication(0.0f);
	}
}
