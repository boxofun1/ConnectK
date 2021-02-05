// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_LineForwardDiagonal.h"

ABoardSpaceBase* UBoard_LineForwardDiagonal::GetNextSpace()
{
	if (!NextSpace)
	{
		return NextSpace;
	}

	ABoardSpaceBase* CurrentSpace = NextSpace;
	if (NextSpace->GetRowIdx() + 1 >= Board->GetRowNum() || NextSpace->GetColumnIdx() + 1 >= Board->GetColumnNum())
	{
		NextSpace = NULL;
	}
	else
	{
		NextSpace = Board->GetBoardSquare(NextSpace->GetRowIdx() + 1, NextSpace->GetColumnIdx() + 1);
	}

	return CurrentSpace;
}