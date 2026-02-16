// Fill out your copyright notice in the Description page of Project Settings.

#include "StaticEnemies/Turrets/BulletTurret/BulletTurret.h"
#include "TimeRunner/Characters/TimeRunner/TimerRunnerCharacter.h"
#include "Global/GameInstance/MainGameInstance.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>

DECLARE_LOG_CATEGORY_CLASS(BulletTurretLog, All, All);

void ABulletTurret::BeginPlay()
{
	Super::BeginPlay();

	InstanceTimer = &GetWorldTimerManager();
	// InstanceTimer = GetWorld()->GetGameInstance<UMainGameInstance>()->GetGlobalTimer();
}

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
	if (TargetAttack == OtherActor)
	{
		StopAttack();
	}
}

void ABulletTurret::StartAttack()
{
	if (!m_RotateTimerHandle.IsValid() && !m_ShotTimerHandle.IsValid() && RateRotation > 0.0f && RateShot > 0.0f)
	{
		FTimerManagerTimerParameters Params;
		Params.bLoop = true;
		Params.bMaxOncePerFrame = true;

		InstanceTimer->SetTimer(m_RotateTimerHandle, this, &ABulletTurret::OnRotateTurret, RateRotation, true); // GetWorldTimerManager().SetTimer(
		//	m_RotateTimerHandle, this, &ThisClass::OnRotateTurret, RateRotation, Params);
		InstanceTimer->SetTimer(m_ShotTimerHandle, this, &ThisClass::NotifyShot, RateShot, true);
	}
	else
	{
		UE_LOG(BulletTurretLog, Warning, TEXT("RateRotation or RateShot are less or equal 0"));
	}
}

void ABulletTurret::OnRotateTurret()
{
	if (TargetAttack.IsValid())
	{
		UE_LOG(BulletTurretLog, Display, TEXT("%s"), *TargetAttack->GetActorLocation().ToString());
		const float Distance = (TargetAttack->GetActorLocation() - GetActorLocation()).Length();
		if (Distance > DistanceAttack) return;

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
	UE_LOG(BulletTurretLog, Display, TEXT("Turret yaw: %f"), MeshYaw);
	const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(MeshRotation->GetComponentLocation(), TargetAttack->GetActorLocation());
	UE_LOG(BulletTurretLog, Display, TEXT("Player yaw: %f"), TargetRotation.Yaw);

	float InterpTargetYaw = TurnToTarget(MeshYaw, TargetRotation.Yaw, SpeedRotate, GetWorld()->GetDeltaSeconds());
	// float InterpTargetYaw = FMath::FInterpConstantTo(MeshYaw, TargetRotation.Yaw, GetWorld()->GetDeltaSeconds(), SpeedRotate);

	return FRotator(0.0f, InterpTargetYaw, 0.0f);
}

void ABulletTurret::OnShot_Implementation()
{
}

void ABulletTurret::StopAttack()
{
	if (m_RotateTimerHandle.IsValid() && m_ShotTimerHandle.IsValid())
	{
		InstanceTimer->ClearTimer(m_RotateTimerHandle);
		InstanceTimer->ClearTimer(m_ShotTimerHandle);
	}
}

float ABulletTurret::TurnToTarget(float Current, float Target, const float Speed, const float DeltaTime)
{
	if (Target == 0.f)
	{
		return FRotator3f::ClampAxis(Current);
	}

	if (Target >= 360.f)
	{
		return FRotator3f::ClampAxis(Current);
	}

	const float DeltaSpeed = Speed * DeltaTime;
	float result = FRotator3f::ClampAxis(Current);
	Current = result;
	Target = FRotator3f::ClampAxis(Target);

	if (Current > Target)
	{
		if (Current - Target < 180.f)
			result -= FMath::Clamp(Current - Target, -DeltaSpeed, DeltaSpeed);
		else
			result += FMath::Clamp((Target + 360.f - Current), -DeltaSpeed, DeltaSpeed);
	}
	else
	{
		if (Target - Current < 180.f)
			result += FMath::Clamp((Target - Current), -DeltaSpeed, DeltaSpeed);
		else
			result -= FMath::Clamp((Current + 360.f - Target), -DeltaSpeed, DeltaSpeed);
	}
	return FRotator3f::ClampAxis(result);
}

void ABulletTurret::NotifyShot()
{
	if (!TargetAttack.IsValid()) return;

	if (FVector::Dist(GetActorLocation(), TargetAttack->GetActorLocation()) <= DistanceAttack)
	{
		OnShot();
	}
}
