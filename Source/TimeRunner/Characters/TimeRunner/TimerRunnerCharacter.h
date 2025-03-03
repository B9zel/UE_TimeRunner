// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimeRunner/Characters/BaseCharacter.h"
#include "TimerRunnerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class UIntoxicationComponent;
class UHealthComponent;
struct FInputActionInstance;

USTRUCT(BlueprintType)
struct FInput
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	UInputAction* RunAction;
	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	UInputAction* LookActions;
	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	UInputAction* JumpActions;

	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	UInputMappingContext* InputContext;
};

UCLASS()
class TIMERUNNER_API ATimerRunnerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ATimerRunnerCharacter();

public:

	// @return The range of output value is from 0 to 1
	UFUNCTION(BlueprintPure)
	inline float GetRunWorldTime() const;
	// @return The range of output value is from 0 to 1
	UFUNCTION(BlueprintPure)
	inline float GetWalkWorldTime() const;

	inline const FInput& GetInputObject() const;

	inline bool GetIsTimeDilation() const;

	// @param The range of input value is from 0 to 1
	UFUNCTION(BlueprintCallable)
	void SetRunWorldTime(const float NewTime);
	// @param The range of input value is from 0 to 1
	UFUNCTION(BlueprintCallable)
	void SetWalkWorldTime(const float NewTime);

protected:

	// Character method begin
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
												  const FVector& PreviousLocation, float TimeDelta) override;
	// Character method end

	void InputRunTriggerCharacter(const FInputActionInstance& Instance);
	void InputRunStartCharacter(const FInputActionInstance& Instance);
	void InputRunCompletedCharacter(const FInputActionInstance& Instance);
	void InputLookCharacter(const FInputActionInstance& Instance);
	void InputJumpStartCharacter(const FInputActionInstance& Instance);
	void InputJumpCompletedCharacter(const FInputActionInstance& Instance);

	void ApplyTimeDilation();
	void ResetTimeDilation();

private:

	// Delay one frame
	void ApplySettingDilation();
	void Launch(const FVector& Direction, const float ZForce);

protected:

	// Component begin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UHealthComponent* HealthComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UIntoxicationComponent* IntoxicationComponent;
	// Component end

	UPROPERTY(EditAnywhere)
	FInput InputObjects;

private:

	UPROPERTY(EditAnywhere, meta = (Categories = "Time", ClampMin = "0.0", ClampMax = "1.0"))
	float m_RunWorldTime;
	UPROPERTY(EditAnywhere, meta = (Categories = "Time", ClampMin = "0.0", ClampMax = "1.0"))
	float m_WalkWorldTime;
	UPROPERTY(EditAnywhere, meta = (Categories = "Launch", ClampMin = "0.0"))
	float m_ForceLaunch;
	UPROPERTY(EditAnywhere, meta = (Categories = "Launch", ClampMin = "0.0"))
	float m_ZForceLaunch;
	UPROPERTY()
	bool m_IsTimeDilation;
};
