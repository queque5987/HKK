#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "HKK_Delegates.h"
#include "Widget_Item_Inventory.generated.h"

class UDragDropOperation;

UCLASS()
class HKK_API UWidget_Item_Inventory : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuickSlotIdx;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCount;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemCountX;

	UPROPERTY()
	TObjectPtr<UObject> ListItemObject;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drag and Drop")
	TSubclassOf<UWidget_Item_Inventory> DragVisualClass;

	int8 QuickslotKey;
	FOnItemSlotUpdated* OnItemSlotUpdated_DelegatePtr = nullptr;
	FDelegateHandle OnItemSlotUpdatedDelegateHandle;
public:
	//~ Begin UUserWidget Drag and Drop
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//~ End UUserWidget Drag and Drop
	
};
