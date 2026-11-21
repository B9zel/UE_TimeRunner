// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier_CameraShake.h"
#include "RealTimeModifier_CameraShake.generated.h"

/**
 * 
 */
UCLASS()
class TIMERUNNER_API URealTimeModifier_CameraShake : public UCameraModifier_CameraShake
{
	GENERATED_BODY()

public:

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
	
};
