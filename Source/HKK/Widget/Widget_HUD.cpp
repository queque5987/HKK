#include "Widget/Widget_HUD.h"
#include "Widget/Object/ItemDataObject.h"
#include "Kismet/KismetMaterialLibrary.h"

void UWidget_HUD::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	//Execute_OnSetVisibility(this, InVisibility == ESlateVisibility::Collapsed || InVisibility == ESlateVisibility::Hidden ? false : true);
}

void UWidget_HUD::OnUpdatePlayerStatFloat_Implementation(const EPlayerStatType PlayerStatType, float CurrStat, float MaxStat)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("UI Function Called! HP: %f / %f"), CurrStat, MaxStat));
	switch (PlayerStatType)
	{
	case(EPlayerStatType::EPST_HP):
		if (HPBar == nullptr) return;
		HPBar->SetPercent(CurrStat / MaxStat);
		break;
	case(EPlayerStatType::EPST_Stamina):
		if (StaminaBar == nullptr) return;
		StaminaBar->SetPercent(CurrStat / MaxStat);
		break;
	default:
		break;
	}

}

void UWidget_HUD::EquipItem_Implementation(const FItemConfig& EquipItemConfig)
{
	if (Image_PassedWeaponPreview)
	{
		Image_PassedWeaponPreview->SetBrushFromMaterial(
			MID_WeaponPreview != nullptr ? MID_WeaponPreview : MI_DefaultWeaponPreview
		);
	}
	MID_WeaponPreview = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), EquipItemConfig.HUDMaterialInstance);
	if (Image_WeaponPreview)
	{
		Image_WeaponPreview->SetBrushFromMaterial(
			MID_WeaponPreview != nullptr ? MID_WeaponPreview : MI_DefaultWeaponPreview
		);
	}
	if (Text_WeaponName)
	{
		Text_WeaponName->SetText(
			MID_WeaponPreview != nullptr ? EquipItemConfig.ItemName : FText::FromString(TEXT("맨손")));
	}
	Execute_OnEquipmentScrolled(this, IsLastTriggeredScrollUp);
}

void UWidget_HUD::OnUpdateQuickSlot_Implementation(UObject* UpdatedItem, FKey Key)
{
}

void UWidget_HUD::OnKeyTriggered_Implementation(FKey Key)
{
	if (Key == FKey("MouseScrollUp"))
	{
		IsLastTriggeredScrollUp = true;
	}
	else if (Key == FKey("MouseScrollDown"))
	{
		IsLastTriggeredScrollUp = false;
	}

}

void UWidget_HUD::UpdateQuickSlotObjectArr(TArray<UObject*>& InQuickSlotObjectArr)
{
	if (QuickSlotTile) QuickSlotTile->SetListItems(InQuickSlotObjectArr);
}

void UWidget_HUD::QuickSlot_AddItemAsObject(UObject* InItem)
{
	if (InItem != nullptr) QuickSlotTile->AddItem(InItem);
}
