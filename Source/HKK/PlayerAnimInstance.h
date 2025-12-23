#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HKK_Delegates.h"
#include "Interface/Character/ICharacterAnimInstance.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class HKK_API UPlayerAnimInstance : public UAnimInstance, public IICharacterAnimInstance
{
	GENERATED_BODY()

	UPlayerAnimInstance();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentSpeedXY = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrentSpeedZ = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimingAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bBusterR = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bBusterL = false;

	TObjectPtr<class APawn> OwningPawn;
	TObjectPtr<class IICharacterMovement> Owning_Interface_CharacterMovemnet;

	//FOnAiming* OnAiming;
public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void CallBack_OnPlayAnimation(UAnimSequence* PlayAnimation);

	/*
		CharacterAnimInstance Start
	*/
public:
	virtual void PlaySlotAnimation(class UAnimSequence* ToPlayAnimation, FName SlotName, float BlendInTime = 0.25f, float BlendOutTime = 0.25f, float InPlayRate = 1.f, int32 LoopCount = 1, float BlendOutTriggerTime = -1.f, float InTimeToStartMontageAt = 0.f) override;
	/*
		CharacterAnimInstance End
	*/

};
