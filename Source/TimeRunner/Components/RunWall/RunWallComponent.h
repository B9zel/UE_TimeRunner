// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RunWallComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FToggleRunWall);

class UAbilitySystemComponent;
class UArrowComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TIMERUNNER_API URunWallComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	// Sets default values for this component's properties
	URunWallComponent();

public:

	// Character override
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//

	void StartRunWall();
	void StopRunWall(const bool DisableCanWall);

	UFUNCTION(BlueprintPure)
	bool GetIsWallRunning() const;

	UFUNCTION(BlueprintPure)
	bool GetCanWallRun() const;
	UFUNCTION(BlueprintPure)
	bool GetWasJumpOffWall() const;

	UFUNCTION(BlueprintPure)
	float GetTimeWallRun() const;

	void JumpOffWall();
	// @param Direcion must be normalized
	void SetCurrentDirectionTrace(const float Direction);

	void SetArrowComponent(const UArrowComponent* Arrow);

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

private:

	bool CalculateCanRunWall(FHitResult& Res);
	void Launch(const FVector& Direction, const float DirectionForce, const float ZForce);

	UFUNCTION()
	void LandedOwnerCharacter(const FHitResult& Hit);

	void DeactivateRunWall();
	void ActivateRunWall();

public:

	FToggleRunWall ActivateRunWallDispatcher;
	FToggleRunWall DeactivateRunWallDispatcher;

private:

	UPROPERTY()
	TWeakObjectPtr<ACharacter> OwnerCharacter;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<const UArrowComponent> ArrowOwnerComponent;
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAbilitySystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "Parameters|RunWall", AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> TypeChannelTrace;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Category = "Parameters|RunWall", ClampMin = "0.0", Units = "s", AllowPrivateAccess = "true"))
	float TimeWallRun{0.0f};

	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|RunWall", ClampMin = "0.0"))
	float MinWallRunSpeed{10.0f};
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|RunWall|Jump", ClampMin = "0.0"))
	float JumpFromWallForce{0.0f};
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|RunWall|Jump", ClampMin = "0.0"))
	float ZJumpFromWallForce{0.0f};
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|RunWall|Jump", ClampMin = "0.0"))
	float PercentDirectlyJumpOffWall{0.6f};

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsWallRunning{false};
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool CanRunWall{false};

	UPROPERTY(EditAnywhere, Category = "Parameters|RunWall|Tag")
	FGameplayTagContainer RunWallTagAbility;
	UPROPERTY(EditAnywhere, meta = (Category = "Parameters|RunWall"))
	float LengthTraceRunWall;

	FHitResult WallTrace;
	bool WasJumpOffWall{false};
	bool IsTouchWall{false};
	float DirectionTrace{0.0f};
};
