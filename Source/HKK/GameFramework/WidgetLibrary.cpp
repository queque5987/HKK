#include "GameFramework/WidgetLibrary.h"
#include "Interface/Controller/IWidgetController.h"

void UWidgetLibrary::QuickSlotChanged(UObject* ChangedPlayerController, UObject* ChangedItemObject, FKey ChangedKey)
{
	IIWidgetController* IWidgetController = Cast<IIWidgetController>(ChangedPlayerController);
	if (IWidgetController == nullptr) return;

	IIWidgetController::Execute_ChangeQuickSlot(ChangedPlayerController, ChangedItemObject, ChangedKey);
}
