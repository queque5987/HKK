#include "Animation/AnimNotify_OnChangePlayerState.h"
#include "GameFramework/CombatLibrary.h" 

void UAnimNotify_OnChangePlayerState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner()) UCombatLibrary::AnimInstance_SetBoolValue((UObject*)MeshComp->GetOwner(), ChangePlayerState, true);
}
