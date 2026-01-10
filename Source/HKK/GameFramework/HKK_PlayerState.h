#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interface/GameFramework/IPlayerState.h"
#include "HKK_Delegates.h"
#include "HKK_PCH.h"
#include "HKK_Structs.h"
#include "HKK_PlayerState.generated.h"

class UItemDataObject;

UCLASS()
class HKK_API AHKK_PlayerState : public APlayerState, public IIPlayerState
{
	GENERATED_BODY()
	AHKK_PlayerState();
private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrHP, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrHP = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_MaxHP, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxHP = 100.f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float CurrStamina;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float MaxStamina;
	//UPROPERTY(ReplicatedUsing = OnRep_PossesingItem, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TArray<FItemConfig> PossesingItem;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UItemDataObject>> PossingItemData;

	TArray<TWeakObjectPtr<UItemDataObject>> QuickSlotItemData;

	FOnUpdateStatFloat OnUpdateStatFloat;
	FOnSetItemInteractWidget OnUpdateItemToInventory;
	FOnAddItemDataObject OnAddItemDataObject;
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetCurrHP_Implementation() override { return CurrHP; };
	virtual float GetCurrStamina_Implementation() override { return CurrStamina; };
	virtual float GetMaxHP_Implementation() override { return MaxHP; };
	virtual float GetMaxStamina_Implementation() override { return MaxStamina; };

	virtual void SetCurrHP_Implementation(float e) override { Server_SetCurrHP(e); };
	virtual void SetCurrStamina_Implementation(float e) override { Server_SetCurrStamina(e); };
	virtual void SetMaxHP_Implementation(float e) override { Server_SetMaxHP(e); };
	virtual void SetMaxStamina_Implementation(float e) override { Server_SetMaxStamina(e); };

	virtual bool BindDelegate_HUDWidget_Implementation(class UObject* BindWidget) override;
	virtual bool BindDelegate_InventoryWidget_Implementation(class UObject* BindWidget) override;
	virtual void GetItem_Implementation(const FItemConfig& ItemConfig) override;
	UFUNCTION(Server, Reliable)
	void Server_GetItem(const FItemConfig& ItemConfig);
	UFUNCTION(Server, Reliable)
	void Server_SetCurrHP(float e);
	UFUNCTION(Server, Reliable)
	void Server_SetCurrStamina(float e);
	UFUNCTION(Server, Reliable)
	void Server_SetMaxHP(float e);
	UFUNCTION(Server, Reliable)
	void Server_SetMaxStamina(float e);

	UFUNCTION()
	void OnRep_CurrStamina();
	UFUNCTION()
	void OnRep_CurrHP();
	UFUNCTION()
	void OnRep_MaxStamina();
	UFUNCTION()
	void OnRep_MaxHP();

	UFUNCTION()
	void OnItemDataUpdated(UObject* UpdatedItem);
};
