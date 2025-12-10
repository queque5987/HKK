#include "Component/CCharacterCombatComponent.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

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
}

void UCCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bTrace) return;
	if (GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] UCCharacterCombatComponent::TickComponent Called."), *UEnum::GetValueAsString(GetOwnerRole()));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] UCCharacterCombatComponent::TickComponent Called."), *UEnum::GetValueAsString(GetOwnerRole()));
	}
}

void UCCharacterCombatComponent::Multicast_HitTraceStart_Implementation(const FHitTraceConfig& _HitTraceConfig, float MaxTime)
{
	if (GetOwner()->HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(HitTraceTimerHandle, this, &UCCharacterCombatComponent::HitTraceEnd, MaxTime, false);
	}
	bTrace = true;
	HitTraceConfig = _HitTraceConfig;
}

//bool UCCharacterCombatComponent::HitTraceStart(FHitTraceConfig** _HitTraceConfig, float MaxTime)
//{
//	GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
//	GetWorld()->GetTimerManager().SetTimer(HitTraceTimerHandle, this, &UCCharacterCombatComponent::HitTraceEnd, MaxTime, false);
//	bTrace = true;
//	HitTraceConfig = *_HitTraceConfig;
//	return true;
//}

void UCCharacterCombatComponent::HitTraceEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
	if (!bTrace) return;
	bTrace = false;
}
