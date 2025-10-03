// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Global/GlobalTimer/GlobalTimer.h"
#include "CountermotionComponent.generated.h"


USTRUCT(BlueprintType)
struct FCountermotionData
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite)
	FVector Location;
	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation;
	UPROPERTY(BlueprintReadWrite)
	float Health;
};	



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TIMERUNNER_API UCountermotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCountermotionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION(BlueprintPure)
	const FCountermotionData& GetOldData() const;

	UFUNCTION(BlueprintPure)
	int32 GetOldDataIndex() const;
	
	// Min stores begin of range, Max stores end of range
	UFUNCTION(BlueprintPure)
	FInt32Range GetRangeOfOldestData() const;
	

private:

	UFUNCTION()
	void SaveFrameData();

	int32 GetMaxCountElementsOfSaveData() const;

	FCountermotionData CollectData() const;

private:

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FCountermotionData> CountermotionDataArray;

	int32 CurrentIndexSave{0};
	FGlobalTimerHandle RateSaveDataHandle;

	UPROPERTY(EditAnywhere, Category = "Countermotion", meta = (ClampMin = "1"))
	int32 CountSaveDataPerSecond{1};	

	UPROPERTY(EditAnywhere, Category = "Countermotion", meta = (Units = "s", ClampMin = "0.001"))
	float DurationSave{1.0f};


};
