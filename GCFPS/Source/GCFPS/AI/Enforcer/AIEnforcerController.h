// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/AI/Base/AIBaseController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIEnforcerController.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API AAIEnforcerController : public AAIBaseController
{
	GENERATED_BODY()

public:
	AAIEnforcerController();

	virtual void OnTargetDetected( AActor* actor, FAIStimulus stimulus ) override;
};
