// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/BaseAttributeSet.h"
#include "IntoxicationAttributeSet.generated.h"

/**
 *
 */
UCLASS()
class TIMERUNNER_API UIntoxicationAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:

	ATTRIBUTE_ACCESSORS(ThisClass, Intoxication)
	ATTRIBUTE_ACCESSORS(ThisClass, MaxIntoxication)

public:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

private:

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Intoxication;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxIntoxication;
};
