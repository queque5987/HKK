#include "CPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HKKPlayerController.h"
#include "Component/CCharacterAnimationComponent.h"
#include "Component/CCharacterCombatComponent.h"
#include "Interface/Character/ICharacterAnimInstance.h" 
#include "Net/UnrealNetwork.h"


ACPlayerCharacter::ACPlayerCharacter()
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

	AnimationComponent = CreateDefaultSubobject<UCCharacterAnimationComponent>(TEXT("AnimationComponent"));
	CombatComponent = CreateDefaultSubobject<UCCharacterCombatComponent>(TEXT("CombatComponent"));
	
	GetMesh()->SetCustomDepthStencilValue((int32)ECustomStensilDepth::ECSD_CharacterDepth);
	GetMesh()->SetRenderCustomDepth(true);
}

void ACPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AHKKPlayerController* tempController = Cast<AHKKPlayerController>(GetController());
	if (tempController == nullptr) return;
	OnAiming = tempController->GetOnAiming();
	if (OnAiming != nullptr)
	{
		OnAiming->AddLambda([&](float Yaw) {
			Server_OnAiming(Yaw);
			}
		);
	}
	OnPlayAnimation = tempController->GetOnPlayAnimation();
	OnAttack = tempController->GetOnAttack();
	if (OnAttack != nullptr)
	{
		OnAttack->AddUFunction(this, TEXT("Callback_OnAttack"));
	}
	IAnimInstace = Cast<IICharacterAnimInstance>(GetMesh()->GetAnimInstance());

	CombatComponent->Server_SetOwnerMeshComp(GetMesh());
}

void ACPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Server_RefreshVelocity_Implementation();
}

void ACPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACPlayerCharacter, CharacterMovementState);
}

void ACPlayerCharacter::Multicast_PlayAnimation_Implementation(UAnimSequence* PlayAnimation)
{
	if (IAnimInstace == nullptr)
	{
		IAnimInstace = Cast<IICharacterAnimInstance>(GetMesh()->GetAnimInstance());
		UE_LOG(LogTemp, Warning, TEXT("[%s] Multicast_PlayAttackAnimation_Implementation Called_AnimInstance Casted."), *UEnum::GetValueAsString(GetLocalRole()));
	}
	if (IAnimInstace != nullptr)
	{
		IAnimInstace->PlaySlotAnimation(PlayAnimation, TEXT("AttackSlot"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Multicast_PlayAttackAnimation_Implementation Called_AnimInstance Failed."), *UEnum::GetValueAsString(GetLocalRole()));
	}
}

void ACPlayerCharacter::Server_PlayAnimation_Implementation(UAnimSequence* PlayAnimation)
{
	Multicast_PlayAnimation(PlayAnimation);
}

void ACPlayerCharacter::Callback_OnAttack(const EPlayerAnimation AttackType)
{
	if (AnimationComponent == nullptr) return;
	Server_PlayAnimation(AnimationComponent->GetAnimationSequence(AttackType));
}

bool ACPlayerCharacter::HitTraceStart(FHitTraceConfig* HitTraceConfig, float MaxTraceTime)
{
	if (CombatComponent != nullptr) CombatComponent->Multicast_HitTraceStart(*HitTraceConfig, MaxTraceTime);
	return CombatComponent == nullptr ? false : true;
}

bool ACPlayerCharacter::HitTraceEnd()
{
	if (CombatComponent == nullptr) return false;
	CombatComponent->Multicast_HitTraceEnd();
	return true;
}

bool ACPlayerCharacter::HitTrace(FHitTraceConfig* HitTraceConfig)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] HitTrace Called."), *UEnum::GetValueAsString(GetLocalRole()));
	return false;
}

void ACPlayerCharacter::Multicast_HitDamage_Implementation(const FHitDamageConfig& HitTraceConfig)
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

void ACPlayerCharacter::Server_RefreshVelocity_Implementation()
{
	CharacterMovementState.Velocity = GetVelocity();
}

void ACPlayerCharacter::Server_OnAiming_Implementation(float Yaw)
{
	CharacterMovementState.FacingYaw = Yaw;
}

