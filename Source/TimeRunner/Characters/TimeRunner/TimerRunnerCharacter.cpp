
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
#include "TimeRunner/Components/TimeDilation/TimeDilationComponent.h"

ATimerRunnerCharacter::ATimerRunnerCharacter()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetMesh());

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	IntoxicationComponent = CreateDefaultSubobject<UIntoxicationComponent>(TEXT("Intoxication component"));
	DilationComponent = CreateDefaultSubobject<UTimeDilationComponent>(TEXT("Dilation component"));

	check(HealthComponent);
	check(IntoxicationComponent);
	check(DilationComponent);

	m_IsInputMove = false;
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
	Input->BindAction(InputObjects.SwitchSpeedActions, ETriggerEvent::Triggered, this, &ATimerRunnerCharacter::InputSwitchSpeedTriggerCharacter);
	Input->BindAction(InputObjects.CrouchActions, ETriggerEvent::Started, this, &ATimerRunnerCharacter::InputCrouchStartCharacter);
}

void ATimerRunnerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (GetIsInputMove())
	{
		ApplyTimeDilation();
	}
}

void ATimerRunnerCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
															 const FVector& PreviousLocation, float TimeDelta)
{
	Super::OnWalkingOffLedge_Implementation(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation, TimeDelta);

	if (DilationComponent->GetIsTimeDilation())
	{
		ResetTimeDilation();
		Launch(GetCharacterMovement()->Velocity.GetSafeNormal(), 0.0f);
	}
}

void ATimerRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (DilationComponent)
	{
		DilationComponent->ChanageSpeedDelegate.AddDynamic(this, &ATimerRunnerCharacter::OnChangeLevelOfSpeed);
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
	SetIsInputMove(true);
	if (!GetCharacterMovement()->IsWalking() || GetCharacterMovement()->IsCrouching()) return;

	ApplyTimeDilation();
}

void ATimerRunnerCharacter::InputRunCompletedCharacter(const FInputActionInstance& Instance)
{
	// if (UGameplayStatics::GetGlobalTimeDilation(this) != DilationComponent->GetRunWorldTime()) return;
	SetIsInputMove(false);
	ResetTimeDilation();
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

	/*if (DilationComponent->GetIsTimeDilation())
	{
		ResetTimeDilation();
		Launch(GetCharacterMovement()->Velocity.GetSafeNormal(), m_ZForceLaunch);
	}*/
}

void ATimerRunnerCharacter::InputJumpCompletedCharacter(const FInputActionInstance& Instance)
{
	StopJumping();
}

void ATimerRunnerCharacter::InputSwitchSpeedTriggerCharacter(const FInputActionInstance& Instance)
{
	const float DirectScroll = Instance.GetValue().Get<float>();
	if (DirectScroll > 0.0f)
	{
		DilationComponent->IncreaseSpeed();
	}
	else
	{
		DilationComponent->DecreaseSpeed();
	}
	// IntoxicationComponent->ActivateIntixication();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
									 FString::Printf(TEXT("%f     %f"), Instance.GetValue().Get<float>(), Instance.GetValue().Get<float>()));
}

void ATimerRunnerCharacter::InputCrouchStartCharacter(const FInputActionInstance& Instance)
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
		if (GetIsInputMove() && !GetCharacterMovement()->IsFalling())
		{
			ApplyTimeDilation();
		}
	}
	else
	{
		Crouch();
		ResetTimeDilation();
	}
}

void ATimerRunnerCharacter::ApplyTimeDilation()
{
	DilationComponent->EnableTimeDilation();
	IntoxicationComponent->ActivateIntixication();
}

void ATimerRunnerCharacter::ResetTimeDilation()
{
	DilationComponent->DisableTimeDilation();
	IntoxicationComponent->DeactivateIntixication();
}

void ATimerRunnerCharacter::Launch(const FVector& Direction, const float ZForce)
{
	FVector ForceLaunch = Direction * m_ForceLaunch;
	ForceLaunch.Z += ZForce;

	LaunchCharacter(ForceLaunch, true, true);
}

void ATimerRunnerCharacter::OnChangeLevelOfSpeed(const ELevelSpeed NewSpeed)
{
	if (DilationComponent->GetIsTimeDilation())
	{
		IntoxicationComponent->RestartIntoxication();
	}
}

inline const FInput& ATimerRunnerCharacter::GetInputObject() const
{
	return InputObjects;
}

bool ATimerRunnerCharacter::GetIsInputMove() const
{
	return m_IsInputMove;
}

void ATimerRunnerCharacter::SetIsInputMove(const bool InputMove)
{
	m_IsInputMove = InputMove;
}