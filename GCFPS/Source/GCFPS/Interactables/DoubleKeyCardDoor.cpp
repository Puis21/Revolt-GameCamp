// Fill out your copyright notice in the Description page of Project Settings.


#include "DoubleKeyCardDoor.h"
#include "Components/TimelineComponent.h"
#include "GCFPS/PickUps/KeyCardPickUp.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADoubleKeyCardDoor::ADoubleKeyCardDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CanMove = false;

	VBoxRadius = FVector(32.0f, 32.0f, 32.0f);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Comp"));
	RootComponent = SceneComponent;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxCollision->SetBoxExtent(VBoxRadius);
	BoxCollision->SetCollisionProfileName("Trigger");
	BoxCollision->SetupAttachment(RootComponent);
	//RootComponent = BoxCollision;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MY MESH"));
	MyMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ADoubleKeyCardDoor::BeginPlay()
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
void ADoubleKeyCardDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMove)
	{
		CurveTimeline.TickTimeline(DeltaTime);
	}

	if ( pKeyCard && pKeyCard->GetPickUpBool() && pKeyCard2 && pKeyCard2->GetPickUpBool())
	{
		//Destroy();
		SetBool();
	}

}


void ADoubleKeyCardDoor::TimelineProgress(float value)
{
	FVector NewLocation = FMath::Lerp(StartLoc, EndLoc, value);
	MyMesh->SetRelativeLocation(NewLocation);
}


void ADoubleKeyCardDoor::SetBool()
{
	if (!CanMove)
	{
		CanMove = true;
	}

	CurveTimeline.Play();
}

