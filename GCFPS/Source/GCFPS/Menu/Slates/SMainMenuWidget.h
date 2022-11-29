// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

/**
 * 
 */
class SMainMenuWidget : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SMainMenuWidget) {}

	SLATE_ARGUMENT(TWeakObjectPtr<class ASlatesMenuHUD>, OwningHUD)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply OnPlayClicked() const;
	FReply OnQuitClicked() const;

	TWeakObjectPtr<class ASlatesMenuHUD> OwningHUD;

	virtual bool SupportsKeyboardFocus() const override { return true; };
};
