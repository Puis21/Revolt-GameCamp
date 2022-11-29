// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpComponent.h"
#include "GCFPS/PickUps/KeyCardPickUp.h"
#include "GCFPS/Player/FPCharacter.h"
#include "GCFPS/PickUps/BasePickUp.h"

// Sets default values for this component's properties
UPickUpComponent::UPickUpComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPickUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPickUpComponent::OnOverLap(AActor* OtherActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("ALOO"));

	pKeyCard = Cast<AKeyCardPickUp>(OtherActor);
	pPickUp = Cast<ABasePickUp>(OtherActor);

	if (pKeyCard)
	{
		pKeyCard->SetPickUpBool(true);
		pKeyCard->Destroy();
	}

	if (pPickUp)
	{
		AFPCharacter* pPlayer = Cast<AFPCharacter>(GetOwner());
		pPickUp->Interact(pPlayer);
		//pPickUp->Destroy();
	}

}