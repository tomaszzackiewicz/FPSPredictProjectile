// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSPredictProjectileHUD.generated.h"

UCLASS()
class AFPSPredictProjectileHUD : public AHUD
{
	GENERATED_BODY()

public:
	AFPSPredictProjectileHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

