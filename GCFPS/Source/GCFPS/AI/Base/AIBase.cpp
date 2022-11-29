// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBase.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <Components/PrimitiveComponent.h>
#include <GameFramework/CharacterMovementComponent.h>

#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GCFPS/AI/Components/Shooting/GenericAIShootingComponent.h"
#include "DrawDebugHelpers.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Components/Health/AIHealthComponent.h"
#include "GCFPS/Components/HealthComponent.h"
#include "GCFPS/AI/Explosion/ExplosionActor.h"
#include "GCFPS/AI/TokenSystem/AIToken.h"
#include "GCFPS/AI/TokenSystem/AITokenHandler.h"
#include "GCFPS/EnemyEncounters/EnemyEncounter.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

#include "GCFPS/PickUps/PickUp.h"
#include "GCFPS/PickUps/AmmoPickUps/BaseAmmo.h"
#include "GCFPS/PickUps/AmmoPickUps/AssaultRifleAmmo.h"

#include "GCFPS/PickUps/BasePickUp.h"

#include "GCFPS/Player/Components/Stats/PlayerRegenStatsComponent.h"

#include "GCFPS/Weapons/BaseWeapon.h"
#include "GCFPS/Weapons/WeaponAssaultRifle.h"
#include "GCFPS/Weapons/WeaponShotgun.h"
#include "GCFPS/Weapons/WeaponFlarePistol.h"
#include "GCFPS/EnemyEncounters/CombatManager/CombatManager.h"

FName AAIBase::ShootingComponentName( TEXT( "ShootingComp" ) );


UGenericAIShootingComponent* AAIBase::GetShootingComponent() const
{
	return m_pACShootingComponent;
}

// Sets default values
AAIBase::AAIBase( const FObjectInitializer& ObjectInitializer )
	: m_bHasAttackToken				( false )
	, m_bCanStealToken				( false )
	, m_bWasTokenRequestSuccessful	( false )
	, m_bCanAttack					( false )
	, m_bIsTaunting					( false )
	, m_bIsInTokenQueue				( false )
	, m_bIsAddedToEncounter			( false )
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	

	AIBaseSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "CharacterMesh1P" ) );
	AIBaseSkeletalMesh->SetupAttachment( RootComponent );

	// Set up AI_Gun mesh
	AI_GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "AI_GunMesh" ) );
	//AI_GunMesh->SetOnlyOwnerSee( false );
	AI_GunMesh->bCastDynamicShadow = false;
	AI_GunMesh->CastShadow = false;
	AI_GunMesh->SetupAttachment( RootComponent );

	m_pMuzzleComponent = CreateDefaultSubobject<USceneComponent>( TEXT( "MuzzleLocation" ) );
	m_pMuzzleComponent->SetupAttachment( AI_GunMesh );	//, "Muzzle" );
	//AI_MuzzleLocation->SetRelativeLocation( FVector( 0.2f, 48.4f, -10.6f ) );

	//Initialize AI Health Component
	AIHealth = CreateDefaultSubobject<UAIHealthComponent>( TEXT( "Stats" ) );

	// Initialize Shooting Component
	m_pACShootingComponent = CreateDefaultSubobject<UGenericAIShootingComponent>( AAIBase::ShootingComponentName );

	m_FireComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireComponent"));
	m_FireComponent->SetupAttachment(RootComponent);
	m_FireComponent->bAutoActivate = false;

	m_bHasAttackPermission = false;

	// -----
	// Pickup Drops
	
	// Set default values
	m_fRandomNumber = 0.0f;
	m_bCanDropPickUp = false;

	AmmoWeightRangeMin = 0.05f;
	AmmoWeightRangeMax = 0.7f;

	m_bIsOnFire = false;
	m_fFireDPSRepeat = 0.2f;
}

void AAIBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (m_pACShootingComponent != nullptr)
	{
		m_pACShootingComponent->SetOwnerAI(*this);
	}
}

UGenericAIShootingComponent* AAIBase::GetGenericAIShootingComponent() const
{
	return m_pACShootingComponent;
}


// Called to bind functionality to input
//void AAIBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

// Called when the game starts or when spawned
//void AAIBase::BeginPlay()
//{
	//Super::BeginPlay();

	//m_paPlayerWeaponArray = m_pPlayerCharacter->GetWeaponArray();
//}

void AAIBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Debug - Check which ai has a token
	//if (m_bHasAttackToken == true)
	//{
	//	DrawDebugSphere( GetWorld(), GetActorLocation(), 40.0f, 10.0f, FColor::Red, false, -1.0f, 0, 2 );
	//}
	//else if (m_bHasAttackToken == false)
	//{
	//	DrawDebugSphere( GetWorld(), GetActorLocation(), 40.0f, 10.0f, FColor::Green, false, -1.0f, 0, 2 );
	//}
	
	if (m_bIsOnFire)
	{

		if (AIHealth->getCurrentHealth() <= 0 || AIHealth->getCurrentFireStackAmount() == 0)
		{
			m_bIsOnFire = false;
		}
		else 
		{
			//UGameplayStatics::SpawnEmitterAttached(m_FireParticles, AIBaseSkeletalMesh, FName("AI"), AIBaseSkeletalMesh->GetSocketLocation(FName("AI")),
				//AIBaseSkeletalMesh->GetSocketRotation(FName("AI")), m_FireParticlesSize, EAttachLocation::KeepWorldPosition, true);
			AIHealth->FireDPS(DeltaTime);
		}
	}
	else
	{
		if (m_FireComponent != nullptr)
		{
			m_FireComponent->DeactivaateNextTick();
		}
	}
	
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Cyan, FString::FromInt(AIHealth->getCurrentHealth()));
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Black, FString::Printf(TEXT("Bool: %s"), m_bIsOnFire ? TEXT("true") : TEXT("false")));
		if( m_bHasAttackPermission )
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Orange, GetName() + FString(" Has Permission") );
		}

	}
}

// -----

//void AAIBase::SpawnExplosion()
//{
//	FActorSpawnParameters ActorSpawnParams;
//
//	FTransform ActorTransform = GetTransform();
//
//	//const FRotator SpawnRotation = GetControlRotation();
//	//const FVector SpawnLocation = GetActorLocation();
//
//	GetWorld()->SpawnActor<AExplosionActor>( ExplosionActorClass, ActorTransform, ActorSpawnParams );
//}

void AAIBase::SpawnExplosion( float Damage, float Radius, float ImpulseStrength, float TimeToExplosion )
{
	FTransform ActorTransform = GetTransform();

	// Spawning with parameters cannot be done, so use BeginDeferredActorSpawnFromClass 
	// to pass in set values for the explosion
	auto explosionActor = Cast<AExplosionActor>( UGameplayStatics::BeginDeferredActorSpawnFromClass( this, ExplosionActorClass, ActorTransform ) );
	if (explosionActor != nullptr)
	{
		explosionActor->Init( Damage, Radius, ImpulseStrength, TimeToExplosion );

		UGameplayStatics::FinishSpawningActor( explosionActor, ActorTransform );
	}
}

void AAIBase::MeleeTell()
{
	//GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Blue, TEXT( "Melee Tell" ) );

	// Do attack after attack tell
	GetWorld()->GetTimerManager().SetTimer( MeleeTellTimerHandle, this, &AAIBase::MeleeAttack, MeleeTellTime, false );
}

void AAIBase::RangedTell()
{
	//GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Blue, TEXT( "Ranged Tell" ) );

	// Do attack after attack tell
	GetWorld()->GetTimerManager().SetTimer( RangedTellTimerHandle, this, &AAIBase::RangedAttack, RangedTellTime, false );
}

void AAIBase::MeleeAttack()
{

}

void AAIBase::RangedAttack()
{

}

void AAIBase::Taunt()
{

}

