// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Board_Line.h"
#include "Board_LineBackDiagonal.generated.h"

/**
 * 
 */
UCLASS()
class CONNECTK_API UBoard_LineBackDiagonal : public UBoard_Line
{
	GENERATED_BODY()

public:
	virtual ABoardSpaceBase* GetNextSpace();
};
