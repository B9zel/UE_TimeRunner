// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Health/HealthComponent.h"
#include "AbilitySystem/Attributes/Health/HealthAttributeSet.h"

#include "Characters/BaseCharacter.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (Owner.IsValid())
	{
	}
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner<ABaseCharacter>();
	check(Owner.Get());

	Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakePlayerDamage);

	HealthAttribute = Owner->GetAbilitySystemComponent()->GetSet<UHealthAttributeSet>();
	check(HealthAttribute);

	HealthAttribute->ChangeHealth.AddUObject(this, &ThisClass::OnChangeHealth);

}

void UHealthComponent::TakeAbilityDamage(AActor* Instigator, const float OldValue, const float NewValue)
{
	ChangeHealth.Broadcast(Instigator, OldValue, NewValue);
}

void UHealthComponent::TakePlayerDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	// m_CurrentHP = FMath::Clamp(m_CurrentHP - Damage, 0.0f, m_MaxHP);
}

void UHealthComponent::OnChangeHealth(AActor* Instigator, const float OldValue, const float NewValue)
{
	ChangeHealth.Broadcast(Instigator, OldValue, NewValue);
	if (FMath::IsNearlyZero(NewValue))
	{
		Dead.Broadcast(Instigator);
	}
}

inline float UHealthComponent::GetMaxHP() const
{
	return HealthAttribute->GetMaxHealth();
}

inline float UHealthComponent::GetCurrentHP() const
{
	return HealthAttribute->GetHealth();
}
