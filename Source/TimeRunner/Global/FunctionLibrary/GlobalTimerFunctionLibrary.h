// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Global/GlobalTimer/GlobalTimer.h"
#include "GlobalTimerFunctionLibrary.generated.h"

class FGlobalTimerDelegate;
struct FGlobalTimerHandle;
/**
 *
 */
UCLASS()
class TIMERUNNER_API UGlobalTimerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Global timer")
	static FGlobalTimerHandle SetGlobalTimer(FGlobalTimerDelegate Delegate, const float Rate, const bool IsLoop = false);

	UFUNCTION(BlueprintCallable, Category = "Global timer", meta = (DefaultToSelf = "Object", WorldContext = "Object"))
	static void ClearGlobalTimer(const UObject* Object, UPARAM(ref) FGlobalTimerHandle& Timer);

	UFUNCTION(BlueprintPure, Category = "Global timer")
	static bool IsValidTimer(FGlobalTimerHandle TimerHandle);

	UFUNCTION(BlueprintPure, Category = "Time")
	static double GetTotalSeconds();
};
