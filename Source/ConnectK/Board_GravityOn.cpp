// Fill out your copyright notice in the Description page of Project Settings.


#include "Board_GravityOn.h"

void ABoard_GravityOn::CreateBoardSpaces()
{
	Super::CreateBoardSpaces();

	FBoardRow SelectorRow;
	for (int ColumnIdx = 0; ColumnIdx < ColumnNum; ++ColumnIdx)
	{
		SelectorRow.Add(GetWorld()->SpawnActor<ABoardSpaceBase>(ColumnSelectorClass, FVector::ZeroVector, FRotator::ZeroRotator, FActorSpawnParameters()));
	}

	BoardSpaces.Add(SelectorRow);
}

float ABoard_GravityOn::GetAnimFlashDuration()
{
	return 0.05f;
}

void ABoard_GravityOn::UpdateAnimData()
{
	AnimDataQueue.Peek()->RowIdx--;
}

bool ABoard_GravityOn::AnimShouldStop()
{
	return AnimDataQueue.Peek()->BottomRowIdx == AnimDataQueue.Peek()->RowIdx;
}

FBoardAnimData& ABoard_GravityOn::InitAnimData(FBoardAnimData& AnimData, int RowIdx, int ColumnIdx, int ClaimingTeamIdx)
{
	AnimData.RowIdx = RowNum - 1;
	AnimData.ColumnIdx = ColumnIdx;
	AnimData.ClaimingTeamIdx = ClaimingTeamIdx;
	AnimData.BottomRowIdx = RowIdx;

	return AnimData;
}

TArray<FIntPoint> ABoard_GravityOn::GetPossibleFutureMoves()
{
	int ColumnPositiveIdx = ColumnNum / 2;
	int ColumnNegativeIdx = ColumnPositiveIdx - 1;
	int ColumnIdx = ColumnPositiveIdx;

	TArray<FIntPoint> PossibleFutureMoves;
	for (int ColumnNumIdx = 0; ColumnNumIdx < ColumnNum; ColumnNumIdx % 2 == 0 ? ColumnIdx = ColumnNegativeIdx-- : ColumnIdx = ++ColumnPositiveIdx, ColumnNumIdx++)
	{
		if (BoardSpaces[RowNum - 1][ColumnIdx]->ClaimedTeamIdx == -1)
		{
			for (int RowNumIdx = RowNum - 1; RowNumIdx >= 0; RowNumIdx--)
			{
				if (RowNumIdx == 0 && BoardSpaces[RowNumIdx][ColumnIdx]->ClaimedTeamIdx == -1)
				{
					PossibleFutureMoves.Add(FIntPoint(ColumnIdx, RowNumIdx));
					break;
				}

				if (BoardSpaces[RowNumIdx][ColumnIdx]->ClaimedTeamIdx != -1)
				{
					PossibleFutureMoves.Add(FIntPoint(ColumnIdx, RowNumIdx + 1));
					break;
				}
			}
		}


	}

	return PossibleFutureMoves;
}