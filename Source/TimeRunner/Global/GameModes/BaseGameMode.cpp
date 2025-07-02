// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/GameModes/BaseGameMode.h"
#include "Global/GameInstance/MainGameInstance.h"
#include "TimeRunner/Global/Pools/BulletPool/BulletPool.h"

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (auto* GameInstance = GetGameInstance<UMainGameInstance>())
	{
		GameInstance->StartGlobalTimer();
	}

	if (BulletPoolClass.Get())
	{
		BulletPool = NewObject<UBulletPool>(this, BulletPoolClass);
		check(BulletPool);
		BulletPool->InitializePool();
	}
}
