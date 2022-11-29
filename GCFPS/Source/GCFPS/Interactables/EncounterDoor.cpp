// Fill out your copyright notice in the Description page of Project Settings.


#include "EncounterDoor.h"
#include "Components/TimelineComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/Player/FPCharacter.h"

#include "GCFPS/Waypoints/Waypoint.h"

// Sets default values
AEncounterDoor::AEncounterDoor()
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

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AEncounterDoor::OnOverlapBegin);

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MY MESH"));
	MyMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEncounterDoor::BeginPlay()
{
	Super::BeginPlay();

	AFPCharacter* Character = Cast<AFPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	Character->GetComponentByClass(UObjectivesComponent::StaticClass());
	ObjectivesComponent = Cast<UObjectivesComponent>(Character->GetComponentByClass(UObjectivesComponent::StaticClass()));

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
void AEncounterDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanMove)
	{
		CurveTimeline.TickTimeline(DeltaTime);
	}
}

void AEncounterDoor::TimelineProgress(float value)
{
	FVector NewLocation = FMath::Lerp(StartLoc, EndLoc, value);
	MyMesh->SetRelativeLocation(NewLocation);
}

void AEncounterDoor::SetBool()
{
	if (!CanMove)
	{
		CanMove = true;
	}

	CurveTimeline.Play();
}

void AEncounterDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && CanMove)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ProximityCheck"));
		ObjectivesComponent->NewObjective(ObjectiveTitle, ObjectiveText);

		//ObjectiveWaypoint->IncrementWaypointLocation();
		//ObjectiveWaypoint->SetWaypointPosition();
	}
}

