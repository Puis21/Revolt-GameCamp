// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyCardPickUp.h"
#include "DrawDebugHelpers.h"
//#include "TestProj2/TestProj2Character.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"

// Sets default values
AKeyCardPickUp::AKeyCardPickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxCollision->SetBoxExtent(m_VBoxRadius);
	BoxCollision->SetCollisionProfileName("Trigger");
	RootComponent = BoxCollision;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MY MESH"));
	MyMesh->SetupAttachment(RootComponent);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AKeyCardPickUp::OnOverLap);
}

// Called when the game starts or when spawned
void AKeyCardPickUp::BeginPlay()
{
	Super::BeginPlay();

	//ATestProj2Character* Character = Cast<ATestProj2Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//Character->GetComponentByClass(UPickUpComponent::StaticClass());
	//PickUpComponent = Cast<UPickUpComponent>(Character->GetComponentByClass(UPickUpComponent::StaticClass()));

}

// Called every frame
void AKeyCardPickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("BOOL: %s"), PickedUp ? TEXT("true") : TEXT("false"));
}

void AKeyCardPickUp::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//Destroy();
		//MyMesh->Deactivate();
		//BoxCollision->Deactivate();
		//MyMesh->SetVisibility(false);
		MyMesh->SetStaticMesh(nullptr);
		BoxCollision->SetBoxExtent(FVector(0.f, 0.f, 0.f));
	}
}
