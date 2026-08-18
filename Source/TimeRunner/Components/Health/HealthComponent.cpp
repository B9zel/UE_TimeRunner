// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Health/HealthComponent.h"
#include "AbilitySystem/Attributes/Health/HealthAttributeSet.h"

#include "Characters/BaseCharacter.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::TakeAbilityDamage(AActor* Instigator, const float OldValue, const float NewValue)
{
	// ChangeHealth.Broadcast(Instigator, OldValue, NewValue);
}

void UHealthComponent::OnFullInitOwner()
{
	Owner = GetOwner();
	check(Owner.Get());

	UAbilitySystemComponent* AbilityComponent = nullptr;

	if (auto* AbilityInterface = Cast<IAbilitySystemInterface>(Owner))
	{
		AbilityComponent = AbilityInterface->GetAbilitySystemComponent();
	}
	else if (auto* Component = Owner->FindComponentByClass<UAbilitySystemComponent>())
	{
		AbilityComponent = Component;
	}

	if (!AbilityComponent) return;

	HealthAttribute = AbilityComponent->GetSet<UHealthAttributeSet>();
	check(HealthAttribute);

	HealthAttribute->ChangeHealth.AddUObject(this, &ThisClass::OnChangeHealth);
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
