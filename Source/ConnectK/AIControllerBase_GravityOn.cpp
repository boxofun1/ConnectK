// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase_GravityOn.h"
#include "ConnectKGameStateBase.h"
#include "Board_GravityOff.h"
#include "BoardSpaceBase.h"

TArray<FSpaceGroup> AAIControllerBase_GravityOn::GetAllThreatSpaces(const FBoardEvaluationData& EvaluationData)
{
    TArray<FSpaceGroup> AllThreatSpaces;

    for (int i = 0; i < EvaluationData.AllTeamEvaluationData.Num(); i++)
    {
        FSpaceGroup PlayerThreats;
        FSpaceGroups SpaceGroups = EvaluationData.AllTeamEvaluationData[i].IndexedSpaceGroups[EvaluationData.AllTeamEvaluationData[i].IndexedSpaceGroups.Num() - 2];

        for (FSpaceGroup& SpaceGroup : SpaceGroups.SpaceGroups)
        {
            for (ABoardSpaceBase* BoardSpace : SpaceGroup.SpaceGroup)
            {
                if (BoardSpace->ClaimedTeamIdx == -1 && BoardSpace->GetRowIdx() != 0 && Board->GetBoardSquare(BoardSpace->GetRowIdx() - 1, BoardSpace->GetColumnIdx())->ClaimedTeamIdx == -1)
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

    return AllThreatSpaces;
}

TArray<FSpaceGroup> AAIControllerBase_GravityOn::GetAllPlayerColumnThreats(TArray<FSpaceGroup>& AllThreatSpaces, int ColumnIdx)
{
    TArray<FSpaceGroup> AllPlayerColumnThreats;

    for (FSpaceGroup& TeamThreatSpaces : AllThreatSpaces)
    {
        FSpaceGroup ThreatsInColumn;
        for (ABoardSpaceBase* ThreatSpace : TeamThreatSpaces.SpaceGroup)
        {
            if (ThreatSpace->GetColumnIdx() == ColumnIdx)
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

    return AllPlayerColumnThreats;
}

TArray<ABoardSpaceBase*> AAIControllerBase_GravityOn::GetSharedThreats(TArray<ABoardSpaceBase*>& PlayerThreats, TArray<ABoardSpaceBase*>& EnemyThreats)
{
    TArray<ABoardSpaceBase*> SharedThreats;
    for (ABoardSpaceBase* ThreatSpace : PlayerThreats)
    {
        for (ABoardSpaceBase* EnemyThreatSpace : EnemyThreats)
        {
            if (ThreatSpace->GetRowIdx() >= EnemyThreatSpace->GetRowIdx())
            {
                Mutex.Lock();
                SharedThreats.Add(ThreatSpace);
                Mutex.Unlock();
            }
        }
    }

    return SharedThreats;
}

TArray<ABoardSpaceBase*> AAIControllerBase_GravityOn::GetUnsharedThreats(TArray<ABoardSpaceBase*>& PlayerThreats, TArray<ABoardSpaceBase*>& SharedThreats)
{
    TArray<ABoardSpaceBase*> UnsharedThreats;
    for (ABoardSpaceBase* ThreatSpace : PlayerThreats)
    {
        if (!SharedThreats.Find(ThreatSpace))
        {
            Mutex.Lock();
            UnsharedThreats.Add(ThreatSpace);
            Mutex.Unlock();
        }
    }

    return UnsharedThreats;
}

void AAIControllerBase_GravityOn::IncrementRowThreat(TArray<ABoardSpaceBase*>& Threats, bool bOdd, int &ThreatsNum)
{
    for (ABoardSpaceBase* Threat : Threats)
    {
        bool bIncrement = bOdd ? (Threat->GetRowIdx() + 1) % 2 == 1 : (Threat->GetRowIdx() + 1) % 2 == 0;
        if (bIncrement)
        {
            ThreatsNum++;
            break;
        }
    }
}

FAllThreatData AAIControllerBase_GravityOn::GetThreatData(const FBoardEvaluationData& EvaluationData)
{
    FAllThreatData ThreatData;
    TArray<FSpaceGroup> AllThreatSpaces = GetAllThreatSpaces(EvaluationData);

    int ColumnNum = Board->GetColumnNum();

    for (int ColumnNumIdx = 0; ColumnNumIdx < ColumnNum; ColumnNumIdx++)
    {
        TArray<FSpaceGroup> AllPlayerColumnThreats = GetAllPlayerColumnThreats(AllThreatSpaces, ColumnNumIdx);

        for (int AllPlayerColumnThreatsIdx = 0; AllPlayerColumnThreatsIdx < AllPlayerColumnThreats.Num(); AllPlayerColumnThreatsIdx++)
        {
            TArray<ABoardSpaceBase*>& EnemyPlayerThreats = AllPlayerColumnThreatsIdx == 0 ? AllPlayerColumnThreats[1].SpaceGroup : AllPlayerColumnThreats[0].SpaceGroup;
            TArray<ABoardSpaceBase*> SharedThreats = GetSharedThreats(AllPlayerColumnThreats[AllPlayerColumnThreatsIdx].SpaceGroup, EnemyPlayerThreats);
            TArray<ABoardSpaceBase*> UnsharedThreats = GetUnsharedThreats(AllPlayerColumnThreats[AllPlayerColumnThreatsIdx].SpaceGroup, SharedThreats);

            IncrementRowThreat(UnsharedThreats, true, ThreatData.AllThreats[AllPlayerColumnThreatsIdx].UnsharedOddThreatsNum);
            IncrementRowThreat(UnsharedThreats, false, ThreatData.AllThreats[AllPlayerColumnThreatsIdx].UnsharedEvenThreatsNum);
            IncrementRowThreat(SharedThreats, true, ThreatData.AllThreats[AllPlayerColumnThreatsIdx].SharedOddThreatsNum);
            IncrementRowThreat(SharedThreats, false, ThreatData.AllThreats[AllPlayerColumnThreatsIdx].SharedEvenThreatsNum);
        }
    }

	return ThreatData;
}

void AAIControllerBase_GravityOn::AddGameModeScore(const FBoardEvaluationData& EvaluationData, int& AITotalScore, int& OpponentTotalScore)
{

	FAllThreatData AllThreatData = GetThreatData(EvaluationData);

	int Domain = Board->GetRowNum() * Board->GetColumnNum();
	int FirstPlayerTotal = 0;
	int SecondPlayerTotal = 0;

	int FirstPlayerTeamIdx = CKGameState->FirstPlayerTeam;
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
}