void AAIBase::DropPickup()
{
	if( m_pPlayerCharRegenStatsComp )
	{
		// Calculate weight of Armour

		// Get the current percentage of the armour the player has
		m_fPlayerCurrentArmourPercentage = ( m_pPlayerCharRegenStatsComp->GetCurrentArmour() / m_pPlayerCharRegenStatsComp->GetMaximumArmour() ) * 100.0f;

		// Calculate the missing percentage of armour the player has
		if (m_fPlayerCurrentArmourPercentage == 0.0f)
		{
			m_fPlayerMissingArmourPercentage = 100.0f;
		}
		else
		{
			m_fPlayerMissingArmourPercentage = 100.0f - m_fPlayerCurrentArmourPercentage;
		} 
		// Armour Weight range
		float fArmourWeightRange = ArmourWeightRangeMax - ArmourWeightRangeMin;	// = 0.85f
		// Set the armour weight between the armour weight range, based on percentage of armour the player currently has
		m_fArmourWeight = (m_fPlayerMissingArmourPercentage * fArmourWeightRange / 100.0f) + ArmourWeightRangeMin;

		// -----
		// Calculate the weight of all the ammo

		// Calculate Ammo Weight Range
		m_fAmmoWeightRange = AmmoWeightRangeMax - AmmoWeightRangeMin;

		// Shotgun Weight
		m_pShotgunWeapon = m_pPlayerCharacter->GetWeaponArray()[0];
		if (m_pShotgunWeapon)
		{
			// Calculate the current percentage of shotgun ammo the player has
			m_fPlayerCurrentShotGunAmmoPercentage = (float( m_pShotgunWeapon->GetWeaponAmmo() ) / float( m_pShotgunWeapon->GetWeaponMaxReserveAmmo() )) * 100.0f;
			
			// Calculate the missing percentage of shotgun ammo the player has
			if (m_fPlayerCurrentShotGunAmmoPercentage == 0.0f)
			{
				m_fPlayerMissingShotGunAmmoPercentage = 100.0f;
			}
			else
			{		
				m_fPlayerMissingShotGunAmmoPercentage = 100.0f - m_fPlayerCurrentShotGunAmmoPercentage;
			}

			m_fShotgunAmmoWeight = (m_fPlayerMissingShotGunAmmoPercentage * m_fAmmoWeightRange / 100.0f) + AmmoWeightRangeMin;
		}

		// Assault Rifle Weight
		m_pAssaultRifleWeapon = m_pPlayerCharacter->GetWeaponArray()[1];
		if (m_pAssaultRifleWeapon)
		{
			// Calculate current percentage of assault rifle ammo the player has
			m_fPlayerCurrentAssaultRifleAmmoPercentage = (float( m_pAssaultRifleWeapon->GetWeaponAmmo() ) / float( m_pAssaultRifleWeapon->GetWeaponMaxReserveAmmo() )) * 100.0f;

			// Calcualte the missing percentage of ammo the player has
			if (m_fPlayerCurrentAssaultRifleAmmoPercentage == 0.0f)
			{
				m_fPlayerMissingAssaultRifleAmmoPercentage = 100.0f;
			}
			else
			{
				m_fPlayerMissingAssaultRifleAmmoPercentage = 100.0f - m_fPlayerCurrentAssaultRifleAmmoPercentage;
			}

			m_fAssaultRifleAmmoWeight = (m_fPlayerMissingAssaultRifleAmmoPercentage * m_fAmmoWeightRange / 100.0f) + AmmoWeightRangeMin;
		}

		// Flare Pistol Weight
		m_pFlarePistolWeapon = m_pPlayerCharacter->GetWeaponArray()[2];
		if (m_pFlarePistolWeapon)
		{
			// Calculate current percentage of flare pistol ammo the player has
			m_fPlayerCurrentFlarePistolAmmoPercentage = (float( m_pFlarePistolWeapon->GetWeaponAmmo() ) / float( m_pFlarePistolWeapon->GetWeaponMaxReserveAmmo() )) * 100.0f;

			// Calculate missing percentage of ammo the player has
			if (m_fPlayerCurrentFlarePistolAmmoPercentage == 0.0f)
			{
				m_fPlayerMissingFlarePistolAmmoPercentage = 100.0f;
			}
			else
			{
				m_fPlayerMissingFlarePistolAmmoPercentage = 100.0f - m_fPlayerCurrentFlarePistolAmmoPercentage;
			}

			m_fFlarePistolAmmoWeight = (m_fPlayerMissingFlarePistolAmmoPercentage * m_fAmmoWeightRange / 100.0f) + AmmoWeightRangeMin;
		}

		// -----
		// Add total weights
		m_fTotalWeights = m_fArmourWeight + m_fShotgunAmmoWeight + m_fAssaultRifleAmmoWeight + m_fFlarePistolAmmoWeight;
		// Get a random number between 0 and the m_fTotalWeights
		float fRandomNumForWeights = FMath::RandRange( 0.0f, m_fTotalWeights );

		// Put weights into array
		TArray<float> Weights = { m_fArmourWeight,
								  m_fShotgunAmmoWeight,
								  m_fAssaultRifleAmmoWeight,
								  m_fFlarePistolAmmoWeight };

		// Sort out the weights based on highest to smallest, where the highest weight has a greate chance of spawning as it 
		// is the first compared to the fRandomNumberForWeights
		Weights.StableSort( []( const float& A, const float& B )
			{
				return A > B;
			} );

		// Set the weights inside the pickups for sorting
		ArmourActor->GetDefaultObject<ABasePickUp>()->SetWeight( m_fArmourWeight );
		ShotGunAmmoActor->GetDefaultObject<ABasePickUp>()->SetWeight( m_fShotgunAmmoWeight );
		AssaultRifleActor->GetDefaultObject<ABasePickUp>()->SetWeight( m_fAssaultRifleAmmoWeight );
		FlarePistolActor->GetDefaultObject<ABasePickUp>()->SetWeight( m_fFlarePistolAmmoWeight );

		// Store types of pickups into array
		TArray<TSubclassOf<ABasePickUp>> PickUps = { ArmourActor,
													 ShotGunAmmoActor,
													 AssaultRifleActor,
													 FlarePistolActor };
			
		// Order is (PickUpWeights):
		// Armour
		// Shotgun ammo
		// AR ammo
		// Flare pistol ammo

		// Array of pickups which will be used to determine which pickup to drop
		TArray<TSubclassOf<ABasePickUp>> PickUpsToDrop;	 

		// Loop through TArray Weights and compare to the weights in TArray PickUps
		// to decide the order of pickups to drop
		// - As Weights are already sorted from highest to lowest priority
		for (int i = 0; i < Weights.Num(); i++)
		{
			if (Weights[i] == PickUps[0]->GetDefaultObject<ABasePickUp>()->GetWeight())
			{
				PickUpsToDrop.Add( PickUps[0] );
			}	
			else if (Weights[i] == PickUps[1]->GetDefaultObject<ABasePickUp>()->GetWeight())
			{
				PickUpsToDrop.Add( PickUps[1] );
			}			
			else if (Weights[i] == PickUps[2]->GetDefaultObject<ABasePickUp>()->GetWeight())
			{
				PickUpsToDrop.Add( PickUps[2] );
			}
			else if (Weights[i] == PickUps[3]->GetDefaultObject<ABasePickUp>()->GetWeight())
			{
				PickUpsToDrop.Add( PickUps[3] );
			}
		}
	
		// Determine which pickup to drop based on the 
		for (int i = 0; i < Weights.Num(); ++i)
		{
			// Loop through weights and check if the random number is less than this weight
			if (fRandomNumForWeights < Weights[i])
			{	
				// Actor to spawn is PickUps[i]
				PickUpToSpawn = PickUpsToDrop[i];
				// Get out of loop as the pickup to drop has been set
				break;	
			}

			// If not, take this weight from 
			fRandomNumForWeights -= Weights[i];	
		}

		// Don't drop a pickup if:
		if (	( m_pPlayerCharRegenStatsComp->GetCurrentArmour() == m_pPlayerCharRegenStatsComp->GetMaximumArmour() ) 
			&&	( m_fPlayerCurrentShotGunAmmoPercentage		 >= 80.0f)
			&&	( m_fPlayerCurrentAssaultRifleAmmoPercentage >= 80.0f)
			&&	( m_fPlayerCurrentFlarePistolAmmoPercentage  >= 80.0f) )
		{
			m_bCanDropPickUp = false;
		}
		else
		{
			// If any of these conditions fail
			// This enemy can drop a pickup
			m_bCanDropPickUp = true;
		}

		// -----
		// Spawn a pickup
		if (m_bCanDropPickUp == true)
		{
			FActorSpawnParameters ActorSpawnParams;
			const FVector SpawnLocation = GetActorLocation();
			const FRotator SpawnRotation = GetActorRotation();

			// Calculate random number
			m_fRandomNumber = FMath::RandRange( 0.0f, 1.0f );

			// Based on enemy type
			// & if the random number is greater than the Drop1Value
			switch (m_eEnemyType)
			{
				case (EEnemyType::Seeker):
				{
					if (m_fRandomNumber > m_fSeekerDrop1Value)
					{
						// Spawn 1
						GetWorld()->SpawnActor<ABasePickUp>( PickUpToSpawn, SpawnLocation, SpawnRotation, ActorSpawnParams );
					}
				}
				break;

				case (EEnemyType::Turret):
				{
					//if (m_fRandomNumber > TurretDrop1RangeMin && m_fRandomNumber <= TurretDrop1RangeMax)
					if (m_fRandomNumber > m_fTurretDrop1Value)
					{
						// Spawn 1
						GetWorld()->SpawnActor<ABasePickUp>( PickUpToSpawn, SpawnLocation, SpawnRotation, ActorSpawnParams );
					}
					//else if (m_fRandomNumber > TurretDrop2Value)
					//{
					//	// Spawn 2
					//	GetWorld()->SpawnActor<ABasePickUp>( PickUpToSpawn, SpawnLocation, SpawnRotation, ActorSpawnParams );
					//	// Increase the spawn locationg slightly, so you are able to tell if two pickups spawn - NOT FINAL
					//	FVector IncreasedSpawnLocation = SpawnLocation + 30.0f;
					//	GetWorld()->SpawnActor<ABasePickUp>( PickUpToSpawn, SpawnLocation, SpawnRotation, ActorSpawnParams );
					//}

				}
				break;

				case (EEnemyType::Enforcer):
				{
					//if (m_fRandomNumber > EnforcerDrop1RangeMin && m_fRandomNumber <= EnforcerDrop1RangeMax)
					if (m_fRandomNumber > m_fEnforcerDrop1Value)
					{
						// Spawn 1
						GetWorld()->SpawnActor<ABasePickUp>( PickUpToSpawn, SpawnLocation, SpawnRotation, ActorSpawnParams );
					}
					//else if (m_fRandomNumber > EnforcerDrop2Value)
					//{
					//	// Spawn 2
					//	GetWorld()->SpawnActor<ABasePickUp>( PickUpToSpawn, SpawnLocation, SpawnRotation, ActorSpawnParams );
					//	// Increase the spawn locationg slightly, so you are able to tell if two pickups spawn - NOT FINAL
					//	FVector IncreasedSpawnLocation = SpawnLocation + 30.0f;
					//	GetWorld()->SpawnActor<ABasePickUp>( PickUpToSpawn, SpawnLocation, SpawnRotation, ActorSpawnParams );
					//}
				}
				break;

				default:
				{
					GEngine->AddOnScreenDebugMessage( -1, 3.0f, FColor::Orange, TEXT( "No enemy type for pickup to drop" ) );
				}
				break;
			}
		}
	}
}

