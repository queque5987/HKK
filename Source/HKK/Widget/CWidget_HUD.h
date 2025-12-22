#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "CWidget_HUD.generated.h"

UCLASS()
class HKK_API UCWidget_HUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* Image_WeaponPreview;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_AmmoLeft;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_AmmoMax;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_WeaponName;
};
