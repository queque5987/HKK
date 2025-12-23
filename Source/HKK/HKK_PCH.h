#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPlayerStatType : uint8
{
	EPST_HP,
	EPST_Stamina
};
UENUM(BlueprintType)
enum class EUserWidget : uint8
{
	EUW_ItemInteract
};
UENUM(BlueprintType)
enum class ECustomStencilValue : uint8
{
	ECSV_Default,
	ECSV_Item,
	ECSV_CharacterDepth,
	ECSV_ItemSelected,
	ECSV_ItemOccupied
};

UENUM(BlueprintType)
enum class EPlayerAnimation : uint8
{
	EPA_Attack_RFist,
	EPA_HitReact_L,
	EPA_HitReact_R,
	EPA_HitReact_F,
	EPA_HitReact_B
};

class HKK_API HKK_PCH
{
public:
	HKK_PCH();
	~HKK_PCH();
};
