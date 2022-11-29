#pragma once

#include "CoreMinimal.h"

#include <Containers/Array.h>
#include <UObject/UObjectGlobals.h>
#include "Engine/World.h"

template <typename InElementType>
class Node;

template <typename InElementType>
class PoolTemplate // Template for pooling resources
{
private:
	int m_iMaxNodes;
	int m_iNumActive;
	InElementType* m_pActiveHead;
	InElementType* m_pInactiveHead;

	virtual void OnNodeAdded( InElementType* pNode ) {};
	virtual void OnNodeRemoved( InElementType* pNode ) {};	// Called just before removing Node
public:
	PoolTemplate();
	virtual ~PoolTemplate();

	InElementType* AddToActive();
	void AddToActive( InElementType* pNode );
	void RemoveFromActive( InElementType* pNode );

	void ResetPool();

	void PushToInactive( InElementType* pNewNode );
	void SetActiveHead( InElementType* pNode );

	// Getters
	int				GetMaxNodes()		const;
	int				GetNumActive()		const; // Returns num of active nodes
	InElementType*	GetActiveHead()		const;
	InElementType*	GetInactiveHead()	const;
	InElementType*	GetActiveByIndex( int iNodeIndex )const;

	void SetMaxNodes( int iMaxNodes );
};

template <typename InElementType>
void PoolTemplate<InElementType>::SetMaxNodes( int iMaxNodes )
{
	m_iMaxNodes = iMaxNodes;
}

template <typename InElementType>
void PoolTemplate<InElementType>::SetActiveHead( InElementType* pNode )
{
	m_pActiveHead = pNode;
	m_pActiveHead->SetNextNode( pNode );
	m_pActiveHead->SetPreviousNode( pNode );
}

template <typename InElementType>
PoolTemplate<InElementType>::PoolTemplate()
	: m_iMaxNodes( 0 )
	, m_iNumActive( 0 )
	, m_pActiveHead( nullptr )
	, m_pInactiveHead( nullptr )
{
}

// Destructor
template <typename InElementType>
PoolTemplate<InElementType>::~PoolTemplate()
{
}

template <typename InElementType>
void PoolTemplate<InElementType>::PushToInactive( InElementType* pNewNode )
{
	// If Inactive Head exists
	if( m_pInactiveHead )
	{
		// Get Last Node ( before we push the new element )
		InElementType* pLastNode = m_pInactiveHead->GetPreviousNode();

		// Assign new Previous and Next Nodes to new element
		pNewNode->SetNextNode( m_pInactiveHead );
		pNewNode->SetPreviousNode( pLastNode );

		// Replace the Last Node's Next Node to be the new element
		pLastNode->SetNextNode( pNewNode );
		// Set the inactive head's previous node to be the new element
		m_pInactiveHead->SetPreviousNode( pNewNode );
	}
	// If Inactive Head doesn't exist
	else
	{
		// Make the new node the Inactive Head
		m_pInactiveHead = pNewNode;
		m_pInactiveHead->SetPreviousNode( m_pInactiveHead );
		m_pInactiveHead->SetNextNode( m_pInactiveHead );
	}
}

template <typename InElementType>
InElementType* PoolTemplate<InElementType>::AddToActive()
{
	// Get next free node
	InElementType* pNode = m_pInactiveHead->GetNextNode();

	// Call OnNodeAdded()
	OnNodeAdded( pNode );

	// INACTIVE LIST
	// Erase pTempNode from the inactive list
	m_pInactiveHead->SetNextNode( pNode->GetNextNode() );
	pNode->GetNextNode()->SetPreviousNode( m_pInactiveHead );

	// ACTIVE LIST
	// Some loop logic to get the last active node
	InElementType* pLastActiveNode = m_pActiveHead->GetPreviousNode();
	// If Active list is empty, m_pActiveHead's previous link
	// will be pointing to m_pActiveHead
	pLastActiveNode->SetNextNode( pNode );
	m_pActiveHead->SetPreviousNode( pNode );
	pNode->SetPreviousNode( pLastActiveNode );
	pNode->SetNextNode( m_pActiveHead );

	// Set Node as active
	pNode->SetIsNodeActive( true );

	// Increment active count
	m_iNumActive++;

	return pNode;
}

