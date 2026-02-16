// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AI/BaseAIController.h"
#include "Components/StateTreeAIComponent.h"

ABaseAIController::ABaseAIController()
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("State tree component"));
	check(StateTreeComponent);
}
