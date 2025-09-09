// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WallRunInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWallRunInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class TIMERUNNER_API IWallRunInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartRunWall(); // PURE_VIRTUAL(IWallRunInterface, );
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopRunWall(const bool DisableCanWall = true); // PURE_VIRTUAL(IWallRunInterface, );

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetTimeActiveRunWall();
};
