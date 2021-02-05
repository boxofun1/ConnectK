// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_LineEvaluator.h"

TMap<FString, Board_LineEvaluatorFactory*> UBoard_LineEvaluator::Factories = TMap<FString, Board_LineEvaluatorFactory*>();

void UBoard_LineEvaluator::RegisterType(const FString& LineType, Board_LineEvaluatorFactory* Factory)
{
	Factories.Add(LineType, Factory);
}

UBoard_LineEvaluator* UBoard_LineEvaluator::Create(FString LineType, ABoard_GravityOff* InBoard)
{
	return Factories[LineType]->Create(InBoard);
}

void UBoard_LineEvaluator::Init(ABoard_GravityOff* InBoard)
{
	Board = InBoard;
}
