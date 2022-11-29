// Fill out your copyright notice in the Description page of Project Settings.


#include "AISeekerController.h"


AAISeekerController::AAISeekerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAISeekerController::BeginPlay()
{
	Super::BeginPlay();
}

void AAISeekerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AAISeekerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}





