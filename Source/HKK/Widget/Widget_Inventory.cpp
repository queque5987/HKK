#include "Widget/Widget_Inventory.h"
#include "Widget/Object/ItemDataObject.h"
#include "HKK_PCH.h"

void UWidget_Inventory::OnUpdatePlayerStatFloat_Implementation(const EPlayerStatType PlayerStatType, float CurrStat, float MaxStat)
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

void UWidget_Inventory::AddItem_Implementation(bool bPutIn, EUserWidget AddWidgetType, const FItemConfig& ItemConfig)
{
	UItemDataObject* DataObject = NewObject<UItemDataObject>(this);
	if (DataObject == nullptr) return;
	DataObject->ItemConfig = ItemConfig;
	InventoryTree->AddItem(DataObject);
}
