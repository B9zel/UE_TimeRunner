// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "BaseUtils.h"
#include "BaseAttributeSet.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FChangeDelegate, const FGameplayAttribute&, const float);
/**
 *
 */
UCLASS()
class TIMERUNNER_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

protected:

	void ClampAttributes(const FGameplayAttribute& InAttributes, const FGameplayAttribute& SelfAttributes, float& Current, const float Min, const float Max);

public:

	FChangeDelegate ChangeAttribute;
};
