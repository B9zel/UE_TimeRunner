// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Intoxication/IntoxicationComponent.h"
#include <Kismet/GameplayStatics.h>

UIntoxicationComponent::UIntoxicationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	m_CurrentIntoxication = 0.0f;
	m_MaxIntoxication = 100.0f;

	m_RateDecrease = 0.0f;
	m_RateIncrease = 0.0f;

	m_SpeedDecrease = 1.0f;
	m_SpeedIncrease = 1.0f;
}

void UIntoxicationComponent::ActivateIntixication(const float DilationForComponent)
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	if (m_IncreaseTimer.IsValid()) return;

	TimerManager.ClearTimer(m_DecreaseTimer);
	TimerManager.SetTimer(m_IncreaseTimer, this, &UIntoxicationComponent::IncreaseIntoxication,
						  m_RateIncrease * UGameplayStatics::GetGlobalTimeDilation(GetWorld()), true);
}

void UIntoxicationComponent::DeactivateIntixication(const float DilationForComponent)
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	if (m_DecreaseTimer.IsValid()) return;

	TimerManager.ClearTimer(m_IncreaseTimer);
	TimerManager.SetTimer(m_DecreaseTimer, this, &UIntoxicationComponent::DecreaseIntoxication,
						  m_RateDecrease * UGameplayStatics::GetGlobalTimeDilation(GetWorld()), true);
}

void UIntoxicationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UIntoxicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UIntoxicationComponent::IncreaseIntoxication()
{
	if (GetCurrentIntoxication() / GetMaxIntoxication() >= m_SafeBorder && !m_ApplyDamageTimer.IsValid())
	{
		StartApplyDamageTimer();
	}
	if (GetCurrentIntoxication() >= GetMaxIntoxication())
	{
		GetWorld()->GetTimerManager().ClearTimer(m_IncreaseTimer);
	}

	m_CurrentIntoxication = FMath::Clamp(m_CurrentIntoxication + m_SpeedIncrease, 0.0f, m_MaxIntoxication);
}

void UIntoxicationComponent::DecreaseIntoxication()
{
	if (GetCurrentIntoxication() / GetMaxIntoxication() < m_SafeBorder && m_ApplyDamageTimer.IsValid())
	{
		StopApplyDamageTimer();
	}
	if (GetCurrentIntoxication() <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(m_DecreaseTimer);
	}
	m_CurrentIntoxication = FMath::Clamp(m_CurrentIntoxication - m_SpeedDecrease, 0.0f, m_MaxIntoxication);
}

void UIntoxicationComponent::ApplyIntoxicationDamage()
{
	TSubclassOf<UDamageType> TypeDamage;
	UGameplayStatics::ApplyDamage(GetOwner(), m_Damage, GetOwner()->GetInstigatorController(), nullptr, TypeDamage);
}

void UIntoxicationComponent::StartApplyDamageTimer()
{
	auto& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(m_ApplyDamageTimer);
	TimerManager.SetTimer(m_ApplyDamageTimer, this, &UIntoxicationComponent::CheckApplyDamageTimer, UE_KINDA_SMALL_NUMBER, true);
	m_LastRealTime = FApp::GetCurrentTime();
}

void UIntoxicationComponent::CheckApplyDamageTimer()
{
	const double RealTime = FApp::GetCurrentTime();
	if (RealTime - m_LastRealTime >= m_RateApplyDamage)
	{
		ApplyIntoxicationDamage();
		m_LastRealTime = RealTime;
	}
}

void UIntoxicationComponent::StopApplyDamageTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(m_ApplyDamageTimer);
}

float UIntoxicationComponent::GetCurrentIntoxication() const
{
	return m_CurrentIntoxication;
}

float UIntoxicationComponent::GetMaxIntoxication() const
{
	return m_MaxIntoxication;
}

float UIntoxicationComponent::GetRateIncrease() const
{
	return m_RateIncrease;
}

float UIntoxicationComponent::GetRateDecrease() const
{
	return m_RateDecrease;
}

float UIntoxicationComponent::GetSpeedIncrease() const
{
	return m_SpeedIncrease;
}

float UIntoxicationComponent::GetSpeedDecrease() const
{
	return m_SpeedDecrease;
}

void UIntoxicationComponent::SetCurrentIntoxication(const float NewIntoxication)
{
	m_CurrentIntoxication = FMath::Clamp(NewIntoxication, 0.0, GetMaxIntoxication());
}

void UIntoxicationComponent::SetMaxIntoxication(const float NewMax)
{
	if (NewMax < 0.0f)
	{
		m_MaxIntoxication = 0.0f;
		return;
	}
	m_MaxIntoxication = NewMax;
}

void UIntoxicationComponent::SetRateIncrease(const float NewRate)
{
	if (NewRate <= UE_KINDA_SMALL_NUMBER)
	{
		m_RateIncrease = UE_KINDA_SMALL_NUMBER;
	}
	m_RateIncrease = NewRate;
}

void UIntoxicationComponent::SetRateDecrease(const float NewRate)
{
	if (NewRate <= UE_KINDA_SMALL_NUMBER)
	{
		m_RateDecrease = UE_KINDA_SMALL_NUMBER;
	}
	m_RateDecrease = NewRate;
}

void UIntoxicationComponent::SetSpeedIncreaseIntoxication(const float NewSpeed)
{
	if (NewSpeed <= 0.0f)
	{
		m_SpeedIncrease = 0.0f;
	}
	m_SpeedIncrease = NewSpeed;
}

void UIntoxicationComponent::SetSpeedDecreaseIntoxication(const float NewSpeed)
{
	if (NewSpeed <= 0.0f)
	{
		m_SpeedDecrease = 0.0f;
	}
	m_SpeedDecrease = NewSpeed;
}
