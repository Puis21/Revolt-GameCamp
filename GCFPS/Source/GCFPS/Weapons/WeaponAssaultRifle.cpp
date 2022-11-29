// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAssaultRifle.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/Player/FPCharacter.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "GCFPS/Player/Camera/FPCameraComponent.h"
#include "GCFPS/Player/Animation/AnimInstance/PlayerAnimInstance.h"
#include "GCFPS/GCFPSHUD.h"


AWeaponAssaultRifle::AWeaponAssaultRifle()
{
	m_WeaponName = "Assault Rifle";
	m_eCurrentWeaponType = EWeaponType::AssaultRifle;
}

void AWeaponAssaultRifle::BeginPlay()
{
	ABaseWeapon::BeginPlay();

	/*WeaponAmmo = MagazineMaxAmmo;

	WeaponReserveAmmo = WeaponMaxReserveAmmo;*/

	//Setting Weapon ammo to the max Ammo that is determined in the BP
	m_iWeaponAmmo = m_iWeaponMaxReserveAmmo;
}

void AWeaponAssaultRifle::FireAutomatic()
{
	StartFiringWeapon();//Calling the Function that produces the lines trace, this is inside the automatic fire function and the timer below is calling the function as long as the fire input is triggered
	
	GetWorldTimerManager().SetTimer(th_AutoFire, this, &AWeaponAssaultRifle::StartFiringWeapon, m_fWeaponFireRateInSeconds, true);
	//The Fire rate is a modifiable value inside of the BP
	
}

void AWeaponAssaultRifle::StartFiringWeapon()
{
	if (m_iWeaponAmmo > 0)//Weapon Ammo check
	{
		if (m_pPlayerAnimInstance != nullptr)
		{
			m_pPlayerAnimInstance->ApplyGunRecoil(this->GetCurrentWeaponType(), m_fWeaponFireRateInSeconds);
		}

		for (int i = 0; i < m_iBulletsFiredPerShot; i++)// Value is editable in BP: Determines the amount of line traces sent out each time fire is pressed(Only used for shotgun so far)
		{
			//Bullet spread around the cross hair
			float BulletSpreadX = FMath::RandRange(BulletSpreadNegative, BulletSpreadPositive);
			float BulletSpreadY = FMath::RandRange(BulletSpreadNegative, BulletSpreadPositive);
			float BulletSpreadZ = FMath::RandRange(BulletSpreadNegative, BulletSpreadPositive);

			//adding the spread together
			const FVector BulletSpread= FVector(BulletSpreadX, BulletSpreadY, BulletSpreadZ);


			FVector LineTraceStart = FVector::ZeroVector;

			FVector LineTraceEnd = FVector::ZeroVector;
			
			if(GetPlayer() && GetPlayer()->GetFirstPersonCameraComponent())
			{
				  
				FVector viewDir = GetPlayer()->GetFirstPersonCameraComponent()->GetComponentRotation().Vector();//player Camera rotation
			
				viewDir.Normalize();

				LineTraceStart = GetPlayer()->GetFirstPersonCameraComponent()->GetComponentLocation();//player camera location

				LineTraceEnd = LineTraceStart + BulletSpread + viewDir * m_iWeaponRange;

			}

		//FVector LineTraceStart = WeaponSkeletalMesh->GetSocketLocation(FName("Muzzle"));//Getting location from the weapon Barrel
		//FRotator LineTraceRotation = WeaponSkeletalMesh->GetSocketRotation(FName("Muzzle"));//Getting rotation from the weapon Barrel
		//FVector LineTraceEnd = LineTraceStart + BulletSpread + LineTraceRotation.Vector() * m_iWeaponRange;//Adding the range to the line trace to determine how far the trace will go in the world

			FHitResult HitResult;//Hit object

			FCollisionQueryParams TraceParams;
			TraceParams.bTraceComplex = true;//Can hit objects that have complex collision such as an enemy
			TraceParams.bReturnPhysicalMaterial = true;	//Return physical materials used to make the impact different on materials

			//DrawDebugLine(GetWorld(), LineTraceStart, LineTraceEnd, FColor::Green, false, 5, 5, 5);
			//Debug version of line trace used to test the line trace
			
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, LineTraceStart, LineTraceEnd, ECC_Visibility, TraceParams);
			//Creating the Line trace using the Params set above 
			
			if (bHit)
			{
				DrawDebugBox(GetWorld(), HitResult.ImpactPoint, FVector(5, 5, 5), FColor::Red, false, 2.0f, 5, 5);
				//Debug used to see if a surface has been hit by the line trace, creates a box at the point of impact

				// Apply Damage to the AI
				UDamageType* DamageType;

				UGameplayStatics::ApplyDamage( HitResult.GetActor(), m_iWeaponDamagePerShot, GetPlayer()->GetController(), GetOwner(), DamageType->StaticClass() );



				//Logan
				if (GetWorld()->GetFirstPlayerController() && GetWorld()->GetFirstPlayerController()->GetHUD())
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

				FRotator RandomDecalRotation = HitResult.ImpactNormal.Rotation();
				RandomDecalRotation.Roll = FMath::FRand() * 360.0f;
				//Giving each decal a random rotation to make it different every time you shoot a surface 

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

						UGameplayStatics::PlaySoundAtLocation(this, ImpactSFXWood, HitResult.Location, HitResult.Normal.Rotation(),m_fImpactVolumeMultiplier);
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
						UGameplayStatics::SpawnEmitterAtLocation(this, ImpactParticlesElectrical, HitResult.ImpactPoint, RandomDecalRotation,m_ImpactParticleSize);

						UGameplayStatics::SpawnDecalAttached(ImpactDecalElectrical, m_DecalSize, HitResult.GetComponent(), HitResult.BoneName, HitResult.Location, RandomDecalRotation, EAttachLocation::KeepWorldPosition, m_fImpactDecalLifeTime);

						UGameplayStatics::PlaySoundAtLocation(this, ImpactSFXElectrical, HitResult.Location, HitResult.Normal.Rotation(), m_fImpactVolumeMultiplier);
					}

					break;
				}
			}
		}
	
		m_iWeaponAmmo--;//Take away ammo every time the weapon is shot
		
		if (m_FireSound)
		{
			//Fire sound played on the current weapon
			UGameplayStatics::PlaySoundAtLocation(this, m_FireSound, GetActorLocation(), m_fImpactVolumeMultiplier);
		}

		if( m_MuzzleParticles)
		{
			//Spawns a muzzle particle attached to the muzzle of the current weapon
			UGameplayStatics::SpawnEmitterAttached(m_MuzzleParticles, WeaponSkeletalMesh, FName("Muzzle"), WeaponSkeletalMesh->GetSocketLocation(FName("Muzzle")), WeaponSkeletalMesh->GetSocketRotation(FName("Muzzle")),m_MuzzleParticleSize, EAttachLocation::KeepWorldPosition, true);
		}

		if (m_ShellEject)
		{

			//eject a shell when the gun is shot
			UGameplayStatics::SpawnEmitterAttached(m_ShellEject, WeaponSkeletalMesh, FName("ShellEject"), WeaponSkeletalMesh->GetSocketLocation(FName("ShellEject")), WeaponSkeletalMesh->GetSocketRotation(FName("ShellEject")), m_ShellEjectSize, EAttachLocation::KeepWorldPosition, true);
		}
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_DryShot, GetActorLocation());
		//no ammo
	}
}

void AWeaponAssaultRifle::StopFiringWeapon()
{
	GetWorldTimerManager().ClearTimer(th_AutoFire);
	//Clear  timer for auto fire to stop the weapon from firing automatically
}