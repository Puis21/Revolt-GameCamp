// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "ParticleDefinitions.h"
#include "EWeaponType.h"
#include "BaseWeapon.generated.h"

//--Forward Declarations---//
class UPlayerAnimInstance;
class UUMGGameInstance;


UCLASS()
class GCFPS_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	 ABaseWeapon();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Skeletal Mesh")// Mesh for Each Weapon
	USkeletalMeshComponent* GetSkeletalMesh() const { return WeaponSkeletalMesh; }

	UFUNCTION()//Gets current Weapon ammo of the current weapon
	int32 GetWeaponAmmo() const { return m_iWeaponAmmo; }

	//UFUNCTION()
	//int32 GetMagazineMaxAmmo() const { return MagazineMaxAmmo; }

	//Get Max ammo used to set current ammo to max ammo
	int32 GetWeaponReserveAmmo() const { return m_iWeaponReserveAmmo; }
	
	// Get m_iWeaponMaxReserveAmmo
	int32 GetWeaponMaxReserveAmmo() { return m_iWeaponMaxReserveAmmo; }

	//Set Ammo reserves
	void setWeaponReserveAmmo (int C) { m_iWeaponReserveAmmo = C; }

	void setWeaponAmmo(int WeaponAmmo) { m_iWeaponAmmo = WeaponAmmo; }

	EWeaponType GetCurrentWeaponType() { return m_eCurrentWeaponType; }

	//Needs to be public because the player needs to access them
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void StartFiringWeapon(); // Start Fire

	virtual void StopFiringWeapon(); //Stop Fire

	virtual void FireAutomatic(); //If the gun has a timer for auto fire 

	virtual void ReloadWeapon(); //Not in use, here encase we need a reload

	void SetOwner(AActor* OwningActor);

	UFPCameraComponent* GetCameraComponent() const { return m_pCameraComponent; }

	AFPCharacter* GetPlayer() const;
private:

	
	
protected:

	UFPCameraComponent* m_pCameraComponent;// Player camera for line traces

	AFPCharacter* m_pOwnerPlayerCharacter;// Used to get the players camera component
	
	UPROPERTY(EditAnywhere) //Mesh for Weapons
	USkeletalMeshComponent* WeaponSkeletalMesh;  

    UPROPERTY(EditAnywhere)//Ammo that each gun will have
	int32 m_iWeaponMaxReserveAmmo;

	int32 m_iWeaponReserveAmmo;
	
	int32 m_iWeaponAmmo;//Ammo that the Weapons currently have

	UPROPERTY(EditAnywhere)
	int32 m_iWeaponDamagePerShot;// Damage each bullet does 
	
	/*UPROPERTY(EditAnywhere)
	int32 MagazineMaxAmmo;*/
	
	EWeaponType m_eCurrentWeaponType; //Enum for the players current weaapons

	UPROPERTY(EditAnywhere)//Delay Between shots if the weapon is using Delay
	float m_fWeaponFireRateInSeconds;

	UPROPERTY(EditAnywhere)//Gives Each Weapon a name
	FName m_WeaponName;

	UPROPERTY(EditAnywhere)//Weapon Range( Used if the weapon is using Line traces)
	int32 m_iWeaponRange;

	UPROPERTY(EditAnywhere)//Determines the amount of bullets shot each time player presses fire
	int32 m_iBulletsFiredPerShot;
	
	UPROPERTY(EditAnywhere) //Fire animation for Weapons/ if they have a bolt animation
	class UAnimMontage* m_FireAnimation;

	UPROPERTY(EditAnywhere) //Reload animation for Weapons
	class UAnimMontage* m_ReloadAnimation;

	UPROPERTY(EditAnywhere)// Sound played when the weapons are shot
	USoundBase* m_FireSound;

	UPROPERTY(EditAnywhere)// Sound played when the weapons have ran out of ammo
	USoundBase* m_DryShot;

	UPROPERTY(EditAnywhere, Category = ImpactDecals)//Decal LifeTime
	float  m_fImpactDecalLifeTime;

	UPROPERTY(EditAnywhere, Category = ImpactDecals)//Decal size
	FVector m_DecalSize;

	UPROPERTY(EditAnywhere)
	float BulletSpreadPositive;

	UPROPERTY(EditAnywhere)
	float BulletSpreadNegative;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MuzzleParticles)
	FVector m_MuzzleParticleSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactParticles)
	FVector m_ImpactParticleSize;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MuzzleParticles) // Particles that will appear at the muzzle of the weapon when the gun is shot
	class UParticleSystem* m_MuzzleParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShellEject) // Particles that will appear at the Chamber of the weapon when the gun is shot
	class UParticleSystem* m_ShellEject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShellEject)
		FVector m_ShellEjectSize;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Particle for when wood is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactParticles)
	class UParticleSystem* ImpactParticlesWood;
	
	//Particle for when carpet/ anything woven is hit					  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactParticles)
	class UParticleSystem* ImpactParticlesCarpet;	  

	//Particle for when Concrete is hit					 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactParticles)
	class UParticleSystem* ImpactParticlesConcrete;	   
														   								  
	//Particle for when anything electrical is hit					  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactParticles)
	class UParticleSystem* ImpactParticlesElectrical;

	//Particle for when Glass is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactParticles)
	class UParticleSystem* ImpactParticlesGlass;
	
	//Particle for when Metal is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactParticles)
	class UParticleSystem* ImpactParticlesMetal;

	//Particle for when plastic is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactParticles)
	class UParticleSystem* ImpactParticlesPlastic;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Decal for when wood is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactDecals)
	class UMaterialInstance* ImpactDecalWood;

	//Decal for when Carpet/anything woven is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactDecals)
	class UMaterialInstance* ImpactDecalCarpet;

	//Decal for when Concrete is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactDecals)
	class UMaterialInstance* ImpactDecalConcrete;

	//Decal for when Electrical parts are hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactDecals)
	class UMaterialInstance* ImpactDecalElectrical;

	//Decal for when Glass parts are hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactDecals)
	class UMaterialInstance* ImpactDecalGlass;

	//Decal for when Metal parts are hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactDecals)
	class UMaterialInstance* ImpactDecalMetal;

	//Decal for when Plastic parts are hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactDecals)
	class UMaterialInstance* ImpactDecalPlastic;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactSFX)
	float m_fImpactVolumeMultiplier;

	//SFX for when Wood is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactSFX)
	USoundBase* ImpactSFXWood;

	//SFX for when Carpet is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactSFX)
	USoundBase* ImpactSFXCarpet;

	//SFX for when Concrete is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactSFX)
	USoundBase* ImpactSFXConcrete;

	//SFX for when anything Electrical is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactSFX)
	USoundBase* ImpactSFXElectrical;

	//SFX for when Glass is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactSFX)
	USoundBase* ImpactSFXGlass;

	//SFX for when Metal is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactSFX)
	USoundBase* ImpactSFXMetal;

	//SFX for when Plastic is hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ImpactSFX)
	USoundBase* ImpactSFXPlastic;
	
//--Umeer---Recoil-------------------------------------------------//

protected:

	// pointer to Player CharacterAnimInstance
	UPlayerAnimInstance* m_pPlayerAnimInstance;

	// Logan - pointer to Game Instance
	UUMGGameInstance* m_pGameInstance;

//-----------------------------------------------------------------//
	
};
