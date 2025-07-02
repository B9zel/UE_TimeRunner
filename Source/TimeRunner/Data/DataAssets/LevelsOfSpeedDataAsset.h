// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelsOfSpeedDataAsset.generated.h"

UCLASS()
class TIMERUNNER_API ULevelsOfSpeedDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	const float GetSlowLevel() const;
	const float GetMiddleLevel() const;
	const float GetFastLevel() const;
	const float GetVeryFastLevel() const;

protected:

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
	float SlowLevelWorldDilation;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
	float MiddleLevelWorldDilation;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
	float FastLevelWorldDilation;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
	float VeryFastLevelWorldDilation;
};
