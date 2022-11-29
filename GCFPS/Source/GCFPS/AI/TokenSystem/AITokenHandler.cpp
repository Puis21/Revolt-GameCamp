#include "AITokenHandler.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <Components/BillboardComponent.h>

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "GCFPS/AI/Base/AIBase.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/TokenSystem/AIToken.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAITokenHandler::AAITokenHandler()
	: m_fDistanceToPlayer( 0.0f )
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Billboard Component
	m_pBillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>( TEXT( "BillBoard Component" ) );
	if (m_pBillboardComponent)
	{
		RootComponent = m_pBillboardComponent;
	}
}

// Called when the game starts or when spawned
void AAITokenHandler::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters ActorSpawnParams;
	const FRotator SpawnRotation = GetActorRotation();
	const FVector SpawnLocation = GetActorLocation();
	
	//Spawn actors and store into array
	for (int i = 0; i < NumTotalTokens; i++)
	{
		AAIToken* AttackToken = GetWorld()->SpawnActor<AAIToken>( AAIToken::StaticClass(), SpawnLocation, SpawnRotation, ActorSpawnParams );	
	
		// Set the token to be able to be used
		AttackToken->setCanBeUsed( true );
		
		// Store the token into an array 
		AttackTokens.Add( AttackToken );	
	}
}

// Called every frame
void AAITokenHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleTokenDistribution();
	
	if (AttackTokens.Num() == 0)
	{
		HandleTokenStealing();
	}

}

void AAITokenHandler::HandleTokenDistribution()
{
	// Only handle tokens if the AITokenQueue and AttackTokens is not empty
	if (AITokenQueue.Num() > 0 && AttackTokens.Num() > 0) // && GetTokensAvailable() == true )
	{
		GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Green, TEXT( "AI TOKEN QUEUE NOT EMPTY" ) );

		for (int i = 0; i < AITokenQueue.Num(); i++)	//AttackTokens.Num(); i++)
		{
			// The first enemy AI in the queue should be able to attack
			// && if there are tokens available
			if (i == 0 && GetTokensAvailable( true ) == true)
			{
				// Only give a token if it doesn't currently have one
				// This check prevents the enemy from being given more than 1 token
				if (AITokenQueue[i]->getHasAttackToken() == false)	// && AITokenQueue[i]->getCanAttack() == true)	// & can attack
				{
					AITokenQueue[i]->setHasAttackToken( true );
					AITokenQueue[i]->setWasTokenRequestSuccessful( true );
					// Change values in the behaviour tree
					AAIBaseController* pAIController = Cast<AAIBaseController>( AITokenQueue[i]->GetController() );
					if (pAIController)
					{
						pAIController->GetBlackboardComp()->SetValueAsBool( "HasAttackToken", true );
						//pAIController->GetBlackboardComp()->SetValueAsBool( "WasTokenRequestSuccessful", true );
					}

					//AITokenQueue[i]->GetTokenHolder().Add( AttackTokens[m_iAvailableTokenIndex] );
					//AttackTokens.RemoveAt( m_iAvailableTokenIndex );
					// Give the AI an Attack Token
					TransferToken( &AITokenQueue[i]->TokenHolder );

					//auto tokenHolder = AITokenQueue[i]->GetTokenHolder();
					//TransferToken( &tokenHolder );
					//if (AITokenQueue[i]->TokenHolder.Num() != 0)
					//{
					//	GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Red, TEXT( "AI Token Holder is not empty." ) );
					//
					//}

					// Set this token to be active
					AITokenQueue[i]->TokenHolder[i]->setIsActive( true );

					// Remove the AI from the queue as it now has a token
					AITokenQueue[i]->setIsInTokenQueue( false );
					AITokenQueue.RemoveAt( 0 );

				}
			}
		}
	}

	// Allow enemies to steal a token from another enemy if there are no attack tokens available
	//if (AttackTokens.Num() == 0)
	//{
	//	HandleTokenStealing();
	//}
}

