// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPredictProjectileController.h"
#include "DrawDebugHelpers.h"

FVector AFPSPredictProjectileController::GetLookVectorHitLocation() const
{
	
	// Find the crosshair position in pixel coordinates
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);

	// "De-project" the screen position of the crosshair to a world direction
	FVector LookDirection;
	if (!GetLookDirection(ScreenLocation, LookDirection)) {
		return FVector(0);
	}

	// Line-trace along that LookDirection, and see what we hit (up to max range)
	
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * 10000);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility)){
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor(255, 0, 0), false, 5, 0,12.333);
		return HitResult.Location;
	}

	return FVector(0);
}

bool AFPSPredictProjectileController::GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded
	return  DeprojectScreenPositionToWorld(
		ScreenLocation.X,
		ScreenLocation.Y,
		CameraWorldLocation,
		LookDirection
	);
}