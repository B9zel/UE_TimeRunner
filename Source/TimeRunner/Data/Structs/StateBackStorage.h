// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateBackStorage.generated.h"

USTRUCT(BlueprintType)
struct FStateBackStorage
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FTransform Transform;
	UPROPERTY(BlueprintReadWrite)
	float Health;
};