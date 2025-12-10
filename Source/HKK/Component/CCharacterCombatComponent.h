#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HKK_Structs.h"
#include "CCharacterCombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HKK_API UCCharacterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCCharacterCombatComponent();

protected:
	UPROPERTY(Replicated)
	FHitTraceConfig HitTraceConfig;
	FTimerHandle HitTraceTimerHandle;
	UPROPERTY(Replicated)
	bool bTrace;
	virtual void BeginPlay() override;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HitTraceStart(const FHitTraceConfig& _HitTraceConfig, float MaxTime);
	UFUNCTION()
	void HitTraceEnd();
};
