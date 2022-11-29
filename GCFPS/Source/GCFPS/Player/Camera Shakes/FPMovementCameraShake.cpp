// ---------------------------------------------------- //
//	Gamer Camp 2020-21 -> Revolt						//
// ---------------------------------------------------- //
//	Contributors:										//
//		Henrique Teixeira								//
// ---------------------------------------------------- //
// This is an extremely simple camera shake that plays when player starts sprinting

#include "FPMovementCameraShake.h"

UFPMovementCameraShake::UFPMovementCameraShake()
{
	// Infinite Oscillation Duration
	OscillationDuration = -1.0f;
	OscillationBlendInTime = 1.0f;
	OscillationBlendOutTime = 0.3f;

	RotOscillation.Pitch.Amplitude = 5.0f;
	RotOscillation.Pitch.Frequency = 3.0f;
}