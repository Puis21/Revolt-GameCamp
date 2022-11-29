// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectivesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GCFPS_API UObjectivesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObjectivesComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EditText")
		FString ObjectiveTitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EditText")
		FString ObjectiveText;

	// -----

	FTimerHandle UpdateObjHUDTimerHandle;

	float TimeToSetNewObj;

	UFUNCTION()
		void SetNewObj();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FString GetObjTitle() { return ObjectiveTitle; }

	FString GetObjText() { return ObjectiveText; }

	UFUNCTION()
		void NewObjective(FString ObjTitle, FString ObjText);


	void ClearObjective();


};