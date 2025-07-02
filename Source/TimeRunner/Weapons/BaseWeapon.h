// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class TIMERUNNER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	ABaseWeapon();

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Attack();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StopAttack();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters", meta = (Units = "s", ClampMin = "0.0"))
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters", meta = (Units = "s", ClampMin = "0.0"))
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	bool IsReloading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameters")
	bool CanAttack;
};
