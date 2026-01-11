#include "Component/ControllerWidgetComponent.h"
#include "Interface/Controller/IWidgetController.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Widget_ItemInteract.h"
#include "Widget/Widget_HUD.h"
#include "Widget/Widget_Inventory.h"
#include "GameFramework/CombatLibrary.h"
#include "Widget/Object/ItemDataObject.h"

class IIPlayerState;

UControllerWidgetComponent::UControllerWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	WidgetFloating = 0;
	QuickslotObjects.Reserve(3);
	QuickslotKeySetting.Reserve(3);
	EquipmentslotObjects.Reserve(3);
}

void UControllerWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UControllerWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (WidgetController == nullptr) return;

	FVector2D ProjectLocation = FVector2D::ZeroVector;
	const APlayerController* PC = Cast<APlayerController>(WidgetController->_getUObject());
	//UGameplayStatics::ProjectWorldToScreen(PC, WidgetController->GetPlayerLocation(), ProjectLocation);
	int32 ScreensizeX;
	int32 ScreensizeY;
	PC->GetViewportSize(ScreensizeX, ScreensizeY);
	if (WidgetFloating & 1 << (uint8)EUserWidget::EUW_ItemInteract)
	{
		if (InteractingPickableItem != nullptr)
		{
			UGameplayStatics::ProjectWorldToScreen(PC, InteractingPickableItem->GetActorLocation(), ProjectLocation);
			//ProjectLocation.X += ScreensizeX / 12.f; TODO; X + Get Actor Size?
		}
		Widget_ItemInteract->SetPositionInViewport(ProjectLocation);
	}
	if (InventoryInterval > 0.f) InventoryInterval -= DeltaTime;
}

bool UControllerWidgetComponent::SetController(TScriptInterface<IIWidgetController> InWidgetController)
{
	WidgetController = InWidgetController;
	
	if (WidgetController == nullptr) return false;
	if (!WidgetController->GetOnSetItemInteractPickupWidget().IsBoundToObject(WidgetController->_getUObject()))
	{
		WidgetController->GetOnSetItemInteractPickupWidget().AddUFunction(this, TEXT("SetItemInteractPickupWidget"));
		WidgetController->GetOnKeyTriggered().AddUFunction(this, TEXT("Callback_OnKeyTriggered"));
		WidgetController->GetOnKeyReleased().AddUFunction(this, TEXT("Callback_OnKeyReleased"));
		WidgetController->GetOnQuickSlotUpdated().AddUFunction(this, TEXT("Callback_ChangedQuickSlot"));
		WidgetController->GetOnQuickSlotUpdated().AddUFunction(WidgetController->GetPlayerStateObject(), TEXT("Callback_ChangedQuickSlot"));
		WidgetController->GetOnItemEquiped().AddUFunction(this, TEXT("Callback_ChangedEquipment"));
		WidgetController->GetOnItemEquiped().AddUFunction(WidgetController->GetPlayerStateObject(), TEXT("Callback_OnEquipmentItemSlotChanged"));

		Widget_Inventory->SetQuickSlotEmptyObjects(&QuickslotObjects, &QuickslotKeySetting);
		Widget_HUD->SetQuickSlotEmptyObjects(&QuickslotObjects, &QuickslotKeySetting);
	}
	if (OnGetItem == nullptr)
	{
		OnGetItem = &WidgetController->GetOnGetItem();
		OnGetItem->AddLambda([this](const FItemConfig& ItemConfig, UObject* OwningPlayerState)
			{
				SetItemInteractPickupWidget(false, EUserWidget::EUW_ItemInteract, ItemConfig);
			}
		);
	}
	if (Widget_ItemInteract == nullptr) return false;
	if (Widget_HUD == nullptr) return false;
	if (Widget_Inventory == nullptr) return false;
	APlayerController* OwningController = Cast<APlayerController>(InWidgetController->_getUObject());

	Widget_ItemInteract->AddToViewport();
	Widget_ItemInteract->SetOwningPlayer(OwningController);
	Widget_ItemInteract->SetVisibility(ESlateVisibility::Collapsed);
	Widget_HUD->AddToViewport();
	Widget_HUD->SetVisibility(ESlateVisibility::HitTestInvisible);
	Widget_HUD->SetOwningPlayer(OwningController);
	Widget_Inventory->AddToViewport();
	Widget_Inventory->SetVisibility(ESlateVisibility::Collapsed);
	Widget_Inventory->SetOwningPlayer(OwningController);

	if (QuickslotObjects.Num() < 3)
	{
		for (int8 i = 1; i < 4; i++)
		{
			UItemDataObject* DataObject = NewObject<UItemDataObject>(this);
			FKey QKey = FKey(*FString::FromInt(i));
			DataObject->OwningPlayer = OwningController;
			DataObject->ItemConfig.ItemIcon;
			DataObject->ItemConfig.ItemName;
			DataObject->ItemConfig.ItemCount = 0;
			DataObject->ItemConfig.QuickSlotKey = QKey;
			DataObject->SetIsQuickSlotInitializer(QKey);
			DataObject->UseAsEmptySlot();
			Widget_HUD->QuickSlot_AddItemAsObject(DataObject);
			Widget_Inventory->QuickSlot_AddItemAsObject(DataObject);
			QuickslotKeySetting.Add(QKey);
			QuickslotObjects.Add(DataObject);
		}
	}
	if (EquipmentslotObjects.Num() < 3)
	{
		for (uint8 i = 1; i < 4; i++)
		{
			UItemDataObject* DataObject = NewObject<UItemDataObject>(this);
			DataObject->OwningPlayer = OwningController;
			DataObject->ItemConfig.ItemCount = 0;
			DataObject->ItemConfig.EquipmentSlotType = StaticCast<EEquipmentSlotType>(i);
			DataObject->UseAsEmptySlot();
			Widget_Inventory->EquipmentSlot_AddItemAsObject(DataObject);
			EquipmentslotObjects.Add(DataObject);
		}
	}

	return true;
}

