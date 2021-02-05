// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIControllerBase_GravityOff.h"
#include "AIControllerBase_GravityOn.generated.h"

class ABoard_GravityOff;
/**
 * 
 */
UCLASS(Blueprintable)
class CONNECTK_API AAIControllerBase_GravityOn : public AAIControllerBase_GravityOff
{
	GENERATED_BODY()
	
public:
	virtual int HFunc(ABoard_GravityOff* Board);
};
