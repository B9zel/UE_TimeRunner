// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TIMERUNNER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UHealthComponent();

public:

	UFUNCTION(BlueprintPure)
	inline float GetMaxHP() const;
	UFUNCTION(BlueprintPure)
	inline float GetCurrentHP() const;

protected:

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void TakePlayerDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

private:

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float m_MaxHP;
	UPROPERTY()
	float m_CurrentHP;
};
