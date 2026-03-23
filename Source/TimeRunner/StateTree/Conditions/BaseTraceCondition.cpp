// Fill out your copyright notice in the Description page of Project Settings.

#include "StateTree/Conditions/BaseTraceCondition.h"
#include "GameFramework/Character.h"

FVector UBaseTraceCondition::GetEndTraceLocation() const
{
	if (IsDirection)
	{
		return Source->GetActorLocation() + (GetDirectionFromEnum() * Distance);
	}
	return EndTrace;
}

const TArray<AActor*>& UBaseTraceCondition::GetIgnoreActors() const
{
	if (IgnoreSelf)
	{
		IgnoreActors.Add(Source);
	}
	return IgnoreActors;
}

FVector UBaseTraceCondition::GetDirectionFromEnum() const
{
	switch (Direction)
	{
	case EDirectionTrace::Up:
		return Source->GetActorUpVector();
	case EDirectionTrace::Down:
		return Source->GetActorUpVector() * -1.0f;
	case EDirectionTrace::Right:
		return Source->GetActorRightVector();
	case EDirectionTrace::Left:
		return Source->GetActorRightVector() * -1.0f;
	case EDirectionTrace::Forward:
		return Source->GetActorForwardVector();
	case EDirectionTrace::Backward:
		return Source->GetActorForwardVector() * -1.0f;
	}

	return FVector::ZeroVector;
}
