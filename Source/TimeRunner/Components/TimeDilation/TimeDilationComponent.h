// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Kismet/GameplayStatics.h>
#include <GameplayTagContainer.h>
#include "TimeDilationComponent.generated.h"

class AActor;
class ABaseCharacter;
class ULevelsOfSpeedDataAsset;

UENUM()
enum class ELevelSpeed : uint8
{
	SLOW = 0,
	MIDDLE,
	FAST,
	VERY_FAST,

	MAX_ELEMENT
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeSpeed, const ELevelSpeed, NewSpeed);

// The higher level speed, the less effect of time dilation
class BaseSpeed
{
protected:

	BaseSpeed() = default;

public:

	virtual void UseWorldDilation(AActor* ChangeableDilationActor, const float BaseTimeDilation)
	{
		UGameplayStatics::SetGlobalTimeDilation(ChangeableDilationActor, BaseTimeDilation * SpeedActor);
	}

	virtual void UseActorDilation(AActor* ChangeableDilationActor, const float BaseTimeDilation)
	{
		const float CurrentDilation = UGameplayStatics::GetGlobalTimeDilation(ChangeableDilationActor);
		ChangeableDilationActor->CustomTimeDilation = 1.0f / (CurrentDilation <= 0.0f ? UE_SMALL_NUMBER : CurrentDilation) * SpeedActor;
	}

	float GetSpeedActor() const
	{
		return SpeedActor;
	}

protected:

	float SpeedActor;
};

class LowSpeed : public BaseSpeed
{
public:

	LowSpeed(const float Dilation = 1.0f)
	{
		SpeedActor = Dilation;
	}
};

class MiddleSpeed : public BaseSpeed
{
public:

	MiddleSpeed(const float Dilation = 2.0f)
	{
		SpeedActor = Dilation;
	}
};

class HighSpeed : public BaseSpeed
{
public:

	HighSpeed(const float Dilation = 4.0f)
	{
		SpeedActor = Dilation;
	}
};

class VeryHighSpeed : public BaseSpeed
{
public:

	VeryHighSpeed(const float Dilation = 8.0f)
	{
		SpeedActor = Dilation;
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TIMERUNNER_API UTimeDilationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UTimeDilationComponent();

protected:

	virtual void BeginPlay() override;

public:

	// @return The range of output value is from 0 to 1
	UFUNCTION(BlueprintPure)
	inline float GetRunWorldTime() const;
	// @return The range of output value is from 0 to 1
	UFUNCTION(BlueprintPure)
	inline float GetWalkWorldTime() const;

	inline bool GetIsTimeDilation() const;
	// @return Current dilation id it doesn't exist then -1.0f
	UFUNCTION(BlueprintPure)
	float GetCurrentDilationByEnum(const ELevelSpeed Speed) const;

	// @param The range of input value is from 0 to 1
	UFUNCTION(BlueprintCallable)
	void SetRunWorldTime(const float NewTime);
	// @param The range of input value is from 0 to 1
	UFUNCTION(BlueprintCallable)
	void SetWalkWorldTime(const float NewTime);
	UFUNCTION(BlueprintPure)
	float GetActorDilationByEnum(const ELevelSpeed Speed) const;

	void EnableTimeDilation();
	void DisableTimeDilation();

	void IncreaseSpeed();
	void DecreaseSpeed();

protected:

	UFUNCTION(BlueprintPure)
	const ELevelSpeed GetEnumCurrentSpeed() const;

private:

	// Delay one frame
	void ApplySettingDilation();

	inline ACharacter* GetOwnerCharacter() const;
	inline const uint32 GetCurrentSpeed() const;

public:

	FChangeSpeed ChanageSpeedDelegate;

protected:

	UPROPERTY(EditAnywhere)
	ULevelsOfSpeedDataAsset* LevelSpeedsConfig;

private:

	UPROPERTY(EditAnywhere, meta = (Categories = "Time", ClampMin = "0.0", ClampMax = "1.0"))
	float m_RunWorldTime;
	UPROPERTY(EditAnywhere, meta = (Categories = "Time", ClampMin = "0.0", ClampMax = "1.0"))
	float m_WalkWorldTime;
	UPROPERTY()
	bool m_IsTimeDilation;

	TMap<ELevelSpeed, TUniquePtr<BaseSpeed>> m_Speeds;
	uint32 m_CurrentSpeed;

	UPROPERTY(EditAnywhere)
	FGameplayTag DilationAbilityTag;

	TObjectPtr<ABaseCharacter> OwnerCharacter;
};
