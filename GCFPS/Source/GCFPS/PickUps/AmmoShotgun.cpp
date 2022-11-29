// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoShotgun.h"
#include "GCFPS/Weapons/BaseWeapon.h"
#include "GCFPS/Weapons/WeaponShotgun.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/GCFPSHUD.h"
#include "GCFPS/Menu/HUDWidget.h"

// Sets default values
AAmmoShotgun::AAmmoShotgun()
{
	m_ePickUpType = EPickUpType::E_AmmoShotgun;
	PickUpNotifText = "Shotgun Ammo";

	AmmoIncrease = 25;
}

void AAmmoShotgun::Interact(AFPCharacter* pPlayer)
{
	Super::Interact( pPlayer );
	
	AWeaponShotgun* pShotugn = Cast<AWeaponShotgun>(pPlayer->GetWeaponArray()[0]);
	if (pShotugn)
	{
		const int ReserveAmmo = pShotugn->GetWeaponAmmo();
		const int kiTotalAmmo = ReserveAmmo + AmmoIncrease;
		if (kiTotalAmmo < pShotugn->GetWeaponMaxReserveAmmo())
		{
			pShotugn->setWeaponAmmo(kiTotalAmmo);
			AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
			HUDWidgetRef->GetHUDWidgetRef()->UpdatePickUpNotif(GetPickUpNotifText());
			Destroy();
			//UE_LOG(LogTemp, Warning, TEXT("ONE"))
		}		
		else if(ReserveAmmo < pShotugn->GetWeaponMaxReserveAmmo())
		{
			pShotugn->setWeaponAmmo(pShotugn->GetWeaponMaxReserveAmmo());
			AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
			HUDWidgetRef->GetHUDWidgetRef()->UpdatePickUpNotif(GetPickUpNotifText());
			Destroy();
			//UE_LOG(LogTemp, Warning, TEXT("TWO"))
		}
		else if (ReserveAmmo == pShotugn->GetWeaponMaxReserveAmmo())
		{
			 pShotugn->setWeaponAmmo(pShotugn->GetWeaponMaxReserveAmmo());
			 //UE_LOG(LogTemp, Warning, TEXT("THREE"))
		}
	}
}
