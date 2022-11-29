// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AITurretController.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API AAITurretController : public AAIBaseController
{
	GENERATED_BODY()

protected:
	virtual void OnPossess( APawn* InPawn ) override;

public:

	AAITurretController();

	virtual void BeginPlay() override;

	virtual void Tick( float DeltaTime ) override;

};
