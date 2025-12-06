#include "CPlayerAnimInstance.h"
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
	AHKKPlayerController* tempController = Cast<AHKKPlayerController>(OwningPawn->GetController());
	if (tempController == nullptr) return;
	OnAiming = tempController->GetOnAiming();
	if (OnAiming != nullptr)
	{
		OnAiming->AddUFunction(this, FName("CallBack_OnAiming"));
	}

}

void UCPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwningPawn == nullptr) OwningPawn = TryGetPawnOwner();
	else
	{
		CurrentSpeed = OwningPawn->GetVelocity().Size();
	}
}

void UCPlayerAnimInstance::CallBack_OnAiming(float Yaw)
{
	AimingAngle = Yaw;
	UE_LOG(LogTemp, Log, TEXT("AimingAngle : %f"), AimingAngle);
}
