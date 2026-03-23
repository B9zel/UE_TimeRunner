// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TRAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEndAbilityDelegate, FGameplayAbilitySpecHandle, Handle, bool, WasCanelled);

UCLASS()
class TIMERUNNER_API UTRAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool TryActivateAbilityByTagOutHandle(const FGameplayTagContainer& Tags, TArray<FGameplayAbilitySpecHandle>& OutHandle, bool bAllowRemoteActivation = true);

	UFUNCTION(BlueprintCallable)
	void CancelAciveAbilities(const FGameplayTagContainer WithTags);

protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintAssignable)
	FEndAbilityDelegate OnEndAbility;

private:

	UFUNCTION()
	void NotifyOnEndAbility(const FAbilityEndedData& Data);
};
