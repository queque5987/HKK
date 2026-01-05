#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Interface/Widget/IUpdatableWidget.h"
#include "HKK_PCH.h"
#include "Widget_HUD.generated.h"

UCLASS()
class HKK_API UWidget_HUD : public UUserWidget, public IIUpdatableWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_WeaponPreview;

	UPROPERTY(meta = (BindWidget))
	class UTileView* QuickSlotTile;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_AmmoLeft;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_AmmoMax;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_WeaponName;
public:
	virtual void OnUpdatePlayerStatFloat_Implementation(const EPlayerStatType PlayerStatType, float CurrStat, float MaxStat) override;

	void QuickSlot_AddItemAsObject(UObject* InItem);
};
