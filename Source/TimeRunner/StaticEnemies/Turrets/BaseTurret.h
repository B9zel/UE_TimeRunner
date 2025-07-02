// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTurret.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS(Abstract)
class TIMERUNNER_API ABaseTurret : public AActor
{
	GENERATED_BODY()

public:

	ABaseTurret();

public:

	virtual void BeginPlay() override;

protected:

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								bool bFromSweep, const FHitResult& SweepResult) PURE_VIRTUAL(ThisClass::OnBeginOverlap, );
	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
		PURE_VIRTUAL(ThisClass::OnEndOverlap, );

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* SphereCollision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshRotation;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float Damage;
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0, Units = "s"))
	float RateRotation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.0))
	float SpeedRotate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.0, Units = "s"))
	float RateShot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.0))
	float DistanceAttack;
};
