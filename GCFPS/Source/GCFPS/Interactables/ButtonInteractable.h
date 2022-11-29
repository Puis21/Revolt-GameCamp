// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCFPS/Interactables/ButtonDoor.h"
#include "GameFramework/Actor.h"
#include "ButtonInteractable.generated.h"

UCLASS()
class GCFPS_API AButtonInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButtonInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Door")
		class AButtonDoor* LinkedDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* MyMesh;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radius")
		FVector VBoxRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer Starter")
	bool CanStartTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OpenDoor();

};
