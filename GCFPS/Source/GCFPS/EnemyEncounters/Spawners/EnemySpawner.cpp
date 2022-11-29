#include "EnemySpawner.h"

#include <Components/ArrowComponent.h>
#include <Components/BillboardComponent.h>
#include "Components/SceneComponent.h"

#include "GCFPS/AI/Base/AIBase.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	m_pSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	if( m_pSceneComponent )
	{
		RootComponent = m_pSceneComponent;
	}

	// Create Billboard Component
	m_pBillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>( TEXT( "BillBoard Component" ) );
	if( m_pBillboardComponent )
	{
		m_pBillboardComponent->SetupAttachment(m_pSceneComponent);
	}
	// Create Arrow Component
	m_pArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>( TEXT( "Arrow Component" ) );
	if( m_pArrowComponent )
	{
		m_pArrowComponent->ArrowColor = FColor( 255, 0, 0 );
		m_pArrowComponent->bTreatAsASprite = true;
		m_pArrowComponent->bIsScreenSizeScaled = true;
		m_pArrowComponent->ArrowSize = 2.0f;
		m_pArrowComponent->ArrowLength = 55.0f;
		m_pArrowComponent->SetupAttachment( m_pBillboardComponent );
	}
}

EEnemyType AEnemySpawner::GetEnemyType() const
{
	return m_eEnemyType;
}