// Fill out your copyright notice in the Description page of Project Settings.

#include "Tests/Characters/Class/TestTimerRunnerCharacter.h"

UIntoxicationComponent* ATestTimerRunnerCharacter::GetIntoxicationComponent() const
{
	return IntoxicationComponent;
}

UHealthComponent* ATestTimerRunnerCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

void ATestTimerRunnerCharacter::ApplyDilation()
{
	ApplyTimeDilation();
}

void ATestTimerRunnerCharacter::StopDilation()
{
	ResetTimeDilation();
}

inline UTimeDilationComponent* ATestTimerRunnerCharacter::GetTimeDilationComponent() const
{
	return nullptr;
	// DilationComponent;
}