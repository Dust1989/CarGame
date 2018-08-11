// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "KrazeKartsHud.generated.h"


UCLASS(config = Game)
class AKrazeKartsHud : public AHUD
{
	GENERATED_BODY()

public:
	AKrazeKartsHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
