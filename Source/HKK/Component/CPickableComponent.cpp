#include "Component/CPickableComponent.h"


UCPickableComponent::UCPickableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCPickableComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCPickableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

