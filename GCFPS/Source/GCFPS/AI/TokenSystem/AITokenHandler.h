// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AITokenHandler.generated.h"

class AAIBase;
class AAIToken;
class UBillboardComponent;
class UBoxComponent;

UCLASS()
class GCFPS_API AAITokenHandler : public AActor
{
	GENERATED_BODY()

private:
	//Tokens Available (for use in combat zone)
	UPROPERTY( VisibleAnywhere, Category = "Token Handler" )
		TArray<AAIToken*> AttackTokens;

	UPROPERTY( VisibleAnywhere, Category = "Token Handler" )
		TArray<AAIBase*> AITokenQueue;
		//TArray<AActor*> AITokenQueue;

protected:
	// Total tokens to put in the AttackTokens array
	// Change so that tokens are set based on difficulty that the player chooses
	UPROPERTY( EditAnywhere, Category = "Token Handler")
	int NumTotalTokens;

	// Which token in the array is available to be given to the enemy
	int m_iAvailableTokenIndex;

	UPROPERTY( EditAnywhere, Category = "Token Handler" )
	float AllowedTokenStealingDistance;

	// AI Distance to player
	// Used to see if it can steal a token
	float m_fDistanceToPlayer;

public:
	TArray<AAIToken*> GetAttackTokens() { return AttackTokens; }

	TArray<AAIBase*> GetAITokenQueue() { return AITokenQueue; }

	// -----

	int getAvailableTokenIndex()	{ return m_iAvailableTokenIndex; }

	void setAvailableTokenIndex( int AvailableTokenIndex ) { m_iAvailableTokenIndex = AvailableTokenIndex; }

	// -----

	//UPROPERTY(EditAnywhere, Category = "Token Handler")
	//	bool m_bTokensCanBeSet;

	// Check of there are any available tokens in the AttackTokens array
	// @ canSetIndex - used if wanting to get an available token and setting the index to the variable m_iAvailableTokenIndex
	//				 - set to false if just wanting to check if there is an available token or not
	//				 - useful for making ai do a non combat action if there is no token available
	bool GetTokensAvailable( bool canSetIndex );

	// Give an enemy a token
	// @ Arr[] - Is the TArray passed in where the available token will move to 
	void TransferToken(TArray<AAIToken*> Arr[]);

	// Add a token into AttackTokens array
	void AddToken( AAIToken* token );

	// Add enemy into AITokenQueue to be given a token if there are tokens 
	// Uses .Add to add the AI to the end of the array -> Use if it will be the first enemy type or the first in the array
	void AddToTokenQueue( AAIBase* AIChar );

	// Overloaded function - Uses .Insert, to add the AI into a specific part of the queue, for priority queueing
	void AddToTokenQueue( AAIBase* AIChar, int index );
	
	// Call the cooldown of the ai token at the end of the AttackTokens array,
	// this is because when the token is added back in it is added to the end of the array
	void CallTokenCooldown( float TokenCooldown );

	// Give token to AI in front of queue
	void HandleTokenDistribution();

	// Check if the AI can steal a token
	void HandleTokenStealing();

public:	
	// Sets default values for this actor's properties
	AAITokenHandler();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// This Billboard Component just adds some visual flair, makes it easy to select this actor in the level editor.
	UPROPERTY( EditDefaultsOnly, Category = "Editor Visuals", DisplayName = "Billboard Component" )
	UBillboardComponent* m_pBillboardComponent;
};
