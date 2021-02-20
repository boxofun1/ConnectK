// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerBase_GravityOff.h"
#include "ConnectKGameStateBase.h"
#include "Board_GravityOff.h"
#include "BoardSpaceBase.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathVectorCommon.h"
#include "Async/Async.h"

void AAIControllerBase_GravityOff::SetTeam(int InTeamIdx)
{
	TeamIdx = InTeamIdx;
}

void AAIControllerBase_GravityOff::BeginPlay()
{
	Super::BeginPlay();

	TeamIdx = 1;
	CKGameState = Cast<AConnectKGameStateBase>(GetWorld()->GetGameState());
	CKGameState->BoardCreatedDelegateProp.AddDynamic(this, &AAIControllerBase_GravityOff::OnBoardCreated);
	OnBoardCreated();
}

void AAIControllerBase_GravityOff::OnBoardCreated()
{
	Board = CKGameState->Board;

	if (!CKGameState->ClaimedSpaceDelegateProp.IsBound())
	{
		CKGameState->ClaimedSpaceDelegateProp.AddDynamic(this, &AAIControllerBase_GravityOff::OnSpaceClaimed);
	}

	OnSpaceClaimed();
}

void AAIControllerBase_GravityOff::OnSpaceClaimed()
{
	if (!Board || !CKGameState->IsMyTurn(TeamIdx))
	{
		return;
	}

	float TimeInSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Real Time Seconds: %f"), TimeInSeconds));

	NextMove = Async(
		EAsyncExecution::Thread,
		[this, TimeInSeconds]() { return GetNextMove(5.0f, TimeInSeconds); },
		[this]() {
			AsyncTask(ENamedThreads::GameThread, [this]()
				{
					MakeNextMove();
				}
			);
		}
	);
}

void AAIControllerBase_GravityOff::MakeNextMove()
{
	CKGameState->ClaimBoardSpace(NextMove.Get().Y, NextMove.Get().X, TeamIdx);
}

FAllJunctionData AAIControllerBase_GravityOff::GetJunctionData(const FBoardEvaluationData& EvaluationData)
{
	FAllJunctionData AllJunctionData;

	for (int AllTeamEvaluationDataIdx = 0; AllTeamEvaluationDataIdx < EvaluationData.AllTeamEvaluationData.Num(); AllTeamEvaluationDataIdx++)
    {
        TArray<FSpaceGroup> PotentialJunctionGroups = EvaluationData.AllTeamEvaluationData[AllTeamEvaluationDataIdx].IndexedSpaceGroups[EvaluationData.AllTeamEvaluationData[AllTeamEvaluationDataIdx].IndexedSpaceGroups.Num() - 3].SpaceGroups;

        for (int PotentialJunctionGroupsIdx = 0; PotentialJunctionGroupsIdx < PotentialJunctionGroups.Num(); PotentialJunctionGroupsIdx++)
        {
            for (int OtherPotentialJunctionGroupsIdx = 0; OtherPotentialJunctionGroupsIdx < PotentialJunctionGroups.Num(); OtherPotentialJunctionGroupsIdx++)
            {
                if (PotentialJunctionGroupsIdx == OtherPotentialJunctionGroupsIdx)
                {
                    continue;
                }

				SetJunction(AllJunctionData.AllJunctions[AllTeamEvaluationDataIdx], PotentialJunctionGroups[PotentialJunctionGroupsIdx].SpaceGroup[0], PotentialJunctionGroups[OtherPotentialJunctionGroupsIdx].SpaceGroup[0]);
				SetJunction(AllJunctionData.AllJunctions[AllTeamEvaluationDataIdx], PotentialJunctionGroups[PotentialJunctionGroupsIdx].SpaceGroup[PotentialJunctionGroups[PotentialJunctionGroupsIdx].SpaceGroup.Num() - 2], PotentialJunctionGroups[OtherPotentialJunctionGroupsIdx].SpaceGroup[PotentialJunctionGroups[OtherPotentialJunctionGroupsIdx].SpaceGroup.Num() - 2]);
				SetJunction(AllJunctionData.AllJunctions[AllTeamEvaluationDataIdx], PotentialJunctionGroups[PotentialJunctionGroupsIdx].SpaceGroup[0], PotentialJunctionGroups[OtherPotentialJunctionGroupsIdx].SpaceGroup[PotentialJunctionGroups[OtherPotentialJunctionGroupsIdx].SpaceGroup.Num() - 2]);
            }
        }
    }

	return AllJunctionData;
}

void AAIControllerBase_GravityOff::SetJunction(FTeamJunctionData& JunctionData, const ABoardSpaceBase* BoardSpace, const ABoardSpaceBase* OtherBoardSpace)
{
	if (BoardSpace == OtherBoardSpace)
	{
		if (BoardSpace->ClaimedTeamIdx == -1)
		{
			JunctionData.bHasEmptyJunction = true;
		}
		else
		{
			JunctionData.bHasJunction = true;
		}
	}
}

