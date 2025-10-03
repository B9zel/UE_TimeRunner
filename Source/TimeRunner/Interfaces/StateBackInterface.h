// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "StateBackInterface.generated.h"

struct FCountermotionData;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStateBackInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class TIMERUNNER_API IStateBackInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BPSetOldestState(const FCountermotionData& Data);

	virtual void SetOldestState(const FCountermotionData& Data) = 0;
};
