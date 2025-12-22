#include "GameFramework/CGameMode_Stage.h"
#include "HKKPlayerController.h"
#include "CPlayerCharacter.h"
#include "GameFramework/CPlayerState.h"
#include "UObject/ConstructorHelpers.h"

ACGameMode_Stage::ACGameMode_Stage() : Super()
{
	PlayerControllerClass = AHKKPlayerController::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/_Player/Blueprint/BP_Robo"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	PlayerStateClass = ACPlayerState::StaticClass();
}
