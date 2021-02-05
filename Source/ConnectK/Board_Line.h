// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BoardSpaceBase.h"
#include "Board_GravityOff.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Board_Line.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CONNECTK_API UBoard_Line : public UObject
{
	GENERATED_BODY()
	
public:
	virtual ABoardSpaceBase* GetNextSpace() PURE_VIRTUAL(UBoard_Line::GetNextSpace, return NULL;);
	void Init(ABoardSpaceBase* InNextSpace, ABoard_GravityOff* InBoard);

protected:
	ABoardSpaceBase* NextSpace;
	ABoard_GravityOff* Board;

};
