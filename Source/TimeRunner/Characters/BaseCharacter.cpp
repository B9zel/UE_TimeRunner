#include "TimeRunner/Characters/BaseCharacter.h"
#include <Abilities/GameplayAbility.h>
#include "Data/DataAssets/ConfigAbilityDataAsset.h"
#include <AbilitySystem/Component/TRAbilitySystemComponent.h>

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilityComponent = CreateDefaultSubobject<UTRAbilitySystemComponent>(TEXT("Ability system component"));
	check(AbilityComponent);
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilityComponent;
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetAbilitySystemComponent()->AddLooseGameplayTags(GameplayTags);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Ability : PreStartAbilties->Abilities)
	{
		FGameplayAbilitySpec Spec(Ability);
		GetAbilitySystemComponent()->GiveAbility(Spec);
	}

	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);
}
