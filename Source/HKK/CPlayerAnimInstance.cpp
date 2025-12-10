#include "CPlayerAnimInstance.h"
#include "CPlayerCharacter.h"
#include "HKKPlayerController.h"
#include "Interface/ICharacterMovement.h"
#include "HKK_Structs.h"
#include "HKKPlayerController.h"

UCPlayerAnimInstance::UCPlayerAnimInstance() : Super()
{
	OwningPawn = nullptr;
}

void UCPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwningPawn = TryGetPawnOwner();

	if (OwningPawn == nullptr) return;
	Owning_Interface_CharacterMovemnet = Cast<IICharacterMovement>(OwningPawn);

	AHKKPlayerController* TempController = Cast<AHKKPlayerController>(OwningPawn->GetController());
	if (TempController == nullptr) return;

	OnPlayAnimation = TempController->GetOnPlayAnimation();
	if (OnPlayAnimation != nullptr)
	{
		OnPlayAnimation->AddUFunction(this, TEXT("CallBack_OnPlayAnimation"));
	}

}

void UCPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwningPawn == nullptr || Owning_Interface_CharacterMovemnet == nullptr)
	{
		OwningPawn = TryGetPawnOwner();
		Owning_Interface_CharacterMovemnet = Cast<IICharacterMovement>(OwningPawn);
	}
	else
	{
		FCharacterMovementState* tempState = Owning_Interface_CharacterMovemnet->GetCharacterMovementState();
		if (tempState != nullptr)
		{
			CurrentSpeedXY	= tempState->Velocity.Size2D();
			CurrentSpeedZ	= tempState->Velocity.Z;
			AimingAngle		= tempState->FacingYaw;
		}
	}
	
}

void UCPlayerAnimInstance::CallBack_OnPlayAnimation(UAnimSequence* PlayAnimation)
{
	PlaySlotAnimationAsDynamicMontage(PlayAnimation, TEXT("AttackSlot"));
}

void UCPlayerAnimInstance::PlaySlotAnimation(UAnimSequence* ToPlayAnimation, FName SlotName, float BlendInTime, float BlendOutTime, float InPlayRate, int32 LoopCount, float BlendOutTriggerTime, float InTimeToStartMontageAt)
{
	PlaySlotAnimationAsDynamicMontage(ToPlayAnimation, SlotName, BlendInTime, BlendOutTime, InPlayRate, LoopCount, BlendOutTriggerTime, InTimeToStartMontageAt);
}

//void UCPlayerAnimInstance::CallBack_OnAiming(float Yaw)
//{
//	//SetAnimingAngle(Yaw);
//}
