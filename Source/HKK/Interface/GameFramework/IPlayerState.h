#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_Delegates.h"
#include "HKK_Structs.h"
#include "IPlayerState.generated.h"

UINTERFACE(MinimalAPI)
class UIPlayerState : public UInterface
{
	GENERATED_BODY()
};

class UObject;

class HKK_API IIPlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetCurrHP();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetCurrStamina();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetMaxHP();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetMaxStamina();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCurrHP(float e);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCurrStamina(float e);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetMaxHP(float e);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetMaxStamina(float e);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool BindDelegate_HUDWidget(UObject* BindWidget);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool BindDelegate_InventoryWidget(UObject* BindWidget);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool BindDelegate_Equipment(UObject* BindCharacter);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetItem(const FItemConfig& ItemConfig);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetEquipmentSlotItemConfig(TArray<FItemConfig>& InEquipmentSlotItemConfig);
};
