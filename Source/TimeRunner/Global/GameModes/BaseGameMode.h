// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

class UBulletPool;

UCLASS()
class TIMERUNNER_API ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UBulletPool> BulletPoolClass;

	UPROPERTY()
	UBulletPool* BulletPool;
};
