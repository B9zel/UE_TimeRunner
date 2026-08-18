// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Health/HealthComponent.h"
#include "TimeRunnerHealthComponent.generated.h"

/**
 *
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class TIMERUNNER_API UTimeRunnerHealthComponent : public UHealthComponent
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float ScaleRunHealing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float ScaleIdleHealing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", Units = "s", ClampMin = "0.001"))
	float RateHealing;
};
