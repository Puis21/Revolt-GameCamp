// Fill out your copyright notice in the Description page of Project Settings.


#include "VADialogueTrigger.h"

#include "Components/BoxComponent.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"
#include "GCFPS/Player/FPCharacter.h"

// Sets default values
AVADialogueTrigger::AVADialogueTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>( TEXT( "Box Collision" ) );
	BoxCollision->SetupAttachment( RootComponent );
	BoxCollision->SetVisibility( true );	// see in game
	BoxCollision->SetHiddenInGame( false ); // see in game

	BoxCollision->OnComponentBeginOverlap.AddDynamic( this, &AVADialogueTrigger::OnOverlapBegin );

}

// Called when the game starts or when spawned
void AVADialogueTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	DisableBoxCollision();
}

// Called every frame
void AVADialogueTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// -----

void AVADialogueTrigger::OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult )
{
	if ((HitActor != nullptr) && (HitActor != this) && (OtherComponent != nullptr))
	{
		AFPCharacter* PlayerChar = Cast<AFPCharacter>( HitActor );

		if (PlayerChar)
		{
			DisableBoxCollision();

			// Voice Acting dialogue for Encounter Halfway
			// Get Game Instance
			//UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>( GetGameInstance() );
			//// Play Encounter Middle VA
			//if (pGameInstance)
			//{
			//	pGameInstance->PlayVAEncounterResistance();
			//}

		}
	}
}

void AVADialogueTrigger::EnableBoxCollision()
{
	BoxCollision->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
	BoxCollision->SetHiddenInGame( false );	// for testing to see in game
}

void AVADialogueTrigger::DisableBoxCollision()
{
	BoxCollision->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	BoxCollision->SetHiddenInGame( true ); // for testing to see in game
}