// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_LineColumn.h"

ABoardSpaceBase* UBoard_LineColumn::GetNextSpace()
{
	if (!NextSpace)
	{
		return NextSpace;
	}

	ABoardSpaceBase* CurrentSpace = NextSpace;
	if (NextSpace->GetRowIdx() + 1 >= Board->GetRowNum())
	{
		NextSpace = NULL;
	}
	else
	{
		NextSpace = Board->GetBoardSquare(NextSpace->GetRowIdx() + 1, NextSpace->GetColumnIdx());
	}

	return CurrentSpace;
}