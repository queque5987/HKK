#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Border.h"
#include "HKK_Delegates.h"
#include "HKK_Structs.h"
#include "Widget_ItemInteract.generated.h"

UCLASS()
class HKK_API UWidget_ItemInteract : public UUserWidget
{
	GENERATED_BODY()

	UWidget_ItemInteract(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UBorder* Button;

	UPROPERTY(meta = (BindWidget))
	class UBorder* Button_Pressed;

	UPROPERTY(meta = (BindWidget))
	class UBorder* Button_BackGround_Pressed;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<class UMaterialInstance> MI_Button_BackGround_Pressed;

	UPROPERTY()
	TObjectPtr<class UMaterialInstanceDynamic> MID_Button_BackGround_Pressed;

	bool bButtonOn = false;
	float fPressedProgress = 0.f;
	FOnGetItem* OnGetItem;
	const FItemConfig* ItemConfig;
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SwitchWidget(bool bIsOn)
	{
		if (WidgetSwitcher)
		{
			WidgetSwitcher->SetActiveWidgetIndex(bIsOn ? 1 : 0);
			bButtonOn = bIsOn;
			if (!bButtonOn) fPressedProgress = 0.f;
		}
	}
	void SetItemConfig(FOnGetItem* InOnGetItem, const FItemConfig& InItemConfig)
	{
		OnGetItem = InOnGetItem; ItemConfig = &InItemConfig;
	}
private:
	void ProgressCompleted();
};

