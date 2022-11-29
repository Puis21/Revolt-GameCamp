// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Animation/AnimInstance.h"
#include "GCFPS/Player/FPCharacter.h"
#include "GCFPS/Player/Animation/AnimInstance/PlayerAnimInstance.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
{
	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh "));
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	m_pOwnerPlayerCharacter = Cast<AFPCharacter>(GetOwner());
	m_pGameInstance = Cast<UUMGGameInstance>(GetGameInstance());
	
	m_fImpactVolumeMultiplier = m_pGameInstance->GetSFXVolumeValue();
}

void ABaseWeapon::SetOwner(AActor *OwningActor)
{
	if(OwningActor)
	{
		m_pOwnerPlayerCharacter = Cast<AFPCharacter>(OwningActor);
		
		if (m_pOwnerPlayerCharacter != nullptr)
		{
			UPlayerAnimInstance* pTempAnimInstance = Cast<UPlayerAnimInstance>(m_pOwnerPlayerCharacter->GetMesh1P()->GetAnimInstance());
			if (pTempAnimInstance != nullptr)
			{
				m_pPlayerAnimInstance = pTempAnimInstance;
			}
		}
	}
}

void ABaseWeapon::StartFiringWeapon()
{
	// Start Fire
}

void ABaseWeapon::StopFiringWeapon()
{
	//Stop Fire
}

void ABaseWeapon::FireAutomatic()
{
	//Auto Fire(If Timer)
}

void ABaseWeapon::ReloadWeapon()
{
	//Reload(not in use)
}


AFPCharacter* ABaseWeapon::GetPlayer() const
{
	return m_pOwnerPlayerCharacter;
}



