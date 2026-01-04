#include "Component/WearableComponent.h"


UWearableComponent::UWearableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWearableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWearableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

USceneComponent* UWearableComponent::GetAttachComponent()
{
	AActor* OwningActor = GetOwner();
	return OwningActor != nullptr ? OwningActor->GetRootComponent() : nullptr;
}

