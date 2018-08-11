// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "KrazeKartsGameMode.h"
#include "KrazeKartsPawn.h"
#include "KrazeKartsHud.h"

AKrazeKartsGameMode::AKrazeKartsGameMode()
{
	DefaultPawnClass = AKrazeKartsPawn::StaticClass();
	HUDClass = AKrazeKartsHud::StaticClass();
}
