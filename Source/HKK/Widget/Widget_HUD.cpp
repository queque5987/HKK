#include "Widget/Widget_HUD.h"
#include "Widget/Object/ItemDataObject.h"

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

void UWidget_HUD::OnUpdateQuickSlot_Implementation(UObject* UpdatedItem, FKey Key)
{
}

void UWidget_HUD::UpdateQuickSlotObjectArr(TArray<UObject*>& InQuickSlotObjectArr)
{
	if (QuickSlotTile) QuickSlotTile->SetListItems(InQuickSlotObjectArr);
}

void UWidget_HUD::QuickSlot_AddItemAsObject(UObject* InItem)
{
	if (InItem != nullptr) QuickSlotTile->AddItem(InItem);
}
