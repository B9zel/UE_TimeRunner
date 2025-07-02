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
class UTimeDilationComponent;
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
	UInputAction* SwitchSpeedActions;
	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	UInputAction* CrouchActions;

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

	inline const FInput& GetInputObject() const;

protected:

	// Character method begin
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
												  const FVector& PreviousLocation, float TimeDelta) override;
	virtual void BeginPlay() override;
	// Character method end

	void InputRunTriggerCharacter(const FInputActionInstance& Instance);
	void InputRunStartCharacter(const FInputActionInstance& Instance);
	void InputRunCompletedCharacter(const FInputActionInstance& Instance);
	void InputLookCharacter(const FInputActionInstance& Instance);
	void InputJumpStartCharacter(const FInputActionInstance& Instance);
	void InputJumpCompletedCharacter(const FInputActionInstance& Instance);
	void InputSwitchSpeedTriggerCharacter(const FInputActionInstance& Instance);
	void InputCrouchStartCharacter(const FInputActionInstance& Instance);

	void ApplyTimeDilation();
	void ResetTimeDilation();

private:

	bool GetIsInputMove() const;
	void SetIsInputMove(const bool InputMove);

	void Launch(const FVector& Direction, const float ZForce);

	UFUNCTION()
	void OnChangeLevelOfSpeed(const ELevelSpeed NewSpeed);

protected:

	// Component begin;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> CameraComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UIntoxicationComponent> IntoxicationComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTimeDilationComponent> DilationComponent;
	//  Component end

	UPROPERTY(EditAnywhere)
	FInput InputObjects;

private:

	UPROPERTY(EditAnywhere, meta = (Categories = "Launch", ClampMin = "0.0"))
	float m_ForceLaunch;
	UPROPERTY(EditAnywhere, meta = (Categories = "Launch", ClampMin = "0.0"))
	float m_ZForceLaunch;

	bool m_IsInputMove;
};
