// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Camera/Modifier/RealTimeModifier_CameraShake.h"

bool URealTimeModifier_CameraShake::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	const float RealTimeSeconds = GetWorld() ? GetWorld()->GetTime().GetDeltaRealTimeSeconds() : DeltaTime;

	return Super::ModifyCamera(RealTimeSeconds, InOutPOV);
}
