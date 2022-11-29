// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickUp.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/GCFPSHUD.h"
#include "GCFPS/Menu/HUDWidget.h"

// Sets default values
ABasePickUp::ABasePickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_VBoxRadius = FVector(100.f, 100.f, 50.f);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxCollision->SetBoxExtent(m_VBoxRadius);
	BoxCollision->SetCollisionProfileName("Trigger");
	RootComponent = BoxCollision;

	MyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MY MESH"));
	MyMesh->SetupAttachment(RootComponent);

	bCanPickUp = false;

	//-----
	// Set the reference to the HUD widget //
	//static ConstructorHelpers::FClassFinder<UUserWidget> HUDWidget( TEXT( "/Game/HUD/BP_HUD" ) );
	//RevoltFPSHUD = HUDWidget.Class;

	//-----
	TimeToDespawn = 10.0f;

	//-----
	m_fWeight = 0.0f;
}

void ABasePickUp::BeginPlay()
{
	Super::BeginPlay();

	if (IsPickupPlacedDown == false)
	{
		GetWorld()->GetTimerManager().SetTimer( PickUpDropTimerHandle, this, &ABasePickUp::DestroyPickup, TimeToDespawn, false );
	}
}

void ABasePickUp::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	Super::EndPlay( EndPlayReason );

	// Clear timer handle/s
	GetWorld()->GetTimerManager().ClearTimer( PickUpDropTimerHandle );

	// Clear all timers for this object
	//GetWorld()->GetTimerManager().ClearAllTimersForObject( this );
}

void ABasePickUp::DestroyPickup()
{
	Destroy();
}

void ABasePickUp::Interact(AFPCharacter* pPlayer)
{
	//AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	//HUDWidgetRef->GetHUDWidgetRef()->UpdatePickUpNotif(GetPickUpNotifText());

}
