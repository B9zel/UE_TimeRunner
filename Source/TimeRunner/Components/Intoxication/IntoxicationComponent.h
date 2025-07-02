// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "IntoxicationComponent.generated.h"

class UIntoxicationAttributeSet;
class ABaseCharacter;
struct FGameplayAttribute;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TIMERUNNER_API UIntoxicationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UIntoxicationComponent();

public:

	UFUNCTION(BlueprintCallable)
	void ActivateIntixication();
	UFUNCTION(BlueprintCallable)
	void DeactivateIntixication();
	UFUNCTION(BlueprintCallable)
	void RestartIntoxication();

	const TObjectPtr<UIntoxicationAttributeSet>& GetIntoxicationAttribute() const;

	UFUNCTION(BlueprintPure)
	float GetMaxIntoxication() const;
	UFUNCTION(BlueprintPure)
	float GetCurrentIntoxication() const;
	UFUNCTION(BlueprintPure)
	float GetRateIncrease() const;
	UFUNCTION(BlueprintPure)
	float GetRateDecrease() const;
	UFUNCTION(BlueprintPure)
	float GetSpeedIncrease() const;
	UFUNCTION(BlueprintPure)
	float GetSpeedDecrease() const;
	UFUNCTION(BlueprintPure)
	float GetIntoxicationDamage() const;
	UFUNCTION(BlueprintPure)
	float GetSafeBorder() const;
	UFUNCTION(BlueprintPure)
	float GetRateApplyDamage() const;

	UFUNCTION(BlueprintCallable)
	void SetMaxIntoxication(const float NewMax);
	UFUNCTION(BlueprintCallable)
	void SetRateIncrease(const float NewRate);
	UFUNCTION(BlueprintCallable)
	void SetRateDecrease(const float NewRate);
	UFUNCTION(BlueprintCallable)
	void SetSpeedIncreaseIntoxication(const float NewSpeed);
	UFUNCTION(BlueprintCallable)
	void SetSpeedDecreaseIntoxication(const float NewSpeed);
	UFUNCTION(BlueprintCallable)
	void SetIntoxicationDamage(const float NewDamage);
	// @param NewBorder: input range from 0 to 1
	UFUNCTION(BlueprintCallable)
	void SetSafeBorder(const float NewBorder);
	UFUNCTION(BlueprintCallable)
	void SetRateApplyDamage(const float NewRate);

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UFUNCTION()
	void IncreaseIntoxication();
	UFUNCTION()
	void DecreaseIntoxication();
	UFUNCTION()
	void ApplyIntoxicationDamage();

	UFUNCTION()
	void StartApplyDamageTimer();
	UFUNCTION()
	void CheckApplyDamageTimer();
	UFUNCTION()
	void StopApplyDamageTimer();

	UFUNCTION()
	void ChangeIntoxication(const FGameplayAttribute& Attribute, const float NewValue);

private:

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1.0"))
	float MaxIntoxication;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float RateIncrease;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float RateDecrease;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float RateApplyDamage;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float Damage;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SafeBorder; // if m_CurrentIntoxication more, apply damage

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float SpeedIncrease;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float SpeedDecrease;

	double m_LastRealTime;

	UPROPERTY()
	TObjectPtr<UIntoxicationAttributeSet> IntoxicationAttribute;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters|Data")
	TObjectPtr<UDataTable> IntoxicationConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Parameters|Tag|Ability")
	FGameplayTagContainer AbilityIntoxicationDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Parameters|Tag|Ability")
	FGameplayTag AbilityIncreaseIntoxication;
	UPROPERTY(EditDefaultsOnly, Category = "Parameters|Tag|Ability")
	FGameplayTag AbilityDecreaseIntoxication;
	UPROPERTY(EditDefaultsOnly, Category = "Parameters|Tag|Effect")
	FGameplayTag EffectIntoxicationDamage;

	TObjectPtr<ABaseCharacter> OwnerCharacter;

	FTimerHandle m_DecreaseTimer;
	FTimerHandle m_IncreaseTimer;
	FTimerHandle m_ApplyDamageTimer;
};
