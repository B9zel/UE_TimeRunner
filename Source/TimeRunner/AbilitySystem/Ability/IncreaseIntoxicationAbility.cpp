// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/IncreaseIntoxicationAbility.h"
#include "AbilitySystemComponent.h"
#include "Components/Intoxication/IntoxicationComponent.h"
#include "AbilitySystem/Attributes/Intoxication/IntoxicationAttributeSet.h"
#include "Components/Intoxication/IntoxicationComponent.h"
#include <Kismet/GameplayStatics.h>

void UIncreaseIntoxicationAbility::CustomTick()
{
	const double Now = FPlatformTime::Seconds();
	if (Now - LastTime >= Period)
	{
		ExecuteIntoxication();
		LastTime = Now;
	}
}

void UIncreaseIntoxicationAbility::ExecuteIntoxication()
{
	const auto& IntoxicationAttrib = IntoxicationComp->GetIntoxicationAttribute();
	const float NewIntoxication = IntoxicationComp->GetCurrentIntoxication() + IntoxicationComp->GetSpeedIncrease();
	GetAbilitySystemComponentFromActorInfo()->SetNumericAttributeBase(IntoxicationAttrib->GetIntoxicationAttribute(), NewIntoxication);
}
