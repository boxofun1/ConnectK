// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Board_LineEvaluator.h"
#include "Board_LineEvaluatorForwardDiagonal.generated.h"

/**
 * 
 */
UCLASS()
class CONNECTK_API UBoard_LineEvaluatorForwardDiagonal : public UBoard_LineEvaluator
{
	GENERATED_BODY()
	
public:
	virtual UBoard_Line* GetNextLine();
	virtual void Init(ABoard_GravityOff* InBoard);
	virtual void CreateBoardLine();
};
