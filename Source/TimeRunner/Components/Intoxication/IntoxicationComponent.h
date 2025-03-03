// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IntoxicationComponent.generated.h"

struct AbsoluteTimer
{
	FTimerHandle Timer;
	double m_LastRealTime;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TIMERUNNER_API UIntoxicationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UIntoxicationComponent();

public:

	UFUNCTION(BlueprintCallable)
	void ActivateIntixication(const float DilationForComponent = 1.0f);
	UFUNCTION(BlueprintCallable)
	void DeactivateIntixication(const float DilationForComponent = 1.0f);

	UFUNCTION(BlueprintPure)
	float GetCurrentIntoxication() const;
	UFUNCTION(BlueprintPure)
	float GetMaxIntoxication() const;
	UFUNCTION(BlueprintPure)
	float GetRateIncrease() const;
	UFUNCTION(BlueprintPure)
	float GetRateDecrease() const;
	UFUNCTION(BlueprintPure)
	float GetSpeedIncrease() const;
	UFUNCTION(BlueprintPure)
	float GetSpeedDecrease() const;

	UFUNCTION(BlueprintCallable)
	void SetCurrentIntoxication(const float NewIntoxication);
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

private:

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float m_MaxIntoxication;
	UPROPERTY()
	float m_CurrentIntoxication;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float m_RateIncrease;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float m_RateDecrease;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float m_RateApplyDamage;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float m_Damage;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float m_SafeBorder; // if m_CurrentIntoxication more, apply damage

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float m_SpeedIncrease;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float m_SpeedDecrease;

	double m_LastRealTime;

	FTimerHandle m_DecreaseTimer;
	FTimerHandle m_IncreaseTimer;
	FTimerHandle m_ApplyDamageTimer;
};
