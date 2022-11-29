// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBaseController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Components/Shooting/GenericAIShootingComponent.h"
#include "GCFPS/EnemyEncounters/EnemyEncounter.h"
#include "GCFPS/Player/FPCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"


AAIBaseController::AAIBaseController()
{
	// Team ID
	SetGenericTeamId( FGenericTeamId( 5 ) );

	// AI Behaviour Tree and Blackboard Component
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>( TEXT( "BehaviorTreeComponent" ) );
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>( TEXT( "BlackboardComponent" ) );

	// -----

	// Set default variables for perception
	//AISightRadius		= 700.0f;
	//AISightAge		= 5.0f;
	//AILoseSightRadius	= 1500.0f;
	//AIFieldOfView		= 90.0f;

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>( TEXT( "Sight Config" ) );
	
	// Create Perception Component
	SetPerceptionComponent( *CreateDefaultSubobject<UAIPerceptionComponent>( TEXT( "Perception Component" ) ) );
	m_pPerceptionComponent = GetPerceptionComponent();
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic( this, &AAIBaseController::OnTargetDetected );

	// -----
	// Configure Sight with specific values
	//SightConfig->SightRadius					= AISightRadius;
	//SightConfig->LoseSightRadius				= AILoseSightRadius;
	//SightConfig->PeripheralVisionAngleDegrees	= AIFieldOfView;
	//SightConfig->SetMaxAge						( AISightAge );
	////SightConfig->AutoSuccessRangeFromLastSeenLocation = 900.0f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Dominant sense is sight
	GetPerceptionComponent()->SetDominantSense( *SightConfig->GetSenseImplementation() );
	GetPerceptionComponent()->ConfigureSense( *SightConfig );
}

void AAIBaseController::OnPossess( APawn* InPawn )
{
	Super::OnPossess( InPawn );

	AAIBase* AIBaseChar = Cast<AAIBase>( InPawn );

	if (AIBaseChar)
	{
		// Initialize blackboard
		if (AIBaseChar->GetBehaviourTree()->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard( *(AIBaseChar->GetBehaviourTree()->BlackboardAsset) );
		}
		// Follow the behaviour tree
		BehaviorTreeComp->StartTree( *AIBaseChar->GetBehaviourTree() );
	}

}

ETeamAttitude::Type AAIBaseController::GetTeamAttitudeTowards( const AActor& Other ) const
{
	if (const APawn* OtherPawn = Cast<APawn>( &Other )) {

		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>( OtherPawn->GetController() ))
		{
			return Super::GetTeamAttitudeTowards( *OtherPawn->GetController() );
		}
	}

	return ETeamAttitude::Neutral;
}


FRotator AAIBaseController::GetControlRotation() const
{
	if (GetPawn() == nullptr)
	{
		return FRotator( 0.0f, 0.0f, 0.0f );
	}

	return FRotator( 0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f );
}


void AAIBaseController::OnTargetDetected( AActor* actor, FAIStimulus stimulus )
{	
	// Player is sensed (Sight)
	if ( auto PlayerChar = Cast<AFPCharacter>( actor ) )
	{
		BlackboardComp->SetValueAsBool( "PlayerSensed", stimulus.WasSuccessfullySensed() );

		BlackboardComp->SetValueAsVector( "PlayerLocation", stimulus.StimulusLocation );


		auto pAIChar = Cast<AAIBase>( GetPawn() );
		if( pAIChar )
		{
			pAIChar->GetGenericAIShootingComponent()->SetIsPlayerSensed( stimulus.WasSuccessfullySensed() );

			if( stimulus.WasSuccessfullySensed() )
			{
				pAIChar->GetGenericAIShootingComponent()->ResetAccuracy();
			}

			// AI can see player
			pAIChar->setCanSeePlayer( stimulus.WasSuccessfullySensed() );
		}
	}
}

void AAIBaseController::SetPlayerCharacter( AFPCharacter* pPlayerCharacter )
{
	BlackboardComp->SetValueAsObject( "PlayerCharacter", pPlayerCharacter );
}

AFPCharacter* AAIBaseController::GetPlayerCharacter() const
{
	AAIBase* pAIChar = Cast<AAIBase>( GetPawn() );
	if( pAIChar )
	{
		return pAIChar->GetPlayerCharacter();
	}
	return nullptr;
}
