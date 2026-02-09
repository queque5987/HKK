#include "Widget/Widget_ItemInteract.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "Interface/Controller/IWidgetController.h"
#include "Components/Border.h"

UWidget_ItemInteract::UWidget_ItemInteract(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UWidget_ItemInteract::NativeConstruct()
{
	Super::NativeConstruct();

	if (MI_Button_BackGround_Pressed && Button_BackGround_Pressed)
	{
		MID_Button_BackGround_Pressed = UMaterialInstanceDynamic::Create(MI_Button_BackGround_Pressed, this);
		Button_BackGround_Pressed->SetBrushFromMaterial(MID_Button_BackGround_Pressed);
	}
}

void UWidget_ItemInteract::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bButtonOn && MID_Button_BackGround_Pressed != nullptr)
	{
		MID_Button_BackGround_Pressed->SetScalarParameterValue(TEXT("Percent"), fPressedProgress);
		fPressedProgress += InDeltaTime * 3.f;
		if (fPressedProgress >= 1.f)
		{
			ProgressCompleted();
		}
	}
}

void UWidget_ItemInteract::SetItemConfig(FOnGetItem* InOnGetItem, const FItemConfig& InItemConfig, AActor* InPlayerController)
{
	OnGetItem = InOnGetItem;
	ItemConfig = InItemConfig;
	OwningController = InPlayerController;
	//if (ItemConfig.SpawnedItemActor == nullptr) return;
	if (ItemConfig.ItemIcon != nullptr && ItemIcon != nullptr) ItemIcon->SetBrushFromTexture(ItemConfig.ItemIcon);
	if (ItemName != nullptr) ItemName->SetText(ItemConfig.ItemName);

	SetByWidgetType(EInteractWidgetType::EIWT_ItemPickup);
}

void UWidget_ItemInteract::SetByWidgetType(EInteractWidgetType InteractWidgetType)
{
	CurrentWidgetType = InteractWidgetType;
	if (InteractWidgetType == EInteractWidgetType::EIWT_ItemPickup)
	{
		ButtonText->SetText(FText::FromString("E"));
		ButtonText_Pressed->SetText(FText::FromString("E"));
	}
	else if (InteractWidgetType == EInteractWidgetType::EIWT_WallCover)
	{
		ButtonText->SetText(FText::FromString("W"));
		ButtonText_Pressed->SetText(FText::FromString("W"));
	}
	else
	{
		ButtonText->SetText(FText::FromString("E"));
		ButtonText_Pressed->SetText(FText::FromString("E"));
	}
	BP_SetByWidgetType(InteractWidgetType);
}

void UWidget_ItemInteract::BP_SetByWidgetType_Implementation(EInteractWidgetType InteractWidgetType)
{
}

void UWidget_ItemInteract::ProgressCompleted()
{
	if (OnGetItem != nullptr)
	{
		if (OwningController != nullptr)
		{
			OwningController->GetPlayerStateObject();
		}
		OnGetItem->Broadcast(ItemConfig, OwningController != nullptr ? OwningController->GetPlayerStateObject() : nullptr);
		OnGetItem = nullptr;
		ItemConfig.ItemIcon = nullptr;
		ItemConfig.ItemName = FText();
		//ItemConfig.SpawnedItemActor = nullptr;
	}
	SwitchWidget(false);
}
