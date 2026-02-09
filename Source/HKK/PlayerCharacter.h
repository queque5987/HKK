#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HKK_Delegates.h"
#include "HKK_PCH.h"
#include "HKK_Structs.h"
#include "Interface/ICharacterMovement.h"
#include "Interface/Character/ICharacterCombat.h"
#include "Interface/Character/ICharacterWidget.h"

#include "PlayerCharacter.generated.h"

class APlayerState;
class UFoliageInteractSourceComponent;
class UItemDataObject;

UCLASS()
class HKK_API APlayerCharacter : public ACharacter, public IICharacterMovement, public IICharacterCombat, public IICharacterWidget
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterAnimationComponent> AnimationComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterCombatComponent> CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFoliageInteractSourceComponent> FoliageInteractSourceComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(Replicated)
	float AimingYaw;
	UPROPERTY(Replicated)
	bool RotatePawnBasedOnControlRotation = false;
	UPROPERTY(Replicated)
	bool RotatePawnBasedFacingWallNormal = false;
	UPROPERTY(ReplicatedUsing = OnRep_MaxWalkSpeed)
	float MaxWalkSpeed = 500.f;
	UPROPERTY(ReplicatedUsing = OnRep_MaxAcceleration)
	float MaxAcceleration = 512.f;
	UPROPERTY(Replicated)
	EPlayerState CurrentPlayerState;

	class IICharacterAnimInstance* IAnimInstace;

	/*
----- Delegates Start
	*/
	//FOnAiming* OnAiming;
	//FOnPlayAnimation* OnPlayAnimation;
	//FOnAttack* OnAttack;
	FOnSetItemInteractWidget* OnSetItemInteractPickupWidget = nullptr;
	/*
----- Delegates End
	*/

	virtual void BeginPlay() override;
	virtual void Restart() override;
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1.0f, bool bForce = false) override;

public:	
	UFUNCTION()
	void OnRep_MaxWalkSpeed();
	UFUNCTION()
	void OnRep_MaxAcceleration();

	FORCEINLINE float GetAimingYaw() { return AimingYaw; }
	virtual UObject* GetAnimInstanceObject_Implementation() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_Callback_PlaySimpleAnimation(EPlayerAnimation ToPlayAnimation, FName SlotName);
	UFUNCTION(Server, Reliable)
	void Server_PlayAnimation(class UAnimSequence* PlayAnimation, FName SlotName = TEXT("AttackSlot"));
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAnimation(class UAnimSequence* PlayAnimation, FName SlotName);

	UFUNCTION(Server, Unreliable)
	void Server_OnAiming(float Yaw);
	UFUNCTION(Server, Unreliable)
	void Server_RefreshVelocity();
	UFUNCTION(Server, Reliable)
	void Server_RotatePawnBasedOnConrolRotation();

	UFUNCTION()
	void Callback_OnAttack(const EPlayerAnimation AttackType);
	UFUNCTION(Client, Unreliable)
	void Client_RefreshCharacterMovementState();
	UFUNCTION(Server, Unreliable)
	void Server_SetCharacterMovementState(FCharacterMovementState NewCharacterMovementState);
	UFUNCTION(Server, Reliable)
	void Server_Callback_OnKeyTriggered(const FKey Key);
	UFUNCTION(Server, Reliable)
	void Server_Callback_OnKeyReleased(const FKey Key);
	UFUNCTION(Server, Unreliable)
	void Server_AnimInstance_SetBoolValue(EPlayerState ToSetPlayerState, bool e);
	/*
----- ICharacter Movement Start
	*/
protected:
	UPROPERTY(Replicated)
	FCharacterMovementState CharacterMovementState;
	
	bool EquipmentBind = false;
	bool ControllerBind = false;

	void LoadingRace();
public:
	virtual FVector _GetVelocity() override { return GetVelocity(); };
	virtual FCharacterMovementState* GetCharacterMovementState() override { return &CharacterMovementState; };
	/*
----- ICharacter Movement End
	*/

	/*
----- ICharacter Combat Start
	*/
	virtual bool HitTraceStart(FHitTraceConfig* HitTraceConfig, float MaxTraceTime) override;
	virtual bool HitTraceEnd() override;
	virtual bool HitTrace(FHitTraceConfig* HitTraceConfig) override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_HitDamage(const FHitDamageConfig& HitTraceConfig) override;
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_KnockBack(FVector Direction) override;
	virtual UObject* GetPlayerStateObject() override { return (UObject*)GetPlayerState(); };
	virtual UObject* GetControllerObject_Implementation() override { return (UObject*)GetController(); };
	virtual void AnimInstance_SetBoolValue_Implementation(EPlayerState ToSetPlayerState, bool e) override;
	virtual bool AttachItem_Implementation(AActor* AttachItemActor, FName AttachSocketName) override;
	virtual bool DetachItem_Implementation(AActor* AttachItemActor) override;
	virtual void RotatePawnBasedOnControlRotation_Implementation() override;
	virtual void SetWallCoverable_Implementation(bool e) override;

	/*
----- ICharacter Combat End
	*/

	/*
----- ICharacter Widget Start
	*/
	virtual void SetItemInteractWidget(bool ToSet, TScriptInterface<class IIPickableItem> PickableItem, const FItemConfig& ItemConfig) override;
	/*
----- ICharacter Widget End
	*/
	UFUNCTION()
	void OnEquipmentSlotSwitched(const FItemConfig& EquipItemConfig);

};
