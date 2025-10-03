// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UHealthAttributeSet;

class ABaseCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TIMERUNNER_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	UHealthComponent();

public:

	virtual void InitializeComponent() override;
	UFUNCTION(BlueprintPure)
	inline float GetMaxHP() const;
	UFUNCTION(BlueprintPure)
	inline float GetCurrentHP() const;

	// UFUNCTION(BlueprintCallable)
	// void SetMaxHP(const float NewMax);
	// UFUNCTION(BlueprintCallable)
	// void SetCurrentHP(const float NewHealt);

protected:

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void TakePlayerDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:

	UPROPERTY(EditAnywhere)
	UDataTable* InitData;

private:

	/*UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float m_MaxHP;
	UPROPERTY()
	float m_CurrentHP;*/
	UPROPERTY()
	TWeakObjectPtr<ABaseCharacter> Owner;

	UPROPERTY()
	TObjectPtr<const UHealthAttributeSet> HealthAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float ScaleRunHealing;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float ScaleIdleHealing;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", Units = "s", ClampMin = "0.001"))
	float RateHealing;
};
