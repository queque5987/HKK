#pragma once

#include "CoreMinimal.h"
#include "Widget/Widget_Item_Inventory.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Interface/Widget/IUpdatableWidget.h"
#include "Widget_Item_Quickslot.generated.h"

UCLASS()
class HKK_API UWidget_Item_Quickslot : public UWidget_Item_Inventory, public IIUpdatableWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuickSlotIdx;
	FKey QuickSlotKeyType;
	virtual bool Initialize() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;

public:
	
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};
