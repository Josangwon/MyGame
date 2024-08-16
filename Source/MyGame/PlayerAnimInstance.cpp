// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM(TEXT("/Script/Engine.AnimMontage'/Game/ParagonTwinblast/Characters/Heroes/TwinBlast/Animations/Primary_Fire_Med_A_Montage.Primary_Fire_Med_A_Montage'"));
	
	if (AM.Succeeded())
	{
		FireMontage = AM.Object;
	}
}

void UPlayerAnimInstance::PlayMontage()
{
	if (IsValid(FireMontage))
	{
		if (!Montage_IsPlaying(FireMontage))
		{
			Montage_Play(FireMontage);
		}
	}
}
