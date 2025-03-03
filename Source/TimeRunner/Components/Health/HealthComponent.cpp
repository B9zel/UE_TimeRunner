// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Health/HealthComponent.h"
#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	m_MaxHP = 100.0f;
	m_CurrentHP = 100.0f;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakePlayerDamage);
	}
}

void UHealthComponent::TakePlayerDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	m_CurrentHP = FMath::Clamp(m_CurrentHP - Damage, 0.0f, m_MaxHP);
}

inline float UHealthComponent::GetMaxHP() const
{
	return m_MaxHP;
}

inline float UHealthComponent::GetCurrentHP() const
{
	return m_CurrentHP;
}