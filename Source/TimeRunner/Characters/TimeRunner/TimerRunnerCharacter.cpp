
#include "TimeRunner/Characters/TimeRunner/TimerRunnerCharacter.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Components/InputComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "TimeRunner/Components/Health/HealthComponent.h"
#include "TimeRunner/Components/Intoxication/IntoxicationComponent.h"

ATimerRunnerCharacter::ATimerRunnerCharacter()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetMesh());

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	IntoxicationComponent = CreateDefaultSubobject<UIntoxicationComponent>(TEXT("Intoxication component"));

	check(HealthComponent);
	check(IntoxicationComponent);

	m_IsTimeDilation = false;
}

void ATimerRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (auto* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (!PlayerController->GetLocalPlayer()) return;
		if (auto* InputSubSystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputObjects.InputContext)
			{
				InputSubSystem->AddMappingContext(InputObjects.InputContext, 0);
			}
		}
	}

	auto* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	check(Input);

	Input->BindAction(InputObjects.RunAction, ETriggerEvent::Triggered, this, &ATimerRunnerCharacter::InputRunTriggerCharacter);
	Input->BindAction(InputObjects.RunAction, ETriggerEvent::Started, this, &ATimerRunnerCharacter::InputRunStartCharacter);
	Input->BindAction(InputObjects.RunAction, ETriggerEvent::Completed, this, &ATimerRunnerCharacter::InputRunCompletedCharacter);
	Input->BindAction(InputObjects.LookActions, ETriggerEvent::Triggered, this, &ATimerRunnerCharacter::InputLookCharacter);
	Input->BindAction(InputObjects.JumpActions, ETriggerEvent::Started, this, &ATimerRunnerCharacter::InputJumpStartCharacter);
	Input->BindAction(InputObjects.JumpActions, ETriggerEvent::Completed, this, &ATimerRunnerCharacter::InputJumpCompletedCharacter);
}

void ATimerRunnerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (GetIsTimeDilation())
	{
		ApplyTimeDilation();
	}
}

void ATimerRunnerCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
															 const FVector& PreviousLocation, float TimeDelta)
{
	Super::OnWalkingOffLedge_Implementation(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation, TimeDelta);

	if (GetIsTimeDilation())
	{
		ResetTimeDilation();
		Launch(GetCharacterMovement()->Velocity.GetSafeNormal(), 0.0f);
	}
}

void ATimerRunnerCharacter::InputRunTriggerCharacter(const FInputActionInstance& Instance)
{
	const FVector2D& Direction = Instance.GetValue().Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), Direction.X);
	AddMovementInput(GetActorRightVector(), Direction.Y);
}

void ATimerRunnerCharacter::InputRunStartCharacter(const FInputActionInstance& Instance)
{
	if (!GetCharacterMovement()->IsWalking()) return;

	ApplyTimeDilation();

	m_IsTimeDilation = true;
}

void ATimerRunnerCharacter::InputRunCompletedCharacter(const FInputActionInstance& Instance)
{
	if (UGameplayStatics::GetGlobalTimeDilation(this) != GetRunWorldTime()) return;

	ResetTimeDilation();

	m_IsTimeDilation = false;
}

void ATimerRunnerCharacter::InputLookCharacter(const FInputActionInstance& Instance)
{
	const FVector2D& Direction = Instance.GetValue().Get<FVector2D>();

	AddControllerYawInput(Direction.X);
	AddControllerPitchInput(Direction.Y);
}

void ATimerRunnerCharacter::InputJumpStartCharacter(const FInputActionInstance& Instance)
{
	Jump();

	if (GetIsTimeDilation())
	{
		ResetTimeDilation();
		Launch(GetCharacterMovement()->Velocity.GetSafeNormal(), m_ZForceLaunch);
	}
}

void ATimerRunnerCharacter::InputJumpCompletedCharacter(const FInputActionInstance& Instance)
{
	StopJumping();
}

void ATimerRunnerCharacter::ApplyTimeDilation()
{
	FTimerHandle timer;
	UGameplayStatics::SetGlobalTimeDilation(this, GetRunWorldTime());

	IntoxicationComponent->ActivateIntixication();

	// Call ApplySettingDilation next frame
	GetWorld()->GetTimerManager().SetTimer(timer, this, &ATimerRunnerCharacter::ApplySettingDilation, UE_SMALL_NUMBER, false);
}

void ATimerRunnerCharacter::ResetTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(this, m_WalkWorldTime);
	CustomTimeDilation = 1.0f;

	IntoxicationComponent->DeactivateIntixication();
}

void ATimerRunnerCharacter::Launch(const FVector& Direction, const float ZForce)
{
	FVector ForceLaunch = Direction * m_ForceLaunch;
	ForceLaunch.Z += ZForce;

	LaunchCharacter(ForceLaunch, true, true);
}

void ATimerRunnerCharacter::ApplySettingDilation()
{
	CustomTimeDilation = 1.0f / GetRunWorldTime();
}

inline float ATimerRunnerCharacter::GetRunWorldTime() const
{
	return m_RunWorldTime;
}

inline float ATimerRunnerCharacter::GetWalkWorldTime() const
{
	return m_WalkWorldTime;
}

inline const FInput& ATimerRunnerCharacter::GetInputObject() const
{
	return InputObjects;
}

inline bool ATimerRunnerCharacter::GetIsTimeDilation() const
{
	return m_IsTimeDilation;
}

void ATimerRunnerCharacter::SetRunWorldTime(const float NewTime)
{
	m_RunWorldTime = FMath::Clamp(NewTime, 0.0f, 1.0f);
}

void ATimerRunnerCharacter::SetWalkWorldTime(const float NewTime)
{
	m_WalkWorldTime = FMath::Clamp(NewTime, 0.0f, 1.0f);
}