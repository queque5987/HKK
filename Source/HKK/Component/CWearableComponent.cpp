#include "Component/CWearableComponent.h"


UCWearableComponent::UCWearableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCWearableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCWearableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

