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

/**
 *
 */
UCLASS()
class TIMERUNNER_API UConfigAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<FGameplayAbilitySpecData> Abilities;
};
