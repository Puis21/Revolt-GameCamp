 //Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponShotgun.h"

#include <string>

#include "DrawDebugHelpers.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "GCFPS/Player/Camera/FPCameraComponent.h"
#include "GCFPS/Player/Animation/AnimInstance/PlayerAnimInstance.h"
#include "GCFPS/GCFPSHUD.h"

AWeaponShotgun::AWeaponShotgun()
{
	m_WeaponName = "Shotgun";
	m_eCurrentWeaponType = EWeaponType::Shotgun;
}

void AWeaponShotgun::BeginPlay()
{
	ABaseWeapon::BeginPlay();
	
	/*WeaponAmmo = MagazineMaxAmmo;

	WeaponReserveAmmo = WeaponMaxReserveAmmo;*/

	//Setting Weapon ammo to the max Ammo that is determined in the BP
	m_iWeaponAmmo = m_iWeaponMaxReserveAmmo;
}

void AWeaponShotgun::FireAutomatic()
{
	//StartFiringWeapon();
	//Calling the Function that produces the lines trace, this is inside the automatic fire function and the timer below is calling the function as long as the fire input is triggered

	float FirstDelay = FMath::Max(m_fLastFireTime + m_fWeaponFireRateInSeconds - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(th_AutoFire, this, &AWeaponShotgun::StartFiringWeapon, m_fWeaponFireRateInSeconds, true, FirstDelay);
	
	//The Fire rate is a modifiable value inside of the BP
}

void AWeaponShotgun::StopFiringWeapon()
{
	GetWorldTimerManager().ClearTimer(th_AutoFire);
	
	//Clear  timer for auto fire to stop the weapon from firing automatically
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Stop Fire"));
	
}

void AWeaponShotgun::StartFiringWeapon()
{
	if (m_iWeaponAmmo > 0)//Weapon Ammo check
	{

		if (m_pPlayerAnimInstance != nullptr)
		{
			// don't do function call inside a function call
			m_pPlayerAnimInstance->ApplyGunRecoil(this->GetCurrentWeaponType(), m_fWeaponFireRateInSeconds);
		}

		for (int i = 0; i < m_iBulletsFiredPerShot; i++)// Value is editable in BP: Determines the amount of line traces sent out each time fire is pressed
		{

			FVector LineTraceStart = FVector::ZeroVector;

			FVector LineTraceEnd = FVector::ZeroVector;

			if (GetPlayer() && GetPlayer()->GetFirstPersonCameraComponent())
			{
				const int RandomValue = FMath::Rand();//Returns random Int
				FRandomStream ShotgunRandomStream(RandomValue);

				FVector viewDir = GetPlayer()->GetFirstPersonCameraComponent()->GetComponentRotation().Vector();//player camera

				viewDir.Normalize();

				LineTraceStart = GetPlayer()->GetFirstPersonCameraComponent()->GetComponentLocation();

				const float SpreadCone = FMath::DegreesToRadians(WeaponSpread * 0.5f); //Take the weapon spread value/ halves it and returns the value in radians

				const FVector ShootDirection = ShotgunRandomStream.VRandCone(viewDir, SpreadCone, SpreadCone); //takes the direction of the muzzle and the horizontal and vertical angles and returns a vector

				LineTraceEnd = LineTraceStart + ShootDirection * m_iWeaponRange; //Creating the line trace with the addition of the weapon range 

			}

			FHitResult HitResult; //Hit object

			FCollisionQueryParams TraceParams;
			TraceParams.bTraceComplex = true; //Can hit objects that have complex collision such as an enemy
			TraceParams.bReturnPhysicalMaterial = true; //Return physical materials used to make the impact different on materials

			//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Orange, false, 5, 5, 5);
			//Debug version of line trace used to test 

			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, LineTraceStart, LineTraceEnd, ECC_Visibility, TraceParams);
			//Creating the Line trace

			if (bHit)
			{
				//DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Red, false, 2.0f, 5, 5);
				//Debug used to see if a surface has been hit by the line trace, creates a box at the point of impact

			   // Apply Damage
				UDamageType* DamageType;
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), m_iWeaponDamagePerShot, GetPlayer()->GetController(), GetOwner(), DamageType->StaticClass());

				
				//Logan
				if(GetWorld()->GetFirstPlayerController() && GetWorld()->GetFirstPlayerController()->GetHUD())
				{
				
					if (AAIBase* HitMarkerCast = Cast<AAIBase>(HitResult.GetActor()))
					{
						if (HitMarkerCast != nullptr)
						{
							AGCFPSHUD* HUDReference = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

							if (HUDReference != nullptr)
							{
								HUDReference->ShowHitMarker();
							}
						}
					}
						
				}
				
				//sets random rotation for the decal when its hit a surface
				FRotator RandomDecalRotation = HitResult.ImpactNormal.Rotation();
				RandomDecalRotation.Roll = FMath::FRand() * 360.0f;


				EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
				//Check to see if the surface that was hit has a physical material attached to it
			   //Below are some of the different types of surface, here will have the vfx sound effects for each type of surface


				switch (SurfaceType)
				{
				case SurfaceType1://Plastic ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if (ImpactParticlesPlastic && ImpactDecalPlastic && ImpactSFXPlastic)
					{
						UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticlesPlastic, HitResult.ImpactPoint, RandomDecalRotation, m_ImpactParticleSize);

						UGameplayStatics::SpawnDecalAttached(ImpactDecalPlastic, m_DecalSize, HitResult.GetComponent(), HitResult.BoneName, HitResult.Location, RandomDecalRotation, EAttachLocation::KeepWorldPosition, m_fImpactDecalLifeTime);

						UGameplayStatics::PlaySoundAtLocation(this, ImpactSFXPlastic, HitResult.Location, HitResult.Normal.Rotation(), m_fImpactVolumeMultiplier);

					}

					break;

				case SurfaceType2://Wood  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if (ImpactParticlesWood && ImpactDecalWood && ImpactSFXWood)
					{
						UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticlesWood, HitResult.ImpactPoint, RandomDecalRotation, m_ImpactParticleSize);

						UGameplayStatics::SpawnDecalAttached(ImpactDecalWood, m_DecalSize, HitResult.GetComponent(), HitResult.BoneName, HitResult.Location, RandomDecalRotation, EAttachLocation::KeepWorldPosition, m_fImpactDecalLifeTime);

						UGameplayStatics::PlaySoundAtLocation(this, ImpactSFXWood, HitResult.Location, HitResult.Normal.Rotation(), m_fImpactVolumeMultiplier);
					}

					break;

				case SurfaceType3://Metal  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if (ImpactParticlesMetal && ImpactDecalMetal && ImpactSFXMetal)
					{
						UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticlesMetal, HitResult.ImpactPoint, RandomDecalRotation, m_ImpactParticleSize);

						UGameplayStatics::SpawnDecalAttached(ImpactDecalMetal, m_DecalSize, HitResult.GetComponent(), HitResult.BoneName, HitResult.Location, RandomDecalRotation, EAttachLocation::KeepWorldPosition, m_fImpactDecalLifeTime);

						UGameplayStatics::PlaySoundAtLocation(this, ImpactSFXMetal, HitResult.Location, HitResult.Normal.Rotation(), m_fImpactVolumeMultiplier);
					}

					break;

				case SurfaceType4://Carpet  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if (ImpactParticlesCarpet && ImpactDecalCarpet && ImpactSFXCarpet)
					{
						UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticlesCarpet, HitResult.ImpactPoint, RandomDecalRotation, m_ImpactParticleSize);

						UGameplayStatics::SpawnDecalAttached(ImpactDecalCarpet, m_DecalSize, HitResult.GetComponent(), HitResult.BoneName, HitResult.Location, RandomDecalRotation, EAttachLocation::KeepWorldPosition, m_fImpactDecalLifeTime);

						UGameplayStatics::PlaySoundAtLocation(this, ImpactSFXCarpet, HitResult.Location, HitResult.Normal.Rotation(), m_fImpactVolumeMultiplier);
					}

					break;

				case SurfaceType5://Glass  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if (ImpactParticlesGlass && ImpactDecalGlass && ImpactSFXGlass)
					{
						UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticlesGlass, HitResult.ImpactPoint, RandomDecalRotation, m_ImpactParticleSize);

						UGameplayStatics::SpawnDecalAttached(ImpactDecalGlass, m_DecalSize, HitResult.GetComponent(), HitResult.BoneName, HitResult.Location, RandomDecalRotation, EAttachLocation::KeepWorldPosition, m_fImpactDecalLifeTime);

						UGameplayStatics::PlaySoundAtLocation(this, ImpactSFXGlass, HitResult.Location, HitResult.Normal.Rotation(), m_fImpactVolumeMultiplier);

					}

					break;

				case SurfaceType6://Concrete  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if (ImpactParticlesConcrete && ImpactDecalConcrete && ImpactSFXConcrete)
					{
						UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticlesConcrete, HitResult.ImpactPoint, RandomDecalRotation, m_ImpactParticleSize);

						UGameplayStatics::SpawnDecalAttached(ImpactDecalConcrete, m_DecalSize, HitResult.GetComponent(), HitResult.BoneName, HitResult.Location, RandomDecalRotation, EAttachLocation::KeepWorldPosition, m_fImpactDecalLifeTime);

						UGameplayStatics::PlaySoundAtLocation(this, ImpactSFXConcrete, HitResult.Location, HitResult.Normal.Rotation(), m_fImpactVolumeMultiplier);
					}

					break;

				case SurfaceType7://Electrical  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if (ImpactParticlesElectrical && ImpactDecalElectrical && ImpactSFXElectrical)
					{
						UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticlesElectrical, HitResult.ImpactPoint, RandomDecalRotation, m_ImpactParticleSize);

						UGameplayStatics::SpawnDecalAttached(ImpactDecalElectrical, m_DecalSize, HitResult.GetComponent(), HitResult.BoneName, HitResult.Location, RandomDecalRotation, EAttachLocation::KeepWorldPosition, m_fImpactDecalLifeTime);

						UGameplayStatics::PlaySoundAtLocation(this, ImpactSFXElectrical, HitResult.Location, HitResult.Normal.Rotation(), m_fImpactVolumeMultiplier);
					}

					break;
				}
			}
		}

		m_iWeaponAmmo--;//Take away ammo every time the weapon is shot

		if (m_FireAnimation)
		{
			//IF the gun has a fire animation such as bolt moving
		}

		if (m_FireSound)
		{
			//Fire sound
			UGameplayStatics::PlaySoundAtLocation(this, m_FireSound, GetActorLocation(), m_fImpactVolumeMultiplier);
		}

		if (m_MuzzleParticles)
		{

			//UGameplayStatics::SpawnEmitterAtLocation(this, m_MuzzleParticles, WeaponSkeletalMesh->GetSocketLocation(FName("Muzzle")),WeaponSkeletalMesh->GetSocketRotation(FName("Muzzle")),m_MuzzleParticleSize);


			UGameplayStatics::SpawnEmitterAttached(m_MuzzleParticles, WeaponSkeletalMesh, FName("Muzzle"), WeaponSkeletalMesh->GetSocketLocation(FName("Muzzle")), WeaponSkeletalMesh->GetSocketRotation(FName("Muzzle")), m_MuzzleParticleSize, EAttachLocation::KeepWorldPosition, true);

		}
		if (m_ShellEject)
		{
			//eject a shell when the gun is shot
			UGameplayStatics::SpawnEmitterAttached(m_ShellEject, WeaponSkeletalMesh, FName("ShellEject"), WeaponSkeletalMesh->GetSocketLocation(FName("ShellEject")), WeaponSkeletalMesh->GetSocketRotation(FName("ShellEject")), m_ShellEjectSize, EAttachLocation::KeepWorldPosition, true);
		}

		//Stops the gun being fired until the timer for shooting resets
		m_fLastFireTime = GetWorld()->TimeSeconds;

	}
	else
	{
	//No AMMo
		UGameplayStatics::PlaySoundAtLocation(this, m_DryShot, GetActorLocation());
	}

}
	 
	 
	/*else
	{
	ReloadWeapon();
	}*/


void AWeaponShotgun::ReloadWeapon()
{
	//REload function here incase we decide to have reloading in the future
	
	/*if(WeaponAmmo !=MagazineMaxAmmo)
	{
		if(WeaponReserveAmmo > 0 )
		{
			
			if(WeaponAmmo <= 0 )
			{
				setWeaponReserveAmmo(GetWeaponReserveAmmo()- MagazineMaxAmmo);
				
				WeaponAmmo = MagazineMaxAmmo;
				
			}
			else if(WeaponAmmo > 0)
			{

				MagazineMaxAmmo - GetWeaponAmmo();


				
			}
			
		}
		
	}*/
}	