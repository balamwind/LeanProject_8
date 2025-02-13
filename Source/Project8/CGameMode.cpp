#include "CGameMode.h"

#include "CGameState.h"
#include "CGameState.h"
#include "Character/CCharacter.h"
#include "Character/CPlayerController.h"

ACGameMode::ACGameMode()
{
	DefaultPawnClass = ACCharacter::StaticClass();
	PlayerControllerClass = ACPlayerController::StaticClass();
	GameStateClass = ACGameState::StaticClass();
}
