#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interface/GameFramework/IPlayerState.h"
#include "CPlayerState.generated.h"

UCLASS()
class HKK_API ACPlayerState : public APlayerState, public IIPlayerState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrHP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxHP;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrStamina;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxStamina;

public:
	FORCEINLINE virtual float GetCurrHP() override { return CurrHP; };
	FORCEINLINE virtual float GetCurrStamina() override { return CurrStamina; };
	FORCEINLINE virtual float GetMaxHP() override { return MaxHP; };
	FORCEINLINE virtual float GetMaxStamina() override { return MaxStamina; };

	virtual void SetCurrHP(float e) override { CurrHP = e; };
	virtual void SetCurrStamina(float e) override { CurrStamina = e; };
	virtual void SetMaxHP(float e) override { MaxHP = e; };
	virtual void SetMaxStamina(float e) override { MaxStamina = e; };
};
