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
	Cast<AConnectKGameStateBase>(GetWorld()->GetGameState())->BoardCreatedDelegateProp.AddDynamic(this, &AAIControllerBase_GravityOff::OnBoardCreated);
	OnBoardCreated();
}

void AAIControllerBase_GravityOff::OnBoardCreated()
{
	if (!Cast<AConnectKGameStateBase>(GetWorld()->GetGameState())->ClaimedSpaceDelegateProp.IsBound())
	{
		Cast<AConnectKGameStateBase>(GetWorld()->GetGameState())->ClaimedSpaceDelegateProp.AddDynamic(this, &AAIControllerBase_GravityOff::OnSpaceClaimed);
	}
	OnSpaceClaimed();
}

void AAIControllerBase_GravityOff::OnSpaceClaimed()
{
	ABoard_GravityOff* Board = Cast<AConnectKGameStateBase>(GetWorld()->GetGameState())->Board;
	if (Board && Cast<AConnectKGameStateBase>(GetWorld()->GetGameState())->IsMyTurn(TeamIdx))
	{
		float TimeInSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());

		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Real Time Seconds: %f"), TimeInSeconds));

		AConnectKGameStateBase* CKGameState = Cast<AConnectKGameStateBase>(GetWorld()->GetGameState());
		NextMove = Async(
			EAsyncExecution::Thread,
			[this, Board, TimeInSeconds]() { return AlphaBetaDeepening(Board, 5.0f, TimeInSeconds); },
			[this]() { 
				AsyncTask(ENamedThreads::GameThread, [this]()
					{
						MakeNextMove();
					}
				);
			}
		);
	}
}

void AAIControllerBase_GravityOff::MakeNextMove()
{
	Cast<AConnectKGameStateBase>(GetWorld()->GetGameState())->ClaimBoardSpace(NextMove.Get().Y, NextMove.Get().X, TeamIdx);
}

int AAIControllerBase_GravityOff::HFunc(ABoard_GravityOff* Board)
{
	int AITotalScore = 0;
	int OpponentTotalScore = 0;

	UBoardEvaluationData* BoardEvaluation = Board->AIEvaluateBoard();

	if (BoardEvaluation->AllTeamEvaluationData[TeamIdx]->bHasJunction)
	{
		AITotalScore += 100;
	}
	else if (BoardEvaluation->AllTeamEvaluationData[TeamIdx]->bHasEmptyJunction)
	{
		AITotalScore += 50;
	}

	if (BoardEvaluation->AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0]->bHasJunction)
	{
		OpponentTotalScore += 100;
	}
	else if (BoardEvaluation->AllTeamEvaluationData[TeamIdx == 0 ? 1 : 0]->bHasEmptyJunction)
	{
		OpponentTotalScore += 50;
	}

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

FIntPoint AAIControllerBase_GravityOff::AlphaBetaDeepening(ABoard_GravityOff* Board, float MaxTimeInSeconds, float StartTimeInSeconds)
{
	TArray<FBoardMove> FoundBoardMoves;
	FBoardMove BestMove;
	FBoardMove CurrentMove;

	int TreeDepthLimit = 1;

	while (UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTimeInSeconds < MaxTimeInSeconds /*&& TreeDepthLimit <= 6*/)
	{
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Reached Depth: %d"), TreeDepthLimit));

		CurrentMove = FirstMax(Board, NEGATIVE_INFINITY, POSITIVE_INFINITY, TreeDepthLimit, MaxTimeInSeconds, StartTimeInSeconds);

		if (CurrentMove.Value == CANCEL_VAL)
		{
			break;
		}

		if (CurrentMove.Value == WIN_VAL)
		{
			//if (GEngine)
			//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("FOUND A WIN!"));
			return CurrentMove.Point;
		}

		//if (CurrentMove.Value == LOSE_VAL)
		//{
		//	if (GEngine)
		//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("GONNA LOSE!"));
		//}

		FoundBoardMoves.Add(CurrentMove);
		TreeDepthLimit++;
	}

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Ran out of time!"));

	//GetWorld()->ForceGarbageCollection(true);

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

FBoardMove AAIControllerBase_GravityOff::FirstMax(ABoard_GravityOff* Board, int Alpha, int Beta, int MaxSearchDepth, float MaxTimeInSeconds, float StartTimeInSeconds)
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

		int WinningTeamIdx = Board->EvaluateBoard();
		if (WinningTeamIdx == TeamIdx)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return FBoardMove(WIN_VAL, PossibleMove);
		}
		else if (WinningTeamIdx != TeamIdx && WinningTeamIdx != -1)
		{
			v = FMath::Max(v, LOSE_VAL);
		}
		else if (Board->RemainingSpaces <= 0)
		{
			v = FMath::Max(v, 0);
		}
		else
		{
			v = FMath::Max(v, MinVal(Board, Alpha, Beta, MaxSearchDepth - 1, MaxTimeInSeconds, StartTimeInSeconds));
		}

		if (v == WIN_VAL)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return FBoardMove(WIN_VAL, PossibleMove);
		}

		if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTimeInSeconds >= MaxTimeInSeconds)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return FBoardMove(CANCEL_VAL, FIntPoint(0.0f, 0.0f));
		}

		if (v > Alpha)
		{
			BestMove = FBoardMove(v, PossibleMove);
		}

		Alpha = FMath::Max(Alpha, v);

		Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
	}

	return BestMove;
}

