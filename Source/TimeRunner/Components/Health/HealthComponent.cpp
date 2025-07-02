// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Health/HealthComponent.h"
#include "AbilitySystem/Attributes/Health/HealthAttributeSet.h"

#include "Characters/BaseCharacter.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	HealthAttribute = CreateDefaultSubobject<UHealthAttributeSet>(TEXT("Health attribute"));
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
	if (Owner.IsValid())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakePlayerDamage);

		Owner->GetAbilitySystemComponent()->AddSpawnedAttribute(HealthAttribute);
		if (InitData)
		{
			HealthAttribute->InitFromMetaDataTable(InitData);
		}
	}
}

void UHealthComponent::TakePlayerDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	// m_CurrentHP = FMath::Clamp(m_CurrentHP - Damage, 0.0f, m_MaxHP);
}

inline float UHealthComponent::GetMaxHP() const
{
	return 0; // HealthAttribute->GetMaxHealth();
}

inline float UHealthComponent::GetCurrentHP() const
{
	return HealthAttribute->GetHealth();
}
