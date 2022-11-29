// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickUpComponent.generated.h"

class AKeyCardPickUp;
class ABasePickUp;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UPickUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPickUpComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

//	UPROPERTY(VisibleAnywhere)
	//	bool KeyCardPicked;

	UPROPERTY(VisibleAnywhere)
		AKeyCardPickUp* pKeyCard;

	UPROPERTY(VisibleAnywhere)
		ABasePickUp* pPickUp;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnOverLap(AActor* OtherActor);
		
};
