#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"

class AAIBase;
class AAIEnforcer;
class AAITurret;
class AFPCharacter;
class UBillboardComponent;

UCLASS()
class GCFPS_API ACombatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatManager();

	void OnEnemyWakeFromSleep( AAIBase* pAIBase );
	void OnEnemyFellAsleep( AAIBase* pAIBase );
	
	void OnWaveFinishedSpawning();

	void TryGivingPermission();

	void GiveEnemyPermission( AAIBase* pEnemy );
	void RemoveEnemyPermission( AAIBase* pEnemy, bool bEnemyIsAlive );

	void SetPlayerCharacterPointer( AFPCharacter* pPlayerCharacter );

	bool ShouldPermissionsBeGiven() const;

	void AddEnemyToIdleArray( AAIBase* pEnemy );
	void RemoveEnemyFromIdleArray( AAIBase* pEnemy );

private:
	// Scene Root component
	UPROPERTY( VisibleDefaultsOnly, Category = "Root", DisplayName = "Root" )
	USceneComponent* m_pRootSceneComponent;

	// This Billboard Component just adds some visual flair, makes it easy to select this actor in the level editor.
	UPROPERTY( EditDefaultsOnly, Category = "Editor Visuals", DisplayName = "Billboard Component" )
	UBillboardComponent* m_pBillboardComponent;

	// Number of enemies that should be attacking at the same time
	UPROPERTY( EditAnywhere, Category = "Active Attackers", DisplayName = "Max Active Attackers", meta = ( ClampMin = "1" ) )
	int m_iMaxActiveAttackers;
	int m_iCurrentActiveAttackers;

	// List of idle Enforcers
	TArray<AAIEnforcer*> m_apIdleEnforcers;
	// List of idle Turrets
	TArray<AAITurret*> m_apIdleTurrets;

	// Pointer to player, this is needed to find the closest enforcers
	AFPCharacter* m_pPlayerCharacter;

	AAIEnforcer* FindClosestAvailableEnforcer() const;
	AAITurret* FindClosestAvailableTurret( bool bNeedsToBeInRange ) const;

	float GetEnemyToPlayerDistanceSquared( AAIBase* pEnemy ) const;
};