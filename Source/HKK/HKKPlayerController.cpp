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
//#include "GameFramework/WidgetLibrary.h"
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
	DOREPLIFETIME(AHKKPlayerController, PlayerMovingState);
	DOREPLIFETIME(AHKKPlayerController, bWallCoverable);
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

bool AHKKPlayerController::Bind_Character_Implementation(UObject* PlayerCharacterObject)
{
	if (PlayerCharacterObject)
	{
		if (!GetOnKeyTriggered().IsBoundToObject(PlayerCharacterObject) && DelegateHandle_OnKeyTriggered.IsValid())
		{
			GetOnKeyTriggered().Remove(DelegateHandle_OnKeyTriggered);
			DelegateHandle_OnKeyTriggered.Reset();
		}
		if (!GetOnKeyReleased().IsBoundToObject(PlayerCharacterObject) && DelegateHandle_OnKeyReleased.IsValid())
		{
			GetOnKeyReleased().Remove(DelegateHandle_OnKeyReleased);
			DelegateHandle_OnKeyReleased.Reset();
		}
		//if (!GetOnPlayerMovingStateChanged().IsBoundToObject(PlayerCharacterObject) && DelegateHandle_OnPlayerMovingStateChanged.IsValid())
		//{
		//	GetOnPlayerMovingStateChanged().Remove(DelegateHandle_OnPlayerMovingStateChanged);
		//	DelegateHandle_OnPlayerMovingStateChanged.Reset();
		//}
		DelegateHandle_OnKeyTriggered = GetOnKeyTriggered().AddUFunction(PlayerCharacterObject, TEXT("Server_Callback_OnKeyTriggered"));
		DelegateHandle_OnKeyReleased = GetOnKeyReleased().AddUFunction(PlayerCharacterObject, TEXT("Server_Callback_OnKeyReleased"));
		//DelegateHandle_OnPlayerMovingStateChanged = GetOnPlayerMovingStateChanged().AddUFunction(PlayerCharacterObject, TEXT("Server_Callback_OnPlayerMovingStateChanged"));
		return true;
	}
	return false;
}

FVector AHKKPlayerController::GetCachedInput_Implementation()
{
	return CachedInput;
}

void AHKKPlayerController::SetWallCoverable_Implementation(bool e)
{
	bWallCoverable = e;
	if (IsLocalPlayerController()) OnCreateInteractWidget.Broadcast(EInteractWidgetType::EIWT_WallCover, e);
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
		EnhancedInputComponent->BindAction(MouseRMBAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::MouseRMBTriggered);
		EnhancedInputComponent->BindAction(MouseRMBAction, ETriggerEvent::Completed, this, &AHKKPlayerController::MouseRMBCompleted);

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
	//if (InputVector.X >= 0.f && bShiftPressed) InputVector.Y = 0.f;
	//UE_LOG(LogTemp, Log, TEXT("Input Vector : %s"), *InputVector.ToString());
	FVector InputDirection{ InputVector.X, InputVector.Y, 0.f };
	float CurrentInputSec = GetWorld() ? GetWorld()->GetTimeSeconds() : -1.f;
	APawn* ControlledPawn = GetPawn();

	//if (FMath::IsNearlyZero(CachedInput.Size()))
	//{
	//	SetCachedDirection(FVector(InputDirection.X, InputDirection.Y, 0.f));
	//}
	//else
	//{
	float HoldingSec = CurrentInputSec - LastInputSec;
	//UE_LOG(LogTemp, Log, TEXT("Holding Sec : %f"), HoldingSec);
	//UE_LOG(LogTemp, Log, TEXT("Do Lerp : %s , %s"), *CachedDirection.ToString(), *InputDirection.ToString());
	//FMath::VInterpTo()
	SetCachedDirection(FMath::Lerp(CachedDirection, InputDirection, FMath::Min(HoldingSec, 5.f) / 5.f));
		//UE_LOG(LogTemp, Log, TEXT("Lerp Alpha : %f"), FMath::Min(HoldingSec, 2.f) / 2.f);
	//}

	bool debugMoveBasedOnCamera = false;
	if (ControlledPawn != nullptr)
	{
		FVector TempDirection = GetCachedDirection();
		float XScaleValue = TempDirection.X;
		float YScaleValue = TempDirection.Y;
		if (XScaleValue < 0.f)
		{
			XScaleValue *= 0.75f;
			YScaleValue *= 0.75f;
			//if (bShiftPressed)
			//{
			//	XScaleValue *= 3.f / 5.f;
			//	YScaleValue *= 3.f / 5.f;
			//}
		}
		MovePawn(ControlledPawn, FVector(XScaleValue, YScaleValue, 0.f));
	}
	if (CachedInput != InputDirection)
	{
		LastInputSec = CurrentInputSec;
	}
	SetCachedInput(InputDirection);
}

