// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Board_Line.h"
#include "Board_LineColumn.generated.h"

/**
 * 
 */
UCLASS()
class CONNECTK_API UBoard_LineColumn : public UBoard_Line
{
	GENERATED_BODY()

public:
	virtual ABoardSpaceBase* GetNextSpace();
};
