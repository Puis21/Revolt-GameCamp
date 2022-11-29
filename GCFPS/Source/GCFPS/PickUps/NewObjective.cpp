// Fill out your copyright notice in the Description page of Project Settings.


#include "NewObjective.h"
#include "DrawDebugHelpers.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANewObjective::ANewObjective()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	VBoxRadius = FVector(32.0f, 32.0f, 32.0f);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxCollision->SetBoxExtent(VBoxRadius);
	BoxCollision->SetCollisionProfileName("Trigger");
	RootComponent = BoxCollision;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MY MESH"));
	MyMesh->SetupAttachment(RootComponent);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ANewObjective::OnOverLap);
}

// Called when the game starts or when spawned
void ANewObjective::BeginPlay()
{
	Super::BeginPlay();

	AFPCharacter* Character = Cast<AFPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	Character->GetComponentByClass(UObjectivesComponent::StaticClass());
	ObjectivesComponent = Cast<UObjectivesComponent>(Character->GetComponentByClass(UObjectivesComponent::StaticClass()));

}

// Called every frame
void ANewObjective::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANewObjective::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		Destroy();

		//ObjectivesComponent->ClearObjective();

		ObjectivesComponent->NewObjective( ObjectiveTitle, ObjectiveText );


	}

}


