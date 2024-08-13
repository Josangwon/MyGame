// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "TwinBlastPlayer.h"
#include "UObject/ConstructorHelpers.h"

AMyGameModeBase::AMyGameModeBase()
{
	// set default pawn class to our Blueprinted character	
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_TwinBlastPlayer"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
