// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Pools/BulletPool/BulletPool.h"
#include "TimeRunner/Consumable/Bullets/BaseBullet.h"

UBulletPool::UBulletPool()
{
	IsInitialize = false;
	CountInitObjects = 100;
}

void UBulletPool::FinishDestroy()
{
	Super::FinishDestroy();

	for (int64 i = 0; i < m_FreeObjects.Num(); i++)
	{
		m_FreeObjects[i]->Destroy();
	}

	while (!m_UsedObjects.IsEmpty())
	{
		auto It = m_UsedObjects.begin();
		(*It)->Destroy();
		m_UsedObjects.Remove(It.GetId());
	}
}

void UBulletPool::InitializePool()
{
	if (IsInitialize) return;

	for (int64 i = 0; i < CountInitObjects; i++)
	{
		m_FreeObjects.Add(GetWorld()->SpawnActorDeferred<ABaseBullet>(BulletClass, FTransform::Identity));
	}

	IsInitialize = true;
}

ABaseBullet* UBulletPool::RequestBulletObject(AActor* Owner, APawn* Instigator, const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
	TObjectPtr<ABaseBullet> Bullet = m_FreeObjects.Pop(EAllowShrinking::No);
	Bullet->EnableObject();
	Bullet->SetInstigator(Instigator);
	Bullet->SetOwner(Owner);

	m_UsedObjects.Add(Bullet);
	Bullet->FinishSpawning(FTransform(Rotation, Location, Scale));
	return Bullet;
}

void UBulletPool::GiveBulletObject(ABaseBullet* Bullet)
{
	if (auto* Object = m_UsedObjects.Find(Bullet))
	{
		Bullet->DisableObject();
		m_UsedObjects.Remove(Bullet);
		m_FreeObjects.Add(Bullet);
	}
}
