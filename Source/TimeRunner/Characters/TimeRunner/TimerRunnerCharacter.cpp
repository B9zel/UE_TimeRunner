
#include "TimeRunner/Characters/TimeRunner/TimerRunnerCharacter.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <InputAction.h>
#include <InputMappingContext.h>
#include <Components/InputComponent.h>
#include <Camera/CameraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>

ATimerRunnerCharacter::ATimerRunnerCharacter()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetMesh());
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
}

void ATimerRunnerCharacter::InputRunTriggerCharacter(const FInputActionInstance& Instance)
{
	const FVector2D& Direction = Instance.GetValue().Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), Direction.X);
	AddMovementInput(GetActorRightVector(), Direction.Y);
	UE_LOG(LogTemp, Display, TEXT("Forward: %s"), *GetActorForwardVector().ToString());
	UE_LOG(LogTemp, Display, TEXT("Right: %s"), *GetActorRightVector().ToString());
}

void ATimerRunnerCharacter::InputRunStartCharacter(const FInputActionInstance& Instance)
{
	if (!GetCharacterMovement()->IsWalking()) return;

	FTimerHandle timer;
	UGameplayStatics::SetGlobalTimeDilation(this, GetRunWorldTime());

	// Call ApplySettingDilation next frame
	GetWorld()->GetTimerManager().SetTimer(timer, this, &ATimerRunnerCharacter::ApplySettingDilation, UE_SMALL_NUMBER, false);
}

void ATimerRunnerCharacter::InputRunCompletedCharacter(const FInputActionInstance& Instance)
{
	if (UGameplayStatics::GetGlobalTimeDilation(this) != GetRunWorldTime()) return;

	UGameplayStatics::SetGlobalTimeDilation(this, m_WalkWorldTime);
	CustomTimeDilation = 1.0f;
}

void ATimerRunnerCharacter::InputLookCharacter(const FInputActionInstance& Instance)
{
	const FVector2D& Direction = Instance.GetValue().Get<FVector2D>();

	AddControllerYawInput(Direction.X);
	AddControllerPitchInput(Direction.Y);
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

void ATimerRunnerCharacter::SetRunWorldTime(const float NewTime)
{
	m_RunWorldTime = FMath::Clamp(NewTime, 0.0f, 1.0f);
}

void ATimerRunnerCharacter::SetWalkWorldTime(const float NewTime)
{
	m_WalkWorldTime = FMath::Clamp(NewTime, 0.0f, 1.0f);
}