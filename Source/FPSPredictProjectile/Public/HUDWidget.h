// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPSPREDICTPROJECTILE_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUDWidget")
	UTexture2D* ArrowTextures;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUDWidget")
	UTexture2D* ArrowNamesTextures;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "HUDWidget", meta = (BindWidget))
	class UTextBlock* ArrowAmountText;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "HUDWidget", meta = (BindWidget))
	class UImage* ArrowNameImage;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "HUDWidget", meta = (BindWidget))
	class UImage* ArrowItemImage;
	
};
