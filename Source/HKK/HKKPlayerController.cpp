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
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AHKKPlayerController::AHKKPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
	PrimaryActorTick.bCanEverTick = true;
}

void AHKKPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
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

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AHKKPlayerController::MoveReleased);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AHKKPlayerController::JumpReleased);

		EnhancedInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::MouseMoved);
		EnhancedInputComponent->BindAction(MouseAttackAction, ETriggerEvent::Triggered, this, &AHKKPlayerController::Attack0_RFistTriggered);
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

// Triggered every frame when the input is held down
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
	FVector2D InputVector = Value.Get<FVector2D>();
	FVector InputDirection{ InputVector.X, InputVector.Y, 0.f };
	float CurrentInputSec = GetWorld() ? GetWorld()->GetTimeSeconds() : -1.f;
	//UE_LOG(LogTemp, Log, TEXT("Input Direction X : %f, Y : %f"), InputDirection.X, InputDirection.Y);

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

	if (ControlledPawn != nullptr)
	{
		ControlledPawn->AddMovementInput(CachedDirection, 1.0, false);
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

void AHKKPlayerController::Jump()
{
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
	FVector2D InputVector = Value.Get<FVector2D>();
	//UE_LOG(LogTemp, Log, TEXT("Mouse Moved X : %f, Y : %f"), InputVector.X, InputVector.Y);
}

void AHKKPlayerController::Attack0_RFistTriggered(const FInputActionValue& Value)
{
	//uint8 Attack_Index = FMath::TruncToInt(Value.Get<float>());
	//EPlayerAnimation AttackType = StaticCast<EPlayerAnimation>(Attack_Index);
	OnAttack.Broadcast(EPlayerAnimation::EPA_Attack_RFist);
	//UE_LOG(LogTemp, Log, TEXT("Attack0_RFistTriggered : %d"), Attack_Index);
}