int AAIControllerBase_GravityOff::MaxVal(ABoard_GravityOff* Board, int Alpha, int Beta, int MaxSearchDepth, float MaxTimeInSeconds, float StartTimeInSeconds)
{

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("IN MAX"));

	if (MaxSearchDepth <= 0)
	{
		return HFunc(Board);
	}

	int v = NEGATIVE_INFINITY;

	TArray<FIntPoint> PossibleMoveCoordinates = Board->GetPossibleFutureMoves();
	for (FIntPoint &PossibleMove : PossibleMoveCoordinates)
	{
		if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTimeInSeconds >= MaxTimeInSeconds)
		{
			return CANCEL_VAL;
		}

		Board->SetBoardSpace(PossibleMove.Y, PossibleMove.X, TeamIdx);

		int WinningTeamIdx = Board->EvaluateBoard();
		if (WinningTeamIdx == TeamIdx)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return WIN_VAL;
		}
		else if (WinningTeamIdx != TeamIdx && WinningTeamIdx != -1)
		{
			v = FMath::Max(v, LOSE_VAL);
		}
		else if (Board->RemainingSpaces <= 0)
		{
			v = FMath::Max(v, 0);
		}
		else
		{
			v = FMath::Max(v, MinVal(Board, Alpha, Beta, MaxSearchDepth - 1, MaxTimeInSeconds, StartTimeInSeconds));
		}


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

int AAIControllerBase_GravityOff::MinVal(ABoard_GravityOff* Board, int Alpha, int Beta, int MaxSearchDepth, float MaxTimeInSeconds, float StartTimeInSeconds)
{

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("IN MIN"));

	if (MaxSearchDepth <= 0)
	{
		return HFunc(Board);
	}

	int v = POSITIVE_INFINITY;

	TArray<FIntPoint> PossibleMoveCoordinates = Board->GetPossibleFutureMoves();
	for (FIntPoint &PossibleMove : PossibleMoveCoordinates)
	{
		if (UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTimeInSeconds >= MaxTimeInSeconds)
		{
			return CANCEL_VAL;
		}

		Board->SetBoardSpace(PossibleMove.Y, PossibleMove.X, TeamIdx == 0 ? 1 : 0);

		int WinningTeamIdx = Board->EvaluateBoard();
		if (WinningTeamIdx != TeamIdx && WinningTeamIdx != -1)
		{
			Board->ClearBoardSpace(PossibleMove.Y, PossibleMove.X);
			return LOSE_VAL;
		}
		else if (WinningTeamIdx == TeamIdx)
		{
			v = FMath::Min(v, WIN_VAL);
		}
		else if (Board->RemainingSpaces <= 0)
		{
			v = FMath::Min(v, 0);
		}
		else
		{
			v = FMath::Min(v, MaxVal(Board, Alpha, Beta, MaxSearchDepth - 1, MaxTimeInSeconds, StartTimeInSeconds));
		}


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