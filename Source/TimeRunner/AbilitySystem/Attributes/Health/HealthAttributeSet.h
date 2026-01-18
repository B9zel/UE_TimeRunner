// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/Attributes/BaseAttributeSet.h"
#include "HealthAttributeSet.generated.h"

class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FChangeHealth,AActor*, const float, const float)

UCLASS()
class TIMERUNNER_API UHealthAttributeSet : public UBaseAttributeSet
{
	GENERATED_BODY()

public:

	UHealthAttributeSet();

public:

	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, AppliedDamage)
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Healing)

public:

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

public:

	mutable FChangeHealth ChangeHealth;

private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", HideFromModifiers))
	FGameplayAttributeData Health;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData AppliedDamage;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayAttributeData Healing;
};