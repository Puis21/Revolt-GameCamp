#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyWave.generated.h"

class AEnforcerSpawner;
class ASeekerSpawner;
class ATurretSpawner;
class UBillboardComponent;
class UTexture2D;

UCLASS()
class GCFPS_API AEnemyWave : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyWave();

public:
	
	// Returns Trigger Condition (m_iWaveTriggerCondition)
	int GetTriggerCondition() const;

	int GetEnforcerSpawnCount() const;
	int GetSeekerSpawnCount() const;
	int GetTurretSpawnCount() const;

	const TArray<AEnforcerSpawner*>* GetEnforcerSpawnerArray() const;
	const TArray<ASeekerSpawner*>* GetSeekerSpawnerArray() const;
	const TArray<ATurretSpawner*>* GetTurretSpawnerArray() const;

private:
	UPROPERTY( VisibleDefaultsOnly, Category = "Editor Visuals", DisplayName = "Billboard Component" )
	// A UBillboardComponent to hold Icon sprite
	UBillboardComponent* m_pBillboardComponent;

	// Enforcer spawners in the level
	UPROPERTY( EditAnywhere, Category = "Spawners", DisplayName = "Enforcer Spawners" )
	TArray<AEnforcerSpawner*> m_aEnforcerSpawners;

	// Seeker spawners in the level
	UPROPERTY( EditAnywhere, Category = "Spawners", DisplayName = "Seeker Spawners" )
	TArray<ASeekerSpawner*> m_aSeekerSpawners;

	// Turret spawners in the level
	UPROPERTY( EditAnywhere, Category = "Spawners", DisplayName = "Turret Spawners" )
	TArray<ATurretSpawner*> m_aTurretSpawners;

	// If this wave is on waiting list, it will trigger once the amount of enemies in the current encounter is equal to or lower than this number.
	UPROPERTY(EditAnywhere, Category = "Trigger Conditions", DisplayName = "Wave Trigger" )
	int m_iWaveTriggerCondition;
};