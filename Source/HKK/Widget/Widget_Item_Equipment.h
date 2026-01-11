#pragma once

#include "CoreMinimal.h"
#include "Widget/Widget_Item_Inventory.h"
#include "Widget_Item_Equipment.generated.h"

class UTexture2D;

UCLASS()
class HKK_API UWidget_Item_Equipment : public UWidget_Item_Inventory
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTexture2D> DefaultImage;

public:

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
};
