// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Countermotion/CountermotionComponent.h"
#include "Components/Health/HealthComponent.h"
#include "Global/GameInstance/MainGameInstance.h"


DECLARE_LOG_CATEGORY_CLASS(CountermotionCompLog, All, All);

// Sets default values for this component's properties
UCountermotionComponent::UCountermotionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UCountermotionComponent::BeginPlay()
{
	Super::BeginPlay();

	const float Second = 1.0f;
	CountermotionDataArray.Reserve(GetMaxCountElementsOfSaveData());

	GetWorld()->GetGameInstance<UMainGameInstance>()->GetGlobalTimer()->SetTimer(RateSaveDataHandle, this, &ThisClass::SaveFrameData,
																				 Second / CountSaveDataPerSecond, true);
	//GetWorld()->GetTimerManager().SetTimer(RateSaveDataHandle, this, &UCountermotionComponent::SaveFrameData, Second / CountSaveDataPerSecond, true);
}

const FCountermotionData& UCountermotionComponent::GetOldData() const
{
	if (CountermotionDataArray.IsEmpty())
	{
		UE_LOG(CountermotionCompLog, Warning, TEXT("CountermotionDataArray is empty"));
		static FCountermotionData DummyRes;
		return DummyRes;
	}
	if (CurrentIndexSave == CountermotionDataArray.Num())
	{
		return *CountermotionDataArray.begin();
	}
	return CountermotionDataArray[CurrentIndexSave];
}


void UCountermotionComponent::SaveFrameData()
{		
	if (CountermotionDataArray.Num() < GetMaxCountElementsOfSaveData())
	{
		CountermotionDataArray.Add(CollectData());
	}
	else
	{
		CountermotionDataArray[CurrentIndexSave] = CollectData();
	}
	CurrentIndexSave = (CurrentIndexSave + 1) % GetMaxCountElementsOfSaveData();

}

int32 UCountermotionComponent::GetMaxCountElementsOfSaveData() const
{
	return CountSaveDataPerSecond * DurationSave;
}

FCountermotionData UCountermotionComponent::CollectData() const
{
	FCountermotionData Data;
	Data.Location = GetOwner()->GetActorLocation();
	Data.Rotation = GetOwner()->GetInstigatorController()->GetControlRotation();
	if (auto* Health = GetOwner()->GetComponentByClass<UHealthComponent>())
	{
		Data.Health = Health->GetCurrentHP();
	}
	return Data;
}

int32 UCountermotionComponent::GetOldDataIndex() const
{
	return CurrentIndexSave;
}

FInt32Range UCountermotionComponent::GetRangeOfOldestData() const
{
	if (CurrentIndexSave == CountermotionDataArray.Num())
	{
		return FInt32Range(0, CurrentIndexSave - 1);
	}
	return FInt32Range(CurrentIndexSave, CountermotionDataArray.Num() - 1);
}
