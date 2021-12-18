// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPredictProjectileController.generated.h"

/**
 * 
 */
UCLASS()
class FPSPREDICTPROJECTILE_API AFPSPredictProjectileController : public APlayerController
{
	GENERATED_BODY()


public:

	//void GetSightRayHitLocation() const;

	FVector GetLookVectorHitLocation() const;

	bool GetLookDirection(FVector2D ScreenLocation, FVector& LookDirection) const;

	FORCEINLINE  FVector GetHitLocation() const { return HitLocation; }

private:

	UPROPERTY(EditDefaultsOnly)
	float CrosshairXLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairYLocation = 0.3333;

	UPROPERTY(EditDefaultsOnly)
	float LineTraceRange = 1000000;

	FVector HitLocation;
	
};