void AAIBase::CallEndPlay()
{
	// Remove from AITokenQueue in TokenHandler if killed
	// As if they are at the front of the queue to recieve a token, but does not exist
	// it will cause a crash
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass( GetWorld(), AAITokenHandler::StaticClass(), FoundActors );

	for (auto& actor : FoundActors)
	{
		AAITokenHandler* pTokenHandler = Cast <AAITokenHandler>( actor );

		if (pTokenHandler)
		{
			if (m_bIsInTokenQueue == true)
			{
				m_bIsInTokenQueue = false;

				pTokenHandler->GetAITokenQueue().Remove( this );
			}
		}
	}

	// -----

	// Release Token on death with 0 cooldown
	ReleaseToken( 0.1f );

	// Spawn explosion actor on death
	//SpawnExplosion();
	SpawnExplosion( ExplosionDamage, ExplosionRadius, ExplosionImpulseStrength, ExplosionTimeToExplode );

	// Drop pickup/s
	//GetWorld()->GetTimerManager().SetTimer( PickUpDropsTimerHandle, this, &AAIBase::DropPickup, 1.0f, false );	

	DropPickup();

	// -----

	// Clear timer handle/s
	GetWorld()->GetTimerManager().ClearTimer( MeleeTellTimerHandle );
	GetWorld()->GetTimerManager().ClearTimer( RangedTellTimerHandle );
	GetWorld()->GetTimerManager().ClearTimer( PickUpDropsTimerHandle );

	// Clear all timers for this object
	//GetWorld()->GetTimerManager().ClearAllTimersForObject( this );
}

