// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/GameInstance/MainGameInstance.h"
#include "Global/GlobalTimer/GlobalTimer.h"

void UMainGameInstance::Init()
{
	UGameInstance::Init();

	TimerGlobalPtr = NewObject<UGlobalTimer>(this);
}

float UMainGameInstance::SmallFloatNumber() const
{
	return UE_KINDA_SMALL_NUMBER;
}

UGlobalTimer* UMainGameInstance::GetGlobalTimer() const
{
	return TimerGlobalPtr;
}

void UMainGameInstance::StartGlobalTimer()
{
	TimerGlobalPtr->StartGlobalTimer();
}
