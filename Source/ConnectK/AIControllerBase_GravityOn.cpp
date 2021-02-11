// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase_GravityOn.h"
#include "ConnectKGameStateBase.h"
#include "Board_GravityOff.h"
#include "BoardSpaceBase.h"

FAllThreatData AAIControllerBase_GravityOn::GetThreatData(const FBoardEvaluationData& EvaluationData)
{
    FAllThreatData ThreatData;
    TArray<FSpaceGroup> AllThreatSpaces;

    for (int i = 0; i < EvaluationData.AllTeamEvaluationData.Num(); i++)
    {
        FSpaceGroup PlayerThreats;
        FSpaceGroups SpaceGroups = EvaluationData.AllTeamEvaluationData[i].IndexedSpaceGroups[EvaluationData.AllTeamEvaluationData[i].IndexedSpaceGroups.Num() - 2];

        for (FSpaceGroup& SpaceGroup : SpaceGroups.SpaceGroups)
        {
            for (ABoardSpaceBase* BoardSpace : SpaceGroup.SpaceGroup)
            {
                if (BoardSpace->ClaimedTeamIdx == -1)
                {
                    Mutex.Lock();
                    PlayerThreats.SpaceGroup.Add(BoardSpace);
                    Mutex.Unlock();
                }
            }
        }

        Mutex.Lock();
        AllThreatSpaces.Add(PlayerThreats);
        Mutex.Unlock();
    }

    Mutex.Lock();
    int ColumnNum = Cast<AConnectKGameStateBase>(GetWorld()->GetGameState())->Board->GetColumnNum();
    Mutex.Unlock();

    for (int ColumnNumIdx = 0; ColumnNumIdx < ColumnNum; ColumnNumIdx++)
    {
        TArray<FSpaceGroup> AllPlayerColumnThreats;

        for (FSpaceGroup& TeamEvaluationData : AllThreatSpaces)
        {
            FSpaceGroup ThreatsInColumn;
            for (ABoardSpaceBase* ThreatSpace : TeamEvaluationData.SpaceGroup)
            {
                if (ThreatSpace->GetColumnIdx() == ColumnNumIdx)
                {
                    Mutex.Lock();
                    ThreatsInColumn.SpaceGroup.Add(ThreatSpace);
                    Mutex.Unlock();
                }
            }

            Mutex.Lock();
            AllPlayerColumnThreats.Add(ThreatsInColumn);
            Mutex.Unlock();
        }

        //Goes through every player
        for (int AllPlayerColumnThreatsIdx = 0; AllPlayerColumnThreatsIdx < AllPlayerColumnThreats.Num(); AllPlayerColumnThreatsIdx++)
        {
            //gets all enemy player threats
            TArray<ABoardSpaceBase*> EnemyPlayerThreats;
            for (int EnemyPlayerColumnThreatsIdx = 0; EnemyPlayerColumnThreatsIdx < AllPlayerColumnThreats.Num(); EnemyPlayerColumnThreatsIdx++)
            {
                if (AllPlayerColumnThreatsIdx == EnemyPlayerColumnThreatsIdx)
                {
                    continue;
                }

                Mutex.Lock();
                EnemyPlayerThreats.Append(AllPlayerColumnThreats[EnemyPlayerColumnThreatsIdx].SpaceGroup);
                Mutex.Unlock();
            }

            //Gets all shared threats
            //If threat is above (or equal to) enemy threat, it is shared
            TArray<ABoardSpaceBase*> SharedThreats;
            for (ABoardSpaceBase* ThreatSpace : AllPlayerColumnThreats[AllPlayerColumnThreatsIdx].SpaceGroup)
            {
                for (ABoardSpaceBase* EnemyThreatSpace : EnemyPlayerThreats)
                {
                    if (ThreatSpace->GetRowIdx() >= EnemyThreatSpace->GetRowIdx())
                    {
                        Mutex.Lock();
                        SharedThreats.Add(ThreatSpace);
                        Mutex.Unlock();
                    }
                }
            }

            //Gets all unshared threats
            //If threat isnt found in shared threats, it's unshared
            TArray<ABoardSpaceBase*> UnsharedThreats;
            for (ABoardSpaceBase* ThreatSpace : AllPlayerColumnThreats[AllPlayerColumnThreatsIdx].SpaceGroup)
            {
                if (!SharedThreats.Find(ThreatSpace))
                {
                    Mutex.Lock();
                    UnsharedThreats.Add(ThreatSpace);
                    Mutex.Unlock();
                }
            }

            //Add all unshared odd threats
            for (ABoardSpaceBase* UnsharedThreat : UnsharedThreats)
            {
                if ((UnsharedThreat->GetRowIdx() + 1) % 2 == 1)
                {
                    ThreatData.AllThreats[AllPlayerColumnThreatsIdx].UnsharedOddThreatsNum++;
                    break;
                }
            }

            //Add all unshared even threats
            for (ABoardSpaceBase* UnsharedThreat : UnsharedThreats)
            {
                if ((UnsharedThreat->GetRowIdx() + 1) % 2 == 0)
                {
                    ThreatData.AllThreats[AllPlayerColumnThreatsIdx].UnsharedEvenThreatsNum++;
                    break;
                }
            }

            //Add all shared odd threats
            for (ABoardSpaceBase* SharedThreat : SharedThreats)
            {
                if ((SharedThreat->GetRowIdx() + 1) % 2 == 1)
                {
                    ThreatData.AllThreats[AllPlayerColumnThreatsIdx].SharedOddThreatsNum++;
                    break;
                }
            }

            //Add all shared even threats
            for (ABoardSpaceBase* SharedThreat : SharedThreats)
            {
                if ((SharedThreat->GetRowIdx() + 1) % 2 == 0)
                {
                    ThreatData.AllThreats[AllPlayerColumnThreatsIdx].SharedEvenThreatsNum++;
                    break;
                }
            }
        }
    }

	return ThreatData;
}

