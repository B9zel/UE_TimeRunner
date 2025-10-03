// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/GlobalTimer/GlobalTimer.h"

static uint64 GeneratedTimerID = 1;

DECLARE_LOG_CATEGORY_CLASS(GlobalTimerLog, All, All);

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

void UGlobalTimer::SetTimer(FGlobalTimerHandle& Handle, FGlobalUnifieldTimerDelegate Delegate, float InRate, bool InbLoop, const bool MaxOncePerFrame)
{
	if (InRate <= 0) return;

	if (Handle.IsValid())
	{
		ClearTimer(Handle);
	}

	GlobalTimerData Data;
	Data.IsLoop = InbLoop;
	Data.Rate = InRate;
	Data.LastTime = FPlatformTime::Seconds();
	Data.OncePerFrame = MaxOncePerFrame;
	Data.Handler = &Handle;

	Data.Delegate = MoveTemp(Delegate);
	UE_LOG(GlobalTimerLog, Display, TEXT("Set timer"));

	uint64 NewId = GenerateHadleID();
	Handle.Id = NewId;
	if (IsUpdatingTimer)
	{
		AddTimer.Add(NewId, MoveTemp(Data));
	}
	else
	{
		Timers.Add(NewId, MoveTemp(Data));
	}
	DeleteTimers.Remove(NewId);
}

void UGlobalTimer::ClearTimer(FGlobalTimerHandle& Handle)
{
	if (Timers.Contains(Handle.Id))
	{
		UE_LOG(GlobalTimerLog, Display, TEXT("Clear timer"));
		// Timers.Remove(Handle.Id);
		DeleteTimers.AddUnique(Handle.Id);
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
	;
	UE_LOG(GlobalTimerLog, Display, TEXT("Start UpdateTimer %d"), Timers.Num());

	for (auto& Timer : AddTimer)
	{
		Timers.Emplace(Timer.Key, MoveTemp(Timer.Value));
	}
	AddTimer.Empty();

	for (uint64 Handle : DeleteTimers)
	{
		Timers.Remove(Handle);
	}
	DeleteTimers.Empty();

	IsUpdatingTimer = true;
	for (auto& Timer : Timers)
	{
		GlobalTimerData& Value = Timer.Value;
		const double Delta = Now - Value.LastTime;
		
		if (Value.Rate <= Delta)
		{
			if (!Value.Delegate.IsBound())
			{
				DeleteTimers.AddUnique(Timer.Key);
				Value.Handler->Invalidate();
				continue;
			}
			// Execute
			const int32 CountExecute = (Value.OncePerFrame || !Value.IsLoop) ? 1 : (Delta / Value.Rate);
			for (int32 i = 0; i < CountExecute; ++i)
			{
				Value.Delegate.Execute();
			}
			// Execute

			if (!Value.IsLoop)
			{
				DeleteTimers.AddUnique(Timer.Key);
				Value.Handler->Invalidate();
			}
			else
			{
				Value.LastTime = Now;
			}
		}
	}
	IsUpdatingTimer = false;

	UE_LOG(GlobalTimerLog, Display, TEXT("Finish UpdateTimer %d"), Timers.Num());
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
