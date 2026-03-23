// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/Alert/AlertAttributeSet.h"
#include <GameplayEffectExtension.h>

UAlertAttributeSet::UAlertAttributeSet() : IncreaseAlert(0.0f), DecreaseAlert(0.0f), Alert(0.0f), MaxAlert(0.0f)
{
}

void UAlertAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	AActor* InstigatActor = Data.EffectSpec.GetEffectContext().GetInstigatorAbilitySystemComponent()->GetOwner();
	if (Data.EvaluatedData.Attribute == GetDecreaseAlertAttribute())
	{
		const float OldValue = GetAlert();
		const float NewValue = FMath::Clamp(GetAlert() - GetDecreaseAlert(), 0.0f, GetMaxAlert());

		SetAlert(NewValue);
		SetDecreaseAlert(0.0f);
		AlertChange.Broadcast(InstigatActor, OldValue, NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetIncreaseAlertAttribute())
	{
		const float OldValue = GetAlert();
		const float NewValue = FMath::Clamp(GetAlert() + GetIncreaseAlert(), 0.0f, GetMaxAlert());

		SetAlert(NewValue);
		SetIncreaseAlert(0.0f);
		AlertChange.Broadcast(InstigatActor, OldValue, NewValue);
	}
}

void UAlertAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	ClampAttributes(Attribute, GetAlertAttribute(), NewValue, 0.0f, GetMaxAlert());
}