USceneComponent* AAIBase::GetMuzzleComponent() const
{
	return m_pMuzzleComponent;
}

void AAIBase::PrintNoEncounterAssignedError() const
{
	if( GEngine )
	{
		//GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Red, ( FString(" DOES NOT HAVE ASSIGNED ENEMY ENCOUNTER " ) + GetName() ) );
	}
}

// -----

void AAIBase::RequestToken()
{
	if (m_pOwnerEncounter)
	{
		AAITokenHandler* pTokenHandler = m_pOwnerEncounter->GetTokenHandler();

		if (pTokenHandler)
		{
			if (pTokenHandler->GetTokensAvailable( false ) == true)
			{
				if (m_bHasAttackToken == false && m_bIsInTokenQueue == false)	// && currentTokenHandler->GetTokensAvailable() == true )
				{
					//GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Red, TEXT( "Token Request - Addding into queue" ) );

					// Add to token handlers token queue to wait to be given a token
					m_bIsInTokenQueue = true;

					// Where will the AI go in the AITokenQueue based on enemy type priority?
					// If the queue is empty, add into the start
					//pTokenHandler->AddToTokenQueue( this );
					if (pTokenHandler->GetAITokenQueue().Num() == 0)
					{
						pTokenHandler->AddToTokenQueue( this );
					}
					else
					{
						// Loop through the AITokenQueue and determine where to place the AI
						for (int i = 0; i < pTokenHandler->GetAITokenQueue().Num(); i++)
						{
							switch (pTokenHandler->GetAITokenQueue()[i]->GetEnemyType())
							{
								case (EEnemyType::Turret):
								{
									//GEngine->AddOnScreenDebugMessage( -1, 3.0f, FColor::Black, TEXT( "ENEMYTYPE::TURRET" ) );

									if (m_eEnemyType == EEnemyType::Turret)
									{
										pTokenHandler->AddToTokenQueue( this, i + 1 );	// , i );		
									}
									else
									{
										pTokenHandler->AddToTokenQueue( this );
									}

								}
								break;

								case (EEnemyType::Enforcer):
								{
									//GEngine->AddOnScreenDebugMessage( -1, 3.0f, FColor::Black, TEXT( "ENEMYTYPE::ENFORCER" ) );
									// Increase the size of the array by 1 to be able to insert the AI into the queue at a certain position
									//pTokenHandler->GetAITokenQueue().
									//pTokenHandler->GetAITokenQueue().SetNum( pTokenHandler->GetAITokenQueue().Num() + 1 );	// , false );
									if (m_eEnemyType == EEnemyType::Enforcer)
									{
										pTokenHandler->AddToTokenQueue( this, i + 1 );
									}
									else
									{
										pTokenHandler->AddToTokenQueue( this );
									}
									// , i + 1 );	// i + 1 -> does not exist	// getting added in twice
									//pTokenHandler->GetAITokenQueue().Add( this );
									// Get the this AI that was recently added
									//auto AIChar = pTokenHandler->GetAITokenQueue()[pTokenHandler->GetAITokenQueue().Num() - 1];
									// Insert into different position in queue
									//pTokenHandler->AddToTokenQueue( AIChar, i);
									//GEngine->AddOnScreenDebugMessage( -1, 1.5f, FColor::Black, TEXT( "ITERATING THROUGH ARRAY QUEUE" ), pTokenHandler->GetAITokenQueue().Num() );
									//GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Purple, FString::FromInt( pTokenHandler->GetAITokenQueue().Num() ) );									

								}
								break;

								default:
								{
									// If enemy type doesn't yet exist in the queue
									// Just add to the end of the queue
									GEngine->AddOnScreenDebugMessage( -1, 3.0f, FColor::Black, TEXT( "Default adding into end of queue" ) );
									pTokenHandler->AddToTokenQueue( this );
								}
								break;
							}

							// Break out of loop, otherwise a crash
							break;
						}
					}
				}
			}
			else if (pTokenHandler->GetTokensAvailable( false ) == false)	// If no token available
			{
				GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Red, TEXT( "Token Request FAIL - TAUNT" ) );

				// If unable to get an attack token the enemy must do something for the mean time
				m_bWasTokenRequestSuccessful = false;

				// Do non combat action
				// Should be able to choose from
				// for now taunting only (battlecry anim)
				m_bIsTaunting = true;

			}
		}
	}
	else
	{
		PrintNoEncounterAssignedError();
	}
}

