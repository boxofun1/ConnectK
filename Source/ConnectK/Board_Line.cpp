// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_Line.h"

void UBoard_Line::Init(ABoardSpaceBase* InNextSpace, ABoard_GravityOff* InBoard)
{
	NextSpace = InNextSpace;
	Board = InBoard;
}
