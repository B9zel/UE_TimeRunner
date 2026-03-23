// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/PlayerState/TRPlayerState.h"
#include "AbilitySystem/Component/TRAbilitySystemComponent.h"

ATRPlayerState::ATRPlayerState()
{
	AbilityComponent = CreateDefaultSubobject<UTRAbilitySystemComponent>(TEXT("Ability system component"));
	check(AbilityComponent);
}

void ATRPlayerState::BeginPlay()
{
	Super::BeginPlay();

	AbilityComponent->InitAbilityActorInfo(this, GetPawn());
}

UAbilitySystemComponent* ATRPlayerState::GetAbilitySystemComponent() const
{
	return AbilityComponent;
}
