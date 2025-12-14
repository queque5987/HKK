#include "Component/CCharacterCombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Interface/Character/ICharacterCombat.h"
#include "DrawDebugHelpers.h"

UCCharacterCombatComponent::UCCharacterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCCharacterCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCCharacterCombatComponent, bTrace);
	DOREPLIFETIME(UCCharacterCombatComponent, HitTraceConfig);
	//DOREPLIFETIME(UCCharacterCombatComponent, CollisionQueryParams);
}

void UCCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->HasAuthority() && bTrace)
	{
		//UE_LOG(LogTemp, Warning, TEXT("[%s] UCCharacterCombatComponent::TickComponent Called."), *UEnum::GetValueAsString(GetOwnerRole()));
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
			//float TraceTotalLength = FVector::Dist(TraceStart, TraceEnd);
			//float CachedTraceTotalLength = FVector::Dist(CachedTraceStartLocation, CachedTraceEndLocation);
			//FVector CachedTraceDirection = (CachedTraceEndLocation - CachedTraceStartLocation).GetSafeNormal();
			for (FHitResult HitResult : HitResults)
			{
				IICharacterCombat* ICombat = Cast<IICharacterCombat>(HitResult.GetActor());
				if (ICombat == nullptr) continue;

				//float HitLength = FVector::Dist(TraceStart, HitResult.Location);
				//FVector PrevHitLocation = CachedTraceStartLocation + CachedTraceDirection * (HitLength / TraceTotalLength * CachedTraceTotalLength);
				FVector HitDirection = (HitResult.GetActor()->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();

				FHitDamageConfig HitDamageConfig;
				HitDamageConfig.HitDamage = HitTraceConfig.HitDamage;
				HitDamageConfig.HitDirection = HitDirection;

				ICombat->Multicast_HitDamage(HitDamageConfig);
			}
			CachedTraceStartLocation = TraceStart;
			CachedTraceEndLocation = TraceEnd;
		}
	}
}

void UCCharacterCombatComponent::Multicast_HitTraceStart_Implementation(const FHitTraceConfig& _HitTraceConfig, float MaxTime)
{
	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(HitTraceTimerHandle, this, &UCCharacterCombatComponent::Multicast_HitTraceEnd, MaxTime, false);
		//CollisionQueryParams = FCollisionQueryParams{};
		CachedTraceStartLocation = GetOwner()->GetActorLocation();
		CachedTraceEndLocation = GetOwner()->GetActorLocation();
	}
	bTrace = true;
	HitTraceConfig = _HitTraceConfig;
}

void UCCharacterCombatComponent::Server_SetOwnerMeshComp_Implementation(USkeletalMeshComponent* MeshComp)
{
	OwnerMeshComp = MeshComp;
}

void UCCharacterCombatComponent::Multicast_HitTraceEnd_Implementation()
{
	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
	}
	bTrace = false;
}
