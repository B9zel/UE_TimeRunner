
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
#include "AbilitySystemComponent.h"
#include <GameplayTagContainer.h>
#include <Components/ArrowComponent.h>

ATimerRunnerCharacter::ATimerRunnerCharacter()
{
	ArrowDirectionComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowDirection"));
	ArrowDirectionComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(GetRootComponent());

	SkeletonMeshUpBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeleton up body"));
	SkeletonMeshUpBody->SetupAttachment(CameraComponent);

	KatanaMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Katana mesh"));
	KatanaMeshComponent->SetupAttachment(SkeletonMeshUpBody);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health component"));
	IntoxicationComponent = CreateDefaultSubobject<UIntoxicationComponent>(TEXT("Intoxication component"));
	DilationComponent = CreateDefaultSubobject<UTimeDilationComponent>(TEXT("Dilation component"));

	check(HealthComponent);
	check(IntoxicationComponent);
	check(DilationComponent);

	GetCharacterMovement()->bNotifyApex = true;

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
	Input->BindAction(InputObjects.AttackAction, ETriggerEvent::Started, this, &ATimerRunnerCharacter::InputAttackStartedCharacter);
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

void ATimerRunnerCharacter::PreRegisterAllComponents()
{
	Super::PreRegisterAllComponents();

	FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
	KatanaMeshComponent->AttachToComponent(SkeletonMeshUpBody, Rules, SocketKatana);
}

void ATimerRunnerCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();

	GetCharacterMovement()->bNotifyApex = true;
}

void ATimerRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (DilationComponent)
	{
		DilationComponent->ChanageSpeedDelegate.AddDynamic(this, &ATimerRunnerCharacter::OnChangeLevelOfSpeed);
	}
}

void ATimerRunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanRunWall)
	{
		if (!CalculateCanRunWall(ResRight))
		{
			CanRunWall = false;
			IsWallRunning = false;
			GetAbilitySystemComponent()->CancelAbilities(&RunWallTagAbility);
			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Blue, FString::Printf(TEXT("Cancel run Ability")));
		}
		else if (!IsWallRunning)
		{
			GetAbilitySystemComponent()->TryActivateAbilitiesByTag(RunWallTagAbility);
			IsWallRunning = true;
			GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Blue, FString::Printf(TEXT("Apply run Ability")));
		}
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, FString::Printf(TEXT("Check runAbility")));
	}
}

void ATimerRunnerCharacter::InputRunTriggerCharacter(const FInputActionInstance& Instance)
{
	const FVector2D& Direction = Instance.GetValue().Get<FVector2D>();
	if (Direction.X > 0)
	{
		DirectionRight = Direction.Y;
	}
	else
	{
		DirectionRight = 0.0f;
	}

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

	if (CalculateCanRunWall(ResRight))
	{
		CanRunWall = true;
		//	IsWallRunning = true;
		// GetAbilitySystemComponent()->TryActivateAbilitiesByTag(RunWallTagAbility);
	}
	else if (DilationComponent->GetIsTimeDilation())
	{
		ResetTimeDilation();
		Launch(GetCharacterMovement()->Velocity.GetSafeNormal(), ZForceLaunch);
	}
	Jump();
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
	else if (!IsWallRunning)
	{
		Crouch();
		ResetTimeDilation();
	}
}

void ATimerRunnerCharacter::InputAttackStartedCharacter(const FInputActionInstance& Instance)
{
	TArray<FHitResult> HitRes;
	TArray<AActor*> Ignore;
	UKismetSystemLibrary::CapsuleTraceMulti(
		this, CameraComponent->GetComponentLocation(), CameraComponent->GetComponentLocation() + CameraComponent->GetForwardVector() * DistanceAttack, 20, 40,
		UCollisionProfile::Get()->ConvertToTraceType(ChannelAttack), false, Ignore, EDrawDebugTrace::ForDuration, HitRes, true);
	// GetWorld()->LineTraceMultiByChannel(HitRes, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * DistanceAttack, ChannelAttack);
	for (auto& Hit : HitRes)
	{
	}
	// GetWorld()->LineTraceMultiByChannel(HitRes, GetActorLocation(), GetActorLocation() + GEtForwardActorLOcation)
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

bool ATimerRunnerCharacter::CalculateCanRunWall(FHitResult& Res)
{
	if (FMath::IsNearlyZero(GetVelocity().Size2D(), MinWallRunSpeed)) return false;

	FVector Right = ArrowDirectionComponent->GetForwardVector() * DirectionRight;
	TArray<AActor*> Ignore;
	bool IsHit = UKismetSystemLibrary::LineTraceSingleForObjects(this, ArrowDirectionComponent->GetComponentLocation(),
																 ArrowDirectionComponent->GetComponentLocation() + Right * LengthTraceRunWall, TypeChannelTrace,
																 false, Ignore, EDrawDebugTrace::ForDuration, Res, true);

	return IsHit;
}

void ATimerRunnerCharacter::Launch(const FVector& Direction, const float ZForce)
{
	FVector LocForceLaunch = Direction * ForceLaunch;
	LocForceLaunch.Z += ZForce;

	LaunchCharacter(LocForceLaunch, true, true);
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