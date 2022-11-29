// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "AISeekerController.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API AAISeekerController : public AAIBaseController
{
	GENERATED_BODY()


protected:
	
	virtual void OnPossess(APawn* InPawn) override;
	
public:

	AAISeekerController();

	virtual  void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
};
