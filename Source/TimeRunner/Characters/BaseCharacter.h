
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include <AbilitySystemInterface.h>
#include "BaseCharacter.generated.h"

class IAbilitySystemInterface;
class UTRAbilitySystemComponent;
class UHealthAttributeSet;
class UConfigAbilityDataAsset;

UCLASS(Abstract)
class TIMERUNNER_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();

public:

	UFUNCTION(BlueprintPure)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const;

	virtual void PostInitializeComponents() override;

protected:

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere, Category = "Parameters|Attack")
	FGameplayTag EventApplyDamage;

private:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTRAbilitySystemComponent> AbilityComponent;

	UPROPERTY(EditAnywhere, Category = "Parameters")
	FGameplayTagContainer GameplayTags;
	UPROPERTY(EditAnywhere, Category = "Parameters")
	TObjectPtr<UConfigAbilityDataAsset> PreStartAbilties;
};
