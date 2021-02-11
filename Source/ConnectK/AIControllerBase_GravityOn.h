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
/**
 * 
 */
UCLASS(Blueprintable)
class CONNECTK_API AAIControllerBase_GravityOn : public AAIControllerBase_GravityOff
{
	GENERATED_BODY()
	
public:
	virtual int HFunc(const FBoardEvaluationData& EvaluationData);
	FAllThreatData GetThreatData(const FBoardEvaluationData& EvaluationData);
};
