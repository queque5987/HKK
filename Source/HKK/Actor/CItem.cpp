#include "Actor/CItem.h"
#include "Components/StaticMeshComponent.h"
#include "Component/CPickableComponent.h"
#include "Component/CWearableComponent.h"


ACItem::ACItem()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	PickableComponent = CreateDefaultSubobject<UCPickableComponent>(TEXT("PickableComponent"));
	WearableComponent = CreateDefaultSubobject<UCWearableComponent>(TEXT("WearableComponent"));

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	StaticMeshComponent->SetCustomDepthStencilValue((int32)ECustomStencilValue::ECSV_Item);
	StaticMeshComponent->SetRenderCustomDepth(true);
}

void ACItem::BeginPlay()
{
	Super::BeginPlay();
	SMCurrentRelativeLocation = StaticMeshComponent->GetRelativeLocation();

	if (PickableComponent != nullptr)
	{
		PickableComponent->SetBeginOverlapEvent(this);
	}
	ItemConfig.SpawnedItemActor = this;
}

void ACItem::Tick(float DeltaTime)
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

void ACItem::OnItemStencilValueChange(ECustomStencilValue CustomStencilValue)
{
	StaticMeshComponent->SetCustomDepthStencilValue((int32)CustomStencilValue);
}

FComponentBeginOverlapSignature* ACItem::GetComponentBeginOverlapSignature()
{
	return &StaticMeshComponent->OnComponentBeginOverlap;
}

FComponentEndOverlapSignature* ACItem::GetComponentEndOverlapSignature()
{
	return &StaticMeshComponent->OnComponentEndOverlap;
}
