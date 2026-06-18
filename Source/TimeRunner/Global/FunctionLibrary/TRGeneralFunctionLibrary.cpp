// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/FunctionLibrary/TRGeneralFunctionLibrary.h"

#define NEAREST_ZERO_BLUEPRINT 1.e-6f

float UTRGeneralFunctionLibrary::GetMinNearestZeroFloatValue()
{
	return NEAREST_ZERO_BLUEPRINT;
}
