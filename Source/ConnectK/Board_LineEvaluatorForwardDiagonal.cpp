// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_LineEvaluatorForwardDiagonal.h"
#include "Board_LineForwardDiagonal.h"

REGISTER_TYPE(UBoard_LineEvaluatorForwardDiagonal)

UBoard_Line* UBoard_LineEvaluatorForwardDiagonal::GetNextLine()
{
	if (!NextLineStart)
	{
		return NULL;
	}

	BoardLine->Init(NextLineStart, Board);

	if (NextLineStart->GetRowIdx() - 1 >= 0)
	{
		NextLineStart = Board->GetBoardSquare(NextLineStart->GetRowIdx() - 1, NextLineStart->GetColumnIdx());
	}
	else if (NextLineStart->GetColumnIdx() + 1 + Board->GetK() <= Board->GetColumnNum())
	{
		NextLineStart = Board->GetBoardSquare(NextLineStart->GetRowIdx(), NextLineStart->GetColumnIdx() + 1);
	}
	else
	{
		NextLineStart = NULL;
	}

	return BoardLine;
}

void UBoard_LineEvaluatorForwardDiagonal::Init(ABoard_GravityOff* InBoard)
{
	Super::Init(InBoard);

	NextLineStart = Board->GetBoardSquare(Board->GetRowNum() - Board->GetK(), 0);
}

void UBoard_LineEvaluatorForwardDiagonal::CreateBoardLine()
{
	BoardLine = NewObject<UBoard_LineForwardDiagonal>();
}