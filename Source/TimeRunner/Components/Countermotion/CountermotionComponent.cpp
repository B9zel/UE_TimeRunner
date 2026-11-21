// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/Countermotion/CountermotionComponent.h"
#include "Components/Health/HealthComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
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
	if (APawn* Pawn = GetOwner<APawn>())
	{
		Pawn->ReceiveControllerChangedDelegate.AddDynamic(this, &ThisClass::OnControllerChanged);
	}
	Super::BeginPlay();
}

void UCountermotionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (APawn* Pawn = GetOwner<APawn>())
	{
		Pawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &ThisClass::OnControllerChanged);
	}
	StopRecording();
	RecordingTimer.Reset();
	Super::EndPlay(EndPlayReason);
}

void UCountermotionComponent::OnFullInitOwner()
{
	if (!HasBegunPlay()) return;
	CountermotionDataArray.Reserve(GetMaxCountElementsOfSaveData());
	if (const UMainGameInstance* GameInstance = GetWorld()->GetGameInstance<UMainGameInstance>())
	{
		RecordingTimer = GameInstance->GetGlobalTimer();
	}
	StartRecording();
}

void UCountermotionComponent::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	StopRecording();
	if (IsValid(NewController)) StartRecording();
}

void UCountermotionComponent::StartRecording()
{
	if (!HasBegunPlay() || !IsValid(GetOwner()) || !IsValid(GetOwner()->GetInstigatorController())) return;
	if (const APawn* Pawn = GetOwner<APawn>(); Pawn && !IsValid(Pawn->GetController())) return;
	if (UGlobalTimer* Timer = RecordingTimer.Get(); Timer && !Timer->IsValidTimer(RateSaveDataHandle))
	{
		Timer->SetTimer(RateSaveDataHandle, this, &ThisClass::SaveFrameData, 1.0f / CountSaveDataPerSecond, true);
	}
}

void UCountermotionComponent::StopRecording()
{
	if (UGlobalTimer* Timer = RecordingTimer.Get())
	{
		Timer->ClearTimer(RateSaveDataHandle);
	}
	else
	{
		RateSaveDataHandle.Invalidate();
	}
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
	FCountermotionData Data;
	if (!TryCollectData(Data)) return;
	if (CountermotionDataArray.Num() < GetMaxCountElementsOfSaveData())
	{
		CountermotionDataArray.Add(Data);
	}
	else
	{
		CountermotionDataArray[CurrentIndexSave] = Data;
	}
	CurrentIndexSave = (CurrentIndexSave + 1) % GetMaxCountElementsOfSaveData();
}

int32 UCountermotionComponent::GetMaxCountElementsOfSaveData() const
{
	return CountSaveDataPerSecond * DurationSave;
}

bool UCountermotionComponent::TryCollectData(FCountermotionData& Data) const
{
	const AActor* Owner = GetOwner();
	if (!IsValid(Owner)) return false;
	const AController* Controller = Owner->GetInstigatorController();
	if (!IsValid(Controller)) return false;
	Data.Location = Owner->GetActorLocation();
	Data.Rotation = Controller->GetControlRotation();
	if (auto* Health = Owner->GetComponentByClass<UHealthComponent>())
	{
		Data.Health = Health->GetCurrentHP();
	}
	return true;
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
