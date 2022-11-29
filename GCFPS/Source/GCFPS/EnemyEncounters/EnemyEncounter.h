#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "GCFPS/EnemyEncounters/EnemyTypes.h"
#include "GCFPs/Player/Components/Stats/ObjectivesComponent.h"

#include "EnemyEncounter.generated.h"

class AAIBase;
class AAIEnforcer;
class AAISeeker;
class AAISentry;
class AAITurret;
class AAITokenHandler;
class AEnemyEncounterGroupManager;
class AEnemySpawner;
class AEnemyWave;
class ASimpleWaypoint;
class ATriggerBase;
class AVADialogueTrigger;
class AWaypoint;
class UBoxComponent;
class UBillboardComponent;
class UBoxComponent;
class USoundBase;

class ACombatManager;

template <typename InElementType>
class EnemyPool;

UENUM( BlueprintType )
enum class EEncounterState : uint8
{
	PendingStart	UMETA( DisplayName = "Pending Start" ),
	Active			UMETA( DisplayName = "Active" ),
	Over			UMETA( DisplayName = "Over" )
};

UCLASS()
class GCFPS_API AEnemyEncounter : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyEncounter();

	// Only called every frame during the active state of the encounter, and purely for debug purposes
	virtual void Tick(float DeltaTime) override;

	// Called when an enemy in this encounter dies
	virtual void OnEnemyDeath( AAIBase* pEnemy );

	// Called when an enemy in this encounter spawns
	virtual void OnEnemySpawn( AAIBase* pEnemy );

	// Returns the amount of enemies in this encounter, currently alive
	int GetAliveEnemyCount() const;

	// Returns pointer to the assigned Combat Manager
	ACombatManager* GetCombatManager();

	void Reset();

	// Returns current EncounterState
	EEncounterState GetEncounterState() const;

	void SetGroupManager( AEnemyEncounterGroupManager* pEnemyEncounterGroupManager );

	int GetEnforcerPoolMinSize() const;
	int GetSeekerPoolMinSize() const;
	int GetTurretPoolMinSize() const;

	// Returns pointer to TArray of waves
	TArray<AEnemyWave*>* GetWaveArray();

	// This function determines how many enemies need spawning in total for this encounter
	void CalculateAllTimeMaxEnemies();

	// Returns the linked TokenHandler
	AAITokenHandler* GetTokenHandler() const;

public:
	// Called when player enters the zone for the first time
	void BeginEncounter();
private:
	// Called when player kills enough enemies
	void EndEncounter();

protected:
	// Called in BeginEncounter(), override this function for encounter-specific interactions (i.e. close doors / play audio cue)
	virtual void OnEncounterStarted();

	// Called in EndEncounter(), override this function for encounter-specific interactions (i.e. open doors / play audio cue)
	virtual void OnEncounterEnded();

