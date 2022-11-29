#include "CheatComponent.h"

#include <Misc/Char.h>

#include "GCFPS/Player/Components/Movement/FPCharacterMovementComponent.h"
#include "GCFPS/Player/Components/Stats/PlayerRegenStatsComponent.h"
#include "GCFPS/Player/FPCharacter.h"

// Sets default values for this component's properties
UCheatComponent::UCheatComponent()

{
	PrimaryComponentTick.bCanEverTick = false;
	m_bDebugMode = false;


	// Setup Cheat Codes
	// Become Unkillable
	m_sGodModeString = FString( "GODMODE" );

	// Turn the parkour up to 11
	m_sPizzaModeString = FString( "HENRIQUELOVESPIZZA" );

	// Guns / Infinite Ammo ( Not Implemented / Doesn't do anything )
	m_sGunslingerString = FString( "PEWPEW" );

	// Konami Code activates all cheats at once
	// ( a bit more complicated because it uses arrow keys )
	m_sKonamiCode = FString( "UPUPDOWNDOWNLEFTRIGHTLEFTRIGHTBA" );
}

void UCheatComponent::OnKeyPressed( FKey sKey )
{
	FString sKeyName = sKey.GetFName().ToString();
	if( !sKeyName.Contains( "Mouse" ) )
	{
		for( int iCharIndex = 0; iCharIndex < sKeyName.Len(); iCharIndex++ )
		{
			// Get TCHAR
			TCHAR cCurrentChar = sKeyName[iCharIndex];
			// Convert into FString
			FString sUsableChar = FString( "" ) + cCurrentChar;
			// Force uppercase
			sUsableChar.ToUpperInline();
			// Update TCHAR to be uppercase
			cCurrentChar = sUsableChar[0];

			// Debug print the TCHAR
			if( m_bDebugMode )
			{
				GEngine->AddOnScreenDebugMessage( -1, 1.0f, FColor::Green, sUsableChar );
			}

			// If not reading anything at the moment
			if( m_iIndex == 0 )
			{
				ReadFirstInput( cCurrentChar );
			}
			// If currently reading a code
			else
			{
				// Get Correct Cheat Code
				FString sCurrentCheat;
				switch( m_eCheat )
				{
					case ECheat::GodMode:
					{
						sCurrentCheat = m_sGodModeString;
					}
					break;
					case ECheat::PizzaMode:
					{
						sCurrentCheat = m_sPizzaModeString;
					}
					break;
					case ECheat::Gunslinger:
					{
						sCurrentCheat = m_sGunslingerString;
					}
					break;
					case ECheat::Konami:
					{
						sCurrentCheat = m_sKonamiCode;
					}
					break;
				}
				// If our index counter is still within boundaries
				if( m_iIndex < sCurrentCheat.Len() )
				{
					// If current char is the expected char on this cheat code
					if( cCurrentChar == sCurrentCheat[m_iIndex] )
					{
						// Increment char index
						m_iIndex++;
						// If char index reaches the length of the cheat code, means cheat code complete
						if( m_iIndex == sCurrentCheat.Len() )
						{
							// Activate the current cheat code
							ActivateCurrentCheat();
						}
					}
					// If wrong or unexpected char
					else
					{
						// Reset State
						ResetCheatState();
						// Update State so it's reading this as first char
						ReadFirstInput( cCurrentChar );
					}
				}
				// If somehow the index counter broke, just reset cheat state
				// (this shouldn't ever happen, just defensive programming)
				else
				{
					ResetCheatState();
				}
			}
		}
	}
}

void UCheatComponent::BeginPlay()
{
	Super::BeginPlay();

	m_pPlayerCharacter = Cast<AFPCharacter>( GetOwner() );


	// Bind Input Event
	//AFPCharacter* pPlayer = Cast<AFPCharacter>( GetOwner() );
	//if( pPlayer )
	//{
	//	//pPlayer->InputComponent->BindAction( "AnyKey", IE_Pressed, this, &UCheatComponent::OnKeyPressed );
	//}
}

void UCheatComponent::ActivateGodMode()
{
	if( m_bDebugMode )
	{
		GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Red, m_sGodModeString + " GodMode Code Activated" );
	}
	m_pPlayerCharacter->GetRegenComponent()->TriggerGodMode();
}

void UCheatComponent::ActivateGunslinger()
{
	if( m_bDebugMode )
	{
		GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Red, m_sGunslingerString + " Gunslinger Code Activated" );
	}
}

void UCheatComponent::ActivatePizzaMode()
{
	if( m_bDebugMode )
	{
		GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Red, m_sPizzaModeString + " Pizza Mode Code Activated" );
	}
	m_pPlayerCharacter->GetCharacterMovementComponent()->TriggerPizzaMode();
}

void UCheatComponent::ActivateKonami()
{
	if( m_bDebugMode )
	{
		GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Red, m_sKonamiCode + " Konami Code Activated" );
	}
	// Activate everything
	// Let Chaos Reign
	ActivateGodMode();
	ActivateGunslinger();
	ActivatePizzaMode();
}

void UCheatComponent::ActivateCurrentCheat()
{
	// Switch on current cheat to trigger correct function
	switch( m_eCheat )
	{
		case ECheat::GodMode:
		{
			ActivateGodMode();
		}
		break;
		case ECheat::PizzaMode:
		{
			ActivatePizzaMode();
		}
		break;
		case ECheat::Gunslinger:
		{
			ActivateGunslinger();
		}
		break;
		case ECheat::Konami:
		{
			ActivateKonami();
		}
		break;
	}

	// Finally, reset the cheat state
	ResetCheatState();
}

void UCheatComponent::ResetCheatState()
{
	m_iIndex = 0;
	m_eCheat = ECheat::Undefined;
	if( m_bDebugMode )
	{
		GEngine->AddOnScreenDebugMessage( -1, 1.0f, FColor::Red, TEXT( "Reset cheats" ) );
	}
}

void UCheatComponent::ReadFirstInput( const TCHAR cCurrentChar )
{
	// Check if this matches any of the cheat codes' starting chars
	if( cCurrentChar == m_sGodModeString[0] )
	{
		m_eCheat = ECheat::GodMode;
	}
	else if( cCurrentChar == m_sPizzaModeString[0] )
	{
		m_eCheat = ECheat::PizzaMode;
	}
	else if( cCurrentChar == m_sGunslingerString[0] )
	{
		m_eCheat = ECheat::Gunslinger;
	}
	else if( cCurrentChar == m_sKonamiCode[0] )
	{
		m_eCheat = ECheat::Konami;
	}

	// Check if successful, and if so, increment char index
	if( m_eCheat != ECheat::Undefined )
	{
		m_iIndex++;
	}
	// If current char didn't match anything, reset state
	else
	{
		ResetCheatState();
	}
}
