// 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GCFPS/EnemyEncounters/EnemyTypes.h"
#include "EnemyEncounterGroupManager.generated.h"

class AEnemyEncounter;
class AEnemyWave;
class AAIBase;
class AAIEnforcer;
class AAISeeker;
class AAITurret;
class UBoxComponent;
class AFPCharacter;
template <typename InElementType>
class EnemyPool;
class UAudioComponent;


UENUM( BlueprintType )
enum class EEncounterGroupState : uint8
{
	PendingStart			UMETA( DisplayName = "Pending Start" ),
	Active					UMETA( DisplayName = "Active" ),
	Disabled				UMETA( DisplayName = "Disabled"),
	PendingNextEncounter	UMETA( DisplayName = "Pending Next Encounter")
};

UCLASS()
class GCFPS_API AEnemyEncounterGroupManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyEncounterGroupManager();
	virtual ~AEnemyEncounterGroupManager();

	void BeginPlay() override;

	// Called when an actor begins overlap with the main trigger volume
	UFUNCTION()
	void OnEncounterTriggerOverlapBegin( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	// Called when an actor begins overlap with the safe zone
	UFUNCTION()
	void OnSafeZoneOverlapBegin( UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	void OnEncounterEnded();



private:
	// Scene Root component
	UPROPERTY( VisibleDefaultsOnly, Category = "Root", DisplayName = "Root" )
	USceneComponent* m_pRootSceneComponent;

	// This Billboard Component just adds some visual flair, makes it easy to select this actor in the level editor.
	UPROPERTY( EditDefaultsOnly, Category = "Editor Visuals", DisplayName = "Billboard Component" )
	UBillboardComponent* m_pBillboardComponent;

	// When player overlaps with this volume, the encounter will trigger.
	UPROPERTY( VisibleDefaultsOnly, Category = "Trigger Volumes", DisplayName = "Encounter Trigger Box" )
	UBoxComponent* m_pEncounterTriggerVolume;

	// When player overlaps with this volume, the encounter state will go back to pending
	// So that next time the player enters the Encounter Trigger Volume, the next encounter can start
	UPROPERTY( VisibleDefaultsOnly, Category = "Trigger Volumes", DisplayName = "Safe Zone Trigger Box" )
	UBoxComponent* m_pSafeZoneTriggerVolume;


	// Array of EnemyEncounters
	UPROPERTY( EditAnywhere, Category = "Enemy Encounters", DisplayName = "Pre-Confifgured Enemy Encounters" )
	TArray<AEnemyEncounter*> m_aEncounters;

	// Keep track of which is the current encounter from m_aEncounters
	int m_iEncounterIndex;

	// Anmount of times the player has cleared every encounter / beat the game in a row
	// This will also be the number of final waves
	int m_iFinalWavesToAdd;

	// Keep track of the current state of this encounter group
	EEncounterGroupState m_eEncounterGroupState;

	// Array of Waves
	// When the player reaches the last configured encounter, every time the encounter ends,
	// It will add onto itself a new wave, picked randomly, from this wave array
	UPROPERTY( EditAnywhere, Category = "Enemy Encounters", DisplayName = "Possible Last Waves" )
	TArray<AEnemyWave*> m_aLastWaves;

	void AssignEncounterGroupOwnership();

	void ConfigureEnemyPools();
	void CalculateEnemyPoolSizes();
	void RefreshFinalPoolSize( int& riCurrentPoolSize, const int kiMinimumPoolSize );
	void SpawnEnemyPool( const EEnemyType keEnemyType );

	// Called for each enemy spawned in SpawnEnemyPool(), this has a switch state that'll look for the
	// correct pool of enemies to add the enemy pointer passed as an argument
	void PushEnemyIntoInactivePool( AAIBase* pAIBase, const EEnemyType keEnemyType );

	// Called multiple times in PrepareEnemyPools(), after we know whether or not we'll be needing this enemy type.
	// This will spawn 2 raw C++ enemies of this class, they'll be used as the Active and Inactive list heads
	void InitializeEnemyPool( const EEnemyType keEnemyType );
	
	// Called when the player overlaps with the encounter trigger volume
	void OnPlayerEnterEncounterZone();

	// Called when the player overlaps with the safe zone trigger volume
	void OnPlayerEnterSafeZone();

	// Called when the player re-enters the encounters zone
	void TriggerNextEncounter();

	// Called multiple times in PrepareEnemyPools(), this will use the pre calculated enemy pool sizes and spawn that amount of enemies
	// moving them into the inactive pool, specified through the EEnemyType enum
	// This can only be called after InitializeEnemyPool( const EEnemyType keEnemyType ) of the same keEnemyType
	//void PopulateEnemyPool( const EEnemyType keEnemyType );

	// Called for each enemy spawned in PopulateEnemyPool(), this has a switch state that'll look for the
	// correct pool of enemies to add the enemy pointer passed as an argument
	//void PushEnemyIntoInactivePool( AAIBase* pAIBase, const EEnemyType keEnemyType );

private:
	// These classes need to be assigned in the editor, because our enemy encounter
	// needs to spawn enemy blueprints, not the pure C++ classes
	// Another way of doing it would be hardsetting the blueprint paths, and having a class finder assign the class
	// but it's just messy and error prone.
	UPROPERTY( EditDefaultsOnly, Category = "Enemy BluePrints", DisplayName = "Enforcer BluePrint" )
	TSubclassOf<AAIBase> m_bpEnforcerClass;
	UPROPERTY( EditDefaultsOnly, Category = "Enemy BluePrints", DisplayName = "Seeker BluePrint" )
	TSubclassOf<AAIBase> m_bpSeekerClass;
	UPROPERTY( EditDefaultsOnly, Category = "Enemy BluePrints", DisplayName = "Turret BluePrint" )
	TSubclassOf<AAIBase> m_bpTurretClass;

	// Enemy Pools
	// All Encounters will be using these 3 pools
	EnemyPool<AAIEnforcer>* m_pEnforcerPool;
	EnemyPool<AAISeeker>* m_pSeekerPool;
	EnemyPool<AAITurret>* m_pTurretPool;

	// Enemy Pool Sizes
	int m_iAllTimeMaxEnforcers;
	int m_iAllTimeMaxSeekers;
	int m_iAllTimeMaxTurrets;



	// Pool Getters
public:
	EnemyPool<AAIEnforcer>* GetEnforcerPool();
	EnemyPool<AAISeeker>* GetSeekerPool();
	EnemyPool<AAITurret>* GetTurretPool();

private:
	AEnemyWave* GetRandomLastWave() const;
	void AddRandomWavesToFinalEncounter();
	
	// The Final Encounter's initial wave configuration
	// We save this to reuse it every time the final encounter triggers
	// The final encounter will keep getting harder, 
	TArray<AEnemyWave*> m_aFinalEncounterWaveConfiguration;

	UPROPERTY(EditAnywhere, Category = "Fade", meta = (AllowPrivateAccess = "true"))
	float FadeVolume;

	// This function runs an algorith to calculate the max amount of enemies for each type
	// in any given sequence of the last random waves.
	// It's different from the normal encounter algorithm, because that one only has to check
	// the previous wave survivors + the current wave spawns, whereas here we need to consider
	// every possible combination and outcome
	void CalculateMaxEnemiesInLastWaves();

	void CalculateMaxEnemiesInPreConfiguredEncounter();

	void SetPlayerPointerOnEnemies( AFPCharacter* pPlayerCharacter );

	void SetPlayerPointerOnCombatManagers( AFPCharacter* pPlayerCharacter );

// Logan Audio ---------------------------------------------------------------------------------
//public:
//	// Logan - Sound Functions to call when Events change //
//	void OnBackgroundEndAndCombatBegin();
//	void OnCombatEndAndChaseBegin();
//	void OnChaseEndAndBackgroundBegin();
//private:
	//// Logan - Private Audio Components //
	//UAudioComponent* BackgroundMusic;
	//UAudioComponent* CombatMusic;
	//UAudioComponent* ChaseMusic;
};