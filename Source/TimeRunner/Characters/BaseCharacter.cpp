#include "TimeRunner/Characters/BaseCharacter.h"
#include <Abilities/GameplayAbility.h>
#include "Data/DataAssets/ConfigAbilityDataAsset.h"
#include <AbilitySystem/Component/TRAbilitySystemComponent.h>

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!GetAbilitySystemComponent()) return;

	GetAbilitySystemComponent()->AddLooseGameplayTags(GameplayTags);

	if (!PreStartAbilties) return;

	for (auto& Ability : PreStartAbilties->Abilities)
	{
		FGameplayAbilitySpec Spec(Ability.Ability);
		if (Ability.IsAutoActivate)
		{
			GetAbilitySystemComponent()->GiveAbilityAndActivateOnce(Spec);
		}
		else
		{
			GetAbilitySystemComponent()->GiveAbility(Spec);
		}
	}
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}
