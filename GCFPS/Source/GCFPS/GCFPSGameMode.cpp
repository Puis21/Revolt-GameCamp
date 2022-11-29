#include "GCFPSGameMode.h"
#include "GCFPSHUD.h"
#include "GCFPSCharacter.h"
#include "Player/Controllers/FPGameplayPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AGCFPSGameMode::AGCFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Player/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	static ConstructorHelpers::FClassFinder<AHUD> HudClassFinder( TEXT("/Game/HUD/BP_GameHUD") );
	HUDClass = HudClassFinder.Class;

	PlayerControllerClass = AFPGameplayPlayerController::StaticClass();
	
	// use our custom HUD class
	//HUDClass = AGCFPSHUD::StaticClass();
}