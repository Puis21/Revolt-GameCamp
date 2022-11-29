// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Controllers/FPGameplayPlayerController.h"

#include "GCFPS/Player/Camera/FPPlayerCameraManager.h"

AFPGameplayPlayerController::AFPGameplayPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// use custom camera manager class
	PlayerCameraManagerClass = AFPPlayerCameraManager::StaticClass();
	
}
