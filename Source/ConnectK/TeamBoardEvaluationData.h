// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TeamBoardEvaluationData.generated.h"

class ABoardSpaceBase;
USTRUCT()
struct FBoardSpaceGroup
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<ABoardSpaceBase*> BoardSpaces;
};

/**
 * 
 */
UCLASS()
class CONNECTK_API UTeamBoardEvaluationData : public UObject
{
	GENERATED_BODY()
	
public:
	void Init();

	UPROPERTY()
	int UnsharedOddThreatsNum = 0;
	UPROPERTY()
	int UnsharedEvenThreatsNum = 0;

	UPROPERTY()
	int SharedOddThreatsNum = 0;
	UPROPERTY()
	int SharedEvenThreatsNum = 0;

	UPROPERTY()
	bool bHasJunction = false;
	UPROPERTY()
	bool bHasEmptyJunction = false;

	UPROPERTY()
	TArray<FBoardSpaceGroup> PotentialWinningGroups;
	UPROPERTY()
	TArray<ABoardSpaceBase*> ThreatSpaces;
};
