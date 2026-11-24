// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/RunWall/RunWallComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ArrowComponent.h"
#include "AbilitySystem/Component/TRAbilitySystemComponent.h"

URunWallComponent::URunWallComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URunWallComponent::BeginPlay()
{
	Super::BeginPlay();
}

void URunWallComponent::OnFullInitOwner()
{
	OwnerCharacter = GetOwner<ACharacter>();
	check(OwnerCharacter.IsValid());

	if (auto* AbilityInterface = Cast<IAbilitySystemInterface>(OwnerCharacter))
	{
		OwnerAbilitySystem = Cast<UTRAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
	}
	check(OwnerAbilitySystem.IsValid());

	OwnerCharacter->LandedDelegate.AddDynamic(this, &ThisClass::LandedOwnerCharacter);
}

void URunWallComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CanRunWall)
	{
		const bool IsAvailibleWallRun = CalculateCanRunWall(WallTrace);
		if (!IsAvailibleWallRun && IsWallRunning)
		{
			DeactivateRunWall();
		}
		else if (WasJumpOffWall)
		{
			if (!IsAvailibleWallRun)
			{
				WasJumpOffWall = false;
			}
		}
		else if (!IsWallRunning && IsAvailibleWallRun)
		{
			ActivateRunWall();
		}
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("Check runAbility")));
	}
}

void URunWallComponent::DeactivateRunWall()
{
	if (IsWallRunning || OwnerAbilitySystem->IsActiveAbilityByTag(RunWallTagAbility.First()))
	{
		IsWallRunning = false;
		OwnerAbilitySystem->CancelAbilities(&RunWallTagAbility);
		SetCurrentDirectionTrace(0.0f);
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Blue, FString::Printf(TEXT("Cancel run Ability")));
		DeactivateRunWallDispatcher.Broadcast();
	}
}

void URunWallComponent::ActivateRunWall()
{
	if (OwnerAbilitySystem->TryActivateAbilitiesByTag(RunWallTagAbility))
	{
		IsWallRunning = true;
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Blue, FString::Printf(TEXT("Apply run Ability")));

		ActivateRunWallDispatcher.Broadcast();
	}
}

FVector URunWallComponent::GetCurrentDirectionTrace() const
{
	return ArrowOwnerComponent->GetForwardVector() * DirectionTrace;
}

bool URunWallComponent::CalculateCanRunWall(FHitResult& Res)
{
	if (FMath::IsNearlyZero(OwnerCharacter->GetVelocity().Size2D(), MinWallRunSpeed)) return false;
	FVector Right = GetCurrentDirectionTrace();
	TArray<AActor*> Ignore;
	bool IsHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, ArrowOwnerComponent->GetComponentLocation(),
																 ArrowOwnerComponent->GetComponentLocation() + Right * LengthTraceRunWall, TypeChannelTrace,
																 false, Ignore, EDrawDebugTrace::ForDuration, Res, true);

	return IsHit;
}

void URunWallComponent::StartRunWall()
{
	if (!IsActive()) return;

	CanRunWall = true;
}

void URunWallComponent::StopRunWall(const bool DisableCanWall)
{
	CanRunWall = DisableCanWall ? false : CanRunWall;
	IsWallRunning = false;
	OwnerAbilitySystem->CancelAbilities(&RunWallTagAbility);
}

bool URunWallComponent::GetIsWallRunning() const
{
	return IsWallRunning;
}

bool URunWallComponent::GetCanWallRun() const
{
	return CanRunWall;
}

bool URunWallComponent::GetWasJumpOffWall() const
{
	return WasJumpOffWall;
}

float URunWallComponent::GetTimeWallRun() const
{
	return TimeWallRun;
}

void URunWallComponent::JumpOffWall()
{
	FVector Direction = FMath::Lerp(WallTrace.ImpactNormal, OwnerCharacter->GetVelocity().GetSafeNormal2D(), PercentDirectlyJumpOffWall);
	Direction.Z = 0.0f;

	Launch(Direction, JumpFromWallForce, ZJumpFromWallForce);
	WasJumpOffWall = true;
}

void URunWallComponent::SetCurrentDirectionTrace(const float Direction)
{
	if (GetIsWallRunning() && FMath::IsNearlyZero(Direction)) return;

	DirectionTrace = FMath::RoundToInt(FMath::Clamp(Direction, -1, 1));
}

void URunWallComponent::SetArrowComponent(const UArrowComponent* Arrow)
{
	if (Arrow)
	{
		ArrowOwnerComponent = Arrow;
	}
}

bool URunWallComponent::IsRightDirection() const
{
	return GetOwner()->GetActorRightVector().Dot(GetCurrentDirectionTrace()) > 0 && GetOwner()->GetActorForwardVector().Dot(GetCurrentDirectionTrace()) > 0;
}

void URunWallComponent::Launch(const FVector& Direction, const float DirectionForce, const float ZForce)
{
	FVector LocForceLaunch = Direction * DirectionForce;
	LocForceLaunch.Z += ZForce;

	OwnerCharacter->LaunchCharacter(LocForceLaunch, true, true);
}

void URunWallComponent::LandedOwnerCharacter(const FHitResult& Hit)
{
	WasJumpOffWall = false;
}
