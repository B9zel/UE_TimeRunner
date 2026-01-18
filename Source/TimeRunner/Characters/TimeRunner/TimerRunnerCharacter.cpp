
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
#include <Global/GameInstance/MainGameInstance.h>
#include <AbilitySystem/Attributes/Health/HealthAttributeSet.h>
#include "Components/RunWall/RunWallComponent.h"
#include "Components/Countermotion/CountermotionComponent.h"
#include "AbilitySystem/Attributes/Damage/DamageAttributeSet.h"

ATimerRunnerCharacter::ATimerRunnerCharacter()
{
	ArrowDirectionComponent = CreateDefaultSubobject<UArrowComponent>("ArrowDirection");
	ArrowDirectionComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(GetRootComponent());

	SkeletonMeshUpBody = CreateDefaultSubobject<USkeletalMeshComponent>("Skeleton up body");
	SkeletonMeshUpBody->SetupAttachment(CameraComponent);

	KatanaMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Katana mesh");
	KatanaMeshComponent->SetupAttachment(SkeletonMeshUpBody);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("Health component");
	IntoxicationComponent = CreateDefaultSubobject<UIntoxicationComponent>("Intoxication component");
	DilationComponent = CreateDefaultSubobject<UTimeDilationComponent>("Dilation component");
	RunWallComponent = CreateDefaultSubobject<URunWallComponent>("Wall run component");
	CountermotionComponent = CreateDefaultSubobject<UCountermotionComponent>("Countermotion component");

	check(HealthComponent);
	check(IntoxicationComponent);
	check(DilationComponent);
	check(RunWallComponent);
	check(CountermotionComponent);

	GetCharacterMovement()->bNotifyApex = true;
	m_IsInputMove = false;
}

void ATimerRunnerCharacter::PreRegisterAllComponents()
{
	Super::PreRegisterAllComponents();

	FAttachmentTransformRules Rules(EAttachmentRule::KeepRelative, false);
	KatanaMeshComponent->AttachToComponent(SkeletonMeshUpBody, Rules, SocketKatana);

	RunWallComponent->SetArrowComponent(ArrowDirectionComponent);
	RunWallComponent->ActivateRunWallDispatcher.AddUniqueDynamic(this, &ThisClass::OnActivetedRunWall);
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
	Input->BindAction(InputObjects.OldestStateAction, ETriggerEvent::Started, this, &ATimerRunnerCharacter::InputOldestBackStartCharacter);
	Input->BindAction(InputObjects.DashAction, ETriggerEvent::Started, this, &ATimerRunnerCharacter::InputDashStartCharacter);
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
}

void ATimerRunnerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (GetIsInputMove())
	{
		Execute_ApplyTimeDilation(this);
	}
	Execute_StopRunWall(this, true);
	WasDashInAir = false;
}

void ATimerRunnerCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal, const FVector& PreviousFloorContactNormal,
															 const FVector& PreviousLocation, float TimeDelta)
{
	Super::OnWalkingOffLedge_Implementation(PreviousFloorImpactNormal, PreviousFloorContactNormal, PreviousLocation, TimeDelta);

	if (DilationComponent->GetIsTimeDilation())
	{
		Execute_ResetTimeDilation(this);
	}
	Execute_StopRunWall(this, false);
}

void ATimerRunnerCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();

	GetCharacterMovement()->bNotifyApex = true;
}

// Input begin
void ATimerRunnerCharacter::InputRunTriggerCharacter(const FInputActionInstance& Instance)
{
	const FVector2D& Direction = Instance.GetValue().Get<FVector2D>();
	RunWallComponent->SetCurrentDirectionTrace(Direction.X > 0 ? Direction.Y : 0.0f);

	AddMovementInput(GetActorForwardVector(), Direction.X);
	AddMovementInput(GetActorRightVector(), Direction.Y);
}

void ATimerRunnerCharacter::InputRunStartCharacter(const FInputActionInstance& Instance)
{
	SetIsInputMove(true);
	if (!GetCharacterMovement()->IsWalking() || GetCharacterMovement()->IsCrouching()) return;

	Execute_ApplyTimeDilation(this);
}

void ATimerRunnerCharacter::InputRunCompletedCharacter(const FInputActionInstance& Instance)
{
	// if (UGameplayStatics::GetGlobalTimeDilation(this) != DilationComponent->GetRunWorldTime()) return;
	SetIsInputMove(false);
	Execute_ResetTimeDilation(this);
}

