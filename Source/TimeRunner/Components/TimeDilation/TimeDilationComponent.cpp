// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TimeDilation/TimeDilationComponent.h"
#include "Characters/BaseCharacter.h"
#include <AbilitySystemComponent.h>
#include "TimeRunner/Data/DataAssets/LevelsOfSpeedDataAsset.h"

UTimeDilationComponent::UTimeDilationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	m_IsTimeDilation = false;
	m_CurrentSpeed = 0;
}

void UTimeDilationComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = GetOwner<ABaseCharacter>();
	check(OwnerCharacter);

	// clang-format off
	float SlowSpeed		= 1.0f;
	float MidSpeed		= 2.0f;
	float FastSpeed		= 4.0f;
	float VeryFastSpeed = 8.0f;
	if (LevelSpeedsConfig)
	{
		SlowSpeed		= LevelSpeedsConfig->GetSlowLevel();
		MidSpeed		= LevelSpeedsConfig->GetMiddleLevel();
		FastSpeed		= LevelSpeedsConfig->GetFastLevel();
		VeryFastSpeed	= LevelSpeedsConfig->GetVeryFastLevel();
	}
	
	m_Speeds.Add(ELevelSpeed::SLOW,		 MakeUnique<LowSpeed>(SlowSpeed));
	m_Speeds.Add(ELevelSpeed::MIDDLE,	 MakeUnique<MiddleSpeed>(MidSpeed));
	m_Speeds.Add(ELevelSpeed::FAST,		 MakeUnique<HighSpeed>(FastSpeed));
	m_Speeds.Add(ELevelSpeed::VERY_FAST, MakeUnique<VeryHighSpeed>(VeryFastSpeed));
	// clang-format on
}

void UTimeDilationComponent::EnableTimeDilation()
{
	if (DilationAbilityTag.IsValid())
	{
		OwnerCharacter->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(DilationAbilityTag.GetSingleTagContainer());
		ChanageSpeedDelegate.Broadcast(GetEnumCurrentSpeed());
		m_IsTimeDilation = true;
	}
}

void UTimeDilationComponent::DisableTimeDilation()
{
	if (DilationAbilityTag.IsValid())
	{
		const auto& ConteinerTag = DilationAbilityTag.GetSingleTagContainer();
		OwnerCharacter->GetAbilitySystemComponent()->CancelAbilities(&ConteinerTag);
		m_IsTimeDilation = false;
	}
}

void UTimeDilationComponent::IncreaseSpeed()
{
	if (!m_Speeds.Num() || GetCurrentSpeed() == m_Speeds.Num() - 1) return;

	if (DilationAbilityTag.IsValid())
	{
		++m_CurrentSpeed;
		if (GetIsTimeDilation())
		{
			EnableTimeDilation();
		}
	}
}

void UTimeDilationComponent::DecreaseSpeed()
{
	if (!m_Speeds.Num() || !GetCurrentSpeed()) return;

	if (DilationAbilityTag.IsValid())
	{
		--m_CurrentSpeed;
		if (GetIsTimeDilation())
		{
			EnableTimeDilation();
		}
	}
}

inline ACharacter* UTimeDilationComponent::GetOwnerCharacter() const
{
	return OwnerCharacter.Get();
}

inline const ELevelSpeed UTimeDilationComponent::GetEnumCurrentSpeed() const
{
	return static_cast<ELevelSpeed>(GetCurrentSpeed());
}

inline const uint32 UTimeDilationComponent::GetCurrentSpeed() const
{
	check(m_Speeds.Num() != 0);
	return m_CurrentSpeed % m_Speeds.Num();
}

inline float UTimeDilationComponent::GetRunWorldTime() const
{
	auto* SpeedVal = m_Speeds.Find(GetEnumCurrentSpeed());
	return m_RunWorldTime * (SpeedVal->IsValid() ? SpeedVal->Get()->GetSpeedActor() : 1.0f);
}

inline float UTimeDilationComponent::GetWalkWorldTime() const
{
	return m_WalkWorldTime;
}

inline bool UTimeDilationComponent::GetIsTimeDilation() const
{
	return m_IsTimeDilation;
}

float UTimeDilationComponent::GetCurrentDilationByEnum(const ELevelSpeed Speed) const
{
	auto* Dilation = m_Speeds.Find(Speed);
	return Dilation != nullptr ? Dilation->Get()->GetSpeedActor() : -1.0f;
}

void UTimeDilationComponent::SetRunWorldTime(const float NewTime)
{
	m_RunWorldTime = FMath::Clamp(NewTime, 0.0f, 1.0f);
}

void UTimeDilationComponent::SetWalkWorldTime(const float NewTime)
{
	m_WalkWorldTime = FMath::Clamp(NewTime, 0.0f, 1.0f);
}

float UTimeDilationComponent::GetActorDilationByEnum(const ELevelSpeed Speed) const
{
	const float CurrentDilation = FMath::Max(UGameplayStatics::GetGlobalTimeDilation(this), UE_SMALL_NUMBER);
	const float ResSpeed = m_Speeds[Speed]->GetSpeedActor();

	return (1.0f / CurrentDilation) * ResSpeed;
}
