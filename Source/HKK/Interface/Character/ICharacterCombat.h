#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_Structs.h"
#include "ICharacterCombat.generated.h"

UINTERFACE(MinimalAPI)
class UICharacterCombat : public UInterface
{
	GENERATED_BODY()
};

class AActor;

class HKK_API IICharacterCombat
{
	GENERATED_BODY()

public:
	virtual bool HitTrace(FHitTraceConfig* HitTraceConfig) = 0;
	virtual bool HitTraceStart(FHitTraceConfig* HitTraceConfig, float MaxTraceTime) = 0;
	virtual bool HitTraceEnd() = 0;
	
	virtual void Multicast_HitDamage(const FHitDamageConfig& HitTraceConfig) = 0;
	virtual void Multicast_KnockBack(FVector Direction) = 0;
	virtual class UObject* GetPlayerStateObject() = 0;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool AttachItem(AActor* AttachItemActor, FName AttachSocketName);
};
