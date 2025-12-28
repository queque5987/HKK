#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interface/GameFramework/IPlayerState.h"
#include "HKK_Delegates.h"
#include "HKK_PCH.h"
#include "HKK_PlayerState.generated.h"

UCLASS()
class HKK_API AHKK_PlayerState : public APlayerState, public IIPlayerState
{
	GENERATED_BODY()

private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrHP, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrHP = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_MaxHP, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxHP = 100.f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrStamina;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxStamina;

	FOnUpdateStatFloat OnUpdateStatFloat;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetCurrHP_Implementation() override { return CurrHP; };
	virtual float GetCurrStamina_Implementation() override { return CurrStamina; };
	virtual float GetMaxHP_Implementation() override { return MaxHP; };
	virtual float GetMaxStamina_Implementation() override { return MaxStamina; };

	virtual void SetCurrHP_Implementation(float e) override {
		CurrHP = e; 
		if (HasAuthority())OnRep_CurrHP();
	};
	virtual void SetCurrStamina_Implementation(float e) override { CurrStamina = e; };
	virtual void SetMaxHP_Implementation(float e) override {
		MaxHP = e; 
		if (HasAuthority()) OnRep_MaxHP(); 
	};
	virtual void SetMaxStamina_Implementation(float e) override { MaxStamina = e; };

	virtual bool BindDelegate_HUDWidget_Implementation(class UObject* BindWidget) override;

	UFUNCTION()
	void OnRep_CurrStamina();
	UFUNCTION()
	void OnRep_CurrHP();
	UFUNCTION()
	void OnRep_MaxStamina();
	UFUNCTION()
	void OnRep_MaxHP();
};
