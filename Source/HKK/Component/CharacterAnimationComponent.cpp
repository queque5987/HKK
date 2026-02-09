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
	DOREPLIFETIME(UCharacterAnimationComponent, WallCoverNormalVector);
	DOREPLIFETIME(UCharacterAnimationComponent, bWallCovering);
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
	const bool b25LineChecked = ((WallCoverableObjectCheck & 0x1FFFFFF) == 0x1FFFFFF);
	UCombatLibrary::SetWallCoverable(GetOwner(), b25LineChecked);
	//DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), 50.f, 32, bLeast8LineChecked ? FColor::Blue : FColor::Red, false, 0.25f);
}

bool UCharacterAnimationComponent::WallCover_IsPossibleHorizontalMovement(bool IsRight)
{
	uint32 SideMask = 0;
	int8 RightValue = IsRight ? (TraceCount - 1) : 0;

	for (int8 i = 0; i < TraceCount; i++)
	{
		if ((WallCoverableObjectCheck & (1 << (i * TraceCount + RightValue))) == 0)
		{
			return false;
		}
	}

	return true;
}

void UCharacterAnimationComponent::Server_SetWallCovering_Implementation(bool e)
{
	bWallCovering = e;
}

FVector UCharacterAnimationComponent::GetMostNormalVector(const TArray<FVector_NetQuantizeNormal>& Normals)
{
	if (Normals.Num() == 0) return FVector::ZeroVector;

	struct FNormalGroup
	{
		FVector AverageNormal;
		int8 Count = 0;
	};
	TArray<FNormalGroup> Groups;

	const float SimilarityThreshold = 0.99f;

	for (const auto& CurrentNormal : Normals)
	{
		bool bFoundGroup = false;
		for (FNormalGroup& Group : Groups)
		{
			float Dot = FVector::DotProduct(CurrentNormal, Group.AverageNormal);
			if (Dot > SimilarityThreshold)
			{
				Group.Count++;
				Group.AverageNormal = (Group.AverageNormal + CurrentNormal).GetSafeNormal();
				bFoundGroup = true;
				break;
			}
		}

		if (!bFoundGroup)
		{
			Groups.Add({ CurrentNormal, 1 });
		}
	}
	FNormalGroup* BestGroup = &Groups[0];
	for (int32 i = 1; i < Groups.Num(); ++i)
	{
		if (Groups[i].Count > BestGroup->Count)
		{
			BestGroup = &Groups[i];
		}
	}
	return BestGroup->AverageNormal;
}

void UCharacterAnimationComponent::Server_ForwardLineTrace_WallCover_Implementation()
{
	if (GetOwner() == nullptr) return;
	uint8 SucceedCounter = 0;
	FVector ForwardVector = GetOwner()->GetActorForwardVector();
	FVector RightVector = GetOwner()->GetActorRightVector();
	FVector UpVector = GetOwner()->GetActorUpVector();
	FVector Location = GetOwner()->GetActorLocation() - UpVector * 50.f;

	uint32 tempWallCoverableObjectCheck = 0;
	TArray<FVector_NetQuantizeNormal> Normals;
	for (int8 i = 0; i < TraceCount * TraceCount; i++)
	{
		int8 x = i % TraceCount;
		int8 y = i / TraceCount;

		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(GetOwner());
		bool bFlag = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Location + ForwardVector * 10.f + RightVector * 25.f * (x - 2) + UpVector * 25.f * (y - 2),
			Location + ForwardVector * 75.f + RightVector * 25.f * (x - 2) + UpVector * 25.f * (y - 2),
			ECollisionChannel::ECC_Pawn,
			CollisionQueryParams
		);

		DrawDebugLine(
			GetWorld(),
			Location + ForwardVector * 10.f + RightVector * 25.f * (x - 2) + UpVector * 25.f * (y - 2),
			Location + ForwardVector * 75.f + RightVector * 25.f * (x - 2) + UpVector * 25.f * (y - 2),
			bFlag ? FColor::Green : FColor::Red
		);

		if (bFlag)
		{
			tempWallCoverableObjectCheck |= 1 << i;
			SucceedCounter++;
			Normals.Add(HitResult.ImpactNormal);
		}
	}

	if (GetOwner()->HasAuthority())
	{
		if ((tempWallCoverableObjectCheck & 0x1FFFFFF) == 0x1FFFFFF)
		{
			WallCoverNormalVector = GetMostNormalVector(Normals);
		}
		if (tempWallCoverableObjectCheck != WallCoverableObjectCheck)
		{
			WallCoverableObjectCheck = tempWallCoverableObjectCheck;
			OnRep_WallCoverableObjectCheck();
		}
	}
}

