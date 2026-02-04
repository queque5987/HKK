#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "HKKPlayerController.h"
#include "Interface/ICharacterMovement.h"
#include "HKK_Structs.h"
#include "HKKPlayerController.h"

UPlayerAnimInstance::UPlayerAnimInstance() : Super()
{
	OwningPawn = nullptr;
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwningPawn = TryGetPawnOwner();

	if (OwningPawn == nullptr) return;
	Owning_Interface_CharacterMovemnet = Cast<IICharacterMovement>(OwningPawn);

	AHKKPlayerController* TempController = Cast<AHKKPlayerController>(OwningPawn->GetController());
	if (TempController == nullptr) return;

	TempController->GetOnPlayAnimation().AddUFunction(this, TEXT("CallBack_OnPlayAnimation"));

}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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
			HeadingRadian	= tempState->HeadingRadian;
			LocalVelocity_N = tempState->LocalVelocityNormalized;
			CachedInput		= tempState->CachedInput;
		}
	}
}

void UPlayerAnimInstance::CallBack_OnPlayAnimation(UAnimSequence* PlayAnimation)
{
	PlaySlotAnimationAsDynamicMontage(PlayAnimation, TEXT("AttackSlot"));
}

void UPlayerAnimInstance::AnimInstance_SetBoolValue_Implementation(EPlayerState ToSetPlayerState, bool e)
{
	if (ToSetPlayerState == EPlayerState::EPS_Aiming)
	{
		RMBAiming = e;
	}
}

void UPlayerAnimInstance::PlaySlotAnimation(UAnimSequence* ToPlayAnimation, FName SlotName, float BlendInTime, float BlendOutTime, float InPlayRate, int32 LoopCount, float BlendOutTriggerTime, float InTimeToStartMontageAt)
{
	PlaySlotAnimationAsDynamicMontage(ToPlayAnimation, SlotName, BlendInTime, BlendOutTime, InPlayRate, LoopCount, BlendOutTriggerTime, InTimeToStartMontageAt);
}

//void UPlayerAnimInstance::PlayStopSlide(FVector PreStopVector)
//{
//	if (FMath::IsNearlyZero(PreStopVector.Size()))
//	{
//		MovingStopSlide = false;
//	}
//	else
//	{
//		FVector PreStopVector_N = PreStopVector.GetSafeNormal2D();
//		float Rad = FMath::Atan2(PreStopVector_N.Y, PreStopVector_N.X);
//		MovingStopSlideDirection = Rad;
//		MovingStopSlide = true;
//	}
//}


//void UPlayerAnimInstance::CallBack_OnAiming(float Yaw)
//{
//	//SetAnimingAngle(Yaw);
//}
