#include "TimeRunner/Characters/BaseCharacter.h"
#include <Abilities/GameplayAbility.h>
#include <AbilitySystem/Component/TRAbilitySystemComponent.h>
#include "GameFramework/PlayerState.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeAbilitySystem();

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (!AbilitySystemComponent || bStartupAbilitiesGranted) return;

	AbilitySystemComponent->AddLooseGameplayTags(GameplayTags);
	StartupAbilitySystem = AbilitySystemComponent;
	bStartupAbilitiesGranted = true;

	if (!PreStartAbilties) return;

	PreStartAbilties->GiveToAbilitySystem(AbilitySystemComponent, &StartupAbilitySetHandles);

	// for (auto& Ability : PreStartAbilties->Abilities)
	//{
	//	if (!Ability.Ability) continue;
	//	FGameplayAbilitySpec Spec(Ability.Ability);
	//	if (Ability.IsAutoActivate)
	//	{
	//		StartupAbilityHandles.Add(AbilitySystemComponent->GiveAbilityAndActivateOnce(Spec));
	//	}
	//	else
	//	{
	//		StartupAbilityHandles.Add(AbilitySystemComponent->GiveAbility(Spec));
	//	}
	// }
}

void ABaseCharacter::UnPossessed()
{
	ClearStartupAbilities();
	Super::UnPossessed();
}

void ABaseCharacter::ClearStartupAbilities()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	StartupAbilitySetHandles.ClearHandles(ASC);
}

void ABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearStartupAbilities();
	Super::EndPlay(EndPlayReason);
}

void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeAbilitySystem();
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitializeAbilitySystem();
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

bool ABaseCharacter::IsAbilitySystemInitialized() const
{
	return bAbilitySystemInitialized;
}

FAbilitySystemInitializedDelegate& ABaseCharacter::GetAbilitySystemInitializedDelegate()
{
	return AbilitySystemInitializedDelegate;
}

void ABaseCharacter::HandleAbilitySystemInitialized()
{
}

void ABaseCharacter::InitializeAbilitySystem()
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (!AbilitySystemComponent) return;

	AActor* AbilityOwner = GetPlayerState();
	if (!AbilityOwner)
	{
		AbilityOwner = this;
	}

	if (bAbilitySystemInitialized && AbilitySystemComponent->GetOwnerActor() == AbilityOwner && AbilitySystemComponent->GetAvatarActor() == this)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(AbilityOwner, this);
	bAbilitySystemInitialized = true;
	HandleAbilitySystemInitialized();
	AbilitySystemInitializedDelegate.Broadcast();
}
