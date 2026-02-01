#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ICharacterAnimInstance.generated.h"

UINTERFACE(MinimalAPI)
class UICharacterAnimInstance : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IICharacterAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AnimInstance_SetBoolValue(EPlayerState ToSetPlayerState, bool e);

	virtual void PlaySlotAnimation(class UAnimSequence* ToPlayAnimation, FName SlotName, float BlendInTime = 0.25f, float BlendOutTime = 0.25f, float InPlayRate = 1.f, int32 LoopCount = 1, float BlendOutTriggerTime = -1.f, float InTimeToStartMontageAt = 0.f) {};
	
};
