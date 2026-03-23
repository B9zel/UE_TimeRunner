// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AI/AIBaseCharacter.h"
#include "Components/Health/HealthComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Component/TRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/Damage/DamageAttributeSet.h"

AAIBaseCharacter::AAIBaseCharacter()
{
	AbilityComponent = CreateDefaultSubobject<UTRAbilitySystemComponent>(TEXT("Ability system component"));
	check(AbilityComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	check(HealthComponent);
}

void AAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	Damage = GetAbilitySystemComponent()->GetSet<UDamageAttributeSet>();
	check(Damage);

	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
}

UAbilitySystemComponent* AAIBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilityComponent;
}

void AAIBaseCharacter::StopAttackCharacter_Implementation()
{
}

void AAIBaseCharacter::AttackCharacter_Implementation()
{
}
