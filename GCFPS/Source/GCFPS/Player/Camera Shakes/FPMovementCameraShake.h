// ---------------------------------------------------- //
//	Gamer Camp 2020-21 -> Revolt						//
// ---------------------------------------------------- //
//	Contributors:										//
//		Henrique Teixeira								//
// ---------------------------------------------------- //
// This is an extremely simple camera shake that plays when player starts sprinting


#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "FPMovementCameraShake.generated.h"

UCLASS()
class GCFPS_API UFPMovementCameraShake : public UMatineeCameraShake
{
	GENERATED_BODY()
public:
	UFPMovementCameraShake();
};
