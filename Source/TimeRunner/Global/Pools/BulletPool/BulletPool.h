// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BulletPool.generated.h"

class ABaseBullet;

UCLASS()
class TIMERUNNER_API UBulletPool : public UObject
{
	GENERATED_BODY()

public:

	UBulletPool();
	virtual void FinishDestroy() override;

public:

	void InitializePool();
	ABaseBullet* RequestBulletObject(AActor* Owner, APawn* Instigator, const FVector& Location, const FRotator& Rotation,
									 const FVector& Scale = FVector(1.0f, 1.0f, 1.0f));
	void GiveBulletObject(ABaseBullet* Bullet);

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseBullet> BulletClass;

private:

	UPROPERTY()
	TArray<TObjectPtr<ABaseBullet>> m_FreeObjects;
	UPROPERTY()
	TSet<TObjectPtr<ABaseBullet>> m_UsedObjects;

	bool IsInitialize;
	uint32 CountInitObjects;
};
