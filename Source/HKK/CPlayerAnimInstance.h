#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HKK_Delegates.h"
#include "CPlayerAnimInstance.generated.h"

UCLASS()
class HKK_API UCPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UCPlayerAnimInstance();

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

	FOnAiming* OnAiming;
	FOnPlayAnimation* OnPlayAnimation;
public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void CallBack_OnPlayAnimation(UAnimSequence* PlayAnimation);
};
