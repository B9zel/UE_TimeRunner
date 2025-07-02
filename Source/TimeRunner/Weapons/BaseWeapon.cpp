// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/BaseWeapon.h"
#include "BaseWeapon.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseWeapon::Attack_Implementation()
{
}

void ABaseWeapon::StopAttack_Implementation()
{
}
