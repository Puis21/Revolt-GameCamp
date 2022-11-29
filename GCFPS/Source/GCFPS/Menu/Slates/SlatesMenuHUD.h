// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlatesMenuHUD.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API ASlatesMenuHUD : public AHUD
{
	GENERATED_BODY()

protected:

	TSharedPtr<class SMainMenuWidget> MenuWidget;
	TSharedPtr<class SWidget> MenuWidgetContainer;

	virtual void BeginPlay() override;

public:

	void ShowMenu();
	void RemoveMenu();
	
};
