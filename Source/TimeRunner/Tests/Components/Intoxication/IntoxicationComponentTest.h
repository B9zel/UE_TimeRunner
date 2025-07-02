// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "IntoxicationComponentTest.generated.h"

UCLASS(NotBlueprintable)

class UTestCharacter final : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION()

	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
	{
#if (AUTOMATION_TEST)
		WasApplyDamage = true;
#endif
	}
#if (AUTOMATION_TEST)
public:

	bool WasApplyDamage = false;
#endif
};
