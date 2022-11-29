// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Interactables/ProximityDoor.h"
#include "Components/TimelineComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProximityDoor::AProximityDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CanMove = false;
	CanBack = false;

	VBoxRadius = FVector(32.0f, 32.0f, 32.0f);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Comp"));
	RootComponent = SceneComponent;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxCollision->SetBoxExtent(VBoxRadius);
	BoxCollision->SetCollisionProfileName("Trigger");
	BoxCollision->SetupAttachment(RootComponent);
	//RootComponent = BoxCollision;

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AProximityDoor::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AProximityDoor::OnOverlapEnd);

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MY MESH"));
	MyMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AProximityDoor::BeginPlay()
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
void AProximityDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMove || CanBack)
	{
		CurveTimeline.TickTimeline(DeltaTime);
	}
}

void AProximityDoor::TimelineProgress(float value)
{
	FVector NewLocation = FMath::Lerp(StartLoc, EndLoc, value);
	MyMesh->SetRelativeLocation(NewLocation);
}

void AProximityDoor::SetBool()
{
	if (!CanMove)
	{
		CanMove = true;
	}

	if (CanBack)
	{
		CanBack = false;
	}

	CurveTimeline.Play();
}

void AProximityDoor::ReverseAnim()
{
	if (CanMove)
	{
		CanMove = false;
	}

	if (!CanBack)
	{
		CanBack = true;
	}

	CurveTimeline.Reverse();
}

void AProximityDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//UE_LOG(LogTemp, Warning, TEXT("ProximityCheck"));
		//Destroy();
		SetBool();
	}
}

void AProximityDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//UE_LOG(LogTemp, Warning, TEXT("I'm OUT!"));
		ReverseAnim();
	}
}
