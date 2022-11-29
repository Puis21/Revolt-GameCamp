// Fill out your copyright notice in the Description page of Project Settings.


#include "AIToken.h"

#include "DrawDebugHelpers.h"


// Sets default values
AAIToken::AAIToken()
	: m_bCanBeUsed			( false )
	, m_bIsActive			( false )
	//, m_bWasJustReleased	( false )
	, m_bIsOnCooldown		( false )
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIToken::BeginPlay()
{
	Super::BeginPlay();

	//DrawDebugCircle( GetWorld(), GetActorLocation(), 350.0f, 50, FColor( 0, 0, 0 ), true, 0.0f, 1.0f, 0.0f );
	
}

// Called every frame
void AAIToken::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// -----

void AAIToken::EndPlay( const EEndPlayReason::Type EndPlayReason )
{

	Super::EndPlay( EndPlayReason );

	// Clear timer handle
	GetWorld()->GetTimerManager().ClearTimer( AITokenCooldownHandle );

	// Clear all timers for this object
	//GetWorld()->GetTimerManager().ClearAllTimersForObject( this );
}

// -----

void AAIToken::Cooldown( float Cooldown )
{
	if (m_bIsOnCooldown == false)
	{
		m_bIsOnCooldown = true;

		//GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Red, TEXT( "Calling Token Cooldown" ) );


		GetWorld()->GetTimerManager().SetTimer( AITokenCooldownHandle, this, &AAIToken::ResetValues, Cooldown, false );
	}

}

void AAIToken::ResetValues()
{
	m_bIsOnCooldown = false;
	m_bIsActive = false;
	
	//GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Red, TEXT( "Token Cooldown Finished" ) );

}