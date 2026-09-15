// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BaseComponent.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UBaseComponent::UBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	if (auto* BaseCharacter = GetOwner<ABaseCharacter>())
	{
		BaseCharacter->GetAbilitySystemInitializedDelegate().AddUObject(this, &ThisClass::TryInitializeOwner);
		if (BaseCharacter->IsAbilitySystemInitialized())
		{
			TryInitializeOwner();
		}
		return;
	}

	auto* Owner = GetOwner<APawn>();
	// GetWorld()->OnWorldMatchStarting.AddUObject(this, &ThisClass::OnFullInitOwner);
	if (IsValid(Owner))
	{
		if (Owner->GetController())
		{
			TryInitializeOwner();
		}
		else
		{
			Owner->ReceiveControllerChangedDelegate.AddDynamic(this, &ThisClass::OnPossessedOwner);
		}
	}
	else if (IsValid(GetOwner()))
	{
		TryInitializeOwner();
	}
}

void UBaseComponent::OnFullInitOwner()
{
}

void UBaseComponent::TryInitializeOwner()
{
	if (bOwnerInitialized) return;

	bOwnerInitialized = true;
	OnFullInitOwner();
}

void UBaseComponent::OnPlayBeginOwner(APawn* Owner)
{
	TryInitializeOwner();
}

void UBaseComponent::OnPossessedOwner(APawn* Pawn, AController* OldController, AController* NewController)
{
	if (NewController)
	{
		TryInitializeOwner();
	}
}