bool UControllerWidgetComponent::Bind_HUD(TScriptInterface<IIWidgetController> InWidgetController)
{
	if (InWidgetController == nullptr) return false;
	UObject* PlayerStateObject = InWidgetController->GetPlayerStateObject();
	if (PlayerStateObject == nullptr) return false;
	if (Widget_HUD == nullptr) return false;
	if (Widget_Inventory == nullptr) return false;
	UCombatLibrary::Bind_Inventory(Widget_Inventory, PlayerStateObject);
	return (
		UCombatLibrary::Bind_HUD(Widget_HUD,PlayerStateObject) &&
		UCombatLibrary::Bind_HUD(Widget_Inventory,PlayerStateObject)
		);
}

bool UControllerWidgetComponent::Bind_EquipmentSlot(TScriptInterface<IIWidgetController> InWidgetController)
{
	if (InWidgetController == nullptr) return false;
	UObject* PlayerStateObject = InWidgetController->GetPlayerStateObject();
	if (PlayerStateObject == nullptr) return false;
	if (Widget_HUD == nullptr) return false;
	if (Widget_Inventory == nullptr) return false;
	return false;
}

void UControllerWidgetComponent::SetItemInteractPickupWidget(bool ToSet, EUserWidget WidgetType, const FItemConfig& ItemConfig)
{
	FVector2D ProjectLocation;
	InteractingPickableItem = ItemConfig.SpawnedItemActor;
	if (WidgetController == nullptr || WidgetController == nullptr)
	{
		Widget_ItemInteract->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	const APlayerController* PC = Cast<APlayerController>(WidgetController->_getUObject());
	UGameplayStatics::ProjectWorldToScreen(PC, WidgetController->GetPlayerLocation(), ProjectLocation);
	int32 ScreensizeX;
	int32 ScreensizeY;
	PC->GetViewportSize(ScreensizeX, ScreensizeY);

	if (ToSet) WidgetFloating |= 1 << (uint8)EUserWidget::EUW_ItemInteract;
	else WidgetFloating &= ~1 << (uint8)EUserWidget::EUW_ItemInteract;

	ProjectLocation.X += ScreensizeX / 12.f;
	Widget_ItemInteract->SetPositionInViewport(ProjectLocation);
	Widget_ItemInteract->SetVisibility(ToSet ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (ToSet) Widget_ItemInteract->SetItemConfig(OnGetItem, ItemConfig, GetOwner());
}

void UControllerWidgetComponent::Callback_OnKeyTriggered(FKey Key)
{
	if (Key.GetFName() == FName("E") && Widget_ItemInteract != nullptr)
	{
		Widget_ItemInteract->SwitchWidget(true);
	}
	else if (Key.GetFName() == FName("I") && Widget_Inventory != nullptr && WidgetController != nullptr && InventoryInterval <= 0.f)
	{
		bool WasOn = Widget_Inventory->GetVisibility() == ESlateVisibility::Visible;

		if (!WasOn) WidgetFloating |= 1 << (uint8)EUserWidget::EUW_Inventory;
		else WidgetFloating &= ~1 << (uint8)EUserWidget::EUW_Inventory;

		WidgetController->SetCurorVisibility(!WasOn);
		Widget_Inventory->SetVisibility(WasOn ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
		InventoryInterval = 1.f;
	}
	if (Widget_HUD != nullptr)
	{
		Widget_HUD->Execute_OnKeyTriggered(Widget_HUD, Key);
	}
}

void UControllerWidgetComponent::Callback_OnKeyReleased(FKey Key)
{
	if (Key.GetFName() == FName("E") && Widget_ItemInteract != nullptr)
	{
		Widget_ItemInteract->SwitchWidget(false);
	}
}

void UControllerWidgetComponent::Callback_ChangedQuickSlot(UObject* ChangedItemObject, FKey ChangedKey)
{
	int8 idx = 0;
	TArray<UObject*> NewArray;
	if (Widget_Inventory == nullptr) return;
	for (UObject* Item : Widget_Inventory->GetQuickSlotObjectArr())
	{
		UItemDataObject* ItemDataObject = Cast<UItemDataObject>(Item);
		FKey QuickSlotSettingKey = QuickslotKeySetting[idx];
		if (ItemDataObject != nullptr)
		{
			if (Item == ChangedItemObject && QuickSlotSettingKey != ChangedKey)
			{
				NewArray.Add(Cast<UObject>(QuickslotObjects[idx]));
			}
			else if (QuickSlotSettingKey == ChangedKey)
			{
				NewArray.Add(ChangedItemObject);
			}
			else NewArray.Add(Item);
		}
		else
		{
			NewArray.Add(Cast<UObject>(QuickslotObjects[idx]));
		}
		idx++;
	}
	if (Widget_HUD) Widget_HUD->UpdateQuickSlotObjectArr(NewArray);
	if (Widget_Inventory) Widget_Inventory->UpdateQuickSlotObjectArr(NewArray);

	UItemDataObject* UpdatedItemDataObject = Cast<UItemDataObject>(ChangedItemObject);
	if (UpdatedItemDataObject != nullptr) UpdatedItemDataObject->OnItemSlotUpdated.Broadcast(ChangedItemObject);

}

void UControllerWidgetComponent::Callback_ChangedEquipment(UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType)
{
	uint8 idx = 0;
	TArray<UObject*> NewArray;
	for (UObject* Item : Widget_Inventory->GetEquipmentSlotObjectArr())
	{
		UItemDataObject* ItemDataObject = Cast<UItemDataObject>(Item);
		EEquipmentSlotType CurrEquipmentSlotType = StaticCast<EEquipmentSlotType>(idx + 1);
		if (ItemDataObject != nullptr)
		{
			if (Item == ChangedItemObject && CurrEquipmentSlotType != EquipmentSlotType)
			{
				NewArray.Add(Cast<UObject>(EquipmentslotObjects[idx]));
			}
			else if (CurrEquipmentSlotType == EquipmentSlotType)
			{
				NewArray.Add(ChangedItemObject);
			}
			else
			{
				NewArray.Add(Item);
			}
		}
		else NewArray.Add(Item);
		idx++;
	}
	if (Widget_Inventory) Widget_Inventory->UpdateEquipmentSlotObjectArr(NewArray);

	UItemDataObject* UpdatedItemDataObject = Cast<UItemDataObject>(ChangedItemObject);
	if (UpdatedItemDataObject != nullptr) UpdatedItemDataObject->OnItemSlotUpdated.Broadcast(ChangedItemObject);
}

bool UControllerWidgetComponent::IsControllable()
{
	if (WidgetFloating & 1 << (uint8)EUserWidget::EUW_Inventory) return false;
	return true;
}

void UControllerWidgetComponent::OnEquipmentItemDragDetected(bool IsOn)
{
	Widget_HUD->Execute_OnEquipmentItemDragDetected(Widget_HUD, IsOn);
	Widget_Inventory->Execute_OnEquipmentItemDragDetected(Widget_Inventory, IsOn);
}

EEquipmentSlotType UControllerWidgetComponent::GetLeftEquipmentSlotIndex()
{
	for (UObject* Item : Widget_Inventory->GetEquipmentSlotObjectArr())
	{
		UItemDataObject* ItemData = Cast<UItemDataObject>(Item);
		if (ItemData != nullptr && ItemData->IsEmptySlot())
		{
			return ItemData->ItemConfig.EquipmentSlotType;
		}
	}
	return EEquipmentSlotType::EEST_EquipSlot_Default;
}
