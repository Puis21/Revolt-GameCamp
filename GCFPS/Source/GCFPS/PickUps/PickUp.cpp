// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/PickUps/PickUp.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APickUp::APickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_ePickUpType = EPickUpType::E_Armour;

	ArmourIncrease = 20.0f;

}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();
	
	AFPCharacter* Character = Cast<AFPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	Character->GetComponentByClass(UPlayerRegenStatsComponent::StaticClass());
	PlayerComponent = Cast<UPlayerRegenStatsComponent>(Character->GetComponentByClass(UPlayerRegenStatsComponent::StaticClass()));
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
