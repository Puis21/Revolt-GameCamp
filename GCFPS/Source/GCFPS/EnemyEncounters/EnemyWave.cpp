#include "EnemyWave.h"

#include <Components/BillboardComponent.h>

#include "Spawners/EnemySpawner.h"



// Sets default values
AEnemyWave::AEnemyWave()
{
	m_pBillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>( TEXT("BillBoard Component") );
	if ( m_pBillboardComponent )
	{
		RootComponent = m_pBillboardComponent;
	}
}

int AEnemyWave::GetTriggerCondition() const
{
	return m_iWaveTriggerCondition;
}

int AEnemyWave::GetEnforcerSpawnCount() const
{
	return m_aEnforcerSpawners.Num();
}

int AEnemyWave::GetSeekerSpawnCount() const
{
	return m_aSeekerSpawners.Num();
}

int AEnemyWave::GetTurretSpawnCount() const
{
	return m_aTurretSpawners.Num();
}

const TArray<AEnforcerSpawner*>* AEnemyWave::GetEnforcerSpawnerArray() const
{
	return &m_aEnforcerSpawners;
}

const TArray<ASeekerSpawner*>* AEnemyWave::GetSeekerSpawnerArray() const
{
	return &m_aSeekerSpawners;
}

const TArray<ATurretSpawner*>* AEnemyWave::GetTurretSpawnerArray() const
{
	return &m_aTurretSpawners;
}