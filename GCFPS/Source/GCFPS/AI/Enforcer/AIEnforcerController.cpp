// Fill out your copyright notice in the Description page of Project Settings.


#include "AIEnforcerController.h"
#include "GCFPS/Player/FPCharacter.h"


AAIEnforcerController::AAIEnforcerController()
{
	// Activate tick
	//SightConfig->SightRadius					= 800.0f
	//SightConfig->LoseSightRadius				= 1400.0f
	//SightConfig->PeripheralVisionAngleDegrees	= 90.0f
	//SightConfig->SetMaxAge					( 5.0f );

}

void AAIEnforcerController::OnTargetDetected( AActor* actor, FAIStimulus stimulus )
{
	Super::OnTargetDetected( actor, stimulus );

	// Player is sensed (Sight)
	if (auto PlayerChar = Cast<AFPCharacter>( actor ))
	{
		// Always looking at player
		SetFocus( PlayerChar );
	}
}

