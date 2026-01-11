// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "HKK_Delegates.h"
#include "Interface/Controller/IWidgetController.h"
#include "Interface/Controller/ICombatController.h"
#include "GameFramework/Character.h"
#include "HKKPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class IIPlayerState;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class AHKKPlayerController : public APlayerController, public IIWidgetController, public IICombatController
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
	UInputAction* UISwitchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ShiftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MouseMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MouseScrollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MouseAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* QuickSlotAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UControllerWidgetComponent> WidgetComponent;

	FORCEINLINE FOnAiming& GetOnAiming() { return OnAiming; }
	FORCEINLINE FOnPlayAnimation& GetOnPlayAnimation() { return OnPlayAnimation; }
	FORCEINLINE FOnAttack& GetOnAttack() { return OnAttack; }

	/*
-----IIWidgetController Start
	*/

	FORCEINLINE virtual FOnSetItemInteractWidget& GetOnSetItemInteractPickupWidget() override { return OnSetItemInteractPickupWidget; }
	FORCEINLINE virtual FOnKeyInputEvent& GetOnKeyTriggered() override { return OnKeyTriggered; }
	FORCEINLINE virtual FOnKeyInputEvent& GetOnKeyReleased() override { return OnKeyReleased; }
	FORCEINLINE virtual FOnGetItem& GetOnGetItem() override { return OnGetItem; }
	FORCEINLINE virtual FOnQuickSlotUpdated& GetOnQuickSlotUpdated() override { return OnQuickSlotUpdated; }
	FORCEINLINE virtual FOnItemEquiped& GetOnItemEquiped() override { return OnItemEquiped; }
	virtual FVector GetPlayerLocation() override { return GetCharacter() != nullptr ? GetCharacter()->GetActorLocation() : FVector::ZeroVector; };
	virtual UObject* GetPlayerStateObject() override { return (UObject*)PlayerState; };
	virtual void SetCurorVisibility(bool e) override { bShowMouseCursor = e; };

	UFUNCTION()
	virtual void ChangeQuickSlot_Implementation(UObject* ChangedItemObject, FKey ChangedKey) override;
	virtual void ChangeEquipSlot_Implementation(UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType) override;
	virtual void EquipmentItemDragDetected_Implementation(bool e) override;
	virtual EEquipmentSlotType GetLeftEquipmentSlotIndex_Implementation() override;

	/*
-----IIWidgetController End
	*/

	/*
-----IICombatController Start
	*/

	/*
-----IICombatController End
	*/

	virtual void OnRep_PlayerState() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void InitPlayerState() override;
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
	FOnSetItemInteractWidget OnSetItemInteractPickupWidget;
	FOnKeyInputEvent OnKeyTriggered;
	FOnKeyInputEvent OnKeyReleased;
	FOnGetItem OnGetItem;
	FOnSetWalkSpeed OnSetWalkSpeed;
	FOnQuickSlotUpdated OnQuickSlotUpdated;
	FOnItemEquiped OnItemEquiped;

	/*
		Delegates End
	*/
	FInputModeGameOnly InputModeGameOnly;
	FTimerHandle LoadingTimerHandle;

	bool DelegateBind = false;
	bool WidgetControllerSetup = false;
	bool WidgetHUDBind = false;
	bool bShiftPressed = false;

	virtual void SetupInputComponent() override;
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void LoadingRace();

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

	void OnClicked();
	void Move(const FInputActionValue& Value);
	void MoveReleased();
	void ShiftTriggered();
	void ShiftReleased();
	void Jump();
	void JumpReleased();
	void MouseMoved(const FInputActionValue& Value);
	void MouseScrolled(const FInputActionValue& Value);
	void Attack0_RFistTriggered(const FInputActionValue& Value);
	void InteractTriggered();
	void InteractReleased();
	void UISwitchTriggered(const FInputActionValue& Value);
	void QuickSlotTriggered(const FInputActionValue& Value);
private:
	FVector CachedDestination;
	FVector CachedDirection;
	FVector CachedInput;
	float LastInputSec;
	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed
};


