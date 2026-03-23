// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "AttributeSet.h"
#include "AttributeCompare.generated.h"

class UAbilitySystemComponent;

UENUM()
enum class EOpirationValue : uint8
{
	Less = 0,
	LessOrEqual,
	Great,
	GreatOrEqual,
	Equal
};

UCLASS(Abstract, Const, DefaultToInstanced, EditInlineNew)
class UBaseAttributeCompare : public UObject
{
	GENERATED_BODY()

public:

	virtual float GetFirstValue(UAbilitySystemComponent* AbilitySystem) PURE_VIRTUAL(UBaseAttributeCompare, return -1.0f;);
	virtual float GetSecondValue(UAbilitySystemComponent* AbilitySystem) PURE_VIRTUAL(UBaseAttributeCompare, return -1.0f;);
};

UCLASS()
class UAttributeValueCompare : public UBaseAttributeCompare
{
	GENERATED_BODY()

public:

	virtual float GetFirstValue(UAbilitySystemComponent* AbilitySystem) override;
	virtual float GetSecondValue(UAbilitySystemComponent* AbilitySystem) override;

protected:

	UPROPERTY(EditAnywhere)
	FGameplayAttribute LeftAttribute;

	UPROPERTY(EditAnywhere)
	FGameplayAttribute RightAttribute;
};

UCLASS()
class UValueCompare : public UBaseAttributeCompare
{
	GENERATED_BODY()

public:

	virtual float GetFirstValue(UAbilitySystemComponent* AbilitySystem) override;
	virtual float GetSecondValue(UAbilitySystemComponent* AbilitySystem) override;

protected:

	UPROPERTY(EditAnywhere)
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere)
	float Value;
};

/**
 *
 */
UCLASS()
class TIMERUNNER_API UAttributeCompare : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()

protected:

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	bool CheckOpiration(const float Val, const float Value) const;

protected:

	UPROPERTY(EditAnywhere, Category = "Context")
	AActor* Source;

	UPROPERTY(EditAnywhere)
	bool Invert{false};

	UPROPERTY(EditAnywhere)
	EOpirationValue Opiration = EOpirationValue::Less;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBaseAttributeCompare> TypeCompare;
};
