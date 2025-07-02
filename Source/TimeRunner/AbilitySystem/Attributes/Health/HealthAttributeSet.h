// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/Attributes/BaseAttributeSet.h"
#include "HealthAttributeSet.generated.h"

class UAbilitySystemComponent;

UCLASS()
class TIMERUNNER_API UHealthAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:

	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth)

public:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue);

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Health;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;
};