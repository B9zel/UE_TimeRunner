// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "IncreaseIntoxicationAbility.generated.h"

class UIntoxicationComponent;

UCLASS()
class TIMERUNNER_API UIncreaseIntoxicationAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
private:

	UFUNCTION()
	void CustomTick();

	void ExecuteIntoxication();

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ApplyEffect;

private:

	// Time begin
	float Period;
	double LastTime{0.0f};
	// Time end

	UIntoxicationComponent* IntoxicationComp;

	FTimerHandle TickHandle;
	FActiveGameplayEffectHandle EffectHandle;
};
