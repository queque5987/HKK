#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HKK_Delegates.h"
#include "HKK_Structs.h"
#include "HKK_PCH.h"
#include "InputCoreTypes.h"
#include "ControllerWidgetComponent.generated.h"

class IIWidgetController;
class UWidget_ItemInteract;
class UWidget_HUD;
class UWidget_Inventory;
class UItemDataObject;
class AActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HKK_API UControllerWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UControllerWidgetComponent();

protected:
	virtual void BeginPlay() override;

	TScriptInterface<IIWidgetController> WidgetController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UWidget_ItemInteract> Widget_ItemInteractClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidget_ItemInteract> Widget_ItemInteract;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidget_HUD> Widget_HUD;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidget_Inventory> Widget_Inventory;
	float InventoryInterval = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UItemDataObject>> QuickslotObjects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UItemDataObject>> EquipmentslotObjects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FKey> QuickslotKeySetting;

	UINT16 WidgetFloating;

	TObjectPtr<AActor> InteractingPickableItem;
	FOnGetItem* OnGetItem;
	TArray<TWeakObjectPtr<UWidget_ItemInteract>> FloatingInteractWidgetArr;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool SetController(TScriptInterface<IIWidgetController> InWidgetController);
	bool Bind_HUD(TScriptInterface<IIWidgetController> InWidgetController);
	bool Bind_EquipmentSlot(TScriptInterface<IIWidgetController> InWidgetController);
	UFUNCTION()
	void SetItemInteractPickupWidget(bool ToSet, EUserWidget WidgetType, const FItemConfig& ItemConfig, UObject* PickableItemObject = nullptr);
	UFUNCTION()
	void Callback_OnKeyTriggered(const FKey Key);
	UFUNCTION()
	void Callback_OnKeyReleased(const FKey Key);
	UFUNCTION()
	void Callback_ChangedQuickSlot(UObject* ChangedItemObject, FKey ChangedKey);
	UFUNCTION()
	void Callback_ChangedEquipment(UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType);
	UFUNCTION()
	void Callback_OnCreateInteractWidget(EInteractWidgetType InteractWidgetType, bool ToSet);

	bool IsControllable();
	void OnEquipmentItemDragDetected(bool IsOn);

	EEquipmentSlotType GetLeftEquipmentSlotIndex();
};
