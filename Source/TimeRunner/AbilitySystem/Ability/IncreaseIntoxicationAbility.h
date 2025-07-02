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

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
							const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

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
