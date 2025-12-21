#include "Widget/CWidget_ItemInteract.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "Components/Border.h"

UCWidget_ItemInteract::UCWidget_ItemInteract(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UCWidget_ItemInteract::NativeConstruct()
{
	Super::NativeConstruct();

	if (MI_Button_BackGround_Pressed && Button_BackGround_Pressed)
	{
		MID_Button_BackGround_Pressed = UMaterialInstanceDynamic::Create(MI_Button_BackGround_Pressed, this);
		Button_BackGround_Pressed->SetBrushFromMaterial(MID_Button_BackGround_Pressed);
	}
}

void UCWidget_ItemInteract::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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

void UCWidget_ItemInteract::ProgressCompleted()
{
	if (OnGetItem != nullptr)
	{
		OnGetItem->Broadcast(*ItemConfig, GetOwningPlayer<AActor>());
		OnGetItem = nullptr;
		ItemConfig = nullptr;
	}
	SwitchWidget(false);
}
