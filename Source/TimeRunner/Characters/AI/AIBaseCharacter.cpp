// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AI/AIBaseCharacter.h"
#include "Components/Health/HealthComponent.h"

AAIBaseCharacter::AAIBaseCharacter()
{
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	check(HealthComponent);
}

void AAIBaseCharacter::StopAttackCharacter_Implementation()
{
}

void AAIBaseCharacter::AttackCharacter_Implementation()
{
}
