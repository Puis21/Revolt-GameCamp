// Fill out your copyright notice in the Description page of Project Settings.


#include "AITurretController.h"

AAITurretController::AAITurretController()
{
	PrimaryActorTick.bCanEverTick = true;

}


void AAITurretController::BeginPlay()
{
	Super::BeginPlay();

}

void AAITurretController::OnPossess( APawn* InPawn )
{
	Super::OnPossess( InPawn );

}

void AAITurretController::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}
