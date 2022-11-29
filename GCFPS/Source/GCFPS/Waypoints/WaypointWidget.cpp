// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointWidget.h"

#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"


void UWaypointWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* pPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (pPlayerController)
	{
		m_pPlayerController = pPlayerController;
	}
}

void UWaypointWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (WaypointIcon != nullptr)
	{
		if (m_pPlayerController != nullptr)
		{
			FVector2D v2WaypointTranslation = FVector2D::ZeroVector;
			//convert the world location of the waypoint into screen space
			m_pPlayerController->ProjectWorldLocationToScreen(m_vWaypointLocation, v2WaypointTranslation, false);

			// Non scaled Viewport size of the game
			// The Viewport size is needed to check whether the Waypoint Icon is inside the visible space on the screen or not
			// and to limit the waypoint icon's max translation to the viewport size. The Minimum will be 0,0 and the max will 
			// be the x and y components of the ViewportSize, after applying the scale.
			FVector2D v2ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();

			if (GEngine && GEngine->GameViewport)
			{
				// To have the Waypoint Icon in the correct location no matter what size the viewport is in, the Waypoint Icon
				// Translation and ViewportSize need to be scaled accordingly.
				const float kfViewportScale = UWidgetLayoutLibrary::GetViewportScale(GEngine->GameViewport);
				v2WaypointTranslation /= kfViewportScale;
				v2ViewportSize /= kfViewportScale;
			}

			const float kfMinimumWaypointIconTranslation = 0.f;

			// The Waypoint Icon needs to stay within the bounds 0,0 (Top left corner of the viewport) and the scaled viewport
			// size (Bottom Right corner of the viewport).
			// If the WaypointIcon's Translation either exceeds or subceeds the bounds, then set the Translation of the Waypoint
			// Icon to be either the min or max of the bounds accordingly.
			if (v2WaypointTranslation.X >= v2ViewportSize.X)
			{
				v2WaypointTranslation.X = v2ViewportSize.X;
			}
			else if (v2WaypointTranslation.X <= kfMinimumWaypointIconTranslation)
			{
				v2WaypointTranslation.X = kfMinimumWaypointIconTranslation;
			}

			if (v2WaypointTranslation.Y >= v2ViewportSize.Y)
			{
				v2WaypointTranslation.Y = v2ViewportSize.Y;
			}
			else if (v2WaypointTranslation.Y <= kfMinimumWaypointIconTranslation)
			{
				v2WaypointTranslation.Y = kfMinimumWaypointIconTranslation;
			}

			// Places the Waypoint Icon in the specified location in screen space
			WaypointIcon->SetRenderTranslation(v2WaypointTranslation);
		}
	}
}
