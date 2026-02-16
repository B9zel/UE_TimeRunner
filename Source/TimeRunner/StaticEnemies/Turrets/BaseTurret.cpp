// Fill out your copyright notice in the Description page of Project Settings.

#include "StaticEnemies/Turrets/BaseTurret.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/BoxComponent.h>
#include "AbilitySystemComponent.h"

ABaseTurret::ABaseTurret()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere collision"));
	check(SphereCollision);
	SphereCollision->SetupAttachment(RootComponent);

	MeshRotation = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotation mesh"));
	check(MeshRotation);
	MeshRotation->SetupAttachment(SphereCollision);

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability system component"));

	RateRotation = 0.001f;
}

void ABaseTurret::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

UAbilitySystemComponent* ABaseTurret::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}
