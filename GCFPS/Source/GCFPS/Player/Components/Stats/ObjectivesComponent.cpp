// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectivesComponent.h"

#include "GCFPS/GCFPSHUD.h"
#include "GCFPS/Menu/HUDWidget.h"

// Sets default values for this component's properties
UObjectivesComponent::UObjectivesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ObjectiveTitle = "Title oF Objective";

	ObjectiveText = "Objetive Text";

	TimeToSetNewObj = 2.0f;
}


// Called when the game starts
void UObjectivesComponent::BeginPlay()
{
	Super::BeginPlay();


	UE_LOG(LogTemp, Warning, TEXT("Objective: %s"), *ObjectiveText);

}


// Called every frame
void UObjectivesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Cyan, ObjectiveTitle);
		//GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Cyan, ObjectiveText);
	}

}

void UObjectivesComponent::NewObjective( FString ObjTitle, FString ObjText )
{
	//ClearObjective();

	ObjectiveTitle = ObjTitle;
	ObjectiveText = ObjText;

	ClearObjective();

	GetWorld()->GetTimerManager().SetTimer( UpdateObjHUDTimerHandle, this, &UObjectivesComponent::SetNewObj, TimeToSetNewObj, false );




}

void UObjectivesComponent::ClearObjective( )
{
	AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>( GetWorld()->GetFirstPlayerController()->GetHUD() );
	HUDWidgetRef->GetHUDWidgetRef()->ClearObjectives();
}


void UObjectivesComponent::SetNewObj()
{
	AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>( GetWorld()->GetFirstPlayerController()->GetHUD() );
	HUDWidgetRef->GetHUDWidgetRef()->UpdateObjectives( GetObjTitle(), GetObjText() );
}
