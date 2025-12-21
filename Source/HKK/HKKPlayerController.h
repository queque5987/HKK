// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "HKK_Delegates.h"
#include "Interface/Controller/IWidgetController.h"
#include "HKKPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class AHKKPlayerController : public APlayerController, public IIWidgetController
{
	GENERATED_BODY()

public:
	AHKKPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MouseMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MouseAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCControllerWidgetComponent> WidgetComponent;

	FORCEINLINE FOnAiming* GetOnAiming() { return &OnAiming; }
	FORCEINLINE FOnPlayAnimation* GetOnPlayAnimation() { return &OnPlayAnimation; }
	FORCEINLINE FOnAttack* GetOnAttack() { return &OnAttack; }
	FORCEINLINE virtual FOnSetWidget* GetOnSetWidget() override { return &OnSetWidget; }


protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	/*
		Delegates Start
	*/
	// Aiming Rotation
	FOnAiming OnAiming;
	FOnPlayAnimation OnPlayAnimation;
	FOnAttack OnAttack;
	FOnSetWidget OnSetWidget;
	/*
		Delegates End
	*/
	FInputModeGameOnly InputModeGameOnly;

	virtual void SetupInputComponent() override;
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

	void OnClicked();
	void Move(const FInputActionValue& Value);
	void MoveReleased();
	void Jump();
	void JumpReleased();
	void MouseMoved(const FInputActionValue& Value);
	void Attack0_RFistTriggered(const FInputActionValue& Value);
private:
	FVector CachedDestination;
	FVector CachedDirection;
	FVector CachedInput;
	float LastInputSec;
	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed
};


