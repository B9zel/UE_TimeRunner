// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/FunctionLibrary/GlobalTimerFunctionLibrary.h"
#include "Global/GameInstance/MainGameInstance.h"

FGlobalTimerHandle UGlobalTimerFunctionLibrary::SetGlobalTimer(FGlobalTimerDelegate Delegate, const float Rate, const bool IsLoop /*= false*/,
															   const bool MaxOncePerFrame /* = false */)
{
	if (!Delegate.IsBound()) return FGlobalTimerHandle();

	const UWorld* World = GEngine->GetWorldFromContextObject(Delegate.GetUObject(), EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return FGlobalTimerHandle();

	auto* GameInstance = Cast<UMainGameInstance>(World->GetGameInstance());
	if (GameInstance)
	{
		FGlobalTimerHandle NewHandle;
		UE_LOG(LogTemp, Display, TEXT("%s"), *Delegate.GetUObject()->GetName());
		GameInstance->GetGlobalTimer()->SetTimer(NewHandle, Delegate, Rate, IsLoop, MaxOncePerFrame);

		return NewHandle;
	}
	return FGlobalTimerHandle();
}

bool UGlobalTimerFunctionLibrary::IsValidTimer(const UObject* Object, FGlobalTimerHandle TimerHandle)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(Object, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;

	auto* GameInstance = Cast<UMainGameInstance>(World->GetGameInstance());
	if (GameInstance)
	{
		return GameInstance->GetGlobalTimer()->IsValidTimer(TimerHandle);
	}
	return false;
}

double UGlobalTimerFunctionLibrary::GetTotalSeconds()
{
	return FApp::GetCurrentTime();
}

double UGlobalTimerFunctionLibrary::GetDeltaRealSeconds(const UObject* Object)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(Object, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return 0.0f;

	return World->GetTime().GetDeltaRealTimeSeconds();
}

void UGlobalTimerFunctionLibrary::ClearGlobalTimer(const UObject* Object, UPARAM(ref) FGlobalTimerHandle& Timer)
{
	if (!Timer.IsValid()) return;

	const UWorld* World = GEngine->GetWorldFromContextObject(Object, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;

	auto* GameInstance = Cast<UMainGameInstance>(World->GetGameInstance());
	if (GameInstance)
	{
		GameInstance->GetGlobalTimer()->ClearTimer(Timer);
	}
}
