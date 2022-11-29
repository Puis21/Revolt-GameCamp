// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "RecoilCameraShake.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API URecoilCameraShake : public UMatineeCameraShake
{
	GENERATED_BODY()

	URecoilCameraShake();

public:

	void SetOscillationDuration(float fOscillationDuration);

	void SetOscillationRotation(FRotator rOscillationRotation);
};
