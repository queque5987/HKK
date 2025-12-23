#include "GameFramework/CombatLibrary.h"
#include "Interface/Character/ICharacterCombat.h"
#include "Interface/GameFramework/IPlayerState.h"

void UCombatLibrary::SendDamage(TScriptInterface<IICharacterCombat> CausedPlayer, TScriptInterface<IICharacterCombat> DamagedPlayer, FHitDamageConfig HitDamageConfig)
{
	if (CausedPlayer == nullptr)
	{
		LogWarning(3.f, TEXT("Damage Event With No Causer"));
		return;
	}
	if (DamagedPlayer == nullptr)
	{
		LogWarning(3.f, TEXT("Damaged Player Not Found"));
		return;
	}
	auto* PlayerStateObj = DamagedPlayer->GetPlayerStateObject();
	if (PlayerStateObj == nullptr)
	{
		LogWarning(3.f, TEXT("Player State Not Found"));
		return;
	}
	float CurrHP = IIPlayerState::Execute_GetCurrHP(PlayerStateObj);
	IIPlayerState::Execute_SetCurrHP(PlayerStateObj, CurrHP - HitDamageConfig.HitDamage);
	DamagedPlayer->Multicast_KnockBack(HitDamageConfig.HitDirection);
}

bool UCombatLibrary::Bind_HUD(UObject* PlayerHUDObject, UObject* PlayerStateObject)
{
	if (PlayerHUDObject == nullptr)
	{
		LogWarning(3.f, TEXT("PlayerHUDObject Not Found"));
		return false;
	}
	if (PlayerStateObject == nullptr)
	{
		LogWarning(3.f, TEXT("PlayerStateObject Not Found"));
		return false;
	}
	return IIPlayerState::Execute_BindDelegate_HUDWidget(PlayerStateObject, PlayerHUDObject);
}
