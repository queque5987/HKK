#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "HKK_PCH.h"
#include "HKK_Structs.h"
#include "InputCoreTypes.h"
#include "Animation/AnimSequence.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAiming, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttack, const EPlayerAnimation);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayAnimation, UAnimSequence*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeStencilValue, ECustomStencilValue);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnSetItemInteractWidget, bool, EUserWidget, const FItemConfig&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddItemDataObject, UObject*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnKeyInputEvent, const FKey);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetWalkSpeed, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSlotUpdated, UObject*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnQuickSlotUpdated, UObject*, FKey);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemEquiped, UObject*, EEquipmentSlotType);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGetItem, const FItemConfig&, UObject*);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnUpdateStatFloat, const EPlayerStatType, float, float);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayAnimation, UAnimSequence*, PlayAnimation);
class HKK_API HKK_Delegates
{
//public:
	//HKK_Delegates();
	//~HKK_Delegates();
};