void AAIBase::ReleaseToken( float TokenCooldown )
{
	if ( GetEnemyEncounter() )
	{
		AAITokenHandler* pTokenHandler = m_pOwnerEncounter->GetTokenHandler();

		if (pTokenHandler)
		{
			if (m_bHasAttackToken)
			{
				// Move the token back into the token holders AttackTokens array
				pTokenHandler->AddToken( TokenHolder[0] );

				// Call the cooldown of the token
				pTokenHandler->CallTokenCooldown( TokenCooldown );

				// Remove the token from AI TokenHolder
				TokenHolder.RemoveAt( 0 );

				m_bHasAttackToken = false;
				//m_bCanAttack = false;
				//m_bIsInTokenQueue = false;

				//GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Green, TEXT("Released Token") );
			}
		}
	}
}

void AAIBase::StealToken()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass( GetWorld(), AAIBase::StaticClass(), FoundActors );

	for (auto& actors : FoundActors)
	{
		AAIBase* pOtherAI = Cast<AAIBase>( actors );

		AAITokenHandler* pTokenHandler = Cast <AAITokenHandler>( actors );

		if (pOtherAI)
		{
			// If the OtherAI has a token
			if (pOtherAI->getHasAttackToken() == true)
			{
				// Take the token they have and put it into this AI's Token Holder
				AAIToken* pToken = pOtherAI->TokenHolder[0];
				TokenHolder.Add( pToken );

				// Remove the token from the other AI it was taken from 
				pOtherAI->TokenHolder.RemoveAt( 0 );
				// This AI does not have a token
				pOtherAI->setHasAttackToken( false );
				AAIBaseController* pOtherAIController = Cast<AAIBaseController>( pOtherAI->GetController() );
				if (pOtherAIController)
				{
					pOtherAIController->GetBlackboardComp()->SetValueAsBool( "HasAttackToken", false );
				}

				// Debug to show it has stolen a token
				//DrawDebugSphere( GetWorld(), GetActorLocation() + 50.0f, 40.0f, 10.0f, FColor::Black, false, 2.0f, 0, 2 );	// -1.0f, 0, 2 );

				// This AI now has a token				
				m_bHasAttackToken = true;
				// Set in the behaviour tree's blackboard
				AAIBaseController* pAIController = Cast<AAIBaseController>( GetController() );
				if (pAIController)
				{
					pAIController->GetBlackboardComp()->SetValueAsBool( "HasAttackToken", true );
					//pAIController->GetBlackboardComp()->SetValueAsBool( "WasTokenRequestSuccessful", true );	// change, only using as otherwise taunt
				}

				// If this AI was in the TokenHandler's AITokenQueue, Remove it 
				if (pTokenHandler)
				{
					if (m_bIsInTokenQueue == true)
					{
						m_bIsInTokenQueue = false;

						pTokenHandler->GetAITokenQueue().Remove( this );
					GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Red, TEXT( "Stole Token" ) );
					}
				}

				// Only steal 1 token from an AI
				// If no break is used, the AI will steal all tokens
				break;
			}
		}
	}
}

