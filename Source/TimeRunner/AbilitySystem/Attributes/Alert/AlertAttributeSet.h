// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/BaseAttributeSet.h"
#include "AlertAttributeSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FChangeAlert, AActor*, Instigate, const float, OldValue, const float, NewValue);

UCLASS()
class TIMERUNNER_API UAlertAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:

	UAlertAttributeSet();

public:

	ATTRIBUTE_ACCESSORS(UAlertAttributeSet, Alert)
	ATTRIBUTE_ACCESSORS(UAlertAttributeSet, MaxAlert)
	ATTRIBUTE_ACCESSORS(UAlertAttributeSet, IncreaseAlert)
	ATTRIBUTE_ACCESSORS(UAlertAttributeSet, DecreaseAlert)

public:

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

public:

	UPROPERTY(BlueprintAssignable)
	FChangeAlert AlertChange;

private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData IncreaseAlert;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData DecreaseAlert;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", HideFromModifiers))
	FGameplayAttributeData Alert;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", HideFromModifiers))
	FGameplayAttributeData MaxAlert;
};
