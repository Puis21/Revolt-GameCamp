// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Interactables/ButtonDoor.h"
#include "Components/TimelineComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AButtonDoor::AButtonDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CanMove = false;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MY MESH"));
}

// Called when the game starts or when spawned
void AButtonDoor::BeginPlay()
{
	Super::BeginPlay();
	
	if (CurveVector)
	{
		FOnTimelineVector TimelineProgress;
		TimelineProgress.BindUFunction(this, FName("TimelineProgress"));
		CurveTimeline.AddInterpVector(CurveVector, TimelineProgress);
		CurveTimeline.SetLooping(false);

		StartLoc = EndLoc = MyMesh->GetRelativeLocation();
		EndLoc += LocOffset;

	}

}

// Called every frame
void AButtonDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMove)
	{
		CurveTimeline.TickTimeline(DeltaTime);
	}
}

void AButtonDoor::TimelineProgress(float value)
{
	FVector NewLocation = FMath::Lerp(StartLoc, EndLoc, value);
	MyMesh->SetRelativeLocation(NewLocation);
}

void AButtonDoor::SetBool()
{
	if (!CanMove)
	{
		CanMove = true;
	}

	CurveTimeline.Play();
}