void AHKKPlayerController::MoveReleased()
{
	//CachedInput *= 0.f;
	//CachedDirection *= 0.f;
	SetCachedInput(FVector::ZeroVector);

	LastInputSec = GetWorld() ? GetWorld()->GetTimeSeconds() : -1.f;
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AHKKPlayerController::MoveStop_Gradual);
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
	if (UCombatLibrary::CanWeaponScrollSwitch(GetPlayerStateObject()))
	{
		OnAttack.Broadcast(EPlayerAnimation::EPA_EquipWeapon_R);
	}
	//if (GEngine)
	//{
	//	if (ScrollValue >= 0)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString("MouseScrollUp"));
	//	}
	//	else
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString("MouseScrollDown"));
	//	}
	//}
}

void AHKKPlayerController::MouseRMBTriggered()
{
	OnKeyTriggered.Broadcast(FKey("RightMouseButton"));
}

void AHKKPlayerController::MouseRMBCompleted()
{
	OnKeyReleased.Broadcast(FKey("RightMouseButton"));
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

void AHKKPlayerController::MovePawn(APawn* ControlledPawn, FVector MovementInput)
{
	const FRotator YawRotation(0, GetControlRotation().Yaw, 0);
	const FVector ForwardDirection = (ControlledPawn->GetActorForwardVector());
	const FVector RightDirection = (ControlledPawn->GetActorRightVector());
	ControlledPawn->AddMovementInput(ForwardDirection, MovementInput.X);
	ControlledPawn->AddMovementInput(RightDirection, MovementInput.Y);
}

void AHKKPlayerController::MoveStop_Gradual()
{
	float CurrentSec = GetWorld() ? GetWorld()->GetTimeSeconds() : -1.f;
	float NotPressingSec = CurrentSec - LastInputSec;
	FVector TempDirection = GetCachedDirection();
	SetCachedDirection(FMath::Lerp(TempDirection, FVector::ZeroVector, FMath::Min(NotPressingSec, 3.f) / 3.f));
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		MovePawn(ControlledPawn, GetCachedDirection());
		if (NotPressingSec < 5.f)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AHKKPlayerController::MoveStop_Gradual);
		}
		//else
		//{
		//	if (SetPlayerMovingState(EPlayerMovingState::EPMS_Default))
		//	{
		//		OnPlayerMovingStateChanged.Broadcast(EPlayerMovingState::EPMS_Default, FVector::ZeroVector);
		//	}
		//}
	}
}

void AHKKPlayerController::SetCachedDirection(FVector NewDirection)
{
	CachedDirection = NewDirection;
	UE_LOG(LogTemp, Log, TEXT("CachedDirection : %s"), *CachedDirection.ToString());
}

void AHKKPlayerController::SetCachedInput(FVector NewInput)
{
	CachedInput = NewInput;
	//FVector TempDirection = GetCachedDirection().GetSafeNormal2D();
	//if (FVector::DotProduct(TempDirection, CachedInput) < 0.75f || (CachedInput.X == 0 && CachedInput.Y == 0))
	//{
	//	//UE_LOG(LogTemp, Log, TEXT("Do Stop Anim : %s, Input : %s"), *TempDirection.ToString(), *CachedInput.ToString());
	//	if (SetPlayerMovingState(EPlayerMovingState::EPMS_ForwardToStop))
	//	{
	//		OnPlayerMovingStateChanged.Broadcast(EPlayerMovingState::EPMS_ForwardToStop, TempDirection);
	//	}
	//}
	//else
	//{
	//	if (SetPlayerMovingState(EPlayerMovingState::EPMS_Default))
	//	{
	//		OnPlayerMovingStateChanged.Broadcast(EPlayerMovingState::EPMS_Default, FVector::ZeroVector);
	//	}
	//}
}

bool AHKKPlayerController::SetPlayerMovingState(EPlayerMovingState NewMovingState)
{
	if (PlayerMovingState != NewMovingState)
	{
		PlayerMovingState = NewMovingState;
		return true;
	}
	return false;
}

void AHKKPlayerController::Server_SetShiftPressed_Implementation(bool e)
{
	bShiftPressed = e;
}
