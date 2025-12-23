#include "Actor/Item.h"
#include "Components/StaticMeshComponent.h"
#include "Component/PickableComponent.h"
#include "Component/WearableComponent.h"
#include "Components/SphereComponent.h"


AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	PickableComponent = CreateDefaultSubobject<UPickableComponent>(TEXT("PickableComponent"));
	WearableComponent = CreateDefaultSubobject<UWearableComponent>(TEXT("WearableComponent"));
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));

	Collider->SetSphereRadius(100.f);
	Collider->SetupAttachment(StaticMeshComponent);

	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	Collider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	StaticMeshComponent->SetCustomDepthStencilValue((int32)ECustomStencilValue::ECSV_Item);
	StaticMeshComponent->SetRenderCustomDepth(true);

}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	SMCurrentRelativeLocation = StaticMeshComponent->GetRelativeLocation();

	if (PickableComponent != nullptr)
	{
		PickableComponent->SetBeginOverlapEvent(this);
	}
	ItemConfig.SpawnedItemActor = this;
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFloating)
	{
		fElipsedTime += DeltaTime * fFloatingSpeed;
		const float HeightOffset = (FMath::Sin(fElipsedTime) + 1.f) / 2.f * fMaxFloatingHeight;
		StaticMeshComponent->SetRelativeLocation(SMCurrentRelativeLocation + FVector(0.f, 0.f, HeightOffset));
	}
	if (bRotating)
	{
		const float AnglePerTick = DeltaTime * fRotatingSpeed;
		const FQuat NewRotation{ FVector(0.f, 0.f, 1.f), FMath::DegreesToRadians(AnglePerTick)};
		StaticMeshComponent->AddRelativeRotation(NewRotation);
	}
}

void AItem::OnItemStencilValueChange(ECustomStencilValue CustomStencilValue)
{
	StaticMeshComponent->SetCustomDepthStencilValue((int32)CustomStencilValue);
}

FComponentBeginOverlapSignature* AItem::GetComponentBeginOverlapSignature()
{
	return &Collider->OnComponentBeginOverlap;
}

FComponentEndOverlapSignature* AItem::GetComponentEndOverlapSignature()
{
	return &Collider->OnComponentEndOverlap;
}
