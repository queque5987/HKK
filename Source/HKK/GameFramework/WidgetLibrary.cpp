#include "GameFramework/WidgetLibrary.h"
#include "Interface/Controller/IWidgetController.h"
#include "Interface/IPickableItem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ObjectPoolSystem.h"
#include "Blueprint/UserWidget.h"

bool UWidgetLibrary::ItemInteractWidget(bool IsOn, UObject* OwningPlayerController, UObject* PickableItemObject, const FItemConfig& ItemConfig)
{
	if (IIPickableItem::Execute_GetPickableItem(PickableItemObject) || !IsOn)
	{
		IIWidgetController::Execute_ItemInteractPickUpWidget(OwningPlayerController, IsOn, PickableItemObject, ItemConfig);
		return true;
	}
	return false;
}

void UWidgetLibrary::QuickSlotChanged(UObject* ChangedPlayerController, UObject* ChangedItemObject, FKey ChangedKey)
{
	IIWidgetController::Execute_ChangeQuickSlot(ChangedPlayerController, ChangedItemObject, ChangedKey);
}

void UWidgetLibrary::EquipmentSlotChanged(UObject* ChangedPlayerController, UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType)
{
	IIWidgetController::Execute_ChangeEquipSlot(ChangedPlayerController, ChangedItemObject, EquipmentSlotType);
}

void UWidgetLibrary::EquipmentItemDragDetected(UObject* OwningPlayerController, bool e)
{
	IIWidgetController::Execute_EquipmentItemDragDetected(OwningPlayerController, e);
}

EEquipmentSlotType UWidgetLibrary::GetLeftEquipmentSlotIndex(UObject* OwningPlayerController)
{
	return IIWidgetController::Execute_GetLeftEquipmentSlotIndex(OwningPlayerController);
}

UUserWidget* UWidgetLibrary::GetWidget(UObject* OwningPlayerControllerObject, TSubclassOf<UUserWidget> CreateWidgetClass)
{
    if (OwningPlayerControllerObject == nullptr)
    {
        LogWarning(3.f, TEXT("OwningPlayerControllerObject is Null"));
        return nullptr;
    }

    APlayerController* tempController = Cast<APlayerController>(OwningPlayerControllerObject);
    if (tempController == nullptr)
    {
        LogWarning(3.f, TEXT("OwningPlayerControllerObject is NOT PlayerController"));
        return nullptr;
    }

    if (CreateWidgetClass == nullptr)
    {
        LogWarning(3.f, TEXT("CreateWidgetClass is Null"));
        return nullptr;
    }

    UWorld* WorldObject = OwningPlayerControllerObject->GetWorld();
    if (WorldObject == nullptr)
    {
        LogWarning(3.f, TEXT("World Context is Null"));
        return nullptr;
    }

    UObjectPoolSystem* ObjectPoolSystem = WorldObject->GetSubsystem<UObjectPoolSystem>();
    if (ObjectPoolSystem == nullptr)
    {
        LogWarning(3.f, TEXT("ObjectPoolSystem Not Found"));
        return nullptr;
    }

    UUserWidget* tempWidget = ObjectPoolSystem->GetWidgetFromPool(CreateWidgetClass, OwningPlayerControllerObject);
    if (tempWidget == nullptr)
    {
        LogWarning(3.f, TEXT("Failed to Get Widget From Pool"));
        return nullptr;
    }
	return tempWidget;
}

bool UWidgetLibrary::ReturnWidget(UObject* WorldContextObject, TSubclassOf<UUserWidget> ReturnWidgetClass, UUserWidget* InWidget)
{
    if (InWidget == nullptr)
    {
        LogWarning(3.f, TEXT("Returning Widget is Null"));
        return false;
    }
    if (WorldContextObject == nullptr)
    {
        LogWarning(3.f, TEXT("WorldContextObject is Null"));
        return false;
    }
    UWorld* WorldObject = WorldContextObject->GetWorld();
    if (WorldObject == nullptr)
    {
        LogWarning(3.f, TEXT("World Context is Null"));
        return false;
    }
    UObjectPoolSystem* ObjectPoolSystem = WorldObject->GetSubsystem<UObjectPoolSystem>();
    if (ObjectPoolSystem == nullptr)
    {
        LogWarning(3.f, TEXT("ObjectPoolSystem Not Found"));
        return false;
    }
    if (ReturnWidgetClass == nullptr)
    {
        LogWarning(3.f, TEXT("ReturnWidgetClass is Null"));
        return false;
    }
    return ObjectPoolSystem->ReturnWidgetToPool(ReturnWidgetClass, InWidget);
}
