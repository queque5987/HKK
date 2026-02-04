#include "Component/CharacterAnimationComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CombatLibrary.h"
#include "DrawDebugHelpers.h"

UCharacterAnimationComponent::UCharacterAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}


void UCharacterAnimationComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UCharacterAnimationComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterAnimationComponent, WallCoverableObjectCheck);
}

void UCharacterAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	Server_ForwardLineTrace_WallCover();
}

UAnimSequence* UCharacterAnimationComponent::GetAnimationSequence(const EPlayerAnimation PlayerAnimation)
{
	TObjectPtr<UAnimSequence>* rtn = AnimationMap.Find(PlayerAnimation);
	if (rtn != nullptr)
	{
		return *rtn;
	}
	else
	{
		return nullptr;
	}
}

void UCharacterAnimationComponent::OnRep_WallCoverableObjectCheck()
{
	const bool bLeast8LineChecked = (WallCoverableObjectCheck == 0xFF);
	UCombatLibrary::SetWallCoverable(GetOwner(), bLeast8LineChecked);
	DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), 50.f, 32, bLeast8LineChecked ? FColor::Blue : FColor::Red, false, 0.25f);
}

void UCharacterAnimationComponent::Server_ForwardLineTrace_WallCover_Implementation()
{
	if (GetOwner() == nullptr) return;
	uint8 SucceedCounter = 0;
	FVector ForwardVector = GetOwner()->GetActorForwardVector();
	FVector RightVector = GetOwner()->GetActorRightVector();
	FVector UpVector = GetOwner()->GetActorUpVector();
	FVector Location = GetOwner()->GetActorLocation() - UpVector * 50.f;
	uint8 tempWallCoverableObjectCheck = 0;

	for (int8 i = 0; i < 9; i++)
	{
		int8 x = i % 3;
		int8 y = i / 3;

		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		bool bFlag = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Location + ForwardVector * 10.f + RightVector * 25.f * (x - 1) + UpVector * 25.f * (y - 1),
			Location + ForwardVector * 75.f + RightVector * 25.f * (x - 1) + UpVector * 25.f * (y - 1),
			ECollisionChannel::ECC_Pawn,
			CollisionQueryParams
		);

		DrawDebugLine(
			GetWorld(),
			Location + ForwardVector * 10.f + RightVector * 25.f * (x - 1) + UpVector * 25.f * (y - 1),
			Location + ForwardVector * 75.f + RightVector * 25.f * (x - 1) + UpVector * 25.f * (y - 1),
			bFlag ? FColor::Green : FColor::Red
		);

		if (bFlag)
		{
			tempWallCoverableObjectCheck |= 1 << SucceedCounter;
			SucceedCounter++;
		}
		if (SucceedCounter >= 8) break;
	}

	if (GetOwner()->HasAuthority())
	{
		if (tempWallCoverableObjectCheck != WallCoverableObjectCheck)
		{
			WallCoverableObjectCheck = tempWallCoverableObjectCheck;
			OnRep_WallCoverableObjectCheck();
		}
	}

}