int AAIControllerBase_GravityOn::HFunc(const FBoardEvaluationData& EvaluationData)
{
	int AITotalScore = 0;
	int OpponentTotalScore = 0;

    Mutex.Lock();
	AConnectKGameStateBase* GameState = Cast<AConnectKGameStateBase>(GetWorld()->GetGameState());
    Mutex.Unlock();
	ABoard_GravityOff* Board = GameState->Board;
    FAllThreatData AllThreatData = GetThreatData(EvaluationData);

	int Domain = Board->GetRowNum() * Board->GetColumnNum();
	int FirstPlayerTotal = 0;
	int SecondPlayerTotal = 0;

	int FirstPlayerTeamIdx = GameState->FirstPlayerTeam;
	int SecondPlayerTeamIdx = FirstPlayerTeamIdx == 0 ? 1 : 0;

	if (Board->GetRowNum() % 2 == 0)
	{
		if ((AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum - 1 == AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum)
			|| ((AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum == AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum) && (AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedOddThreatsNum % 2 == 1))
			|| (AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum == 0 && (AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedOddThreatsNum + AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum) % 2 == 1))
		{
			FirstPlayerTotal += 100;
		}

		if (((AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum + AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedOddThreatsNum) == 0 && (AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedEvenThreatsNum + AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedEvenThreatsNum) > 0)
			|| ((AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum - 2) == AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum)
			|| ((AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum == AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum) && (AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedOddThreatsNum % 2 == 0 && AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedOddThreatsNum > 0))
			|| ((AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum - 1) == AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum && AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedOddThreatsNum > 0)
			|| (AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum == 0 && (AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum == 1 && AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedOddThreatsNum > 0))
			|| (((AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum + AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedOddThreatsNum) % 2 == 0 && (AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum + AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedOddThreatsNum) > 0) && AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum == 0))
		{
			SecondPlayerTotal += 100;
		}
	}
	else if (Domain % 2 == 0 && Board->GetRowNum() % 2 == 1)
	{
		if (((AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedEvenThreatsNum - 1) == AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedEvenThreatsNum)
			|| (AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedEvenThreatsNum % 2 == 1)
			|| ((AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedEvenThreatsNum + AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedEvenThreatsNum) == 1 && (AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedOddThreatsNum + AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum) == 1))
		{
			FirstPlayerTotal += 100;
		}

		if (((AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedOddThreatsNum + AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedOddThreatsNum) > 0)
			|| (((AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedEvenThreatsNum + AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedEvenThreatsNum) % 2 == 0 && (AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedEvenThreatsNum + AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedEvenThreatsNum) > 0)
				&& (((AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedEvenThreatsNum - 2) == AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedEvenThreatsNum)
					|| (AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedEvenThreatsNum == AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedEvenThreatsNum))))
		{
			SecondPlayerTotal += 100;
		}
	}
	else if (Domain % 2 == 1)
	{
		if (((AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedOddThreatsNum + AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum) > 0)
			|| (((AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedEvenThreatsNum + AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedEvenThreatsNum) % 2 == 0 && (AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedEvenThreatsNum + AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedEvenThreatsNum) > 0)
				&& ((AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedEvenThreatsNum - 2 == AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedEvenThreatsNum)
					|| (AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedEvenThreatsNum == AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedEvenThreatsNum))))
		{
			FirstPlayerTotal += 100;
		}

		if ((AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedEvenThreatsNum - 1 == AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedEvenThreatsNum)
			|| (AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedEvenThreatsNum % 2 == 1)
			|| ((AllThreatData.AllThreats[SecondPlayerTeamIdx].SharedEvenThreatsNum + AllThreatData.AllThreats[SecondPlayerTeamIdx].UnsharedEvenThreatsNum) == 1
				&& (AllThreatData.AllThreats[FirstPlayerTeamIdx].SharedOddThreatsNum + AllThreatData.AllThreats[FirstPlayerTeamIdx].UnsharedOddThreatsNum) == 1))
		{
			SecondPlayerTotal += 100;
		}
	}

	AITotalScore += TeamIdx == FirstPlayerTeamIdx ? FirstPlayerTotal : SecondPlayerTotal;
	OpponentTotalScore += TeamIdx == FirstPlayerTeamIdx ? SecondPlayerTotal : FirstPlayerTotal;

	for (int KIdx = 2; KIdx < EvaluationData.AllTeamEvaluationData[TeamIdx].IndexedSpaceGroups.Num(); KIdx++)
	{
		AITotalScore += (EvaluationData.AllTeamEvaluationData[TeamIdx].IndexedSpaceGroups[KIdx].SpaceGroups.Num() * (KIdx - 1));
	}

	for (int KIdx = 2; KIdx < EvaluationData.AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0].IndexedSpaceGroups.Num(); KIdx++)
	{
		OpponentTotalScore += (EvaluationData.AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0].IndexedSpaceGroups[KIdx].SpaceGroups.Num() * (KIdx - 1));
	}

	if (EvaluationData.AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0].IndexedSpaceGroups[EvaluationData.AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0].IndexedSpaceGroups.Num() - 1].SpaceGroups.Num() != 0)
	{
		return LOSE_VAL;
	}

	if (EvaluationData.AllTeamEvaluationData[TeamIdx].IndexedSpaceGroups[EvaluationData.AllTeamEvaluationData[TeamIdx].IndexedSpaceGroups.Num() - 1].SpaceGroups.Num() != 0)
	{
		return WIN_VAL;
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