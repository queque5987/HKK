#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HKK_Structs.h"
#include "CollisionQueryParams.h"
#include "CCharacterCombatComponent.generated.h"

class USkeletalMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HKK_API UCCharacterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCCharacterCombatComponent();

protected:
	UPROPERTY(Replicated)
	FHitTraceConfig HitTraceConfig;
	UPROPERTY(Replicated)
	bool bTrace;
	//UPROPERTY(Replicated)
	//FCollisionQueryParams CollisionQueryParams;

	FVector CachedTraceStartLocation;
	FVector CachedTraceEndLocation;

	FTimerHandle HitTraceTimerHandle;

	virtual void BeginPlay() override;
	
	TObjectPtr<USkeletalMeshComponent> OwnerMeshComp;
public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HitTraceStart(const FHitTraceConfig& _HitTraceConfig, float MaxTime);
	UFUNCTION(Server, Unreliable)
	void Server_SetOwnerMeshComp(USkeletalMeshComponent* MeshComp);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HitTraceEnd();
};
