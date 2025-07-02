// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/GlobalTimer/GlobalTimer.h"

static uint64 GeneratedTimerID = 1;

void UGlobalTimer::StartGlobalTimer()
{
	if (UpdateTimer.IsValid()) return;

	FTimerManagerTimerParameters Params;
	Params.bLoop = true;
	Params.bMaxOncePerFrame = true;
	GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &UGlobalTimer::UpdateTimers, UE_SMALL_NUMBER, Params);
}

void UGlobalTimer::StopGlobalTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(UpdateTimer);
}

void UGlobalTimer::SetTimer(FGlobalTimerHandle& Handle, FGlobalUnifieldTimerDelegate Delegate, float InRate, bool InbLoop)
{
	if (InRate < 0) return;

	GlobalTimerData Data;
	Data.IsLoop = InbLoop;
	Data.Rate = InRate;
	Data.LastTime = FPlatformTime::Seconds();

	Data.Delegate = MoveTemp(Delegate);
	// UE_LOG(LogTemp, Display, TEXT("%s"), *Delegate.GetUObject()->GetName());

	uint64 NewId = GenerateHadleID();
	Handle.Id = NewId;
	Timers.Add(NewId, MoveTemp(Data));
	DeleteTimer.Remove(NewId);
}

void UGlobalTimer::ClearTimer(FGlobalTimerHandle& Handle)
{
	if (Timers.Contains(Handle.Id))
	{
		DeleteTimer.AddUnique(Handle.Id);
		Handle.Invalidate();
	}
}

int64 UGlobalTimer::GenerateHadleID()
{
	if (GeneratedTimerID == MAX_uint64)
	{
		GeneratedTimerID = 0;
	}

	int32 Offset = 1;
	while (GeneratedTimerID < MAX_uint64)
	{
		if (Timers.Contains(GeneratedTimerID + Offset))
		{
			++Offset;
		}
		else
		{
			break;
		}
	}
	GeneratedTimerID += Offset;
	return GeneratedTimerID;
}

void UGlobalTimer::UpdateTimers()
{
	double Now = FPlatformTime::Seconds();
	double Delta = GetWorld()->GetDeltaSeconds();

	for (auto& Timer : Timers)
	{
		GlobalTimerData& Value = Timer.Value;

		if (Value.Rate <= (Now - Value.LastTime))
		{
			if (!Value.Delegate.IsBound())
			{
				DeleteTimer.AddUnique(Timer.Key);
				continue;
			}
			else
			{
				Value.Delegate.Execute();
			}

			if (!Value.IsLoop)
			{
				DeleteTimer.AddUnique(Timer.Key);
			}
			else
			{
				Value.LastTime = Now;
			}
		}
	}

	for (uint64 Handle : DeleteTimer)
	{
		Timers.Remove(Handle);
	}
}

void FGlobalUnifieldTimerDelegate::Execute()
{
	switch (Variant.GetIndex())
	{
	case FGlobalTimerDelegateVariant::IndexOfType<FGlobalTimerDelegate>():
	{
		const FGlobalTimerDelegate& Delegate = Variant.Get<FGlobalTimerDelegate>();
		if (Delegate.IsBound())
		{
			Delegate.Execute();
		}
		break;
	}
	case FGlobalTimerDelegateVariant::IndexOfType<TGlobalTimerMethodPtr>():
	{
		const TGlobalTimerMethodPtr& Delegate = Variant.Get<TGlobalTimerMethodPtr>();
		if (Delegate.IsBound())
		{
			Delegate.Execute();
		}
		break;
	}
	case FGlobalTimerDelegateVariant::IndexOfType<FGobalTimerFunction>():
	{
		if (const FGobalTimerFunction& Delegate = Variant.Get<FGobalTimerFunction>())
		{
			Delegate();
		}
		break;
	}
	default:
		break;
	}
}

inline bool FGlobalUnifieldTimerDelegate::IsBound() const
{
	switch (Variant.GetIndex())
	{
	case FGlobalTimerDelegateVariant::IndexOfType<FGlobalTimerDelegate>():
		return Variant.Get<FGlobalTimerDelegate>().IsBound();
	case FGlobalTimerDelegateVariant::IndexOfType<TGlobalTimerMethodPtr>():
		return Variant.Get<TGlobalTimerMethodPtr>().IsBound();
	case FGlobalTimerDelegateVariant::IndexOfType<FGobalTimerFunction>():
		return Variant.Get<FGobalTimerFunction>() != nullptr;
	default:
		return false;
	}
}
