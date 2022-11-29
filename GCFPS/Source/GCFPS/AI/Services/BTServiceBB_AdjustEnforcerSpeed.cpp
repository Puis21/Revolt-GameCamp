#include "BTServiceBB_AdjustEnforcerSpeed.h"

#include <BehaviorTree/BlackboardComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Vector.h>
#include <GameFramework/CharacterMovementComponent.h>


#include "GCFPS/AI/Base/AIBaseController.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"

void UBTServiceBB_AdjustEnforcerSpeed::TickNode( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds )
{
	Super::TickNode( OwnerComp, NodeMemory, DeltaSeconds );

	// Get AI Con
	AAIBaseController* pAIController = Cast<AAIBaseController>( OwnerComp.GetAIOwner() );
	if( pAIController )
	{
		// Get BB Component
		UBlackboardComponent* pBlackboardComp = pAIController->GetBlackboardComp();
		if( pBlackboardComp )
		{
			// Declare Goal Location ( will be set in the next few lines )
			FVector v3GoalLocation( FVector::ZeroVector );
			// Declare a bool that will be used to check if we should proceed with the calculations
			bool bSuccessfulLocation = true;
			// We need to check whether the key that's being passed in is the PlayerCharacter itself, or a Patrol Location ( UObject or FVector )

			// Check if it's an AActor key value
			if( BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() )
			{
				UObject* pKeyValue = pBlackboardComp->GetValue<UBlackboardKeyType_Object>( BlackboardKey.GetSelectedKeyID() );
				AActor* pTargetActor = Cast<AActor>( pKeyValue );
				if( pTargetActor )
				{
					// Assign Goal Location to AActor's World Location
					v3GoalLocation = pTargetActor->GetActorLocation();
				}
			}
			// If not an AActor, check if its an FVector
			else if( BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass() )
			{
				// Assign Goal Location to FVector value
				v3GoalLocation = pBlackboardComp->GetValue<UBlackboardKeyType_Vector>( BlackboardKey.GetSelectedKeyID() );
			}
			// If not any of the 2, then this service is being used incorrectly / poorly setup in its Behaviour Tree
			else
			{
				if( GEngine )
				{
					GEngine->AddOnScreenDebugMessage( -1, 10.0f, FColor::Red, TEXT( "ERROR: Misconfigured AI Service Node BTServiceBB_AdjustEnforcerSpeed, wrong key type is being passed in as argument" ) );
					// Set further calculations flag to false
					bSuccessfulLocation = false;
				}
			}

			if( bSuccessfulLocation )
			{
				// Get Enforcer Character
				AAIEnforcer* pEnforcer = Cast<AAIEnforcer>( pAIController->GetPawn() );
				if( pEnforcer )
				{
					// Get Speed Control Float Curve
					UCurveFloat* pSpeedControlCurve = pEnforcer->GetSpeedControlCurve();
					if( pSpeedControlCurve )
					{
						// Calculate accurate distance to goal
						const float kfDistance = ( pEnforcer->GetActorLocation() - v3GoalLocation ).Size();

						// Get Enforcer's Terminal Speed
						const float kfTerminalSpeed = pEnforcer->GetTerminalSpeed();

						// Get Enforcer's Terminal Speed Distance
						const float kfTerminalSpeedDistance = pEnforcer->GetTerminalSpeedDistance();

						// Calculate percent of speed we'll be using ( clamped to max 1.0f for 100% of Terminal Speed)
						float fMoveSpeedPercent = FMath::Clamp( kfDistance / kfTerminalSpeedDistance, 0.0f, 1.0f );
						// Update percent value using the Speed Control Float Curve
						fMoveSpeedPercent = pSpeedControlCurve->GetFloatValue( 1.0f - fMoveSpeedPercent );

						// Calculate new Max Walk Speed for Enforcer
						const float kfNewWalkSpeed = kfTerminalSpeed * fMoveSpeedPercent;

						// Finally, actually update the Enforcer's MaxWalkSpeed to new value
						pEnforcer->GetCharacterMovement()->MaxWalkSpeed = kfNewWalkSpeed;

						GEngine->AddOnScreenDebugMessage( -1, 0.2f, FColor::Yellow, FString::SanitizeFloat( kfNewWalkSpeed ) );
					}
				}
			}
		}
	}
}