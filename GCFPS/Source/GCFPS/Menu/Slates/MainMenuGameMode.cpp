// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "GCFPS/GCFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MenuPlayerController.h"
#include "SlatesMenuHUD.h"

AMainMenuGameMode::AMainMenuGameMode()
	: Super()
{
	PlayerControllerClass = AMenuPlayerController::StaticClass();
	HUDClass = ASlatesMenuHUD::StaticClass();
}
