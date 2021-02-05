// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase_GravityOn.h"
#include "ConnectKGameStateBase.h"
#include "Board_GravityOff.h"
#include "BoardSpaceBase.h"

int AAIControllerBase_GravityOn::HFunc(ABoard_GravityOff* Board)
{
	int AITotalScore = 0;
	int OpponentTotalScore = 0;

	UBoardEvaluationData* BoardEvaluation = Board->AIEvaluateBoard();

	int Domain = Board->GetRowNum() * Board->GetColumnNum();
	int FirstPlayerTotal = 0;
	int SecondPlayerTotal = 0;

	int FirstPlayerTeamIdx = Cast<AConnectKGameStateBase>(GetWorld()->GetGameState())->FirstPlayerTeam;
	int SecondPlayerTeamIdx = FirstPlayerTeamIdx == 0 ? 1 : 0;

	if (Board->GetRowNum() % 2 == 0)
	{
		if ((BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum - 1 == BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum)
			|| ((BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum == BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum) && (BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedOddThreatsNum % 2 == 1))
			|| (BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum == 0 && (BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedOddThreatsNum + BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum) % 2 == 1))
		{
			FirstPlayerTotal += 100;
		}

		if (((BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum + BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedOddThreatsNum) == 0 && (BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedEvenThreatsNum + BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedEvenThreatsNum) > 0)
			|| ((BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum - 2) == BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum)
			|| ((BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum == BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum) && (BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedOddThreatsNum % 2 == 0 && BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedOddThreatsNum > 0))
			|| ((BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum - 1) == BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum && BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedOddThreatsNum > 0)
			|| (BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum == 0 && (BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum == 1 && BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedOddThreatsNum > 0))
			|| (((BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum + BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedOddThreatsNum) % 2 == 0 && (BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum + BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedOddThreatsNum) > 0) && BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum == 0))
		{
			SecondPlayerTotal += 100;
		}
	}
	else if (Domain % 2 == 0 && Board->GetRowNum() % 2 == 1)
	{
		if (((BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedEvenThreatsNum - 1) == BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedEvenThreatsNum)
			|| (BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedEvenThreatsNum % 2 == 1)
			|| ((BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedEvenThreatsNum + BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedEvenThreatsNum) == 1 && (BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedOddThreatsNum + BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum) == 1))
		{
			FirstPlayerTotal += 100;
		}

		if (((BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedOddThreatsNum + BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedOddThreatsNum) > 0)
			|| (((BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedEvenThreatsNum + BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedEvenThreatsNum) % 2 == 0 && (BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedEvenThreatsNum + BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedEvenThreatsNum) > 0)
				&& (((BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedEvenThreatsNum - 2) == BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedEvenThreatsNum)
					|| (BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedEvenThreatsNum == BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedEvenThreatsNum))))
		{
			SecondPlayerTotal += 100;
		}
	}
	else if (Domain % 2 == 1)
	{
		if (((BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedOddThreatsNum + BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum) > 0)
			|| (((BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedEvenThreatsNum + BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedEvenThreatsNum) % 2 == 0 && (BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedEvenThreatsNum + BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedEvenThreatsNum) > 0)
				&& ((BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedEvenThreatsNum - 2 == BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedEvenThreatsNum)
					|| (BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedEvenThreatsNum == BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedEvenThreatsNum))))
		{
			FirstPlayerTotal += 100;
		}

		if ((BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedEvenThreatsNum - 1 == BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedEvenThreatsNum)
			|| (BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedEvenThreatsNum % 2 == 1)
			|| ((BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->SharedEvenThreatsNum + BoardEvaluation->AllTeamEvaluationData[SecondPlayerTeamIdx]->UnsharedEvenThreatsNum) == 1
				&& (BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->SharedOddThreatsNum + BoardEvaluation->AllTeamEvaluationData[FirstPlayerTeamIdx]->UnsharedOddThreatsNum) == 1))
		{
			SecondPlayerTotal += 100;
		}
	}

	AITotalScore += TeamIdx == FirstPlayerTeamIdx ? FirstPlayerTotal : SecondPlayerTotal;
	OpponentTotalScore += TeamIdx == FirstPlayerTeamIdx ? SecondPlayerTotal : FirstPlayerTotal;

	for (int KIdx = 2; KIdx <= Board->GetK(); KIdx++)
	{
		int GroupsWithKIdxNum = 0;
		for (FBoardSpaceGroup SpaceGroup : BoardEvaluation->AllTeamEvaluationData[TeamIdx]->PotentialWinningGroups)
		{
			int GroupFilledInCount = 0;
			for (ABoardSpaceBase* BoardSpace : SpaceGroup.BoardSpaces)
			{
				if (BoardSpace->ClaimedTeamIdx != -1)
				{
					GroupFilledInCount++;
				}
			}

			if (KIdx == GroupFilledInCount)
			{
				GroupsWithKIdxNum++;
			}
		}

		AITotalScore += (GroupsWithKIdxNum * (KIdx - 1));
	}

	for (int KIdx = 2; KIdx <= Board->GetK(); KIdx++)
	{
		int GroupsWithKIdxNum = 0;
		for (FBoardSpaceGroup SpaceGroup : BoardEvaluation->AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0]->PotentialWinningGroups)
		{
			int GroupFilledInCount = 0;
			for (ABoardSpaceBase* BoardSpace : SpaceGroup.BoardSpaces)
			{
				if (BoardSpace->ClaimedTeamIdx != -1)
				{
					GroupFilledInCount++;
				}
			}

			if (KIdx == GroupFilledInCount)
			{
				GroupsWithKIdxNum++;
			}
		}

		OpponentTotalScore += (GroupsWithKIdxNum * (KIdx - 1));
	}

	for (FBoardSpaceGroup SpaceGroup : BoardEvaluation->AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0]->PotentialWinningGroups)
	{
		int FilledSpaces = 0;
		for (ABoardSpaceBase* BoardSpace : SpaceGroup.BoardSpaces)
		{
			if (BoardSpace->ClaimedTeamIdx != -1)
			{
				FilledSpaces++;
			}

		}

		if (FilledSpaces == Board->GetK())
		{
			return LOSE_VAL;
		}
	}

	for (FBoardSpaceGroup SpaceGroup : BoardEvaluation->AllTeamEvaluationData[TeamIdx]->PotentialWinningGroups)
	{
		int FilledSpaces = 0;
		for (ABoardSpaceBase* BoardSpace : SpaceGroup.BoardSpaces)
		{
			if (BoardSpace->ClaimedTeamIdx != -1)
			{
				FilledSpaces++;
			}

		}

		if (FilledSpaces == Board->GetK())
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("RETURNING WIN VAL"));
			return WIN_VAL;
		}
	}

	if (AITotalScore - OpponentTotalScore == 0)
	{
		if (TeamIdx == 1)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}

	return AITotalScore - OpponentTotalScore;
}