// Copyright Epic Games, Inc. All Rights Reserved.

#include "HKKPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "HKKCharacter.h"
#include "Engine/World.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Component/ControllerWidgetComponent.h"
#include "Engine/LocalPlayer.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CombatLibrary.h"
#include "Interface/GameFramework/IPlayerState.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

class IICharacterCombat;

AHKKPlayerController::AHKKPlayerController()
{
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
	PrimaryActorTick.bCanEverTick = true;
	WidgetComponent = CreateDefaultSubobject<UControllerWidgetComponent>(TEXT("WidgetComponent"));
}

void AHKKPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	//Test
	//OnGetItem.AddLambda([this](const FItemConfig& ItemConfig, AActor* OwningActor) 
	//{
	//	UE_LOG(LogTemp, Log, TEXT("OnGetItem Delegate Callback Lambda Triggered."));
	//});

	LoadingRace();
}

void AHKKPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FHitResult HitResult;
		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult))
		{
			FVector HitLocation = HitResult.Location;
			FVector Direction = (HitLocation - ControlledPawn->GetActorLocation()).GetSafeNormal();
			float X = FVector::DotProduct(ControlledPawn->GetActorForwardVector(), Direction);
			float Y = FVector::DotProduct(ControlledPawn->GetActorRightVector(), Direction);
			float Theta = FMath::Atan2(Y, X);
			OnAiming.Broadcast(FMath::RadiansToDegrees(Theta));
		}
	}
}

void AHKKPlayerController::LoadingRace()
{
	if (GetPlayerStateObject() == nullptr || WidgetComponent == nullptr)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AHKKPlayerController::LoadingRace);
		return;
	}
	if (!DelegateBind)
	{
		OnGetItem.AddLambda([this](const FItemConfig& ItemConfig, UObject* OwningPlayerState)
			{
				UCombatLibrary::GetItem(OwningPlayerState, ItemConfig);
			}
		);
		GetOnKeyTriggered().AddUFunction(GetPlayerStateObject(), TEXT("Callback_KeyTriggered"));

		DelegateBind = true;
	}

	if (!WidgetControllerSetup)
	{
		if (WidgetComponent != nullptr && IsLocalController()) WidgetControllerSetup = WidgetComponent->SetController(this);
	}
	if (!WidgetHUDBind)
	{
		if (IsLocalController() && WidgetComponent != nullptr)
		{
			WidgetHUDBind = WidgetHUDBind = WidgetComponent->Bind_HUD(this);
		}
	}
	if (DelegateBind && WidgetControllerSetup && WidgetHUDBind) return;
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AHKKPlayerController::LoadingRace);
}

void AHKKPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHKKPlayerController, bShiftPressed);
}

void AHKKPlayerController::ChangeQuickSlot_Implementation(UObject* ChangedItemObject, FKey ChangedKey)
{
	//if (WidgetComponent != nullptr) WidgetComponent->ChangedQuickSlot(ChangedItemObject, ChangedKey);
	OnQuickSlotUpdated.Broadcast(ChangedItemObject, ChangedKey);
}

void AHKKPlayerController::ChangeEquipSlot_Implementation(UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType)
{
	OnItemEquiped.Broadcast(ChangedItemObject, EquipmentSlotType);
}

void AHKKPlayerController::EquipmentItemDragDetected_Implementation(bool e)
{
	if (WidgetComponent) WidgetComponent->OnEquipmentItemDragDetected(e);
}

EEquipmentSlotType AHKKPlayerController::GetLeftEquipmentSlotIndex_Implementation()
{
	return WidgetComponent ? WidgetComponent->GetLeftEquipmentSlotIndex() : EEquipmentSlotType::EEST_EquipSlot_Default;
}

void AHKKPlayerController::ItemInteractPickUpWidget_Implementation(bool IsOn, UObject* PickableItemObject, const FItemConfig& ItemConfig)
{
	if (WidgetComponent) WidgetComponent->SetItemInteractPickupWidget(IsOn, EUserWidget::EUW_ItemInteract, ItemConfig, PickableItemObject);
}

void AHKKPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

}

void AHKKPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AHKKPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

}

void AHKKPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		//EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AHKKPlayerController::OnClicked);
		//EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::OnSetDestinationTriggered);
		//EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AHKKPlayerController::OnSetDestinationReleased);
		//EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AHKKPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AHKKPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AHKKPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AHKKPlayerController::OnTouchReleased);

		EnhancedInputComponent->BindAction(UISwitchAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::UISwitchTriggered);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AHKKPlayerController::MoveReleased);

		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::ShiftTriggered);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AHKKPlayerController::ShiftReleased);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AHKKPlayerController::JumpReleased);

		EnhancedInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::MouseMoved);
		EnhancedInputComponent->BindAction(MouseScrollAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::MouseScrolled);
		EnhancedInputComponent->BindAction(MouseAttackAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::Attack0_RFistTriggered);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::InteractTriggered);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AHKKPlayerController::InteractReleased);

		EnhancedInputComponent->BindAction(QuickSlotAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::QuickSlotTriggered);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AHKKPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AHKKPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AHKKPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

void AHKKPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AHKKPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AHKKPlayerController::OnClicked()
{
	SetInputMode(InputModeGameOnly);
}

