#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "GCFPS/EnemyEncounters/EnemyTypes.h"

#include "EnemySpawner.generated.h"

class AAIBase;
class AEnemyEncounter;
class UArrowComponent;
class UBillboardComponent;
class USceneComponent;

UCLASS()
class GCFPS_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

	EEnemyType GetEnemyType() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Editor Visuals", DisplayName = "Scene Component")
	USceneComponent* m_pSceneComponent;

	// This Billboard Component just adds some visual flair, makes it easy to select this actor in the level editor.
	UPROPERTY( EditDefaultsOnly, Category = "Editor Visuals", DisplayName = "Billboard Component" )
	UBillboardComponent* m_pBillboardComponent;

	// Arrow showing where the enemy will be facing
	UPROPERTY( EditDefaultsOnly, Category = "Editor Visuals", DisplayName = "Arrow Component" )
	UArrowComponent* m_pArrowComponent;

protected:
	// This is just to more easily access what type of enemy we will be spawning, through an enum
	UPROPERTY( VisibleAnywhere, Category = "Enemy Type", DisplayName = "Enemy Type" )
	EEnemyType m_eEnemyType;
};