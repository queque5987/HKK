#pragma once

#include "CoreMinimal.h"

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
