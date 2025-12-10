#include "CAnimNotify_HitTrace.h"
#include "Interface/Character/ICharacterCombat.h"

void UCAnimNotify_HitTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	OwningActor = MeshComp->GetOwner();
	ICharacterCombat = OwningActor;
	HitTraceConfig.TraceMesh = MeshComp;
	if (OwningActor == nullptr && ICharacterCombat.GetInterface() == nullptr) return;
	if (OwningActor->HasAuthority()) ICharacterCombat->HitTraceStart(&HitTraceConfig, TotalDuration * 1.05f);
}

void UCAnimNotify_HitTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UCAnimNotify_HitTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (OwningActor == nullptr && ICharacterCombat.GetInterface() == nullptr) return;
	if (OwningActor->HasAuthority()) ICharacterCombat->HitTraceEnd();
}