int AAIControllerBase_GravityOff::HFunc(const FBoardEvaluationData& EvaluationData)
{
	int AITotalScore = 0;
	int OpponentTotalScore = 0;

	AddGameModeScore(EvaluationData, AITotalScore, OpponentTotalScore);

	AddSpaceGroupScore(AITotalScore, TeamIdx, EvaluationData);
	AddSpaceGroupScore(OpponentTotalScore, TeamIdx == 0 ? 1 : 0, EvaluationData);

	return GetFinalScore(AITotalScore, OpponentTotalScore, EvaluationData);
}

void AAIControllerBase_GravityOff::AddGameModeScore(const FBoardEvaluationData& EvaluationData, int& AITotalScore, int& OpponentTotalScore)
{
	AddJunctionScore(EvaluationData, AITotalScore, OpponentTotalScore);
}

void AAIControllerBase_GravityOff::AddJunctionScore(const FBoardEvaluationData& EvaluationData, int& AITotalScore, int& OpponentTotalScore)
{
	FAllJunctionData JunctionData = GetJunctionData(EvaluationData);

	AddTeamJunctionScore(JunctionData.AllJunctions[TeamIdx], AITotalScore);
	AddTeamJunctionScore(JunctionData.AllJunctions[TeamIdx == 0 ? 1 : 0], OpponentTotalScore);
}

void AAIControllerBase_GravityOff::AddTeamJunctionScore(const FTeamJunctionData& TeamJunctionData, int& Score)
{
	if (TeamJunctionData.bHasJunction)
	{
		Score += 100;
	}
	else if (TeamJunctionData.bHasEmptyJunction)
	{
		Score += 50;
	}
}

void AAIControllerBase_GravityOff::AddSpaceGroupScore(int& Score, int ScoreTeamIdx, const FBoardEvaluationData& EvaluationData)
{
	for (int KIdx = 2; KIdx < EvaluationData.AllTeamEvaluationData[ScoreTeamIdx].IndexedSpaceGroups.Num(); KIdx++)
	{
		Score += (EvaluationData.AllTeamEvaluationData[ScoreTeamIdx].IndexedSpaceGroups[KIdx].SpaceGroups.Num() * (KIdx - 1));
	}
}

int AAIControllerBase_GravityOff::GetFinalScore(int AITotalScore, int OpponentTotalScore, const FBoardEvaluationData& EvaluationData)
{
	int K = EvaluationData.AllTeamEvaluationData[TeamIdx].IndexedSpaceGroups.Num() - 1;
	if (!EvaluationData.AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0].IndexedSpaceGroups[K].SpaceGroups.Num() != !EvaluationData.AllTeamEvaluationData[TeamIdx].IndexedSpaceGroups[K].SpaceGroups.Num())
	{
		return EvaluationData.AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0].IndexedSpaceGroups[K].SpaceGroups.Num() ? LOSE_VAL : WIN_VAL;
	}

	if (!(AITotalScore - OpponentTotalScore))
	{
		return TeamIdx == 1 ? 1 : -1;
	}

	return AITotalScore - OpponentTotalScore;
}

FIntPoint AAIControllerBase_GravityOff::GetNextMove(float MaxTimeInSeconds, float StartTimeInSeconds)
{
	TArray<FBoardMove> FoundBoardMoves;
	FBoardMove BestMove;
	FBoardMove CurrentMove;

	int TreeDepthLimit = 1;

	while (UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTimeInSeconds < MaxTimeInSeconds)
	{
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Reached Depth: %d"), TreeDepthLimit));

		CurrentMove = FirstMax(NEGATIVE_INFINITY, POSITIVE_INFINITY, TreeDepthLimit, MaxTimeInSeconds, StartTimeInSeconds);

		if (CurrentMove.Value == CANCEL_VAL)
		{
			break;
		}

		if (CurrentMove.Value == WIN_VAL)
		{
			return CurrentMove.Point;
		}

		Mutex.Lock();
		FoundBoardMoves.Add(CurrentMove);
		Mutex.Unlock();
		TreeDepthLimit++;
	}

	if (FoundBoardMoves[FoundBoardMoves.Num() - 1].Value == LOSE_VAL)
	{
		for (int FoundBoardMovesIdx = FoundBoardMoves.Num() - 1; FoundBoardMovesIdx >= 0; FoundBoardMovesIdx--)
		{
			if (FoundBoardMoves[FoundBoardMovesIdx].Value != LOSE_VAL)
			{
				return FoundBoardMoves[FoundBoardMovesIdx].Point;
			}
		}
	}

	return FoundBoardMoves[FoundBoardMoves.Num() - 1].Point;
}

