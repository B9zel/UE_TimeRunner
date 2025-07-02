// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <Abilities/GameplayAbility.h>
#include "ConfigAbilityDataAsset.generated.h"

/**
 *
 */
UCLASS()
class TIMERUNNER_API UConfigAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UGameplayAbility>> Abilities;
};
