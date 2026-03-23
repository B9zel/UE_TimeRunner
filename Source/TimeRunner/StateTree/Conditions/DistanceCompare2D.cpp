// For Daemons and something else videogame purpose only

#include "StateTree/Conditions/DistanceCompare2D.h"

bool UDistanceCompare2D::TestCondition(FStateTreeExecutionContext& Context) const
{
	const bool Result = CheckConditions(FVector::Dist2D(A, B));

	return Invert ? (!Result) : Result;
}

bool UDistanceCompare2D::CheckConditions(const float Dist) const
{
	switch (Operator)
	{
	case EOperationCheck::Less:
		return Dist < Distance;
	case EOperationCheck::LessOrEqual:
		return Dist <= Distance;
	case EOperationCheck::Great:
		return Dist > Distance;
	case EOperationCheck::GreatOrEqual:
		return Dist >= Distance;
	case EOperationCheck::Equal:
		return Dist == Distance;
	case EOperationCheck::InRange:
		return FMath::IsWithinInclusive(Dist, MinDistance, MaxDistance);
	}
	return false;
}
