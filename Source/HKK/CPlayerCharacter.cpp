#include "CPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HKKPlayerController.h"
#include "Component/CCharacterAnimationComponent.h"
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
	//ConstructorHelpers::FObjectFinder<UCCharacterAnimationComponent> AnimComponentFinder(TEXT("/Game/_Player/Blueprint/Animation/BP_Robo_AnimationComponent.BP_Robo_AnimationComponent"));
	//if (AnimComponentFinder.Succeeded()) AnimationComponent = AnimComponentFinder.Object;
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

void ACPlayerCharacter::Callback_OnAttack(int8 AttackType)
{
	if (AnimationComponent == nullptr) return;
	OnPlayAnimation->Broadcast(AnimationComponent->GetAnimationSequence(AttackType));
}

void ACPlayerCharacter::Server_RefreshVelocity_Implementation()
{
	CharacterMovementState.Velocity = GetVelocity();
}

void ACPlayerCharacter::Server_OnAiming_Implementation(float Yaw)
{
	CharacterMovementState.FacingYaw = Yaw;
}

