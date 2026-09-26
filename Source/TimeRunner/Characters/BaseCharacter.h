
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include <AbilitySystemInterface.h>
#include "Data/DataAssets/ConfigAbilityDataAsset.h"
#include "BaseCharacter.generated.h"

class IAbilitySystemInterface;
class UTRAbilitySystemComponent;
class UHealthAttributeSet;
class UConfigAbilityDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilitySystemInitializedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFullInitializeCharacter, ABaseCharacter*, Character);

UCLASS(Abstract)
class TIMERUNNER_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();

public:

	UFUNCTION(BlueprintPure)
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;

	bool IsAbilitySystemInitialized() const;
	FAbilitySystemInitializedDelegate& GetAbilitySystemInitializedDelegate();

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void HandleAbilitySystemInitialized();
	void InitializeAbilitySystem();
	void ClearStartupAbilities();

public:

	UPROPERTY(BlueprintAssignable)
	FFullInitializeCharacter OnFullInitialize;

protected:

	UPROPERTY(EditAnywhere, Category = "Parameters|Attack")
	FGameplayTag EventApplyDamage;

private:

	UPROPERTY(EditAnywhere, Category = "Parameters")
	FGameplayTagContainer GameplayTags;
	UPROPERTY(EditAnywhere, Category = "Parameters")
	TObjectPtr<UConfigAbilityDataAsset> PreStartAbilties;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FAbilitySystemInitializedDelegate AbilitySystemInitializedDelegate;
	FAbilitySetGrantedHandles StartupAbilitySetHandles;

	bool bAbilitySystemInitialized{false};
	bool bStartupAbilitiesGranted{false};
	TWeakObjectPtr<UAbilitySystemComponent> StartupAbilitySystem;
	/*TArray<FGameplayAbilitySpecHandle> StartupAbilityHandles;*/
};
