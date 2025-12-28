#include "Component/FoliageInteractSourceComponent.h"
#include "Actor/FoliageInteractVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

UFoliageInteractSourceComponent::UFoliageInteractSourceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("FoliageCollider"));
	Collider->SetSphereRadius(150.f);
}


void UFoliageInteractSourceComponent::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFoliageInteractVolume::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (AFoliageInteractVolume* Volume = Cast<AFoliageInteractVolume>(Actor))
		{
			ToCheckFoliageVolumes.Add(Volume);
		}
	}
}


void UFoliageInteractSourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//UE_LOG(LogTemp, Log, TEXT("%s"), *GetComponentLocation().ToString());
	for (AFoliageInteractVolume* FIV :ToCheckFoliageVolumes)
	{
		if (FIV->IsInside(GetComponentLocation()))
		{
			FIV->SetImpulse(GetComponentLocation(), Collider->GetScaledSphereRadius());
		}
	}
}

