// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoAssaultRifle.h"
#include "GCFPS/Weapons/BaseWeapon.h"
#include "GCFPS/Weapons/WeaponAssaultRifle.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/GCFPSHUD.h"
#include "GCFPS/Menu/HUDWidget.h"

// Sets default values
AAmmoAssaultRifle::AAmmoAssaultRifle()
{
	m_ePickUpType = EPickUpType::E_AmmoAssaultRifle;
	PickUpNotifText = "Assault Rifle Ammo";

	AmmoIncrease = 25;

}

void AAmmoAssaultRifle::Interact(AFPCharacter* pPlayer)
{
	Super::Interact( pPlayer );
	
	AWeaponAssaultRifle* pAssaultRifle = Cast<AWeaponAssaultRifle>(pPlayer->GetWeaponArray()[1]);
	if (pAssaultRifle)
	{
		const int ReserveAmmo = pAssaultRifle->GetWeaponAmmo();
		const int kiTotalAmmo = ReserveAmmo + AmmoIncrease;
		if (kiTotalAmmo < pAssaultRifle->GetWeaponMaxReserveAmmo())
		{
			pAssaultRifle->setWeaponAmmo(kiTotalAmmo);
			AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
			HUDWidgetRef->GetHUDWidgetRef()->UpdatePickUpNotif(GetPickUpNotifText());
			Destroy();
			//UE_LOG(LogTemp, Warning, TEXT("ONE"))
		}
		else if (ReserveAmmo < pAssaultRifle->GetWeaponMaxReserveAmmo())
		{
			pAssaultRifle->setWeaponAmmo(pAssaultRifle->GetWeaponMaxReserveAmmo());
			AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
			HUDWidgetRef->GetHUDWidgetRef()->UpdatePickUpNotif(GetPickUpNotifText());
			Destroy();
			//UE_LOG(LogTemp, Warning, TEXT("TWO"))
		}
		else if (ReserveAmmo == pAssaultRifle->GetWeaponMaxReserveAmmo())
		{
			pAssaultRifle->setWeaponAmmo(pAssaultRifle->GetWeaponMaxReserveAmmo());
			//UE_LOG(LogTemp, Warning, TEXT("THREE"))
		}
	}
}