// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_LineBackDiagonal.h"

ABoardSpaceBase* UBoard_LineBackDiagonal::GetNextSpace()
{
	if (!NextSpace)
	{
		return NextSpace;
	}

	ABoardSpaceBase* CurrentSpace = NextSpace;
	if (NextSpace->GetRowIdx() + 1 >= Board->GetRowNum() || NextSpace->GetColumnIdx() - 1 < 0)
	{
		NextSpace = NULL;
	}
	else
	{
		NextSpace = Board->GetBoardSquare(NextSpace->GetRowIdx() + 1, NextSpace->GetColumnIdx() - 1);
	}

	return CurrentSpace;
}