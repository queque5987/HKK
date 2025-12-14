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

class HKK_API IICharacterCombat
{
	GENERATED_BODY()

public:
	virtual bool HitTrace(FHitTraceConfig* HitTraceConfig) { return bool(); };
	virtual bool HitTraceStart(FHitTraceConfig* HitTraceConfig, float MaxTraceTime) { return bool(); };
	virtual bool HitTraceEnd() { return bool(); };
	
	virtual void Multicast_HitDamage(const FHitDamageConfig& HitTraceConfig) {};
};
