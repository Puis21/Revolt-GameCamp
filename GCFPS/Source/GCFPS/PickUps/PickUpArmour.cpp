// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpArmour.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/GCFPSHUD.h"
#include "GCFPS/Menu/HUDWidget.h"

// Sets default values
APickUpArmour::APickUpArmour()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_ePickUpType = EPickUpType::E_Armour;
	PickUpNotifText = "Armour";

	ArmourIncrease = 20.0f;

}


void APickUpArmour::Interact(AFPCharacter* pPlayer)
{
	Super::Interact( pPlayer );

	if (pPlayer->GetRegenComponent()->GetCurrentArmour() < pPlayer->GetRegenComponent()->GetMaximumArmour())
	{
		pPlayer->GetRegenComponent()->IncreaseArmour(ArmourIncrease);
		AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		HUDWidgetRef->GetHUDWidgetRef()->UpdatePickUpNotif(GetPickUpNotifText());
		Destroy();
	}

}