private:
	// Called when the trigger condition is met
	virtual void TriggerNextWave();

	// Called in BeginEncounter() and in TriggerNextWave()
	void TriggerSpecificWave( const int kiWaveIndex );

	void SpawnEnemyFromSpawner( AEnemySpawner* pEnemySpawner, AAIBase* pEnemy );

	// Returns Max Enemies of a certain type this wave
	int CalculateMaxEnemiesThisWave( int iEnemySpawnsThisWave, int iMaxEnemySurvivorsPreviousWave ) const;

	// Returns Max Enemies of a certain type that can make it alive to next wave
	int CalculateMaxSurvivorsForNextWave( int iMaxEnemiesThisWave, int iNextWaveTriggerCondition ) const;

	// Refreshes the existing enemies's all time highs, by comparing riCurrentAllTimeMax and iWaveMax
	// If the riCurrentAllTimeMax is lower than the iWaveMax, this will update riCurrentAllTimeMax
	void RefreshAllTimeMax( int& riCurrentAllTimeMax, int iWaveMax );

	void CalculateHalfWayWaveIndex();

	// Pointer to the Owning Encounter Group Manager
	AEnemyEncounterGroupManager* m_pGroupManager;

	// Current Alive Enemies Count
	UPROPERTY( VisibleAnywhere, Category = "General", DisplayName = "Current Enemy Count" )
	int m_iAliveEnemiesCount;

	// If we're on the last wave, the encounter will "end" when the alive enemy count is equal to or lower than this number.
	// Can leave this as 0 by default, meaning the encounter will end only when the player slaughters every single enemy.
	UPROPERTY( EditAnywhere, Category = "General", DisplayName = "End Trigger", meta = ( ClampMin = "0" ) )
	int m_iEndTriggerCondition;

	// Sound that plays when the encounter starts
	UPROPERTY( EditAnywhere, Category = "Audio", DisplayName = "Encounter Start Audio" )
	USoundBase* m_pSFXEncounterStart;

	// Sound that plays halfway through the encounter
	UPROPERTY( EditAnywhere, Category = "Audio", DisplayName = "Encounter HalfWay Audio" )
	USoundBase* m_pSFXEncounterHalfWay;

	// Sound that plays when the encounter ends
	UPROPERTY( EditAnywhere, Category = "Audio", DisplayName = "Encounter End Audio" )
	USoundBase* m_pSFXEncounterEnd;

	// In which wave should we trigger the halfway audio
	int m_iHalfWayWaveIndex;

	// Array of Enemy Waves. Place BP_EnemyWave blueprints in the level, configure them, and then reference them in this list. Make sure to order correctly.
	UPROPERTY( EditAnywhere, Category = "Waves", DisplayName = "Wave Array" )
	TArray<AEnemyWave*> m_aWaves;

	// Keep track of which wave is the current one
	UPROPERTY( VisibleAnywhere, Category = "Waves", DisplayName = "Wave Index" )
	int m_iCurrentWaveIndex;

	// Token handler assigned to this encounter
	UPROPERTY( EditAnywhere, Category = "Tokens", DisplayName = "Token Handler" )
	AAITokenHandler* m_pTokenHandler;

	// Combat manager assigned to this encounter
	UPROPERTY( EditAnywhere, Category = "Combat", DisplayName = "Combat Manager" )
	ACombatManager* m_pCombatManager;

	// This Billboard Component just adds some visual flair, makes it easy to select this actor in the level editor.
	UPROPERTY( EditDefaultsOnly, Category = "Editor Visuals", DisplayName = "Billboard Component" )
	UBillboardComponent* m_pBillboardComponent;

	// Scene Root component
	UPROPERTY( VisibleDefaultsOnly, Category = "Root", DisplayName = "Root")
	USceneComponent* m_pRootSceneComponent;

	// Current state of this encounter. 
	// Can be 'Pending Start', 'Active' or 'Over'.
	UPROPERTY( VisibleAnywhere, Category = "Encounter States", DisplayName = "Encounter State" )
	EEncounterState m_eEncounterState;

	// Number of Max Active Enemies at any time for each Enemy Type
	int m_iAllTimeMaxEnforcers;
	int m_iAllTimeMaxSeekers;
	int m_iAllTimeMaxTurrets;



	// Enemy Pools ( Enforcer / Seeker / Turret )
private:
	EnemyPool<AAIEnforcer>* m_pEnforcerPool;
	//EnemyPool<AAISentry>* m_pSentryPool;
	EnemyPool<AAISeeker>* m_pSeekerPool;
	EnemyPool<AAITurret>* m_pTurretPool;


// Logan -------------------------------------------------------------------------------------------
public:
	UObjectivesComponent* ObjectivesComponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//Objective Title
	UPROPERTY( EditAnywhere, Category = "Objectives", DisplayName = "Objective Title" )
	FString ObjectiveTitle;

	//Objective Name
	UPROPERTY( VisibleAnywhere, Category = "Objectives", DisplayName = "Objective Text" )
	FString ObjectiveText;

	//Objective Title For The Encounter End
	UPROPERTY( EditAnywhere, Category = "Objectives", DisplayName = "Objective Title End" )
	FString ObjectiveTitleEnd;

	//Objective Name for the Encounter End
	UPROPERTY( EditAnywhere, Category = "Objectives", DisplayName = "Objective Text End" )
	FString ObjectiveTextEnd;

	// Waypoint Icon
	UPROPERTY( EditAnywhere, Category = "Waypoint", DisplayName = "End Objective Waypoint" )
	AWaypoint* ObjectiveWaypoint;

	void SetFirstWaypointPos();

	bool m_bHasSetFirstWaypoint;

	//UPROPERTY( EditAnywhere, Category = "Voice Acting Trigger", DisplayName = "VA Trigger Resistance " )
	//	AVADialogueTrigger* m_pVADialogueTriggerResistance;

	void SetFirstEncounterVA();

	bool m_bHasSetFirstEncounterVA;



//--Umeer-------------------------------------------------------------------//

	UPROPERTY(EditAnywhere, Category = "Waypoint", DisplayName = "Simple Waypoint")
	ASimpleWaypoint* m_pSimpleWaypoint;

	void ResetHasSetSetFirstWaypoint();
	void ResetHasSetFirstEncounterVA();


};