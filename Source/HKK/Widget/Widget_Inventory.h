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

UCLASS()
class HKK_API UWidget_Inventory : public UUserWidget, public IIUpdatableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UTreeView* InventoryTree;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UTileView* QuickSlotTile;

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

protected:
	virtual void NativeConstruct() override;

public:
	virtual void OnUpdatePlayerStatFloat_Implementation(const EPlayerStatType PlayerStatType, float CurrStat, float MaxStat) override;
	virtual void AddItem_Implementation(bool bPutIn, EUserWidget AddWidgetType, const FItemConfig& ItemConfig) override;
	void QuickSlot_AddItemAsObject(UObject* InItem);
};