void AAIBase::ReturnToken( TArray<AAIToken*> Arr[] )
{
	Arr->Add( TokenHolder[0] );
	TokenHolder.RemoveAt( 0 );
}

void AAIBase::OnDeath()
{
	// Put to Sleep
	PutToSleep();
		
	CallEndPlay();

	// Inform the enemy encounter
	if( GetEnemyEncounter() )
	{
		m_bIsAddedToEncounter = false;
		m_pOwnerEncounter->OnEnemyDeath( this );
	}
}

void AAIBase::SetEnemyEncounter( AEnemyEncounter* pEnemyEncounter )
{
	m_pOwnerEncounter = pEnemyEncounter;
}

AEnemyEncounter* AAIBase::GetEnemyEncounter() const
{
	if( !m_pOwnerEncounter )
	{
		PrintNoEncounterAssignedError();
		return nullptr;
	}
	return m_pOwnerEncounter;
}

bool AAIBase::GetIsAddedToEncounter() const
{
	return m_bIsAddedToEncounter;
}

void AAIBase::AddToEncounter()
{
	if( !m_bIsAddedToEncounter && GetEnemyEncounter() )
	{
		m_pOwnerEncounter->OnEnemySpawn( this );
		m_bIsAddedToEncounter = true;
	}
}

void AAIBase::PutToSleep()
{
	SetActorHiddenInGame( true );
	UCapsuleComponent* pCapsuleComponent = Cast<UCapsuleComponent>( RootComponent );
	if( pCapsuleComponent )
	{
		SetActorEnableCollision( false );
		pCapsuleComponent->SetEnableGravity( false );
		pCapsuleComponent->SetGenerateOverlapEvents( false );
		pCapsuleComponent->SetPhysicsLinearVelocity( FVector::ZeroVector );
		pCapsuleComponent->PutAllRigidBodiesToSleep();
	}

	// Clear Any Accumulated Forces ( for example, if the enemy is killed by an explosion that pushes the enemy away,
	// there's a chance this enemy will trigger the next wave spawn and instantly be recycled back into the encounter,
	// and it'll spawn flying with the explosion direction still )
	UCharacterMovementComponent* pCharMovement = GetCharacterMovement();
	if( pCharMovement )
	{
		pCharMovement->ClearAccumulatedForces();
	}

	// Deactivate Skeletal Mesh (disables animation)
	if( AIBaseSkeletalMesh )
	{
		AIBaseSkeletalMesh->Deactivate();
	}

	// Stop ticking
	SetActorTickEnabled( false );

	// Get AI Controller
	AController* pController = GetController();
	if( pController )
	{
		// Stop shooting / reset shooting variables and logic
		m_pACShootingComponent->ForceStopShooting();

		// Release Token if has one
		AAIBaseController* pAIController = Cast<AAIBaseController>( pController );
		if( pAIController )
		{
			GetShootingComponent()->ReleaseToken( *pAIController );
		}
		// Unpossess the pawn
		pController->UnPossess();
	}

	// Remove Permission
	RemovePermissionOnSelf();
}

