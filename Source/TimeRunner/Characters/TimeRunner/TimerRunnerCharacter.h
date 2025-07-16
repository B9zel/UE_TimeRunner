// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimeRunner/Characters/BaseCharacter.h"
#include "TimerRunnerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class UArrowComponent;
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
	UInputAction* AttackAction;

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
	virtual void PreRegisterAllComponents() override;
	virtual void NotifyJumpApex() override;

protected:

	// Character method begin
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
												  const FVector& PreviousLocation, float TimeDelta) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// Character method end

	void InputRunTriggerCharacter(const FInputActionInstance& Instance);
	void InputRunStartCharacter(const FInputActionInstance& Instance);
	void InputRunCompletedCharacter(const FInputActionInstance& Instance);
	void InputLookCharacter(const FInputActionInstance& Instance);
	void InputJumpStartCharacter(const FInputActionInstance& Instance);
	void InputJumpCompletedCharacter(const FInputActionInstance& Instance);
	void InputSwitchSpeedTriggerCharacter(const FInputActionInstance& Instance);
	void InputCrouchStartCharacter(const FInputActionInstance& Instance);
	void InputAttackStartedCharacter(const FInputActionInstance& Instance);

	void ApplyTimeDilation();
	void ResetTimeDilation();

	void Attack();

	bool CalculateCanRunWall(FHitResult& Res);

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
	TObjectPtr<UStaticMeshComponent> KatanaMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletonMeshUpBody;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowDirectionComponent;

	// Custom begin
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UIntoxicationComponent> IntoxicationComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTimeDilationComponent> DilationComponent;
	// Castom end

	//  Component end
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Input"))
	FInput InputObjects;

protected:

	// Uproperty variables begin
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Launch", ClampMin = "0.0"))
	float ForceLaunch;
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Launch", ClampMin = "0.0"))
	float ZForceLaunch;
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Attack", ClampMin = "0.0"))
	float Damage;
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Attack", ClampMin = "0.0"))
	float DistanceAttack;
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Attack"))
	TEnumAsByte<ECollisionChannel> ChannelAttack;

	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Sockets"))
	FName SocketKatana;

	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|RunWall"))
	float MinWallRunSpeed{10.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "Parameters|RunWall"))
	TArray<TEnumAsByte<EObjectTypeQuery>> TypeChannelTrace;
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|RunWall"))
	float LengthTraceRunWall;

	// Uproperty variables end

	FHitResult ResRight;

	bool CanRunWall{false};
	UPROPERTY(BlueprintReadOnly)
	bool IsWallRunning{false};
	float DirectionRight{0.0f};

private:

	UPROPERTY(EditAnywhere, Category = "Parameters|RunWall|Tag")
	FGameplayTagContainer RunWallTagAbility;

	bool m_IsInputMove;
};
