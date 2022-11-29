#include "TestMusic.h"
#include "Sound/SoundWave.h"

UTestMusic::UTestMusic(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<USoundWave> AmbienceMusicRef(TEXT("/Game/Levels/SoundAssets/1_Revolt_-_Background_Music"));
	if (AmbienceMusicRef.Succeeded())
	{
		ambienceSound = AmbienceMusicRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> ChaseMusicRef(TEXT("/Game/Levels/SoundAssets/3_Revolt_-_Chase_Music"));
	if (ChaseMusicRef.Succeeded())
	{
		chaseSound = ChaseMusicRef.Object;
	}
}

void UTestMusic::PlayMusic()
{
	SetSound(ambienceSound);
	Play(0.0f);
}

void UTestMusic::SwitchMusic()
{
	SetSound(chaseSound);
	Play(0.0f);
}
