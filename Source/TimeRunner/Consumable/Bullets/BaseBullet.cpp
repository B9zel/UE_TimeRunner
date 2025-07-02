// Fill out your copyright notice in the Description page of Project Settings.

#include "Consumable/Bullets/BaseBullet.h"
#include "TimeRunner/Characters/TimeRunner/TimerRunnerCharacter.h"
#include <Components/BoxComponent.h>
#include <Engine/DamageEvents.h>
#include <Kismet/GameplayStatics.h>

ABaseBullet::ABaseBullet()
{
	PrimaryActorTick.bCanEverTick = true;
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

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	LastTypeCollsion = BoxCollision->GetCollisionEnabled();
}

void ABaseBullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								 bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ATimerRunnerCharacter::StaticClass()))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetActorForwardVector(), SweepResult, GetInstigatorController(), this,
										   TSubclassOf<UDamageType>());
	}
}
