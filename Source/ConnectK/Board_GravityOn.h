// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Board_GravityOff.h"
#include "Board_GravityOn.generated.h"

/**
 * 
 */
UCLASS()
class CONNECTK_API ABoard_GravityOn : public ABoard_GravityOff
{
	GENERATED_BODY()
	
public:
	void CreateBoardSpaces();

	virtual float GetAnimFlashDuration();
	virtual void UpdateAnimData();
	virtual bool AnimShouldStop();
	virtual FBoardAnimData& InitAnimData(FBoardAnimData& AnimData, int RowIdx, int ColumnIdx, int ClaimingTeamIdx);

	virtual TArray<FIntPoint> GetPossibleFutureMoves();
};
