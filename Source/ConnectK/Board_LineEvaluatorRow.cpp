// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_LineEvaluatorRow.h"
#include "Board_LineRow.h"

REGISTER_TYPE(UBoard_LineEvaluatorRow)

UBoard_Line* UBoard_LineEvaluatorRow::GetNextLine()
{
	if (!NextLineStart)
	{
		return NULL;
	}

	BoardLine->Init(NextLineStart, Board);

	if (NextLineStart->GetRowIdx() + 1 >= Board->GetRowNum())
	{
		NextLineStart = NULL;
	}
	else
	{
		NextLineStart = Board->GetBoardSquare(NextLineStart->GetRowIdx() + 1, 0);
	}

	return BoardLine;
}

void UBoard_LineEvaluatorRow::Init(ABoard_GravityOff* InBoard)
{
	Super::Init(InBoard);

	NextLineStart = Board->GetBoardSquare(0, 0);
}

void UBoard_LineEvaluatorRow::CreateBoardLine()
{
	BoardLine = NewObject<UBoard_LineRow>();
}