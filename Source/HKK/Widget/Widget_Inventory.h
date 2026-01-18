#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TreeView.h"
#include "Components/TileView.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Interface/Widget/IUpdatableWidget.h"
#include "Widget_Inventory.generated.h"

class UItemDataObject;

UCLASS()
class HKK_API UWidget_Inventory : public UUserWidget, public IIUpdatableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UTreeView* InventoryTree;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTileView* QuickSlotTile;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTileView* EquipmentSlotTile;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_WeaponName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_AmmoMax;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_AmmoLeft;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UImage* Image_WeaponPreview;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstance> MI_DefaultWeaponPreview;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MID_WeaponPreview;

	TArray<TObjectPtr<UItemDataObject>>* QuickSlotEmptyObjects;
	TArray<FKey>* QuickSlotKeySetting;

protected:
	virtual void NativeConstruct() override;
public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	virtual void OnUpdatePlayerStatFloat_Implementation(const EPlayerStatType PlayerStatType, float CurrStat, float MaxStat) override;
	virtual void AddItem_Implementation(bool bPutIn, EUserWidget AddWidgetType, const FItemConfig& ItemConfig) override;
	virtual void AddNewItemObject_Implementation(UObject* AddItemObject) override;
	virtual void EquipItem_Implementation(const FItemConfig& EquipItemConfig) override;

	UFUNCTION()
	virtual void OnUpdateQuickSlot_Implementation(UObject* UpdatedItem, FKey Key) override;
	void UpdateQuickSlotObjectArr(TArray<UObject*>& InQuickSlotObjectArr);
	void UpdateEquipmentSlotObjectArr(TArray<UObject*>& InEquipmentSlotObjectArr);
	const TArray<UObject*>& GetQuickSlotObjectArr();
	const TArray<UObject*>& GetEquipmentSlotObjectArr();

	void QuickSlot_AddItemAsObject(UObject* InItem);
	void EquipmentSlot_AddItemAsObject(UObject* InItem);

	void SetQuickSlotEmptyObjects(TArray<TObjectPtr<UItemDataObject>>* InQuickSlotEmptyObjects, TArray<FKey>* InQuickSlotKeySetting)
	{
		QuickSlotEmptyObjects = InQuickSlotEmptyObjects; 
		QuickSlotKeySetting = InQuickSlotKeySetting;
	};
};
