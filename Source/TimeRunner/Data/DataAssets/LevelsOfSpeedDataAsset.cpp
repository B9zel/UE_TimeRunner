// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/DataAssets/LevelsOfSpeedDataAsset.h"

const float ULevelsOfSpeedDataAsset::GetSlowLevel() const
{
	return SlowLevelWorldDilation;
}

const float ULevelsOfSpeedDataAsset::GetMiddleLevel() const
{
	return MiddleLevelWorldDilation;
}

const float ULevelsOfSpeedDataAsset::GetFastLevel() const
{
	return FastLevelWorldDilation;
}

const float ULevelsOfSpeedDataAsset::GetVeryFastLevel() const
{
	return VeryFastLevelWorldDilation;
}
