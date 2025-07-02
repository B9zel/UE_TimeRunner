// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/TimeRunner/TimerRunnerCharacter.h"
#include "TestTimerRunnerCharacter.generated.h"

/**
 *
 */
UCLASS()
class TIMERUNNER_API ATestTimerRunnerCharacter : public ATimerRunnerCharacter
{
	GENERATED_BODY()

public:

	void ApplyDilation();
	void StopDilation();

	UTimeDilationComponent* GetTimeDilationComponent() const;
	UIntoxicationComponent* GetIntoxicationComponent() const;
	UHealthComponent* GetHealthComponent() const;
};
