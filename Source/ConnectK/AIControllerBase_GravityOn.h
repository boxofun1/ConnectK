// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIControllerBase_GravityOff.h"
#include "AIControllerBase_GravityOn.generated.h"

USTRUCT()
struct FTeamThreatData
{
	GENERATED_BODY()

	int UnsharedOddThreatsNum = 0;
	int UnsharedEvenThreatsNum = 0;

	int SharedOddThreatsNum = 0;
	int SharedEvenThreatsNum = 0;
};

USTRUCT()
struct FAllThreatData
{
	GENERATED_BODY()

	FTeamThreatData AllThreats[2];
};

struct FBoardEvaluationData;
struct FSpaceGroup;
/**
 * 
 */
UCLASS(Blueprintable)
class CONNECTK_API AAIControllerBase_GravityOn : public AAIControllerBase_GravityOff
{
	GENERATED_BODY()
	
public:
	virtual void AddGameModeScore(const FBoardEvaluationData& EvaluationData, int& AITotalScore, int& OpponentTotalScore);
	FAllThreatData GetThreatData(const FBoardEvaluationData& EvaluationData);
	TArray<FSpaceGroup> GetAllThreatSpaces(const FBoardEvaluationData& EvaluationData);
	TArray<FSpaceGroup> GetAllPlayerColumnThreats(TArray<FSpaceGroup>& AllThreatSpaces, int ColumnIdx);
	TArray<ABoardSpaceBase*> GetSharedThreats(TArray<ABoardSpaceBase*>& PlayerThreats, TArray<ABoardSpaceBase*>& EnemyThreats);
	TArray<ABoardSpaceBase*> GetUnsharedThreats(TArray<ABoardSpaceBase*>& PlayerThreats, TArray<ABoardSpaceBase*>& SharedThreats);
	void IncrementRowThreat(TArray<ABoardSpaceBase*>& Threats, bool bOdd, int& ThreatsNum);
};