void ATimerRunnerCharacter::InputLookCharacter(const FInputActionInstance& Instance)
{
	const FVector2D& Direction = Instance.GetValue().Get<FVector2D>();

	AddControllerYawInput(Direction.X);
	AddControllerPitchInput(Direction.Y);
}

void ATimerRunnerCharacter::InputJumpStartCharacter(const FInputActionInstance& Instance)
{
	// Check is you have complete all the neccessary action
	if (!RunWallComponent->GetIsWallRunning()) // CalculateCanRunWall(ResRight) &&
	{
		Execute_StartRunWall(this);
		Jump();
	}
	// else if (DilationComponent->GetIsTimeDilation() && !CanRunWall)
	//{
	//	Execute_ResetTimeDilation(this);

	//	Launch(GetCharacterMovement()->Velocity.GetSafeNormal(), ZForceLaunch);
	//}
	// Jump off the wall
	else if (RunWallComponent->GetCanWallRun() && RunWallComponent->GetIsWallRunning())
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (GetActorForwardVector().RotateAngleAxis(45, FVector(0, 0, 1) * 200)), FColor::Blue,
					  false, 10);

		Execute_StopRunWall(this, false);
		RunWallComponent->JumpOffWall();
	}

	Execute_ResetTimeDilation(this);
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
			Execute_ApplyTimeDilation(this);
		}
	}
	else if (!RunWallComponent->GetIsWallRunning())
	{
		Crouch();
		Execute_ResetTimeDilation(this);
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

		FGameplayEventData EventData;
		EventData.Target = Hit.GetActor();
		EventData.EventMagnitude = Damage ? Damage->GetDamage() : 0.0f;
		GetAbilitySystemComponent()->HandleGameplayEvent(EventApplyDamage, &EventData);
	}
}

void ATimerRunnerCharacter::InputOldestBackStartCharacter(const FInputActionInstance& Instance)
{
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(ApplyOldestStateAbilityTag);
}

void ATimerRunnerCharacter::InputJumpCompletedCharacter(const FInputActionInstance& Instance)
{
	StopJumping();
}

void ATimerRunnerCharacter::InputDashStartCharacter(const FInputActionInstance& Instance)
{
	ActivateDash();
}

// Input end

void ATimerRunnerCharacter::ActivateDash()
{
	if (WasDashInAir) return;

	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(DashTagAbility);
	if (GetCharacterMovement()->IsFalling())
	{
		WasDashInAir = true;
	}
}

void ATimerRunnerCharacter::OnChangeLevelOfSpeed(const ELevelSpeed NewSpeed)
{
	if (DilationComponent->GetIsTimeDilation())
	{
		IntoxicationComponent->RestartIntoxication();
	}
}


void ATimerRunnerCharacter::StartRunWall_Implementation()
{
	RunWallComponent->StartRunWall();
}

void ATimerRunnerCharacter::StopRunWall_Implementation(const bool DisableCanWall)
{
	RunWallComponent->StopRunWall(DisableCanWall);
}

void ATimerRunnerCharacter::SetOldestState(const FCountermotionData& Data)
{	
	SetActorLocation(Data.Location);
	GetController()->SetControlRotation(Data.Rotation);
	GetAbilitySystemComponent()->SetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute(), Data.Health);
}

void ATimerRunnerCharacter::BPSetOldestState_Implementation(const FCountermotionData& Data)
{
	SetOldestState(Data);
}

void ATimerRunnerCharacter::StopWallRun_Timer()
{
	Execute_StopRunWall(this, false);
}

void ATimerRunnerCharacter::OnActivetedRunWall()
{
	WasDashInAir = false;
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

float ATimerRunnerCharacter::GetTimeActiveRunWall_Implementation()
{
	return RunWallComponent->GetTimeWallRun();
}

void ATimerRunnerCharacter::ApplyTimeDilation_Implementation()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Emerald, TEXT(__FUNCSIG__));
	DilationComponent->EnableTimeDilation();
	IntoxicationComponent->ActivateIntixication();
}

void ATimerRunnerCharacter::ResetTimeDilation_Implementation()
{
	// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Magenta, TEXT(__FUNCSIG__));
	DilationComponent->DisableTimeDilation();
	IntoxicationComponent->DeactivateIntixication();
}
