#include "Actor/Projectile.h"
#include "Component/FoliageInteractSourceComponent.h"
#include "Components/StaticMeshComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	FoliageInteractSourceComponent = CreateDefaultSubobject<UFoliageInteractSourceComponent>(TEXT("FoliageInteractSourceComponent"));
	FoliageInteractSourceComponent->SetupAttachment(StaticMeshComponent);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	FoliageInteractSourceComponent->AttachToComponent(StaticMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (DebugCurrentLifeTime > DebugLifeTime)
	{
		SetActorLocation(GetActorLocation() - FVector(0.f, 0.f, 10000.f));
		Destroy();
	}
	else StaticMeshComponent->AddImpulse(GetActorForwardVector() * DebugProjectSpeed);
	DebugCurrentLifeTime += DeltaTime;
}

