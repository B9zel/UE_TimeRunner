// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Intoxication/IntoxicationComponent.h"
#include "AbilitySystem/Attributes/Intoxication/IntoxicationAttributeSet.h"
#include "Characters/BaseCharacter.h"
#include "AttributeSet.h"
#include <Kismet/GameplayStatics.h>
#include <GameplayTask.h>

UIntoxicationComponent::UIntoxicationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxIntoxication = 100.0f;

	RateDecrease = 0.0f;
	RateIncrease = 0.0f;

	SpeedDecrease = 1.0f;
	SpeedIncrease = 1.0f;
}

void UIntoxicationComponent::ActivateIntixication()
{
	IncreaseIntoxication();
	/*bool res = OwnerCharacter->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(AbilityIncreaseIntoxication.GetSingleTagContainer());
	 */
	// UE_LOG(LogTemp, Display, TEXT("%s"), res ? TEXT("true") : TEXT("false"));

	/*auto& TimerManager = GetWorld()->GetTimerManager();

	if (m_IncreaseTimer.IsValid()) return;

	TimerManager.ClearTimer(m_DecreaseTimer);
	TimerManager.SetTimer(m_IncreaseTimer, this, &UIntoxicationComponent::IncreaseIntoxication,
						  m_RateIncrease * UGameplayStatics::GetGlobalTimeDilation(GetWorld()), true);*/
}

void UIntoxicationComponent::DeactivateIntixication()
{
	DecreaseIntoxication();

	/*const auto& Tag = AbilityIncreaseIntoxication.GetSingleTagContainer();
	OwnerCharacter->GetAbilitySystemComponent()->CancelAbilities(&Tag);*/
	/*auto& TimerManager = GetWorld()->GetTimerManager();

	if (m_DecreaseTimer.IsValid()) return;

	TimerManager.ClearTimer(m_IncreaseTimer);
	TimerManager.SetTimer(m_DecreaseTimer, this, &UIntoxicationComponent::DecreaseIntoxication,
						  m_RateDecrease * UGameplayStatics::GetGlobalTimeDilation(GetWorld()), true);*/
}

void UIntoxicationComponent::RestartIntoxication()
{
	//	auto& TimerManager = GetWorld()->GetTimerManager();
	//	TimerManager.ClearTimer(m_DecreaseTimer);
	//	TimerManager.ClearTimer(m_IncreaseTimer);

	//	TimerManager.SetTimer(m_IncreaseTimer, this, &UIntoxicationComponent::IncreaseIntoxication,
	//						  RateIncrease * UGameplayStatics::GetGlobalTimeDilation(GetWorld()), true);
}

void UIntoxicationComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = GetOwner<ABaseCharacter>();
	check(OwnerCharacter);

	IntoxicationAttribute = OwnerCharacter->GetAbilitySystemComponent()->GetSet<UIntoxicationAttributeSet>();
	check(IntoxicationAttribute);

	ConstCast(IntoxicationAttribute)->ChangeAttribute.AddUObject(this, &UIntoxicationComponent::ChangeIntoxication);
}

void UIntoxicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UIntoxicationComponent::IncreaseIntoxication()
{
	if (AbilityIncreaseIntoxication.IsValid())
	{
		OwnerCharacter->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(AbilityIncreaseIntoxication.GetSingleTagContainer());
	}
}

void UIntoxicationComponent::DecreaseIntoxication()
{
	if (AbilityDecreaseIntoxication.IsValid())
	{
		OwnerCharacter->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(AbilityDecreaseIntoxication.GetSingleTagContainer());
	}
}

void UIntoxicationComponent::ApplyIntoxicationDamage()
{
	UGameplayStatics::ApplyDamage(GetOwner(), Damage, GetOwner()->GetInstigatorController(), nullptr, TSubclassOf<UDamageType>());
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
	if (RealTime - m_LastRealTime >= RateApplyDamage)
	{
		ApplyIntoxicationDamage();
		m_LastRealTime = RealTime;
	}
}

