// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBullet.generated.h"

class UBoxComponent;

UCLASS()
class TIMERUNNER_API ABaseBullet : public AActor
{
	GENERATED_BODY()

public:

	ABaseBullet();

public:

	virtual void DisableObject();
	virtual void EnableObject();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								bool bFromSweep, const FHitResult& SweepResult);

protected:

	// Components
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollision;
	//

	float Damage;

private:

	ECollisionEnabled::Type LastTypeCollsion;
};
