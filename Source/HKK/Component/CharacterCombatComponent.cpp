#include "Component/CharacterCombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Interface/Character/ICharacterCombat.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CombatLibrary.h"
#include "DrawDebugHelpers.h"

UCharacterCombatComponent::UCharacterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCharacterCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharacterCombatComponent, bTrace);
	DOREPLIFETIME(UCharacterCombatComponent, HitTraceConfig);
	DOREPLIFETIME(UCharacterCombatComponent, EquipActor);
	//DOREPLIFETIME(UCharacterCombatComponent, CollisionQueryParams);
}

void UCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->HasAuthority() && bTrace)
	{
		if (OwnerMeshComp != nullptr)
		{
			TArray<FHitResult> HitResults;
			FCollisionShape CollisionShape;
			CollisionShape.ShapeType = ECollisionShape::Sphere;
			CollisionShape.SetSphere(50.f);
			FVector TraceStart = OwnerMeshComp->GetBoneLocation(HitTraceConfig.TraceStartBoneName);
			FVector TraceEnd = OwnerMeshComp->GetBoneLocation(HitTraceConfig.TraceEndBoneName);
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActor(GetOwner());
			GetWorld()->SweepMultiByChannel(
				HitResults,
				TraceStart,
				TraceEnd,
				FQuat::Identity,
				ECollisionChannel::ECC_Pawn,
				CollisionShape,
				CollisionQueryParams
			);

			for (FHitResult HitResult : HitResults)
			{
				if (HitResult.GetActor() == nullptr || !HitResult.GetActor()->Implements<UICharacterCombat>()) continue;

				FVector HitDirection = (HitResult.GetActor()->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
				FHitDamageConfig HitDamageConfig;
				HitDamageConfig.HitDamage = HitTraceConfig.HitDamage;
				HitDamageConfig.HitDirection = HitDirection;
				UCombatLibrary::SendDamage(GetOwner(), HitResult.GetActor(), HitDamageConfig);
			}
			CachedTraceStartLocation = TraceStart;
			CachedTraceEndLocation = TraceEnd;
		}
	}
}

void UCharacterCombatComponent::Server_SpawnAndAttachWeapon_Implementation(const FItemConfig& AttachItemConfig)
{
	//if (!GetOwner()->HasAuthority()) return;
	UCombatLibrary::DetachItem(GetOwner(), EquipActor);
	UCombatLibrary::ReturnToPool(GetOwner(), EquipActorClass, EquipActor);
	EquipActor = nullptr;
	EquipActorClass = nullptr;

	if (AttachItemConfig.WearableSpawnClass != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("TODO UnAttaching Item"));
		UE_LOG(LogTemp, Log, TEXT("Attaching Item %s"), *AttachItemConfig.ItemName.ToString());
		if (AttachItemConfig.WearableSpawnClass != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("Attaching Item class %s"), *AttachItemConfig.WearableSpawnClass->GetName());
		}
		//EquipActor = GetWorld()->SpawnActor<AActor>(AttachItemConfig.WearableSpawnClass);
		EquipActor = UCombatLibrary::SpawnFromPool(GetOwner(), AttachItemConfig.WearableSpawnClass);
		EquipActorClass = AttachItemConfig.WearableSpawnClass;
		OnRep_ItemEquip();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("TODO UnAttaching Item // Do Bare Hands"));
	}
}

void UCharacterCombatComponent::Server_HitTraceEnd_Implementation()
{
	if (!GetOwner()->HasAuthority()) return;
	GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
	bTrace = false;
}

void UCharacterCombatComponent::Server_HitTraceStart_Implementation(const FHitTraceConfig& _HitTraceConfig, float MaxTime)
{
	if (!GetOwner()->HasAuthority()) return;

	bTrace = true;
	HitTraceConfig = _HitTraceConfig;

	GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(HitTraceTimerHandle, this, &UCharacterCombatComponent::Multicast_HitTraceEnd, MaxTime, false);
	CachedTraceStartLocation = GetOwner()->GetActorLocation();
	CachedTraceEndLocation = GetOwner()->GetActorLocation();
}

void UCharacterCombatComponent::Multicast_HitTraceStart_Implementation(const FHitTraceConfig& _HitTraceConfig, float MaxTime)
{
	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(HitTraceTimerHandle, this, &UCharacterCombatComponent::Multicast_HitTraceEnd, MaxTime, false);
		//CollisionQueryParams = FCollisionQueryParams{};
		CachedTraceStartLocation = GetOwner()->GetActorLocation();
		CachedTraceEndLocation = GetOwner()->GetActorLocation();
	}
	bTrace = true;
	HitTraceConfig = _HitTraceConfig;
}

void UCharacterCombatComponent::Server_SetOwnerMeshComp_Implementation(USkeletalMeshComponent* MeshComp)
{
	OwnerMeshComp = MeshComp;
}

void UCharacterCombatComponent::Multicast_HitTraceEnd_Implementation()
{
	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
	}
	bTrace = false;
}

void UCharacterCombatComponent::OnRep_ItemEquip()
{
	if (EquipActor != nullptr)
	{
		UCombatLibrary::AttachItem(GetOwner(), EquipActor, FName("Buster_FixedSocket"));
	}
}
