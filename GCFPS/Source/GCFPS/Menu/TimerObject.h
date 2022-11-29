// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TimerObject.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API UTimerObject : public UObject
{
	GENERATED_BODY()

	UTimerObject();

private:
	bool timerRunning;

	UPROPERTY(EditAnywhere)
	int amountOfTime;

	int numberOfMinutes;
	int numberOfSeconds;

public:
	void StartTimer(int setTimerAmount);
	void UpdateTimer(int TimePassed);

	int GetMinutes();
	int GetSeconds();
};
