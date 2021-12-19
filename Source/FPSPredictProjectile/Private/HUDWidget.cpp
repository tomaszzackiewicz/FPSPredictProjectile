// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"

#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"

bool UHUDWidget::Initialize() {
	bool Success = Super::Initialize();

	if (!Success) {
		return false;
	}

	return true;
}
