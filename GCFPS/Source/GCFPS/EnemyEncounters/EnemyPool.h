#pragma once
#include <Containers/Array.h>

#include <Containers/UnrealString.h>
#include "CoreMinimal.h"
#include <Engine/Engine.h>

#include "GCFPS/Pooling/Pool.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "GCFPS/AI/Turret/AITurret.h"
#include "GCFPS/Player/FPCharacter.h"

template <typename InElementType>
class EnemyPool : public PoolTemplate< InElementType >
{
public:
	EnemyPool();
	//void PopulateEnemyPool( const TArray<InElementType*>& rNodeArray );
	InElementType* GetEnemyFromInactive();

	void SetPlayerPointer( AFPCharacter* pPlayerCharacter );

	TArray<InElementType*> GetActiveEnemiesArray() const;
};

template <typename InElementType>
TArray<InElementType*> EnemyPool<InElementType>::GetActiveEnemiesArray() const
{
	// Declare TArray for holding active enemies
	TArray<InElementType*> apActiveEnemies;
	// Allocate space for the number of active enemies
	const int kiNumActive = PoolTemplate::GetNumActive();
	apActiveEnemies.Reserve( kiNumActive );

	// Get Active Head
	InElementType* pNode = static_cast<InElementType*>( GetActiveHead() );

	// Loop through all active enemy nodes to add them to the array
	for( int iActiveEnemyIndex = 0; iActiveEnemyIndex < kiNumActive; iActiveEnemyIndex++ )
	{
		// Get next node
		pNode = static_cast<InElementType*>( pNode->GetNextNode() );
		// Add to array
		apActiveEnemies.Add( pNode );
	}

	return apActiveEnemies;
}

template <typename InElementType>
void EnemyPool<InElementType>::SetPlayerPointer( AFPCharacter* pPlayerCharacter )
{
	// This function assumes all the nodes are inactive

	// Get MaxNodes
	int iNumNodes = PoolTemplate::GetMaxNodes();

	// Get Inactive list head
	// We'll keep incrementing this pointer to get to the next node, and loop through all
	InElementType* pNode = PoolTemplate::GetInactiveHead();

	// Loop through every inactivate node
	for( int iNodeIndex = 0; iNodeIndex < iNumNodes; iNodeIndex++ )
	{
		// Go to next node
		pNode = pNode->GetNextNode();

		// Cast this node to AAIBase
		AAIBase* pAIChar = Cast<AAIBase>( pNode );

		// Make sure both pointers are valid
		if( pAIChar && pPlayerCharacter )
		{
			pAIChar->SetPlayerCharacter( pPlayerCharacter );
		}
	}
}

template <typename InElementType>
InElementType* EnemyPool<InElementType>::GetEnemyFromInactive()
{
	// Pull an enemy from the inactive list, push it to the active list, and return it as a pointer
	return PoolTemplate::AddToActive();
}

template <typename InElementType>
EnemyPool<InElementType>::EnemyPool()
	: PoolTemplate()
{
}
//
//template <typename InElementType>
//void EnemyPool<InElementType>::PopulateEnemyPool( const TArray<InElementType*>& rNodeArray )
//{
//	
//	int iEnemyCount = 0;
//	for( auto It = rNodeArray.CreateConstIterator(); It; ++It )
//	{
//		InElementType* pEnemy = *It;
//		PoolTemplate::Push( pEnemy );
//		iEnemyCount++;
//	}
//
//	GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Green, FString::SanitizeFloat( iEnemyCount ) );
//}