// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EStateEnemy : uint8
{
	Patrol = 0,
	SearchTarget,
	SightTarget
};