void UIntoxicationComponent::StopApplyDamageTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(m_ApplyDamageTimer);
}

void UIntoxicationComponent::ChangeIntoxication(const FGameplayAttribute& Attribute, const float NewValue)
{
	if (Attribute == IntoxicationAttribute->GetIntoxicationAttribute())
	{
		if (NewValue >= SafeBorder * GetMaxIntoxication())
		{
			OwnerCharacter->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(AbilityIntoxicationDamage);
		}
		else
		{
			OwnerCharacter->GetAbilitySystemComponent()->CancelAbilities(&AbilityIntoxicationDamage);
		}
	}
}

float UIntoxicationComponent::GetMaxIntoxication() const
{
	return IntoxicationAttribute->GetMaxIntoxication();
}

float UIntoxicationComponent::GetCurrentIntoxication() const
{
	return IntoxicationAttribute->GetIntoxication();
}

float UIntoxicationComponent::GetRateIncrease() const
{
	return RateIncrease;
}

float UIntoxicationComponent::GetRateDecrease() const
{
	return RateDecrease;
}

float UIntoxicationComponent::GetSpeedIncrease() const
{
	return SpeedIncrease;
}

float UIntoxicationComponent::GetSpeedDecrease() const
{
	return SpeedDecrease;
}

float UIntoxicationComponent::GetIntoxicationDamage() const
{
	return Damage;
}

float UIntoxicationComponent::GetSafeBorder() const
{
	return SafeBorder;
}

float UIntoxicationComponent::GetRateApplyDamage() const
{
	return RateApplyDamage;
}

void UIntoxicationComponent::SetMaxIntoxication(const float NewMax)
{
	if (NewMax < 1.0f)
	{
		MaxIntoxication = 1.0f;
		return;
	}
	MaxIntoxication = NewMax;
}

void UIntoxicationComponent::SetRateIncrease(const float NewRate)
{
	if (NewRate <= UE_KINDA_SMALL_NUMBER)
	{
		RateIncrease = UE_KINDA_SMALL_NUMBER;
		return;
	}
	RateIncrease = NewRate;
}

void UIntoxicationComponent::SetRateDecrease(const float NewRate)
{
	if (NewRate <= UE_KINDA_SMALL_NUMBER)
	{
		RateDecrease = UE_KINDA_SMALL_NUMBER;
		return;
	}
	RateDecrease = NewRate;
}

void UIntoxicationComponent::SetSpeedIncreaseIntoxication(const float NewSpeed)
{
	if (NewSpeed <= 0.0f)
	{
		SpeedIncrease = 0.0f;
		return;
	}
	SpeedIncrease = NewSpeed;
}

void UIntoxicationComponent::SetSpeedDecreaseIntoxication(const float NewSpeed)
{
	if (NewSpeed <= 0.0f)
	{
		SpeedDecrease = 0.0f;
		return;
	}
	SpeedDecrease = NewSpeed;
}

void UIntoxicationComponent::SetIntoxicationDamage(const float NewDamage)
{
	if (NewDamage <= 0.0f)
	{
		Damage = 0.0f;
		return;
	}
	Damage = NewDamage;
}

void UIntoxicationComponent::SetSafeBorder(const float NewBorder)
{
	SafeBorder = FMath::Clamp(NewBorder, 0.0f, 1.0f);
}

void UIntoxicationComponent::SetRateApplyDamage(const float NewRate)
{
	if (NewRate <= UE_KINDA_SMALL_NUMBER)
	{
		RateApplyDamage = UE_KINDA_SMALL_NUMBER;
		return;
	}
	RateApplyDamage = NewRate;
}

const TObjectPtr<const UIntoxicationAttributeSet>& UIntoxicationComponent::GetIntoxicationAttribute() const
{
	return IntoxicationAttribute;
}