void AAITokenHandler::HandleTokenStealing()
{	
	ACharacter* character = UGameplayStatics::GetPlayerCharacter( GetWorld(), 0 );
	AFPCharacter* playerCharacter = Cast<AFPCharacter>( character );

	// Check for the player
	if (playerCharacter)
	{
		//GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Red, TEXT( "Token handler knows fp character post ") );		
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass( GetWorld(), AAIBase::StaticClass(), FoundActors );

	for (auto& actor : FoundActors)
	{
		AAIBase* AIChar = Cast <AAIBase>( actor );

		if (AIChar && AIChar->GetEnemyType() != EEnemyType::Seeker)	// Seeker not part of token system
		{
			// Only steal if they don't have a token 
			// & if they can also see the player, this is so that it doesn't try to steal a token if they aren't trying to attack the player
			if (AIChar->getHasAttackToken() == false && AIChar->getCanSeePlayer() == true)
			{
				// Calculate AI distance to player to check if it can steal if it doesn't have a token yet.
				FVector AIDistanceToPlayer = playerCharacter->GetActorLocation() - AIChar->GetActorLocation();
				m_fDistanceToPlayer = AIDistanceToPlayer.Size();

				//GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Purple, FString::SanitizeFloat( m_fDistanceToPlayer ) );

				// Is the player being rendered (the player can see it) and within a close range of the player
				if ((AIChar->WasRecentlyRendered()) && (m_fDistanceToPlayer <= AllowedTokenStealingDistance))
				{
					// some debugs
					//GEngine->AddOnScreenDebugMessage( -1, GetWorld()->GetDeltaSeconds(), FColor::Blue, TEXT( "AI can steal token" ) );
					//DrawDebugSphere( GetWorld(), AIChar->GetActorLocation(), 40.0f, 10.0f, FColor::Blue, false, -1.0f, 0, 2 );

					AIChar->setCanStealToken( true );

					// Move the AI that has met the conditions of this statement to the front of the AITokenQueue
					// Store this AI in a temporary var
					//AAIBase* tempAIChar = AIChar;
					// Remove this from the queue
					//AITokenQueue.RemoveAt(i);
					// Place to the front of the queue
					//AITokenQueue.Insert( tempAIChar, 0 );

					// Steal token if in distance AND doesn't already have a token
					if (AIChar->getCanStealToken() == true)	//&& AIChar->getHasAttackToken() == false)
					{
						AIChar->StealToken();
					}
				}
			}
			else
			{
				AIChar->setCanStealToken( false );
			}
		}
	}	
}

bool AAITokenHandler::GetTokensAvailable( bool canSetIndex )
{	
	// For every token in the array
	for (int i = 0; i < AttackTokens.Num(); i++)
	{
		// Check if the attack token is not active and can be used
		if (AttackTokens[i]->getIsActive() == false && AttackTokens[i]->getCanBeUsed() == true)
		{
			// If the conditions of the token at index i are met, set this as the value so that we know which token in the array is available
			if (canSetIndex == true)
			{				
				m_iAvailableTokenIndex = i;

				//GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Cyan, TEXT( "m_iAvailoableTokenIndex is set" ) );
			}

			//GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Cyan, TEXT( "Token Available" ) );

			return true;
		}
	}

	// Retrun false if there are no tokens in the pool/ are available
	return false;
}

void AAITokenHandler::TransferToken( TArray<AAIToken*> Arr[] )
{
	// Move token into the array passed in
	Arr->Add( AttackTokens[m_iAvailableTokenIndex] );
	AttackTokens.RemoveAt(m_iAvailableTokenIndex);

}

void AAITokenHandler::AddToken( AAIToken* token )
{
	AttackTokens.Add( token );
}

// Overloaded
void AAITokenHandler::AddToTokenQueue( AAIBase* AIChar )
{
	AITokenQueue.Add( AIChar );
}

// Overloaded
void AAITokenHandler::AddToTokenQueue( AAIBase* AIChar, int index )
{
	AITokenQueue.Insert( AIChar, index );
}

void AAITokenHandler::CallTokenCooldown( float TokenCooldown )
{
	// Call cooldown of the AIToken at the end of the AttackTokens array
	AttackTokens[AttackTokens.Num() - 1]->Cooldown( TokenCooldown );

	//AttackTokens.Last()->Cooldown( TokenCooldown );
}
