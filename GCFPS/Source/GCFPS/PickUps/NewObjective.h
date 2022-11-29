// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/Player/Components/Stats/ObjectivesComponent.h"
#include "GameFramework/Actor.h"
#include "NewObjective.generated.h"

UCLASS()
class GCFPS_API ANewObjective : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANewObjective();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UObjectivesComponent* ObjectivesComponent;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* MyMesh;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radius")
		FVector VBoxRadius;

	UPROPERTY(EditAnywhere, Category = "Objective | Title")
		FString ObjectiveTitle;

	UPROPERTY( EditAnywhere, Category = "Objective | Text" )
		FString ObjectiveText;

	UFUNCTION()
		void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
