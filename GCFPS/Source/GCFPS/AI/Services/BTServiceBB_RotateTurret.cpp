#include "BTServiceBB_RotateTurret.h"

#include <BehaviorTree/Blackboard/BlackboardKeyType_Bool.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <Kismet/KismetMathLibrary.h>

#include "GCFPS/AI/Components/Shooting/GenericAIShootingComponent.h"
#include "GCFPS/AI/Turret/AITurret.h"
#include "GCFPS/AI/Turret/AITurretController.h"

void UBTServiceBB_RotateTurret::TickNode( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds )
{
	// Get Turret Controller
	AAITurretController* pTurretCon = Cast<AAITurretController>( OwnerComp.GetAIOwner() );
	if( pTurretCon )
	{
		// Get BB Component
		UBlackboardComponent* pBlackboardComp = pTurretCon->GetBlackboardComp();
		if( pBlackboardComp )
		{
			// Check if the key value is a boolean (it should be)
			if( BlackboardKey.SelectedKeyType == UBlackboardKeyType_Bool::StaticClass() )
			{
				// The bool should be whether or not this Turret has attack permission
				const bool kbHasAttackPermission = pBlackboardComp->GetValue<UBlackboardKeyType_Bool>( BlackboardKey.GetSelectedKeyID() );

				// Get Turret Pawn
				AAITurret* pTurret = Cast<AAITurret>( pTurretCon->GetPawn() );
				if( pTurret )
				{
					// If it has attack permission, it should rotate towards the player
					if( kbHasAttackPermission )
					{
						// Get Shooting Component
						const UGenericAIShootingComponent* pShootingComp = pTurret->GetShootingComponent();
						if( pShootingComp )
						{
							// Get Turret Location
							const FVector kv3TurretLocation = pTurret->GetActorLocation();

							// Get Target Location
							const FVector kv3TargetLocation = pShootingComp->GetTargetLocation();
				
							// Find the new target rotation 
							const FRotator krTargetRotation = UKismetMathLibrary::FindLookAtRotation( kv3TurretLocation, kv3TargetLocation );

							// Interp towards new rotation
							pTurret->InterpTurretBodyRotation( &krTargetRotation, false );
						}
					}
					// If not, it should reset to its initial rotation
					else
					{
						// Get Turret Root Rotation
						const FRotator krTurretRotation = pTurret->GetActorRotation();

						// Interp towards the initial rotation
						pTurret->InterpTurretBodyRotation( &krTurretRotation, true );
					}
				}
			}
		}
	}
}