void AHKKPlayerController::Move(const FInputActionValue& Value)
{
	if (!WidgetComponent->IsControllable()) return;

	FVector2D InputVector = Value.Get<FVector2D>();
	if (InputVector.X >= 0.f && bShiftPressed) InputVector.Y = 0.f;
	FVector InputDirection{ InputVector.X, InputVector.Y, 0.f };
	float CurrentInputSec = GetWorld() ? GetWorld()->GetTimeSeconds() : -1.f;
	APawn* ControlledPawn = GetPawn();

	if (FMath::IsNearlyZero(CachedDirection.Size()))
	{
		CachedDirection.X = InputDirection.X;
		CachedDirection.Y = InputDirection.Y;
	}
	else
	{
		float HoldingSec = CurrentInputSec - LastInputSec;
		CachedDirection = FMath::Lerp(CachedDirection, InputDirection, FMath::Min(HoldingSec, 2.f) / 2.f);
	}

	bool debugMoveBasedOnCamera = false;
	if (ControlledPawn != nullptr)
	{
		const FRotator YawRotation(0, GetControlRotation().Yaw, 0);
		const FVector ForwardDirection = (
			debugMoveBasedOnCamera ? 
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) : ControlledPawn->GetActorForwardVector()
			);
		const FVector RightDirection = (
			debugMoveBasedOnCamera ? 
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) : ControlledPawn->GetActorRightVector()
			);
		float XScaleValue = CachedDirection.X;
		float YScaleValue = CachedDirection.Y;
		if (CachedDirection.X < 0.f)
		{
			XScaleValue *= 0.75f;
			YScaleValue *= 0.75f;
			if (bShiftPressed)
			{
				XScaleValue *= 3.f / 5.f;
				YScaleValue *= 3.f / 5.f;
			}
		}

		ControlledPawn->AddMovementInput(ForwardDirection, XScaleValue);
		ControlledPawn->AddMovementInput(RightDirection, YScaleValue);
	}
	if (CachedInput != InputDirection)
	{
		LastInputSec = CurrentInputSec;
	}
	CachedInput = InputDirection;
}

void AHKKPlayerController::MoveReleased()
{
	CachedInput *= 0.f;
	CachedDirection *= 0.f;
}

void AHKKPlayerController::ShiftTriggered()
{
	if (!WidgetComponent->IsControllable()) return;
	Server_SetShiftPressed(true);
	OnKeyTriggered.Broadcast(FKey("Shift"));
}

void AHKKPlayerController::ShiftReleased()
{
	Server_SetShiftPressed(false);
	OnKeyReleased.Broadcast(FKey("Shift"));
}

void AHKKPlayerController::Jump()
{
	if (!WidgetComponent->IsControllable()) return;
	ACharacter* ControlledPawn = GetCharacter();

	if (ControlledPawn == nullptr) return;
	ControlledPawn->Jump();
}

void AHKKPlayerController::JumpReleased()
{
	ACharacter* ControlledPawn = GetCharacter();

	if (ControlledPawn == nullptr) return;
	ControlledPawn->StopJumping();
}

void AHKKPlayerController::MouseMoved(const FInputActionValue& Value)
{
	if (!WidgetComponent->IsControllable()) return;
	FVector2D InputVector = Value.Get<FVector2D>();
	AddYawInput(InputVector.X);
	AddPitchInput(InputVector.Y);

	FRotator LookRotation = { 0.f, GetControlRotation().Yaw, 0.f };
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn == nullptr) return;
	ControlledPawn->GetActorForwardVector();
	LookRotation.Vector();
	
	float theta = FVector::DotProduct(ControlledPawn->GetActorForwardVector(), LookRotation.Vector());
	if (theta <= (ControlledPawn->GetVelocity().Size2D() > 50.f || bShiftPressed ? 0.95f : -0.5f))
	{
		UCombatLibrary::RotatePawnBasedOnControlRotation(ControlledPawn);
	}
}

void AHKKPlayerController::MouseScrolled(const FInputActionValue& Value)
{
	float ScrollValue = Value.Get<float>();
	OnKeyTriggered.Broadcast(ScrollValue >= 0 ? FKey("MouseScrollUp") : FKey("MouseScrollDown"));
}

void AHKKPlayerController::Attack0_RFistTriggered(const FInputActionValue& Value)
{
	if (!WidgetComponent->IsControllable()) return;
	OnAttack.Broadcast(EPlayerAnimation::EPA_Attack_RFist);
}

void AHKKPlayerController::InteractTriggered()
{
	OnKeyTriggered.Broadcast(FKey("E"));
}

void AHKKPlayerController::InteractReleased()
{
	OnKeyReleased.Broadcast(FKey("E"));
}

void AHKKPlayerController::UISwitchTriggered(const FInputActionValue& Value)
{
	float UIIndex = Value.Get<float>();
	uint8 UIIdx = FMath::Floor(UIIndex);
	switch (UIIdx)
	{
	case((uint8)EUserWidget::EUW_Inventory):
		OnKeyTriggered.Broadcast(FKey("I"));
		break;
	default:
		break;
	}
}

void AHKKPlayerController::QuickSlotTriggered(const FInputActionValue& Value)
{
	float QuickslotIndex = Value.Get<float>();
	uint8 QIdx = FMath::Floor(QuickslotIndex);
}

void AHKKPlayerController::Server_SetShiftPressed_Implementation(bool e)
{
	bShiftPressed = e;
}
