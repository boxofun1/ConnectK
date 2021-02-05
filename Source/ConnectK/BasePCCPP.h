// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePCCPP.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CONNECTK_API ABasePCCPP : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void PostSeamlessTravel();

	UFUNCTION(BlueprintNativeEvent)
	void BlueprintPostSeamlessTravel();
};
