// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "AIBaseCharacter.generated.h"

class UHealthComponent;

UCLASS()
class TIMERUNNER_API AAIBaseCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AAIBaseCharacter();

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AttackCharacter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StopAttackCharacter();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;
};
