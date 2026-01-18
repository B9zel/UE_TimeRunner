// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/BaseAttributeSet.h"
#include "DamageAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class TIMERUNNER_API UDamageAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:

	UDamageAttributeSet();

public:

	ATTRIBUTE_ACCESSORS(UDamageAttributeSet, Damage)

protected:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Damage;
};
