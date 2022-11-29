// Fill out your copyright notice in the Description page of Project Settings.

#include "TimerObject.h"

UTimerObject::UTimerObject()
{
	timerRunning = false;
	amountOfTime = 0.0f;

	numberOfMinutes = 0;
	numberOfSeconds = 0;
}

void UTimerObject::StartTimer(int setTimerAmount)
{
	timerRunning = true;
	amountOfTime = setTimerAmount;
}

void UTimerObject::UpdateTimer(int TimePassed)
{
	if (timerRunning == true)
	{
		amountOfTime = amountOfTime - TimePassed;

		if (amountOfTime <= 0)
		{
			timerRunning = false;
		}
	}
}

int UTimerObject::GetMinutes()
{
	numberOfMinutes = amountOfTime / 60;
	return numberOfMinutes;
}

int UTimerObject::GetSeconds()
{
	numberOfSeconds = amountOfTime % 60;
	return numberOfSeconds;
}
