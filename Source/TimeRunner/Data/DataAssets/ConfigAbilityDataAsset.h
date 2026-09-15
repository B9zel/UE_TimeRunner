// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <Abilities/GameplayAbility.h>
#include "ConfigAbilityDataAsset.generated.h"


USTRUCT()
struct FGameplayAbilitySpecData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> Ability;
	
	UPROPERTY(EditAnywhere)
	bool IsAutoActivate = false;
};


USTRUCT()
struct FGameplayEffectSpecData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> Effect;

	UPROPERTY(EditAnywhere)
	float Level = 1.0f;
};


USTRUCT()
struct FAbilitySetGrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilityHandle(FGameplayAbilitySpecHandle Handle)
	{
		AbilityHandles.Add(Handle);
	}

	void AddEffectHandle(FActiveGameplayEffectHandle Handle)
	{
		EffectHandles.Add(Handle);
	}

	void ClearHandles(UAbilitySystemComponent* AbilitySystemComponent);


private:

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectHandles;
};


/**
 *
 */
UCLASS()
class TIMERUNNER_API UConfigAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:


	void GiveToAbilitySystem(UAbilitySystemComponent* AbilitySystemComponent, FAbilitySetGrantedHandles* GrantedHandles) const;


public:

	UPROPERTY(EditAnywhere)
	TArray<FGameplayAbilitySpecData> Abilities;

	UPROPERTY(EditAnywhere)
	TArray<FGameplayEffectSpecData> Effects;
};
