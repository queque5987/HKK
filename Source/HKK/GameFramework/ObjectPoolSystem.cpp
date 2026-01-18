#include "GameFramework/ObjectPoolSystem.h"

AActor* UObjectPoolSystem::GetFromPool(UClass* InClass)
{
	TSharedPtr<FActorPool>* ClassActorPoolPtr = ObjectPool.Find(InClass);
	if (ClassActorPoolPtr) UE_LOG(LogTemp, Log, TEXT("Class: %s | Pool Size: %d"), *InClass->GetName(), (*ClassActorPoolPtr)->CurrentSize);
	if (!ClassActorPoolPtr)
	{
		ClassActorPoolPtr = &(ObjectPool.Add(InClass, MakeShared<FActorPool>()));
	}
	TObjectPtr<AActor> OutActor;
	if ((*ClassActorPoolPtr)->Dequeue(OutActor))
	{
		if (IsValid(OutActor))
		{
			return OutActor;
		}
	}
	return GetWorld()->SpawnActor<AActor>(InClass);
}

bool UObjectPoolSystem::ReturnToPool(UClass* InClass, AActor* InActor)
{
	if (!IsValid(InActor)) return false;

	TSharedPtr<FActorPool>* ClassActorPoolPtr = ObjectPool.Find(InClass);
	if (!ClassActorPoolPtr)
	{
		ClassActorPoolPtr = &(ObjectPool.Add(InClass, MakeShared<FActorPool>()));
	}
	InActor->SetActorLocation(FVector(0.f, 0.f, -99999.f));
	return (*ClassActorPoolPtr)->Enqueue(InActor);
}
