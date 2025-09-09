// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TimeRunner/Characters/BaseCharacter.h"
#include <Data/Structs/StateBackStorage.h>
#include "Interfaces/StateBackInterface.h"
#include "Global/GlobalTimer/GlobalTimer.h"
#include "Interfaces/WallRunInterface.h"
#include "Interfaces/TimeDilationInterface.h"
#include "TimerRunnerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class UArrowComponent;
class UIntoxicationComponent;
class UHealthComponent;
class UTimeDilationComponent;
class URunWallComponent;
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
	UInputAction* StateBackAction;
	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	UInputAction* DashAction;

	UPROPERTY(EditAnywhere, meta = (Categories = "Input"))
	UInputMappingContext* InputContext;
};

UCLASS()
class TIMERUNNER_API ATimerRunnerCharacter : public ABaseCharacter, public IStateBackInterface, public IWallRunInterface, public ITimeDilationInterface
{
	GENERATED_BODY()

public:

	ATimerRunnerCharacter();

public:

	inline const FInput& GetInputObject() const;
	virtual void PreRegisterAllComponents() override;
	virtual void NotifyJumpApex() override;

	// IStateBackInterface begin
	virtual void SetWasState(const FStateBackStorage& State) override;
	void BPSetWasState_Implementation(const FStateBackStorage State);
	// IStateBackInterface end

	// IWallRunInterface begin
	void StartRunWall_Implementation();
	void StopRunWall_Implementation(const bool DisableCanWall = true);
	float GetTimeActiveRunWall_Implementation();
	// IWallRunInterface end

	// ITimeDilationInterface begin
	void ApplyTimeDilation_Implementation();
	void ResetTimeDilation_Implementation();
	// ITimeDilationInterface end

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
	void InputSafeStateBackTriggerCharacter(const FInputActionInstance& Instance);
	void InputStateBackCompleteCharacter(const FInputActionInstance& Instance);
	void InputDashStartCharacter(const FInputActionInstance& Instance);

	void Attack();

	void ActivateDash();

private:

	bool GetIsInputMove() const;
	void SetIsInputMove(const bool InputMove);

	UFUNCTION()
	void OnChangeLevelOfSpeed(const ELevelSpeed NewSpeed);
	void StopWallRun_Timer();

	UFUNCTION()
	void OnActivetedRunWall();

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<URunWallComponent> RunWallComponent;

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

	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Attack", ClampMin = "0.0"))
	float Damage;
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Attack", ClampMin = "0.0"))
	float DistanceAttack;
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Attack"))
	TEnumAsByte<ECollisionChannel> ChannelAttack;

	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|Sockets"))
	FName SocketKatana;

	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|StateBack"))
	FGameplayTagContainer ApplyStateBackAbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters|Dash", meta = (ClampMin = "0"))
	float DurationDash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters|Dash", meta = (ClampMin = "0"))
	float ForceDash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters|Dash", meta = (ClampMin = "0", Units = "s"))
	float TimeReloadDash;

	UPROPERTY(BlueprintReadWrite, Category = "Parameters|Dash")
	bool IsDashing{false};
	// Uproperty variables end

private:

	UPROPERTY(EditAnywhere, Category = "Parameters|Dash|Tag")
	FGameplayTagContainer DashTagAbility;

	FStateBackStorage StateBack;
	bool IsSafeState{false};
	bool HasHoldSafeStateBackInput{false};

	bool WasDashInAir{false};

	bool m_IsInputMove;
};
