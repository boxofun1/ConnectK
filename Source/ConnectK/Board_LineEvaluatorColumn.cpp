// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_LineEvaluatorColumn.h"
#include "Board_LineColumn.h"

REGISTER_TYPE(UBoard_LineEvaluatorColumn)

UBoard_Line* UBoard_LineEvaluatorColumn::GetNextLine()
{
	if (!NextLineStart)
	{
		return NULL;
	}

	BoardLine->Init(NextLineStart, Board);

	if (NextLineStart->GetColumnIdx() + 1 >= Board->GetColumnNum())
	{
		NextLineStart = NULL;
	}
	else
	{
		NextLineStart = Board->GetBoardSquare(0, NextLineStart->GetColumnIdx() + 1);
	}

	return BoardLine;
}

void UBoard_LineEvaluatorColumn::Init(ABoard_GravityOff* InBoard)
{
	Super::Init(InBoard);

	NextLineStart = Board->GetBoardSquare(0, 0);
}

void UBoard_LineEvaluatorColumn::CreateBoardLine()
{
	BoardLine = NewObject<UBoard_LineColumn>();
}