void AAIBase::WakeFromSleep()
{
	// Set actor as invisible
	SetActorHiddenInGame( false );
	UCapsuleComponent* pCapsuleComponent = Cast<UCapsuleComponent>( RootComponent );
	if( pCapsuleComponent )
	{
		SetActorEnableCollision( true );
		pCapsuleComponent->SetEnableGravity( true );
		pCapsuleComponent->SetGenerateOverlapEvents( true );
		pCapsuleComponent->SetPhysicsLinearVelocity( FVector::ZeroVector );
	}
	if( AIBaseSkeletalMesh )
	{
		AIBaseSkeletalMesh->Activate();
	}

	// Check if isn't currently possessed by a controller
	AController* pController = GetController();
	if( !pController && m_pOwnerAIController )
	{
		// Make the initial controller possess this again
		m_pOwnerAIController->Possess( this );
	}

	// Start ticking
	SetActorTickEnabled( true );

	// Reset Health to max
	AIHealth->Reset();

	// Inform the Enemy Encounter
	AddToEncounter();
}

EEnemyType AAIBase::GetEnemyType() const
{
	return m_eEnemyType;
}

AFPCharacter* AAIBase::GetPlayerCharacter() const
{
	return m_pPlayerCharacter;
}

void AAIBase::SetPlayerCharacter( AFPCharacter* pPlayerCharacter )
{
	m_pPlayerCharacter = pPlayerCharacter;
	m_pPlayerCharRegenStatsComp = Cast<UPlayerRegenStatsComponent>( m_pPlayerCharacter->GetComponentByClass( UPlayerRegenStatsComponent::StaticClass() ) );

	if( m_pOwnerAIController )
	{
		m_pOwnerAIController->SetPlayerCharacter( pPlayerCharacter );
	}

	if( m_pACShootingComponent )
	{
		m_pACShootingComponent->SetPlayerPointer( pPlayerCharacter );
	}
}

void AAIBase::ReceiveAttackPermission()
{
	m_bHasAttackPermission = true;

	if( m_pOwnerAIController )
	{
		// Get BB
		UBlackboardComponent* pBBComp = m_pOwnerAIController->GetBlackboardComp();
		// Update boolean on BB
		pBBComp->SetValueAsBool( "HasAttackPermission", m_bHasAttackPermission );
	}
}

void AAIBase::LostAttackPermission()
{
	m_bHasAttackPermission = false;

	if( m_pOwnerAIController )
	{
		// Get BB
		UBlackboardComponent* pBBComp = m_pOwnerAIController->GetBlackboardComp();
		// Update boolean on BB
		pBBComp->SetValueAsBool( "HasAttackPermission", m_bHasAttackPermission );
	}
}

void AAIBase::RemovePermissionOnSelf()
{
	if( m_bHasAttackPermission && m_pOwnerEncounter )
	{
		ACombatManager* pCombatManager = m_pOwnerEncounter->GetCombatManager();
		if( pCombatManager )
		{
			pCombatManager->RemoveEnemyPermission( this, true );
		}
	}
}

bool AAIBase::GetHasAttackPermission() const
{
	return m_bHasAttackPermission;
}

AAIBaseController* AAIBase::GetAIController() const
{
	return m_pOwnerAIController;
}

void AAIBase::PossessedBy( AController* NewController )
{
	Super::PossessedBy( NewController );

	AAIBaseController* pAICon = Cast<AAIBaseController>( NewController );
	if( pAICon )
	{
		m_pOwnerAIController = pAICon;
	}
}

void AAIBase::ApplyStatusEffect()
{
	if (!m_bIsOnFire)
	{
		m_bIsOnFire = true;
		AIHealth->AddFireStack();
		//m_FireComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), m_FireParticles, GetActorLocation(), FRotator::ZeroRotator, true);
		if (m_FireComponent != nullptr)
		{
			m_FireComponent = UGameplayStatics::SpawnEmitterAttached(m_FireParticles, AIBaseSkeletalMesh,
			FName("FireEmitter"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale, true);
		}
		
	}
	else
	{
		AIHealth->AddFireStack();
	}
}