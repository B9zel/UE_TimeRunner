// Fill out your copyright notice in the Description page of Project Settings.

#include "StaticEnemies/Turrets/BulletTurret/BulletTurret.h"
#include "TimeRunner/Characters/TimeRunner/TimerRunnerCharacter.h"
#include "Global/GameInstance/MainGameInstance.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>

DECLARE_LOG_CATEGORY_CLASS(BulletTurretLog, All, All);

void ABulletTurret::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								   bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ATimerRunnerCharacter::StaticClass()))
	{
		TargetAttack = Cast<ATimerRunnerCharacter>(OtherActor);
		StartAttack();
	}
}

void ABulletTurret::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ABulletTurret::StartAttack()
{
	if (!m_RotateTimerHandle.IsValid() && !m_ShotTimerHandle.IsValid() && RateRotation > 0.0f && RateShot > 0.0f)
	{
		FTimerManagerTimerParameters Params;
		Params.bLoop = true;
		Params.bMaxOncePerFrame = true;

		InstanceGlobalTier->SetTimer(m_RotateTimerHandle, this, &ABulletTurret::OnRotateTurret, RateRotation, true); // GetWorldTimerManager().SetTimer(
		//	m_RotateTimerHandle, this, &ThisClass::OnRotateTurret, RateRotation, Params);
		InstanceGlobalTier->SetTimer(m_ShotTimerHandle, this, &ThisClass::OnShot, RateShot, true);
	}
	else
	{
		UE_LOG(BulletTurretLog, Warning, TEXT("RateRotation or RateShot are less or equal 0"));
	}
}

void ABulletTurret::StopAttack()
{
	if (m_RotateTimerHandle.IsValid() && m_ShotTimerHandle.IsValid())
	{
		InstanceGlobalTier->ClearTimer(m_RotateTimerHandle);
		InstanceGlobalTier->ClearTimer(m_ShotTimerHandle);
	}
}
void ABulletTurret::OnRotateTurret()
{
	if (TargetAttack.IsValid())
	{
		UE_LOG(BulletTurretLog, Display, TEXT("%s"), *TargetAttack->GetActorLocation().ToString());
		if ((TargetAttack->GetActorLocation() - GetActorLocation()).Length() > DistanceAttack) return;

		TArray<FHitResult> Hits;
		TArray<AActor*> Ignore;

		const bool IsHit = UKismetSystemLibrary::LineTraceMultiForObjects(this, GetActorLocation(), TargetAttack->GetActorLocation(), TargetCollisionChannels,
																		  false, Ignore, EDrawDebugTrace::ForDuration, Hits, true);

		if (!IsHit)
		{
			StopAttack();
			TargetAttack = nullptr;
			return;
		}

		MeshRotation->SetWorldRotation(CalculateNewRotateTurret());
	}
}

FRotator ABulletTurret::CalculateNewRotateTurret_Implementation()
{
	const float MeshYaw = MeshRotation->GetComponentRotation().Yaw;
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(MeshRotation->GetComponentLocation(), TargetAttack->GetActorLocation());
	const float InterpTargetYaw = FMath::FInterpTo(MeshYaw, TargetRotation.Yaw, GetWorld()->GetDeltaSeconds(), SpeedRotate);
	return FRotator(0.0f, InterpTargetYaw, 0.0f);
}

void ABulletTurret::OnShot_Implementation()
{
}

void ABulletTurret::BeginPlay()
{
	Super::BeginPlay();

	InstanceGlobalTier = GetWorld()->GetGameInstance<UMainGameInstance>()->GetGlobalTimer();
}
