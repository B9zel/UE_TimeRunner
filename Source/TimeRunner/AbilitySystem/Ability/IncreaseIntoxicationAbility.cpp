// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/IncreaseIntoxicationAbility.h"
#include "AbilitySystemComponent.h"
#include "Components/Intoxication/IntoxicationComponent.h"
#include "AbilitySystem/Attributes/Intoxication/IntoxicationAttributeSet.h"
#include "Components/Intoxication/IntoxicationComponent.h"
#include <Kismet/GameplayStatics.h>

void UIncreaseIntoxicationAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
												   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	FTimerManagerTimerParameters Param;
	Param.bLoop = true;
	Param.bMaxOncePerFrame = false;
	GetWorld()->GetTimerManager().SetTimer(TickHandle, this, &ThisClass::CustomTick, UE_FLOAT_NORMAL_THRESH, Param);

	IntoxicationComp = GetOwningActorFromActorInfo()->FindComponentByClass<UIntoxicationComponent>();
	Period = IntoxicationComp->GetRateIncrease();

	/*SpecHandle.Data->Period = IntoxicationComp->GetRateIncrease() * UGameplayStatics::GetGlobalTimeDilation(GetOwningActorFromActorInfo());
	UE_LOG(LogTemp, Display, TEXT("%f"), SpecHandle.Data->GetPeriod());
	EffectHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());*/

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UIncreaseIntoxicationAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
											  const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (EffectHandle.IsValid())
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(EffectHandle);
		EffectHandle.Invalidate();
	}
}

void UIncreaseIntoxicationAbility::CustomTick()
{
	const double Now = FPlatformTime::Seconds();
	if (Now - LastTime >= Period)
	{
		ExecuteIntoxication();
		LastTime = Now;
	}
}

void UIncreaseIntoxicationAbility::ExecuteIntoxication()
{
	const auto& IntoxicationAttrib = IntoxicationComp->GetIntoxicationAttribute();
	const float NewIntoxication = IntoxicationComp->GetCurrentIntoxication() + IntoxicationComp->GetSpeedIncrease();
	GetAbilitySystemComponentFromActorInfo()->SetNumericAttributeBase(IntoxicationAttrib->GetIntoxicationAttribute(), NewIntoxication);
}
