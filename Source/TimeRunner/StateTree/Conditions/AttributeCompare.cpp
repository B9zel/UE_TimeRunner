// Fill out your copyright notice in the Description page of Project Settings.

#include "StateTree/Conditions/AttributeCompare.h"
#include "AbilitySystemComponent.h"

bool UAttributeCompare::TestCondition(FStateTreeExecutionContext& Context) const
{
	auto* AbilitySystem = Source->FindComponentByClass<UAbilitySystemComponent>();
	if (!AbilitySystem) return false;

	const bool Res = CheckOpiration(TypeCompare->GetFirstValue(AbilitySystem), TypeCompare->GetSecondValue(AbilitySystem));

	return Invert ? !Res : Res;
}

bool UAttributeCompare::CheckOpiration(const float Val, const float Value) const
{
	switch (Opiration)
	{
	case EOpirationValue::Less:
		return Val < Value;
	case EOpirationValue::LessOrEqual:
		return Val <= Value;
	case EOpirationValue::Great:
		return Val > Value;
	case EOpirationValue::GreatOrEqual:
		return Val >= Value;
	case EOpirationValue::Equal:
		return Val == Value;
	}

	return false;
}

float UAttributeValueCompare::GetFirstValue(UAbilitySystemComponent* AbilitySystem)
{
	bool IsFound;
	return AbilitySystem->GetGameplayAttributeValue(LeftAttribute, IsFound);
}

float UAttributeValueCompare::GetSecondValue(UAbilitySystemComponent* AbilitySystem)
{
	bool IsFound;
	return AbilitySystem->GetGameplayAttributeValue(RightAttribute, IsFound);
}

float UValueCompare::GetFirstValue(UAbilitySystemComponent* AbilitySystem)
{
	bool IsFound;
	return AbilitySystem->GetGameplayAttributeValue(Attribute, IsFound);
}

float UValueCompare::GetSecondValue(UAbilitySystemComponent* AbilitySystem)
{
	return Value;
}
