// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPistol.h"
#include "GCFPS/Weapons/BaseWeapon.h"
#include "GCFPS/Weapons/WeaponFlarePistol.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/GCFPSHUD.h"
#include "GCFPS/Menu/HUDWidget.h"

// Sets default values
AAmmoPistol::AAmmoPistol()
{
	m_ePickUpType = EPickUpType::E_AmmoPistol;
	PickUpNotifText = "Flare Pistol Ammo";

	AmmoIncrease = 25;
}

void AAmmoPistol::Interact(AFPCharacter* pPlayer)
{
	Super::Interact( pPlayer );
	
	AWeaponFlarePistol* pPistol = Cast<AWeaponFlarePistol>(pPlayer->GetWeaponArray()[2]);
	if (pPistol)
	{
		const int ReserveAmmo = pPistol->GetWeaponAmmo();
		const int kiTotalAmmo = ReserveAmmo + AmmoIncrease;
		if (kiTotalAmmo < pPistol->GetWeaponMaxReserveAmmo())
		{
			pPistol->setWeaponAmmo(kiTotalAmmo);
			AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
			HUDWidgetRef->GetHUDWidgetRef()->UpdatePickUpNotif(GetPickUpNotifText());
			Destroy();
			//UE_LOG(LogTemp, Warning, TEXT("ONE"))
		}
		else if (ReserveAmmo < pPistol->GetWeaponMaxReserveAmmo())
		{
			pPistol->setWeaponAmmo(pPistol->GetWeaponMaxReserveAmmo());
			AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
			HUDWidgetRef->GetHUDWidgetRef()->UpdatePickUpNotif(GetPickUpNotifText());
			Destroy();
			//UE_LOG(LogTemp, Warning, TEXT("TWO"))
		}
		else if (ReserveAmmo == pPistol->GetWeaponMaxReserveAmmo())
		{
			pPistol->setWeaponAmmo(pPistol->GetWeaponMaxReserveAmmo());
			//UE_LOG(LogTemp, Warning, TEXT("THREE"))
		}
	}
}