FBoardMove AAIControllerBase_GravityOff::FirstMax(int Alpha, int Beta, int MaxSearchDepth, float MaxTimeInSeconds, float StartTimeInSeconds)
{

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("IN FIRST MAX"));

	int v = NEGATIVE_INFINITY;
	FBoardMove BestMove;

	TArray<FIntPoint> PossibleMoveCoordinates = Board->GetPossibleFutureMoves();
	for (FIntPoint &PossibleMove : PossibleMoveCoordinates)
	{
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Checking Move: (X: %d, Y: %d)"), PossibleMove.X, PossibleMove.Y));

		Board->SetBoardSpace(PossibleMove.Y, PossibleMove.X, TeamIdx);

		int Min = MinVal(Alpha, Beta, MaxSearchDepth - 1, MaxTimeInSeconds, StartTimeInSeconds);
		if (Min == CANCEL_VAL)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return FBoardMove(CANCEL_VAL, FIntPoint(0.0f, 0.0f));
		}
		if (Min == WIN_VAL)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return FBoardMove(WIN_VAL, PossibleMove);
		}

		v = FMath::Max(v, Min);

		if (v > Alpha)
		{
			BestMove = FBoardMove(v, PossibleMove);
		}

		Alpha = FMath::Max(Alpha, v);

		Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
	}

	return BestMove;
}

int AAIControllerBase_GravityOff::MaxVal(int Alpha, int Beta, int MaxSearchDepth, float MaxTimeInSeconds, float StartTimeInSeconds)
{

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("IN MAX"));
	if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTimeInSeconds >= MaxTimeInSeconds)
	{
		return CANCEL_VAL;
	}

	{
		FBoardEvaluationData EvaluationData = Board->EvaluateBoard();

		if (EvaluationData.BoardWinner == TeamIdx)
		{
			return WIN_VAL;
		}

		if (EvaluationData.BoardWinner != TeamIdx && EvaluationData.BoardWinner != -1)
		{
			return LOSE_VAL;
		}

		if (Board->RemainingSpaces <= 0)
		{
			return 0;
		}

		if (MaxSearchDepth <= 0)
		{
			return HFunc(EvaluationData);
		}
	}

	int v = NEGATIVE_INFINITY;

	TArray<FIntPoint> PossibleMoveCoordinates = Board->GetPossibleFutureMoves();
	for (FIntPoint &PossibleMove : PossibleMoveCoordinates)
	{
		Board->SetBoardSpace(PossibleMove.Y, PossibleMove.X, TeamIdx);

		int Min = MinVal(Alpha, Beta, MaxSearchDepth - 1, MaxTimeInSeconds, StartTimeInSeconds);
		if (Min == CANCEL_VAL)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return CANCEL_VAL;
		}

		if (Min == WIN_VAL)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return WIN_VAL;
		}

		v = FMath::Max(v, Min);

		if (v >= Beta)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return v;
		}

		Alpha = FMath::Max(Alpha, v);

		Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
	}

	return v;
}

int AAIControllerBase_GravityOff::MinVal(int Alpha, int Beta, int MaxSearchDepth, float MaxTimeInSeconds, float StartTimeInSeconds)
{

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("IN MIN"));
	if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTimeInSeconds >= MaxTimeInSeconds)
	{
		return CANCEL_VAL;
	}

	{
		FBoardEvaluationData EvaluationData = Board->EvaluateBoard();
		if (EvaluationData.BoardWinner == TeamIdx)
		{
			return WIN_VAL;
		}

		if (EvaluationData.BoardWinner != TeamIdx && EvaluationData.BoardWinner != -1)
		{
			return LOSE_VAL;
		}

		if (Board->RemainingSpaces <= 0)
		{
			return 0;
		}

		if (MaxSearchDepth <= 0)
		{
			return HFunc(EvaluationData);
		}
	}

	int v = POSITIVE_INFINITY;

	TArray<FIntPoint> PossibleMoveCoordinates = Board->GetPossibleFutureMoves();
	for (FIntPoint &PossibleMove : PossibleMoveCoordinates)
	{
		Board->SetBoardSpace(PossibleMove.Y, PossibleMove.X, TeamIdx == 0 ? 1 : 0);

		int Max = MaxVal(Alpha, Beta, MaxSearchDepth - 1, MaxTimeInSeconds, StartTimeInSeconds);
		if (Max == CANCEL_VAL)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return CANCEL_VAL;
		}

		if (Max == LOSE_VAL)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return LOSE_VAL;
		}

		v = FMath::Min(v, Max);

		if (v <= Alpha)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return v;
		}

		Beta = FMath::Min(Beta, v);

		Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
	}

	return v;
}