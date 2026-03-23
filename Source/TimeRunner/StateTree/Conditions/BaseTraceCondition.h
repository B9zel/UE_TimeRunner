// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "BaseTraceCondition.generated.h"

UENUM()
enum class EDirectionTrace : uint8
{
	Up = 0,
	Down,
	Right,
	Left,
	Forward,
	Backward
};

/**
 *
 */
UCLASS(Abstract)
class TIMERUNNER_API UBaseTraceCondition : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintPure)
	FVector GetEndTraceLocation() const;

	UFUNCTION(BlueprintPure)
	const TArray<AActor*>& GetIgnoreActors() const;

private:

	FVector GetDirectionFromEnum() const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Context")
	ACharacter* Source;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Invert{false};

	UPROPERTY(EditAnywhere)
	bool IsDirection{false};

	UPROPERTY(EditAnywhere, meta = (EditCondition = "IsDirection==true", EditConditionHides))
	EDirectionTrace Direction;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "IsDirection==true", EditConditionHides))
	float Distance{0.0f};

	UPROPERTY(EditAnywhere, meta = (EditCondition = "IsDirection==false", EditConditionHides))
	FVector EndTrace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ETraceTypeQuery> TraceChannel;

	UPROPERTY(EditAnywhere)
	mutable TArray<AActor*> IgnoreActors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IgnoreSelf{true};
};
