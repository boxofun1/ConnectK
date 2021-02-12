// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase_GravityOff.generated.h"

constexpr int WIN_VAL = 1000;
constexpr int LOSE_VAL = -1000;
constexpr int POSITIVE_INFINITY = 100000;
constexpr int NEGATIVE_INFINITY = -100000;
constexpr int CANCEL_VAL = -111111;

USTRUCT()
struct FBoardMove
{
	GENERATED_BODY()

	UPROPERTY()
	int Value;
	UPROPERTY()
	FIntPoint Point;

	FBoardMove(int InValue = 0, FIntPoint InPoint = FIntPoint(0.0f, 0.0f)) : Value(InValue), Point(InPoint) {}
};

USTRUCT()
struct FTeamJunctionData
{
	GENERATED_BODY()

	bool bHasJunction = false;
	bool bHasEmptyJunction = false;
};

USTRUCT()
struct FAllJunctionData
{
	GENERATED_BODY()

	FTeamJunctionData AllJunctions[2];
};

class ABoard_GravityOff;
class ABoardSpaceBase;
struct FBoardEvaluationData;
/**
 * 
 */
UCLASS(Blueprintable)
class CONNECTK_API AAIControllerBase_GravityOff : public AAIController
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable)
	void SetTeam(int InTeamIdx);

	UFUNCTION()
	void OnBoardCreated();

	UFUNCTION()
	void OnSpaceClaimed();

	void MakeNextMove();
	FAllJunctionData GetJunctionData(const FBoardEvaluationData& EvaluationData);
	void AddSpaceGroupScore(int& Score, int TeamIdx, const FBoardEvaluationData& EvaluationData);
	int GetFinalScore(int AITotalScore, int OpponentTotalScore, const FBoardEvaluationData& EvaluationData);
	void SetJunction(FTeamJunctionData& JunctionData, const ABoardSpaceBase* BoardSpace, const ABoardSpaceBase* OtherBoardSpace);
	void AddJunctionScore(const FBoardEvaluationData& EvaluationData, int& AITotalScore, int& OpponentTotalScore);
	void AddTeamJunctionScore(const FTeamJunctionData& TeamJunctionData, int& Score);
	virtual void AddGameModeScore(const FBoardEvaluationData& EvaluationData, int& AITotalScore, int& OpponentTotalScore);

	FIntPoint GetNextMove(ABoard_GravityOff* Board, float MaxTimeInSeconds, float StartTimeInSeconds);
	FBoardMove FirstMax(ABoard_GravityOff* Board, int Alpha, int Beta, int MaxSearchDepth, float MaxTimeInSeconds, float StartTimeInSeconds);
	int MaxVal(ABoard_GravityOff* Board, int Alpha, int Beta, int MaxSearchDepth, float MaxTimeInSeconds, float StartTimeInSeconds);
	int MinVal(ABoard_GravityOff* Board, int Alpha, int Beta, int MaxSearchDepth, float MaxTimeInSeconds, float StartTimeInSeconds);
	int HFunc(const FBoardEvaluationData &EvaluationData);


protected:
	virtual void BeginPlay() override;

	int TeamIdx;
	TFuture<FIntPoint> NextMove;

	FCriticalSection Mutex;
};
