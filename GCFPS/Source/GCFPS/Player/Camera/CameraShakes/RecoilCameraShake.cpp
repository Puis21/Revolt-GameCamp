// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Camera/CameraShakes/RecoilCameraShake.h"

URecoilCameraShake::URecoilCameraShake()
{
	OscillationBlendInTime = 0.05f;
	OscillationBlendOutTime = 0.05f;

	OscillationDuration = 0.1f;

	RotOscillation.Pitch.Amplitude = 8.f;
	RotOscillation.Pitch.Frequency = 1.f;
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;

	RotOscillation.Yaw.Amplitude = 4.f;
	RotOscillation.Yaw.Frequency = 1.f;
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
}

void URecoilCameraShake::SetOscillationDuration(float fOscillationDuration)
{
	// set the oscillation duration based on the given value, in this case the fire rate of the equipped gun
	OscillationDuration = fOscillationDuration;
}

void URecoilCameraShake::SetOscillationRotation(FRotator rOscillationRotation)
{
	RotOscillation.Pitch.Amplitude = rOscillationRotation.Pitch;
	RotOscillation.Yaw.Amplitude = rOscillationRotation.Yaw;
}