// For Daemons and something else videogame purpose only

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "DistanceCompare2D.generated.h"

UENUM(BlueprintType)
enum class EOperationCheck : uint8
{
	Less = 0,
	LessOrEqual,
	Great,
	GreatOrEqual,
	Equal,
	InRange
};

/**
 *
 */
UCLASS()
class TIMERUNNER_API UDistanceCompare2D : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()

protected:

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

	bool CheckConditions(const float Dist) const;

protected:

	UPROPERTY(EditAnywhere)
	bool Invert{false};

	UPROPERTY(EditAnywhere)
	EOperationCheck Operator = EOperationCheck::Less;

	UPROPERTY(EditAnywhere)
	FVector A;
	UPROPERTY(EditAnywhere)
	FVector B;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Operator!=EDistanceCheck::InRange", EditConditionHides))
	float Distance{0.0f};

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Operator==EDistanceCheck::InRange", EditConditionHides))
	float MaxDistance{0.0f};

	UPROPERTY(EditAnywhere, meta = (EditCondition = "Operator==EDistanceCheck::InRange", EditConditionHides))
	float MinDistance{0.0f};
};
