// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Board_Line.h"
#include "Board_GravityOff.h"
#include "Templates/SharedPointer.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Board_LineEvaluator.generated.h"

//might have to change second create func to take board data and board so i can still have access to board K info
#define REGISTER_TYPE(LineClass) \
	class LineClass##Factory : public Board_LineEvaluatorFactory { \
	public: \
		LineClass##Factory() \
		{ \
			UBoard_LineEvaluator::RegisterType(#LineClass, this); \
		} \
		virtual void Init() { \
			if (!IsValid(LineEvaluatorSingleTon)) { \
				LineEvaluatorSingleTon = NewObject<LineClass>(); \
				LineEvaluatorSingleTon->AddToRoot(); \
				LineEvaluatorSingleTon->CreateBoardLine(); \
			} \
		} \
		virtual UBoard_LineEvaluator* Create(ABoard_GravityOff* InBoard) { \
			LineEvaluatorSingleTon->Init(InBoard); \
			return LineEvaluatorSingleTon; \
		} \
		virtual ~LineClass##Factory() \
		{ \
			LineEvaluatorSingleTon->RemoveFromRoot(); \
		} \
		UBoard_LineEvaluator* LineEvaluatorSingleTon; \
	}; \
	static LineClass##Factory global_##LineClass##Factory;


class Board_LineEvaluatorFactory
{
public:
	virtual UBoard_LineEvaluator* Create(ABoard_GravityOff* InBoard) = 0;
	virtual void Init() = 0;
};

/**
 * 
 */
UCLASS(Abstract)
class CONNECTK_API UBoard_LineEvaluator : public UObject
{
	GENERATED_BODY()
	
public:
	static void RegisterType(const FString& LineType, Board_LineEvaluatorFactory* Factory);
	static UBoard_LineEvaluator* Create(FString LineType, ABoard_GravityOff* InBoard);

	virtual void Init(ABoard_GravityOff* InBoard);

	virtual UBoard_Line* GetNextLine() PURE_VIRTUAL(UBoard_LineEvaluator::GetNextLine, return NULL;);
	virtual void CreateBoardLine() PURE_VIRTUAL(UBoard_LineEvaluator::CreateBoardLine);

	static TMap<FString, Board_LineEvaluatorFactory*> Factories;

protected:
	ABoard_GravityOff* Board;
	ABoardSpaceBase* NextLineStart;

	UPROPERTY()
	UBoard_Line* BoardLine;
};
