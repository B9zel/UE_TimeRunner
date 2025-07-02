// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

class UGlobalTimer;

UCLASS()
class TIMERUNNER_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	UFUNCTION(BlueprintPure)
	float SmallFloatNumber() const;

	UFUNCTION(BlueprintPure)
	UGlobalTimer* GetGlobalTimer() const;

	void StartGlobalTimer();

private:

	UPROPERTY()
	TObjectPtr<UGlobalTimer> TimerGlobalPtr;
};
