#include "Component/FoliageInteractSourceComponent.h"
#include "Actor/FoliageInteractVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

UFoliageInteractSourceComponent::UFoliageInteractSourceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("FoliageCollider"));
	
	Collider->SetSphereRadius(ColliderRadius);
	Collider->SetMobility(EComponentMobility::Movable);
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

void UFoliageInteractSourceComponent::OnAttachmentChanged()
{
	Super::OnAttachmentChanged();

	if (!GetWorld()) return;
	GetWorld()->GetTimerManager().ClearTimer(VelocityUpdateTimerHandle);
	CachedSkelMeshParent = nullptr;
	CachedVelocity = FVector::ZeroVector;

	USceneComponent* ParentComponent = GetAttachParent();
	if (ParentComponent == nullptr)
	{
		return;
	}

	if (USkeletalMeshComponent* SkelMeshParent = Cast<USkeletalMeshComponent>(ParentComponent))
	{
		CachedSkelMeshParent = SkelMeshParent;
		if (SocketName != NAME_None)
		{
			PreviousBoneLocation = SkelMeshParent->GetSocketLocation(SocketName);
			GetWorld()->GetTimerManager().SetTimer(
				VelocityUpdateTimerHandle,
				this,
				&UFoliageInteractSourceComponent::UpdateSocketVelocity,
				VelocityUpdateInterval,
				true);
		}
	}
	else if (UStaticMeshComponent* StaticMeshParent = Cast<UStaticMeshComponent>(ParentComponent))
	{
		CachedStaticMeshParent = StaticMeshParent;
		if (SocketName != NAME_None)
		{
			PreviousBoneLocation = StaticMeshParent->GetSocketLocation(SocketName);
			GetWorld()->GetTimerManager().SetTimer(
				VelocityUpdateTimerHandle,
				this,
				&UFoliageInteractSourceComponent::UpdateSocketVelocity,
				VelocityUpdateInterval,
				true);
		}
	}
}

void UFoliageInteractSourceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(VelocityUpdateTimerHandle);
	bEneable = false;

	Super::EndPlay(EndPlayReason);
}

void UFoliageInteractSourceComponent::UpdateSocketVelocity()
{
	if (CachedSkelMeshParent.IsValid())
	{
		FVector CurrentBoneLocation;
		if (SocketName != NAME_None) CurrentBoneLocation = CachedSkelMeshParent->GetSocketLocation(SocketName);
		else CurrentBoneLocation = CachedSkelMeshParent->GetComponentLocation();
		CachedVelocity = (CurrentBoneLocation - PreviousBoneLocation) / VelocityUpdateInterval;
		PreviousBoneLocation = CurrentBoneLocation;
	}
	else if (CachedStaticMeshParent.IsValid())
	{
		FVector CurrentBoneLocation;
		if (SocketName != NAME_None) CurrentBoneLocation = CachedStaticMeshParent->GetSocketLocation(SocketName);
		else CachedStaticMeshParent->GetComponentLocation();
		CachedVelocity = (CurrentBoneLocation - PreviousBoneLocation) / VelocityUpdateInterval;
		PreviousBoneLocation = CurrentBoneLocation;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(VelocityUpdateTimerHandle);
		CachedVelocity = FVector::ZeroVector;
	}
}


void UFoliageInteractSourceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bEneable) return;
	//if (Collider) UE_LOG(LogTemp, Log, TEXT("%s"), *Collider->GetComponentVelocity().ToString());
	for (AFoliageInteractVolume* FIV :ToCheckFoliageVolumes)
	{
		if (FIV->IsInside(GetComponentLocation()))
		{
			if (CurrentOverlappingVolume == FIV) continue;
			CurrentOverlappingVolume = FIV;
			//FIV->SetImpulse(Collider->GetComponentLocation(), Collider->GetScaledSphereRadius());
			TScriptInterface<IICharacterMovement> tempInterface;
			tempInterface = this;
			FIV->PushInteractSourceComponent(tempInterface);
			//DrawDebugSphere(GetWorld(), GetComponentLocation(), Collider->GetScaledSphereRadius(), 32, FColor::Red, false);
		}
		else
		{
			CurrentOverlappingVolume = nullptr;
		}
	}
}

bool UFoliageInteractSourceComponent::AttachToComponent(USceneComponent* InParent, const FAttachmentTransformRules& AttachmentRules, FName InSocketName)
{
	bool rtn = Super::AttachToComponent(InParent, AttachmentRules, InSocketName);
	if (Collider != nullptr)
	{
		rtn &= Collider->AttachToComponent(InParent, AttachmentRules, InSocketName);
	}
	return rtn;
}

FVector UFoliageInteractSourceComponent::_GetVelocity()
{
	if (CachedSkelMeshParent.IsValid())
	{
		return CachedVelocity;
	}

	if (GetAttachParent() != nullptr)
	{
		return GetAttachParent()->GetComponentVelocity();
	}

	return FVector::ZeroVector;
}

FVector UFoliageInteractSourceComponent::_GetLocation()
{
	return Collider->GetComponentLocation();;
}

float UFoliageInteractSourceComponent::GetComponentCollideRadius()
{
	return Collider->GetScaledSphereRadius();
}

void UFoliageInteractSourceComponent::BeginDestroy()
{
	Super::BeginDestroy();
}

