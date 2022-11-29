#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIBaseController.generated.h"

class AAIBase;
class AFPCharacter;

UCLASS()
class GCFPS_API AAIBaseController : public AAIController
{
	GENERATED_BODY()
public:
	AAIBaseController();

private:
	// Behaviour tree component
	class UBehaviorTreeComponent* BehaviorTreeComp;

	// Blackboard component
	class UBlackboardComponent* BlackboardComp;

protected:
	virtual void OnPossess( APawn* InPawn ) override;

	//UPROPERTY( VisibleAnywhere, Category = "AI | Sight" )
		class UAISenseConfig_Sight* SightConfig;

	//UPROPERTY( EditAnywhere, Category = "AI | Sight" )
	//	float AISightRadius;								   
	//														   
	//UPROPERTY( EditAnywhere, Category = "AI | Sight" )
	//	float AISightAge;									   
	//														   
	//UPROPERTY( EditAnywhere, Category = "AI | Sight" )
	//	float AILoseSightRadius;							  
	//														  
	//UPROPERTY( EditAnywhere, Category = "AI | Sight" )
	//	float AIFieldOfView;


	UPROPERTY( EditAnywhere, Category = "Perception" )
	UAIPerceptionComponent* m_pPerceptionComponent;

public:
	virtual FRotator GetControlRotation() const override;

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }

	// To avoid fighting against others of the same enemy type
	ETeamAttitude::Type GetTeamAttitudeTowards( const AActor& Other ) const override;

	UFUNCTION()
		virtual void OnTargetDetected( AActor* actor, FAIStimulus stimulus );

	AFPCharacter* GetPlayerCharacter() const;

	void SetPlayerCharacter( AFPCharacter* pPlayerCharacter );

};