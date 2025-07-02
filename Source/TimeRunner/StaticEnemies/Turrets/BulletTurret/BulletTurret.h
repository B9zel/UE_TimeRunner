// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StaticEnemies/Turrets/BaseTurret.h"
#include "Global/GlobalTimer/GlobalTimer.h"
#include "BulletTurret.generated.h"

class ATimerRunnerCharacter;

UCLASS()
class TIMERUNNER_API ABulletTurret : public ABaseTurret
{
	GENERATED_BODY()

protected:

	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
								bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	virtual void StartAttack();
	virtual void StopAttack();
	UFUNCTION()
	virtual void OnRotateTurret();

	UFUNCTION(BlueprintNativeEvent)
	FRotator CalculateNewRotateTurret();

	UFUNCTION(BlueprintNativeEvent)
	void OnShot();

	virtual void BeginPlay() override;

protected:

	UGlobalTimer* InstanceGlobalTier{nullptr};

private:

	FGlobalTimerHandle m_RotateTimerHandle;
	FGlobalTimerHandle m_ShotTimerHandle;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<ATimerRunnerCharacter> TargetAttack;
	UPROPERTY(EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetCollisionChannels;
};
