// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "AIBaseCharacter.generated.h"

class UHealthComponent;
class UDamageAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackDelegate);

UCLASS()
class TIMERUNNER_API AAIBaseCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AAIBaseCharacter();

public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AttackCharacter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StopAttackCharacter();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsCooldown() const;

protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAttackDelegate StopAttack;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAttackDelegate StartCooldown;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAttackDelegate EndCooldown;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBayondTheFence{false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "Parameters|Attack", ClampMin = "0.0"))
	TObjectPtr<const UDamageAttributeSet> Damage;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTRAbilitySystemComponent> AbilityComponent;
};
