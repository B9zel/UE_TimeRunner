// Fill out your copyright notice in the Description page of Project Settings.

#include "Consumable/Bullets/BaseBullet.h"
#include "TimeRunner/Characters/TimeRunner/TimerRunnerCharacter.h"
#include <Components/BoxComponent.h>
#include <Engine/DamageEvents.h>
#include <Kismet/GameplayStatics.h>

ABaseBullet::ABaseBullet()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision bullet"));
	check(BoxCollision);
	SetRootComponent(BoxCollision);
}

void ABaseBullet::DisableObject()
{
	LastTypeCollsion = BoxCollision->GetCollisionEnabled();
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorLocation(FVector::ZeroVector);
}

void ABaseBullet::EnableObject()
{
	BoxCollision->SetCollisionEnabled(LastTypeCollsion);
	SetActorHiddenInGame(false);
	Damage = 0.0f;
}

void ABaseBullet::BeginPlay()
{
	Super::BeginPlay();

	LastTypeCollsion = BoxCollision->GetCollisionEnabled();
}
