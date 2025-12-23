#include "Widget/Widget_ItemInteract.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
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

void UWidget_ItemInteract::ProgressCompleted()
{
	if (OnGetItem != nullptr)
	{
		OnGetItem->Broadcast(*ItemConfig, GetOwningPlayer<AActor>());
		OnGetItem = nullptr;
		ItemConfig = nullptr;
	}
	SwitchWidget(false);
}
