// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <TimerManager.h>
#include "GlobalTimer.generated.h"

DECLARE_DYNAMIC_DELEGATE(FGlobalTimerDelegate);

using FGobalTimerFunction = TFunction<void(void)>;
using TGlobalTimerMethodPtr = TDelegate<void(), FNotThreadSafeNotCheckedDelegateUserPolicy>;

// template <typename UserClass, typename... VarType> using TGlobalTimerMethodPtr = typename TMemFunPtrType<false, UserClass, void()>::Type;

using FGlobalTimerDelegateVariant = TVariant<TYPE_OF_NULLPTR, TGlobalTimerMethodPtr, FGlobalTimerDelegate, FGobalTimerFunction>;

struct FGlobalUnifieldTimerDelegate
{
public:

	FGlobalTimerDelegateVariant Variant;

	FGlobalUnifieldTimerDelegate()
	{
	}

	FGlobalUnifieldTimerDelegate(const FGlobalTimerDelegate& Delegate) : Variant(TInPlaceType<FGlobalTimerDelegate>(), Delegate)
	{
	}
	FGlobalUnifieldTimerDelegate(const TGlobalTimerMethodPtr& Delegate) : Variant(TInPlaceType<TGlobalTimerMethodPtr>(), Delegate)
	{
	}
	FGlobalUnifieldTimerDelegate(FGobalTimerFunction&& Delegate) : Variant(TInPlaceType<FGobalTimerFunction>(), MoveTemp(Delegate))
	{
	}

	inline void Execute();
	inline bool IsBound() const;

	void UnBind()
	{
		Variant.Set<TYPE_OF_NULLPTR>(nullptr);
	}

	FGlobalUnifieldTimerDelegate(FGlobalUnifieldTimerDelegate&&) = default;
	FGlobalUnifieldTimerDelegate(const FGlobalUnifieldTimerDelegate&) = delete;
	FGlobalUnifieldTimerDelegate& operator=(FGlobalUnifieldTimerDelegate&&) = default;
	FGlobalUnifieldTimerDelegate& operator=(const FGlobalUnifieldTimerDelegate&) = delete;
};

USTRUCT(BlueprintType)
struct FGlobalTimerHandle
{
	GENERATED_BODY()

	friend UGlobalTimer;

public:

	bool IsValid() const
	{
		return Id != 0;
	}

	void Invalidate()
	{
		Id = 0;
	}

private:

	uint64 Id = 0;
};

struct GlobalTimerData
{
public:

	GlobalTimerData() = default;
	~GlobalTimerData() = default;

	/*GlobalTimerData(const GlobalTimerData& Other)
	{
		Delegate = Other.Delegate;
		Rate = Other.Rate;
		LastTime = Other.LastTime;
		IsLoop = Other.IsLoop;
	}*/

	GlobalTimerData(GlobalTimerData&& Other) noexcept
	{
		Delegate = MoveTemp(Other.Delegate);
		Rate = Other.Rate;
		LastTime = Other.LastTime;
		IsLoop = Other.IsLoop;
		Handler = Other.Handler;

		Other.Delegate.UnBind();
		Other.Rate = 0.0f;
		Other.LastTime = 0.0f;
		Other.IsLoop = false;
		Other.Handler = nullptr;
	}

	FGlobalUnifieldTimerDelegate Delegate;
	FGlobalTimerHandle* Handler;
	float Rate{0.0f};
	double LastTime{0.0};
	bool IsLoop{false};
	bool OncePerFrame{false};
};

UCLASS()
class TIMERUNNER_API UGlobalTimer : public UObject
{
	GENERATED_BODY()

private:

	using DelegateGlobalTimer = TDelegate<void()>;

public:

	void StartGlobalTimer();
	void StopGlobalTimer();

	void SetTimer(FGlobalTimerHandle& Handle, FGlobalUnifieldTimerDelegate Deligate, float InRate, bool InbLoop = false, const bool MaxOncePerFrame=false);
	template <class UserClass>
	void SetTimer(FGlobalTimerHandle& Handle, UserClass* Object, typename FTimerDelegate::TMethodPtr<UserClass> Method, float InRate, bool InbLoop = false);
	void ClearTimer(FGlobalTimerHandle& Handle);

private:

	void UpdateTimers();
	int64 GenerateHadleID();

private:

	TMap<uint64, GlobalTimerData> Timers;
	TArray<uint64> DeleteTimers;
	TMap<uint64, GlobalTimerData> AddTimer;
	FTimerHandle UpdateTimer;

	bool IsUpdatingTimer{false};
};

template <class UserClass>
void UGlobalTimer::SetTimer(FGlobalTimerHandle& Handle, UserClass* Object, typename FTimerDelegate::TMethodPtr<UserClass> Method, float InRate, bool InbLoop)
{

	SetTimer(Handle, FGlobalUnifieldTimerDelegate(FTimerDelegate::CreateUObject(Object, Method)), InRate, InbLoop);
}
