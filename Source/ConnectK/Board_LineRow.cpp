// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_LineRow.h"

ABoardSpaceBase* UBoard_LineRow::GetNextSpace()
{
	if (!NextSpace)
	{
		return NextSpace;
	}

	ABoardSpaceBase* CurrentSpace = NextSpace;
	if (NextSpace->GetColumnIdx() + 1 >= Board->GetColumnNum())
	{
		NextSpace = NULL;
	}
	else
	{
		NextSpace = Board->GetBoardSquare(NextSpace->GetRowIdx(), NextSpace->GetColumnIdx() + 1);
	}

	return CurrentSpace;
}