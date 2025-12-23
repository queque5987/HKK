#include "GameFramework/HKK_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Interface/Widget/IUpdatableWidget.h"

void AHKK_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHKK_PlayerState, CurrHP);
	DOREPLIFETIME(AHKK_PlayerState, MaxHP);
	DOREPLIFETIME(AHKK_PlayerState, CurrStamina);
	DOREPLIFETIME(AHKK_PlayerState, MaxStamina);
}

bool AHKK_PlayerState::BindDelegate_HUDWidget_Implementation(UObject* BindWidget)
{
	if (BindWidget == nullptr) return false;
	OnUpdateStatFloat.AddUFunction(BindWidget, "OnUpdatePlayerStatFloat");
	IIUpdatableWidget::Execute_OnUpdatePlayerStatFloat(BindWidget, EPlayerStatType::EPST_HP, 1.f, 1.f);
	return OnUpdateStatFloat.IsBoundToObject(BindWidget);
}

void AHKK_PlayerState::OnRep_CurrHP()
{
	OnUpdateStatFloat.Broadcast(EPlayerStatType::EPST_HP, CurrHP, MaxHP);
	
	if (GetOwningController() != nullptr && OnUpdateStatFloat.IsBound()) DrawDebugString(GetWorld(), FVector(0.f, 0.f, 120.f), TEXT("Hit"), GetOwningController()->GetPawn(), FColor::Red, 2.f);
}

void AHKK_PlayerState::OnRep_MaxHP()
{
	OnUpdateStatFloat.Broadcast(EPlayerStatType::EPST_HP, CurrHP, MaxHP);
}
