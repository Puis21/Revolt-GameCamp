// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponGrenadeLauncher.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "../GCFPSProjectile.h"


AWeaponGrenadeLauncher::AWeaponGrenadeLauncher()
{
	m_WeaponName = "Grenade Launcher";
	m_eCurrentWeaponType = EWeaponType::DefaultWep;
}

void AWeaponGrenadeLauncher::BeginPlay()
{
	ABaseWeapon::BeginPlay();
	
	m_iWeaponAmmo = m_iWeaponMaxReserveAmmo;//Setting Weapon ammo to the max Ammo that is determined in the BP
}


void AWeaponGrenadeLauncher::FireAutomatic()
{
	if(m_iWeaponAmmo > 0)//Weapon Ammo check
	{
		
		for (int i = 0; i < m_iBulletsFiredPerShot; i++)// Value is editable in BP: Determines the amount of line traces sent out each time fire is pressed
		{

			// try and fire a projectile
			if (ProjectileClass != nullptr)//Class that is set in the BP will fire when the triggered
			{

				FRotator SpawnRotation = WeaponSkeletalMesh->GetSocketRotation(FName("Muzzle"));//Getting rotation from the weapon Barrel
				FVector SpawnPosition = WeaponSkeletalMesh->GetSocketLocation(FName("Muzzle"));//Getting location from the weapon Barrel

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				GetWorld()->SpawnActor<AGCFPSProjectile>(ProjectileClass, SpawnPosition, SpawnRotation, ActorSpawnParams);

			}

		}
	
		m_iWeaponAmmo--;//Take away ammo every time the weapon is shot
	
		if (m_FireSound)
		{
			//Fire sound
		UGameplayStatics::PlaySoundAtLocation(this, m_FireSound, GetActorLocation());
		}
		
	}

}