template<typename InElementType>
inline void PoolTemplate<InElementType>::AddToActive( InElementType* pNode )
{
	// Call onAdd()
	OnNodeAdded( pNode );

	// INACTIVE LIST
	// Erase pTempNode from the inactive list
	pNode->GetPreviousNode()->SetNextNode( pNode->GetNextNode() );
	pNode->GetNextNode()->SetPreviousNode( pNode->GetPreviousNode() );

	// ACTIVE LIST
	// Some loop logic to get the last active node
	InElementType* pLastActiveNode = m_pActiveHead->GetPreviousNode();
	// If Active list is empty, m_pActiveHead's previous link
	// will be pointing to m_pActiveHead
	pLastActiveNode->SetNextNode( pNode );
	m_pActiveHead->SetPreviousNode( pNode );
	pNode->SetPreviousNode( pLastActiveNode );
	pNode->SetNextNode( m_pActiveHead );

	// Set Node as active
	pNode->SetIsNodeActive( true );

	// Increment active count
	m_iNumActive++;
}

template <typename InElementType>
void PoolTemplate<InElementType>::RemoveFromActive( InElementType* pNode )
{
	// Call OnNodeRemoved()
	OnNodeRemoved( pNode );

	// ACTIVE LIST
	// Erase bullet from the active list

	pNode->GetPreviousNode()->SetNextNode( pNode->GetNextNode() );
	pNode->GetNextNode()->SetPreviousNode( pNode->GetPreviousNode() );

	// INACTIVE LIST
	// Add bullet to the inactives
	m_pInactiveHead->GetNextNode()->SetPreviousNode( pNode );
	pNode->SetNextNode( m_pInactiveHead->GetNextNode() );
	pNode->SetPreviousNode( m_pInactiveHead );
	m_pInactiveHead->SetNextNode( pNode );

	// Set Node as inactive
	pNode->SetIsNodeActive( false );

	// Decrement active count
	m_iNumActive--;
}

template<typename InElementType>
void PoolTemplate<InElementType>::ResetPool()
{
	for( int i = 0; i < m_iNumActive; i++ )
	{
		// Get first active in list
		InElementType* pNode = m_pActiveHead->GetNextNode();

		// Call OnNodeRemoved()
		OnNodeRemoved( pNode );

		// ACTIVE LIST
		// Erase bullet from the active list
		m_pActiveHead->SetNextNode( pNode->GetNextNode() );
		pNode->GetNextNode()->SetPreviousNode( m_pActiveHead );

		// INACTIVE LIST
		// Add bullet to the inactives
		m_pInactiveHead->GetNextNode()->SetPreviousNode( pNode );
		pNode->SetNextNode( m_pInactiveHead->GetNextNode() );
		pNode->SetPreviousNode( m_pInactiveHead );
		m_pInactiveHead->SetNextNode( pNode );

		// Set Node as inactive
		pNode->SetIsNodeActive( false );
	}
	// Reset numActive
	m_iNumActive = 0;
}

template <typename InElementType>
int PoolTemplate<InElementType>::GetMaxNodes() const
{
	return m_iMaxNodes;
}

template <typename InElementType>
int PoolTemplate<InElementType>::GetNumActive() const
{
	return m_iNumActive;
}

template <typename InElementType>
InElementType* PoolTemplate<InElementType>::GetActiveHead() const
{
	return m_pActiveHead;
}

template <typename InElementType>
InElementType* PoolTemplate<InElementType>::GetInactiveHead() const
{
	return m_pInactiveHead;
}

template <typename InElementType>
InElementType* PoolTemplate<InElementType>::GetActiveByIndex( int index ) const
{
	// Get pActiveHead's next ( index is 0 )
	InElementType* pNode = static_cast<InElementType*>( m_pActiveHead->GetNextNode() );

	for( int i = 0; i < index; i++ )
	{
		pNode = static_cast<InElementType*>( pNode->GetNextNode() );
	}
	return pNode;
}