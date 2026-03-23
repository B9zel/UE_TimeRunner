// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TIMERUNNER_API UBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void OnFullInitOwner();


public:	

	void OnPlayBeginOwner(APawn* Owner);
	UFUNCTION()
	void OnPossessedOwner(APawn* Pawn, AController* OldController, AController* NewController);
};
