// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BoardEvaluationData.generated.h"

class UTeamBoardEvaluationData;
/**
 * 
 */
UCLASS()
class CONNECTK_API UBoardEvaluationData : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TArray<UTeamBoardEvaluationData*> AllTeamEvaluationData;
};
