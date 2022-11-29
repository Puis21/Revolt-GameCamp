// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Interactables/ButtonInteractable.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/GCFPSHUD.h"

// Sets default values
AButtonInteractable::AButtonInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxCollision->SetBoxExtent(VBoxRadius);
	BoxCollision->SetCollisionProfileName("Trigger");
	RootComponent = BoxCollision;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));
	MyMesh->SetupAttachment(RootComponent);

	CanStartTimer = false;
}

// Called when the game starts or when spawned
void AButtonInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AButtonInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AButtonInteractable::OpenDoor()
{
	if(LinkedDoor != nullptr)
	LinkedDoor->SetBool();

	if (CanStartTimer)
	{
		AGCFPSHUD* HUDReference = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		HUDReference->StartTimer();
	}
}

