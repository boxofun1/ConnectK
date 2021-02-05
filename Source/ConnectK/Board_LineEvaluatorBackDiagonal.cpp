// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_LineEvaluatorBackDiagonal.h"
#include "Board_LineBackDiagonal.h"

REGISTER_TYPE(UBoard_LineEvaluatorBackDiagonal)

UBoard_Line* UBoard_LineEvaluatorBackDiagonal::GetNextLine()
{
	if (!NextLineStart)
	{
		return NULL;
	}

	BoardLine->Init(NextLineStart, Board);

	if (NextLineStart->GetColumnIdx() + 1 < Board->GetColumnNum())
	{
		NextLineStart = Board->GetBoardSquare(NextLineStart->GetRowIdx(), NextLineStart->GetColumnIdx() + 1);
	}
	else if (NextLineStart->GetRowIdx() + 1 + Board->GetK() <= Board->GetRowNum())
	{
		NextLineStart = Board->GetBoardSquare(NextLineStart->GetRowIdx() + 1, NextLineStart->GetColumnIdx());
	}
	else
	{
		NextLineStart = NULL;
	}

	return BoardLine;
}

void UBoard_LineEvaluatorBackDiagonal::Init(ABoard_GravityOff* InBoard)
{
	Super::Init(InBoard);

	NextLineStart = Board->GetBoardSquare(0, Board->GetK() - 1);
}

void UBoard_LineEvaluatorBackDiagonal::CreateBoardLine()
{
	BoardLine = NewObject<UBoard_LineBackDiagonal>();
}
