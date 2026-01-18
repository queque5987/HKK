#include "Actor/Item.h"
#include "Components/StaticMeshComponent.h"
#include "Component/PickableComponent.h"
#include "Component/WearableComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"


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

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AItem, bPickableItem);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	SMCurrentRelativeLocation = StaticMeshComponent->GetRelativeLocation();

	if (PickableComponent != nullptr)
	{
		PickableComponent->SetBeginOverlapEvent(this);
	}
	//ItemConfig.SpawnedItemActor = this;
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bPickableItem)
	{
		fElipsedTime += DeltaTime * fFloatingSpeed;
		const float HeightOffset = (FMath::Sin(fElipsedTime) + 1.f) / 2.f * fMaxFloatingHeight;
		StaticMeshComponent->SetRelativeLocation(SMCurrentRelativeLocation + FVector(0.f, 0.f, HeightOffset));

		const float AnglePerTick = DeltaTime * fRotatingSpeed;
		const FQuat NewRotation{ FVector(0.f, 0.f, 1.f), FMath::DegreesToRadians(AnglePerTick) };
		StaticMeshComponent->AddRelativeRotation(NewRotation);
	}
}

bool AItem::GetPickableItem_Implementation()
{
	return bPickableItem;
}

void AItem::SetPickableItem_Implementation(bool e)
{
	bPickableItem = e;
	bFloating = e;
	bRotating = e;
	//Server_SetPickableItem(e);
	if (HasAuthority()) OnRep_PickableItem();
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

void AItem::OnRep_PickableItem()
{
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetRelativeLocation(SMCurrentRelativeLocation);
		StaticMeshComponent->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void AItem::Server_SetPickableItem_Implementation(bool e)
{
	bPickableItem = e;
	bFloating = e;
	bRotating = e;
}
