// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Characters/AI/AIGeneral.h"
#include "BaseAIController.generated.h"

class UStateTreeAIComponent;
/**
 *
 */
UCLASS()
class TIMERUNNER_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:

	ABaseAIController();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EStateEnemy State;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStateTreeAIComponent> StateTreeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ACharacter* TargetCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DistanceAttack{100.0f};

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector NextPatrolPoint;
};
