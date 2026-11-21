// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/GlobalTimer/GlobalTimer.h"
#include <cmath>

namespace
{
// Drop excess catch-up callbacks, but retain the fractional interval and original cadence.
constexpr int32 MaxCallbacksPerTimerPerTick = 256;
} // namespace

static uint64 GeneratedTimerID = 1;

DECLARE_LOG_CATEGORY_CLASS(GlobalTimerLog, All, All);

void UGlobalTimer::Tick(float DeltaTime)
{
	UpdateTimers(FPlatformTime::Seconds());
}

TStatId UGlobalTimer::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UGlobalTimer, STATGROUP_Tickables);
}

bool UGlobalTimer::IsTickable() const
{
	return CanTick;
}

void UGlobalTimer::StartGlobalTimer()
{
	CanTick = true;
}

void UGlobalTimer::StopGlobalTimer()
{
	CanTick = false;
}

void UGlobalTimer::SetTimer(FGlobalTimerHandle& Handle, FGlobalUnifieldTimerDelegate Delegate, float InRate, bool InbLoop, const bool MaxOncePerFrame)
{
	if (!FMath::IsFinite(InRate) || InRate <= 0 || !Delegate.IsBound())
	{
		ClearTimer(Handle);
		return;
	}

	if (Handle.IsValid())
	{
		ClearTimer(Handle);
	}

	uint64 NewId = GenerateHadleID();
	Handle.Id = NewId;

	GlobalTimerData Data;
	Data.IsLoop = InbLoop;
	Data.Rate = InRate;
	Data.LastTime = FPlatformTime::Seconds();
	Data.OncePerFrame = MaxOncePerFrame;
	Data.Handler = Handle;

	Data.Delegate = MoveTemp(Delegate);
	UE_LOG(GlobalTimerLog, Display, TEXT("Set timer"));

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
	const uint64 Id = Handle.Id;
	Handle.Invalidate();
	if ((Timers.Contains(Id) || AddTimer.Contains(Id)) && !DeleteTimers.Contains(Id))
	{
		UE_LOG(GlobalTimerLog, Display, TEXT("Clear timer"));
		DeleteTimers.Add(Id);
	}
	RemovePendingTimers();
}

bool UGlobalTimer::IsValidTimer(const FGlobalTimerHandle& Handle) const
{
	if (DeleteTimers.Contains(Handle.Id)) return false;
	const GlobalTimerData* Timer = Timers.Find(Handle.Id);
	if (!Timer) Timer = AddTimer.Find(Handle.Id);
	return Timer && Timer->Delegate.IsBound();
}

void UGlobalTimer::RemovePendingTimers()
{
	// An executing callback must keep its captures alive until it returns.
	if (IsUpdatingTimer) return;
	TGuardValue<bool> UpdatingGuard(IsUpdatingTimer, true);
	while (!DeleteTimers.IsEmpty())
	{
		// Keep other canceled IDs marked while destructors run; do not retain an iterator across them.
		const uint64 Id = *DeleteTimers.CreateConstIterator();
		for (TMap<uint64, GlobalTimerData>* Storage : {&Timers, &AddTimer})
		{
			if (GlobalTimerData* Data = Storage->Find(Id))
			{
				// Destroy captures after removing the entry: their destructors may call back into this manager.
				GlobalTimerData Removed(MoveTemp(*Data));
				Storage->Remove(Id);
			}
		}
		DeleteTimers.Remove(Id);
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

void UGlobalTimer::UpdateTimers(double Now)
{
	if (!CanTick || IsUpdatingTimer) return;

	IsUpdatingTimer = true;
	{
		// Keep pending additions separate from additions made by capture destructors.
		TMap<uint64, GlobalTimerData> PendingAdds = MoveTemp(AddTimer);
		for (auto& Timer : PendingAdds)
		{
			if (!DeleteTimers.Contains(Timer.Key) && Timer.Value.Delegate.IsBound())
			{
				Timers.Emplace(Timer.Key, MoveTemp(Timer.Value));
			}
		}
	}
	for (auto& Timer : Timers)
	{
		if (!CanTick) break;
		if (DeleteTimers.Contains(Timer.Key)) continue;

		GlobalTimerData& Value = Timer.Value;
		if (!Value.Delegate.IsBound())
		{
			DeleteTimers.Add(Timer.Key);
			Value.Handler.Invalidate();
			continue;
		}
		const double Delta = Now - Value.LastTime;

		if (Value.Rate <= Delta)
		{
			// Execute
			// Clamp while still in double precision, before the conversion to int32.
			const int32 CountExecute = (Value.OncePerFrame || !Value.IsLoop)
										   ? 1
										   : static_cast<int32>(FMath::Min(Delta / Value.Rate, static_cast<double>(MaxCallbacksPerTimerPerTick)));
			for (int32 i = 0; i < CountExecute && CanTick && !DeleteTimers.Contains(Timer.Key) && Value.Delegate.IsBound(); ++i)
			{
				Value.Delegate.Execute();
			}
			// Execute

			if (!Value.IsLoop || !Value.Delegate.IsBound())
			{
				DeleteTimers.Add(Timer.Key);
				Value.Handler.Invalidate();
			}
			else
			{
				Value.LastTime = Now - std::fmod(Delta, static_cast<double>(Value.Rate));
			}
		}
	}
	IsUpdatingTimer = false;
	RemovePendingTimers();
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
