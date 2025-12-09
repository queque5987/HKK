#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Animation/AnimSequence.h"

//class UAnimSequence;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAiming, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttack, int8);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayAnimation, UAnimSequence*);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayAnimation, UAnimSequence*, PlayAnimation);
class HKK_API HKK_Delegates
{
//public:
	//HKK_Delegates();
	//~HKK_Delegates();
};
