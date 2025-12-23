#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HKKPlayerController.h"
#include "Component/CharacterAnimationComponent.h"
#include "Component/CharacterCombatComponent.h"
#include "Interface/Character/ICharacterAnimInstance.h" 
#include "Interface/GameFramework/IPlayerState.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "Interface/GameFramework/IPlayerState.h"


APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bReplicates = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	AnimationComponent = CreateDefaultSubobject<UCharacterAnimationComponent>(TEXT("AnimationComponent"));
	CombatComponent = CreateDefaultSubobject<UCharacterCombatComponent>(TEXT("CombatComponent"));
	
	GetMesh()->SetCustomDepthStencilValue((int32)ECustomStencilValue::ECSV_CharacterDepth);
	GetMesh()->SetRenderCustomDepth(true);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AHKKPlayerController* tempController = Cast<AHKKPlayerController>(GetController());
	if (tempController == nullptr) return;
	tempController->GetOnAiming().AddLambda([&](float Yaw) {
		Server_OnAiming(Yaw);
		}
	);
	//OnPlayAnimation = tempController->GetOnPlayAnimation();
	//OnAttack = tempController->GetOnAttack();
	tempController->GetOnAttack().AddUFunction(this, TEXT("Callback_OnAttack"));
	OnSetItemInteractPickupWidget = &tempController->GetOnSetItemInteractPickupWidget();
	IAnimInstace = Cast<IICharacterAnimInstance>(GetMesh()->GetAnimInstance());

	CombatComponent->Server_SetOwnerMeshComp(GetMesh());
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Server_RefreshVelocity_Implementation();
	IIPlayerState* PS = Cast<IIPlayerState>(GetPlayerState());
	if (PS != nullptr)
	{
		FString Temp = FString::Printf(TEXT("HP : %f"), PS->Execute_GetCurrHP(GetPlayerState()));
		DrawDebugString(GetWorld(), FVector(0.f, 0.f, 150.f), *Temp, this, FColor::Red, DeltaTime);
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, CharacterMovementState);
}

void APlayerCharacter::Multicast_PlayAnimation_Implementation(UAnimSequence* PlayAnimation)
{
	if (IAnimInstace == nullptr)
	{
		IAnimInstace = Cast<IICharacterAnimInstance>(GetMesh()->GetAnimInstance());
		UE_LOG(LogTemp, Warning, TEXT("[%s] Multicast_PlayAnimation_Implementation Called_AnimInstance Casted."), *UEnum::GetValueAsString(GetLocalRole()));
	}
	if (IAnimInstace != nullptr)
	{
		IAnimInstace->PlaySlotAnimation(PlayAnimation, TEXT("AttackSlot"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Multicast_PlayAnimation_Implementation Called_AnimInstance Failed."), *UEnum::GetValueAsString(GetLocalRole()));
	}
}

void APlayerCharacter::Server_PlayAnimation_Implementation(UAnimSequence* PlayAnimation)
{
	Multicast_PlayAnimation(PlayAnimation);
}

void APlayerCharacter::Callback_OnAttack(const EPlayerAnimation AttackType)
{
	if (AnimationComponent == nullptr) return;
	Server_PlayAnimation(AnimationComponent->GetAnimationSequence(AttackType));
}

bool APlayerCharacter::HitTraceStart(FHitTraceConfig* HitTraceConfig, float MaxTraceTime)
{
	if (CombatComponent != nullptr) CombatComponent->Server_HitTraceStart(*HitTraceConfig, MaxTraceTime);
	return CombatComponent == nullptr ? false : true;
}

bool APlayerCharacter::HitTraceEnd()
{
	if (CombatComponent == nullptr) return false;
	CombatComponent->Server_HitTraceEnd();
	return true;
}

bool APlayerCharacter::HitTrace(FHitTraceConfig* HitTraceConfig)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] HitTrace Called."), *UEnum::GetValueAsString(GetLocalRole()));
	return false;
}

void APlayerCharacter::Multicast_KnockBack_Implementation(FVector Direction)
{
	if (HasAuthority())
	{
		UAnimSequence* ToPlayReactAnim = nullptr;
		if (AnimationComponent != nullptr)
		{
			float Dot_Forward = FVector::DotProduct(GetActorForwardVector(), Direction);
			float Dot_Right = FVector::DotProduct(GetActorRightVector(), Direction);
			if (FMath::Abs(Dot_Forward) > FMath::Abs(Dot_Right))
			{
				ToPlayReactAnim = AnimationComponent->GetAnimationSequence(
					Dot_Forward > 0.f ? EPlayerAnimation::EPA_HitReact_F : EPlayerAnimation::EPA_HitReact_B
				);
			}
			else
			{
				ToPlayReactAnim = AnimationComponent->GetAnimationSequence(
					Dot_Right > 0.f ? EPlayerAnimation::EPA_HitReact_R : EPlayerAnimation::EPA_HitReact_L
				);
			}
			if (ToPlayReactAnim != nullptr) Server_PlayAnimation(ToPlayReactAnim);
		}
		LaunchCharacter(Direction * 100.f, false, false);
	}
}

void APlayerCharacter::SetItemInteractWidget(bool ToSet, TScriptInterface<class IIPickableItem> PickableItem, const FItemConfig& ItemConfig)
{
	OnSetItemInteractPickupWidget->Broadcast(ToSet, EUserWidget::EUW_ItemInteract, ItemConfig);
}

void APlayerCharacter::Multicast_HitDamage_Implementation(const FHitDamageConfig& HitTraceConfig)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Multicast_HitDamage : %f Direction : %s."), *UEnum::GetValueAsString(GetLocalRole()), HitTraceConfig.HitDamage, *HitTraceConfig.HitDirection.ToString());
	if (HasAuthority())
	{
		FVector HitDirection = HitTraceConfig.HitDirection;
		UAnimSequence* ToPlayReactAnim = nullptr;
		if (AnimationComponent != nullptr)
		{
			float Dot_Forward = FVector::DotProduct(GetActorForwardVector(), HitDirection);
			float Dot_Right = FVector::DotProduct(GetActorRightVector(), HitDirection);
			if (FMath::Abs(Dot_Forward) > FMath::Abs(Dot_Right))
			{
				ToPlayReactAnim = AnimationComponent->GetAnimationSequence(
					Dot_Forward > 0.f ? EPlayerAnimation::EPA_HitReact_F : EPlayerAnimation::EPA_HitReact_B
				);
			}
			else
			{
				ToPlayReactAnim = AnimationComponent->GetAnimationSequence(
					Dot_Right > 0.f ? EPlayerAnimation::EPA_HitReact_R : EPlayerAnimation::EPA_HitReact_L
				);
			}
			if (ToPlayReactAnim != nullptr) Server_PlayAnimation(ToPlayReactAnim);
		}
		LaunchCharacter(HitTraceConfig.HitDirection * 100.f, false, false);
	}
}

void APlayerCharacter::Server_RefreshVelocity_Implementation()
{
	CharacterMovementState.Velocity = GetVelocity();
}

void APlayerCharacter::Server_OnAiming_Implementation(float Yaw)
{
	CharacterMovementState.FacingYaw = Yaw;
}

