#include "Component/WearableComponent.h"


UWearableComponent::UWearableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWearableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWearableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

