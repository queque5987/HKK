#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HKK_Delegates.h"
#include "HKK_Structs.h"
#include "Interface/ICharacterMovement.h"
#include "Interface/Character/ICharacterCombat.h"
#include "CPlayerCharacter.generated.h"

UCLASS()
class HKK_API ACPlayerCharacter : public ACharacter, public IICharacterMovement, public IICharacterCombat
{
	GENERATED_BODY()

public:
	ACPlayerCharacter();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCCharacterAnimationComponent> AnimationComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCCharacterCombatComponent> CombatComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(Replicated)
	float AimingYaw;

	class IICharacterAnimInstance* IAnimInstace;

	/*
----- Delegates Start
	*/
	FOnAiming* OnAiming;
	FOnPlayAnimation* OnPlayAnimation;
	FOnAttack* OnAttack;
	/*
----- Delegates End
	*/

	virtual void BeginPlay() override;

public:	

	FORCEINLINE float GetAimingYaw() { return AimingYaw; }

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_PlayAnimation(class UAnimSequence* PlayAnimation);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAnimation(class UAnimSequence* PlayAnimation);

	UFUNCTION(Server, Unreliable)
	void Server_OnAiming(float Yaw);
	UFUNCTION(Server, Unreliable)
	void Server_RefreshVelocity();
	

	UFUNCTION()
	void Callback_OnAttack(int8 AttackType);

	/*
----- ICharacter Movement Start
	*/
protected:
	UPROPERTY(Replicated)
	FCharacterMovementState CharacterMovementState;

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

	/*
----- ICharacter Combat End
	*/
};
