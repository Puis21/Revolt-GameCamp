#pragma once

#include "CoreMinimal.h"

template< typename InElementType >
class Node
{
private:
	InElementType* m_pNext;
	InElementType* m_pPrevious;
	bool m_bIsActive;
public:
	
	// Constructor
	Node()
		: m_pNext( nullptr )
		, m_pPrevious( nullptr )
		, m_bIsActive( false )
	{}

	// Destructor
	virtual ~Node()
	{}
	
	// Override = operator
	Node& operator= ( const Node& other )
	{
		m_pNext = other.m_pNext;
		m_pPrevious = other.m_pPrevious;
		m_bIsActive = other.m_bIsActive;
		return  *this;
	}

	// Getters
	InElementType* GetNextNode() const
	{
		return m_pNext;
	}
	InElementType* GetPreviousNode() const
	{
		return m_pPrevious;
	}
	bool GetIsNodeActive()const
	{
		return m_bIsActive;
	}

	// Setters
	void SetNextNode( InElementType* pNext )
	{
		m_pNext = pNext;
	}
	void SetPreviousNode( InElementType* pPrevious )
	{
		m_pPrevious = pPrevious;
	}
	void SetIsNodeActive( bool bIsActive )
	{
		m_bIsActive = bIsActive;
	}
};