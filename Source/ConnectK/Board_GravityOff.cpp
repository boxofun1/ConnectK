// Fill out your copyright notice in the Description page of Project Settings.

#include "Board_GravityOff.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Board_LineEvaluator.h"
#include "Board_Line.h"

// Sets default values
ABoard_GravityOff::ABoard_GravityOff()
{
    //Replication
    //SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
    bReplicates = true;
    bAlwaysRelevant = true;
    bOnlyRelevantToOwner = false;
}

// Called when the game starts or when spawned
void ABoard_GravityOff::BeginPlay()
{
	Super::BeginPlay();

    for (TPair<FString, Board_LineEvaluatorFactory*> Pair : UBoard_LineEvaluator::Factories)
    {
        Pair.Value->Init();
    }

    EvaluationData = NewObject<UBoardEvaluationData>();
    EvaluationData->AllTeamEvaluationData.Add(NewObject<UTeamBoardEvaluationData>());
    EvaluationData->AllTeamEvaluationData.Add(NewObject<UTeamBoardEvaluationData>());
}

// Called every frame
void ABoard_GravityOff::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoard_GravityOff::GenerateNewBoard(int InRowNum, int InColumnNum, int InK)
{
    InitBoard(InRowNum, InColumnNum, InK);
    CreateBoardSpaces();
    PositionBoardSpaces();

    //if (GEngine)
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("BoxExtentX: %f. BoxExtentY: %f."), BoardSpaceDefault->GetBoardSpaceWidth(), BoardSpaceDefault->GetBoardSpaceHeight()));
    //}

    ////Right Border
    //const FVector RightStartLocation(BOARD_ORIGIN.X + BOARD_EXTENT_X / 2.0f, BOARD_ORIGIN.Y + BOARD_EXTENT_Y / 2.0f, BOARD_ORIGIN.Z);
    //const FVector RightEndLocation(BOARD_ORIGIN.X + BOARD_EXTENT_X / 2.0f, BOARD_ORIGIN.Y - BOARD_EXTENT_Y / 2.0f, BOARD_ORIGIN.Z);
    //DrawDebugLine(GetWorld(), RightStartLocation, RightEndLocation, FColor::Blue, true);

    ////Left Border
    //const FVector LeftStartLocation(BOARD_ORIGIN.X - BOARD_EXTENT_X / 2.0f, BOARD_ORIGIN.Y + BOARD_EXTENT_Y / 2.0f, BOARD_ORIGIN.Z);
    //const FVector LeftEndLocation(BOARD_ORIGIN.X - BOARD_EXTENT_X / 2.0f, BOARD_ORIGIN.Y - BOARD_EXTENT_Y / 2.0f, BOARD_ORIGIN.Z);
    //DrawDebugLine(GetWorld(), LeftStartLocation, LeftEndLocation, FColor::Blue, true);

    ////Bottom Border
    //const FVector TopStartLocation(BOARD_ORIGIN.X - BOARD_EXTENT_X / 2.0f, BOARD_ORIGIN.Y + BOARD_EXTENT_Y / 2.0f, BOARD_ORIGIN.Z);
    //const FVector TopEndLocation(BOARD_ORIGIN.X + BOARD_EXTENT_X / 2.0f, BOARD_ORIGIN.Y + BOARD_EXTENT_Y / 2.0f, BOARD_ORIGIN.Z);
    //DrawDebugLine(GetWorld(), TopStartLocation, TopEndLocation, FColor::Blue, true);

    ////Top Border
    //const FVector BottomStartLocation(BOARD_ORIGIN.X - BOARD_EXTENT_X / 2.0f, BOARD_ORIGIN.Y - BOARD_EXTENT_Y / 2.0f, BOARD_ORIGIN.Z);
    //const FVector BottomEndLocation(BOARD_ORIGIN.X + BOARD_EXTENT_X / 2.0f, BOARD_ORIGIN.Y - BOARD_EXTENT_Y / 2.0f, BOARD_ORIGIN.Z);
    //DrawDebugLine(GetWorld(), BottomStartLocation, BottomEndLocation, FColor::Blue, true);

    //DrawDebugPoint(GetWorld(), FVector(BOARD_ORIGIN.X, BOARD_ORIGIN.Y, BOARD_ORIGIN.Z), 10.0f, FColor::Blue, true);
}

void ABoard_GravityOff::InitBoard(int InRowNum, int InColumnNum, int InK)
{
    ClearBoard();

    ColumnNum = InColumnNum;
    RowNum = InRowNum;
    K = InK;
    RemainingSpaces = ColumnNum * RowNum;
}

void ABoard_GravityOff::ClearBoard()
{
    BoardSpaces.Empty();

    ColumnNum = 0;
    RowNum = 0;
    K = 0;
    RemainingSpaces = 0;
}

void ABoard_GravityOff::CreateBoardSpaces()
{
    for (int RowIdx = 0; RowIdx < RowNum; ++RowIdx)
    {
        BoardSpaces.Add(FBoardRow());

        for (int ColumnIdx = 0; ColumnIdx < ColumnNum; ++ColumnIdx)
        {
            BoardSpaces[RowIdx].Add(GetWorld()->SpawnActor<ABoardSpaceBase>(BoardSpaceClass, FVector::ZeroVector, FRotator::ZeroRotator, FActorSpawnParameters()));
        }
    }
}

void ABoard_GravityOff::PositionBoardSpaces()
{
    ABoardSpaceBase* BoardSpaceDefault = Cast<ABoardSpaceBase>(ABoardSpaceBase::StaticClass()->GetDefaultObject(true));

    int ArrayColumnNum = BoardSpaces[0].Num();
    int ArrayRowNum = BoardSpaces.Num();

    float RowLength = ArrayColumnNum * BoardSpaceDefault->GetBoardSpaceWidth() + (ArrayColumnNum - 1) * BOARD_SPACE_BUFFER_X;
    float ColumnLength = ArrayRowNum * BoardSpaceDefault->GetBoardSpaceHeight() + (ArrayRowNum - 1) * BOARD_SPACE_BUFFER_Y;

    float BoardSpaceScale;
    if (RowLength - BOARD_EXTENT_X <= 0 && ColumnLength - BOARD_EXTENT_Y <= 0)
    {
        BoardSpaceScale = 1.0f;
    }
    else
    {
        BoardSpaceScale = RowLength - BOARD_EXTENT_X >= ColumnLength - BOARD_EXTENT_Y ? BOARD_EXTENT_X / RowLength : BOARD_EXTENT_Y / ColumnLength;
    }

    for (int RowIdx = 0; RowIdx < ArrayRowNum; ++RowIdx)
    {
        float RowY = BOARD_ORIGIN.Y + BoardSpaceScale * ColumnLength / 2.0f - BoardSpaceScale * BoardSpaceDefault->GetBoardSpaceHeight() / 2.0f - RowIdx * BoardSpaceScale * BoardSpaceDefault->GetBoardSpaceHeight() - RowIdx * BoardSpaceScale * BOARD_SPACE_BUFFER_Y;

        for (int ColumnIdx = 0; ColumnIdx < ArrayColumnNum; ++ColumnIdx)
        {
            float ColumnX = BOARD_ORIGIN.X - BoardSpaceScale * RowLength / 2.0f + BoardSpaceScale * BoardSpaceDefault->GetBoardSpaceWidth() / 2.0f + ColumnIdx * BoardSpaceScale * BoardSpaceDefault->GetBoardSpaceWidth() + ColumnIdx * BoardSpaceScale * BOARD_SPACE_BUFFER_X;

            BoardSpaces[RowIdx][ColumnIdx]->SetActorLocation(FVector(ColumnX, RowY, BOARD_ORIGIN.Z));
            BoardSpaces[RowIdx][ColumnIdx]->SetActorScale3D(FVector(BoardSpaceScale, BoardSpaceScale, BoardSpaceScale));
            BoardSpaces[RowIdx][ColumnIdx]->SetPosition(RowIdx, ColumnIdx);
        }
    }
}

void ABoard_GravityOff::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABoard_GravityOff, BoardSpaces);
    DOREPLIFETIME(ABoard_GravityOff, RowNum);
}

ABoardSpaceBase* ABoard_GravityOff::GetBoardSquare(int RowIdx, int ColumnIdx)
{
    return BoardSpaces[RowIdx][ColumnIdx];
}

int ABoard_GravityOff::EvaluateBoard()
{
    FString LineTypes[] = { "UBoard_LineEvaluatorRow", "UBoard_LineEvaluatorColumn", "UBoard_LineEvaluatorForwardDiagonal",  "UBoard_LineEvaluatorBackDiagonal" };

    for (FString LineType : LineTypes)
    {
        UBoard_LineEvaluator* LineEvaluator = UBoard_LineEvaluator::Create(LineType, this);

        UBoard_Line *Line = LineEvaluator->GetNextLine();
        while (Line)
        {
            int CurrentTeamIdx = -1;
            int CurrentTeamConcecutiveSpaces = 0;

            ABoardSpaceBase *CurrentSpace = Line->GetNextSpace();

            while (CurrentSpace)
            {
                if (CurrentSpace->ClaimedTeamIdx == -1)
                {
                    CurrentTeamIdx = -1;
                    CurrentTeamConcecutiveSpaces = 0;
                    CurrentSpace = Line->GetNextSpace();
                    continue;
                }

                if (CurrentSpace->ClaimedTeamIdx == CurrentTeamIdx)
                {
                    CurrentTeamConcecutiveSpaces++;
                }
                else
                {
                    CurrentTeamIdx = CurrentSpace->ClaimedTeamIdx;
                    CurrentTeamConcecutiveSpaces = 1;
                }

                if (CurrentTeamConcecutiveSpaces == K)
                {
                    return CurrentTeamIdx;
                }

                CurrentSpace = Line->GetNextSpace();
            }

            Line = LineEvaluator->GetNextLine();
        }
    }

    return -1;
}

UBoardEvaluationData* ABoard_GravityOff::AIEvaluateBoard()
{
    for (UTeamBoardEvaluationData* TeamData : EvaluationData->AllTeamEvaluationData)
    {
        TeamData->Init();
    }

    FString LineTypes[] = { "UBoard_LineEvaluatorRow", "UBoard_LineEvaluatorColumn", "UBoard_LineEvaluatorForwardDiagonal",  "UBoard_LineEvaluatorBackDiagonal" };

    for (FString LineType : LineTypes)
    {
        UBoard_LineEvaluator* LineEvaluator = UBoard_LineEvaluator::Create(LineType, this);

        UBoard_Line* Line = LineEvaluator->GetNextLine();
        while (Line)
        {
            TArray<ABoardSpaceBase*> KGroup;

            ABoardSpaceBase* CurrentSpace = Line->GetNextSpace();
            while (CurrentSpace)
            {
                KGroup.Add(CurrentSpace);

                if (KGroup.Num() > K)
                {
                    KGroup.RemoveAt(0);
                }

                if (KGroup.Num() == K)
                {
                    int OwningTeamIdx = -1;

                    for (int KGroupIdx = 0; KGroupIdx < KGroup.Num(); KGroupIdx++)
                    {
                        if (OwningTeamIdx != -1 && KGroup[KGroupIdx]->ClaimedTeamIdx != OwningTeamIdx)
                        {
                            OwningTeamIdx = -1;
                            break;
                        }

                        if (OwningTeamIdx == -1 && KGroup[KGroupIdx]->ClaimedTeamIdx != -1)
                        {
                            OwningTeamIdx = KGroup[KGroupIdx]->ClaimedTeamIdx;
                        }
                    }

                    if (OwningTeamIdx != -1)
                    {
                        FBoardSpaceGroup NewSpaceGroup;
                        NewSpaceGroup.BoardSpaces.Append(KGroup);
                        EvaluationData->AllTeamEvaluationData[OwningTeamIdx]->PotentialWinningGroups.Add(NewSpaceGroup);
                    }
                }

                CurrentSpace = Line->GetNextSpace();
            }

            Line = LineEvaluator->GetNextLine();
        }
    }

    GenerateThreatData();
    GenerateJunctionData();

    return EvaluationData;
}

void ABoard_GravityOff::GenerateThreatData()
{
    for (int AllTeamEvaluationDataIdx = 0; AllTeamEvaluationDataIdx < EvaluationData->AllTeamEvaluationData.Num(); AllTeamEvaluationDataIdx++)
    {
        for (FBoardSpaceGroup Group : EvaluationData->AllTeamEvaluationData[AllTeamEvaluationDataIdx]->PotentialWinningGroups)
        {
            int FilledSpacesCount = 0;
            UPROPERTY()
            ABoardSpaceBase* EmptySpace = Group.BoardSpaces[0];
            for (ABoardSpaceBase* BoardSpace : Group.BoardSpaces)
            {
                if (BoardSpace->ClaimedTeamIdx != -1)
                {
                    FilledSpacesCount++;
                }
                else
                {
                    EmptySpace = BoardSpace;
                }
            }

            if (FilledSpacesCount == K - 1 && 
                EmptySpace->GetRowIdx() != 0 && 
                GetBoardSquare(EmptySpace->GetRowIdx() - 1, EmptySpace->GetColumnIdx())->ClaimedTeamIdx == -1)
            {
                EvaluationData->AllTeamEvaluationData[AllTeamEvaluationDataIdx]->ThreatSpaces.Add(EmptySpace);
            }
        }
    }

    for (int ColumnNumIdx = 0; ColumnNumIdx < ColumnNum; ColumnNumIdx++)
    {
        TArray<FBoardSpaceGroup> AllPlayerColumnThreats;

        for (UTeamBoardEvaluationData* TeamEvaluationData : EvaluationData->AllTeamEvaluationData)
        {
            FBoardSpaceGroup ThreatsInColumn;
            for (ABoardSpaceBase* ThreatSpace : TeamEvaluationData->ThreatSpaces)
            {
                if (ThreatSpace->GetColumnIdx() == ColumnNumIdx)
                {
                    ThreatsInColumn.BoardSpaces.Add(ThreatSpace);
                }
            }

            AllPlayerColumnThreats.Add(ThreatsInColumn);
        }

        for (int AllPlayerColumnThreatsIdx = 0; AllPlayerColumnThreatsIdx < AllPlayerColumnThreats.Num(); AllPlayerColumnThreatsIdx++)
        {
            TArray<ABoardSpaceBase*> EnemyPlayerThreats;
            for (int EnemyPlayerColumnThreatsIdx = 0; EnemyPlayerColumnThreatsIdx < AllPlayerColumnThreats.Num(); EnemyPlayerColumnThreatsIdx++)
            {
                if (AllPlayerColumnThreatsIdx == EnemyPlayerColumnThreatsIdx)
                {
                    continue;
                }

                EnemyPlayerThreats.Append(AllPlayerColumnThreats[EnemyPlayerColumnThreatsIdx].BoardSpaces);
            }


            TArray<ABoardSpaceBase*> SharedThreats;
            for (ABoardSpaceBase* ThreatSpace : AllPlayerColumnThreats[AllPlayerColumnThreatsIdx].BoardSpaces)
            {
                for (ABoardSpaceBase* EnemyThreatSpace : EnemyPlayerThreats)
                {
                    if (ThreatSpace->GetRowIdx() >= EnemyThreatSpace->GetRowIdx())
                    {
                        SharedThreats.Add(ThreatSpace);
                    }
                }
            }

            TArray<ABoardSpaceBase*> UnsharedThreats;
            for (ABoardSpaceBase* ThreatSpace : AllPlayerColumnThreats[AllPlayerColumnThreatsIdx].BoardSpaces)
            {
                if (!SharedThreats.Find(ThreatSpace))
                {
                    UnsharedThreats.Add(ThreatSpace);
                }
            }

            for (ABoardSpaceBase* UnharedThreat : UnsharedThreats)
            {
                if ((UnharedThreat->GetRowIdx() + 1) % 2 == 1)
                {
                    EvaluationData->AllTeamEvaluationData[AllPlayerColumnThreatsIdx]->UnsharedOddThreatsNum++;
                    break;
                }
            }

            for (ABoardSpaceBase* UnharedThreat : UnsharedThreats)
            {
                if ((UnharedThreat->GetRowIdx() + 1) % 2 == 0)
                {
                    EvaluationData->AllTeamEvaluationData[AllPlayerColumnThreatsIdx]->UnsharedEvenThreatsNum++;
                    break;
                }
            }

            for (ABoardSpaceBase* SharedThreat : SharedThreats)
            {
                if ((SharedThreat->GetRowIdx() + 1) % 2 == 1)
                {
                    EvaluationData->AllTeamEvaluationData[AllPlayerColumnThreatsIdx]->SharedOddThreatsNum++;
                    break;
                }
            }

            for (ABoardSpaceBase* SharedThreat : SharedThreats)
            {
                if ((SharedThreat->GetRowIdx() + 1) % 2 == 0)
                {
                    EvaluationData->AllTeamEvaluationData[AllPlayerColumnThreatsIdx]->SharedEvenThreatsNum++;
                    break;
                }
            }
        }
    }
}

void ABoard_GravityOff::GenerateJunctionData()
{
    for (int AllTeamEvaluationDataIdx = 0; AllTeamEvaluationDataIdx < EvaluationData->AllTeamEvaluationData.Num(); AllTeamEvaluationDataIdx++)
    {
        TArray<FBoardSpaceGroup> PotentialJunctionGroups;
        for (FBoardSpaceGroup Group : EvaluationData->AllTeamEvaluationData[AllTeamEvaluationDataIdx]->PotentialWinningGroups)
        {
            int FilledSpaces = 0;
            for (ABoardSpaceBase* BoardSpace : Group.BoardSpaces)
            {
                if (BoardSpace->ClaimedTeamIdx != -1)
                {
                    FilledSpaces++;
                }
            }

            if (FilledSpaces == K - 2)
            {
                PotentialJunctionGroups.Add(Group);
            }
        }

        for (int PotentialJunctionGroupsIdx = 0; PotentialJunctionGroupsIdx < PotentialJunctionGroups.Num(); PotentialJunctionGroupsIdx++)
        {
            for (int OtherPotentialJunctionGroupsIdx = 0; OtherPotentialJunctionGroupsIdx < PotentialJunctionGroups.Num(); OtherPotentialJunctionGroupsIdx++)
            {
                if (PotentialJunctionGroupsIdx == OtherPotentialJunctionGroupsIdx)
                {
                    continue;
                }

                if (PotentialJunctionGroups[PotentialJunctionGroupsIdx].BoardSpaces[0] == PotentialJunctionGroups[OtherPotentialJunctionGroupsIdx].BoardSpaces[0])
                {
                    if (PotentialJunctionGroups[PotentialJunctionGroupsIdx].BoardSpaces[0]->ClaimedTeamIdx == -1)
                    {
                        EvaluationData->AllTeamEvaluationData[AllTeamEvaluationDataIdx]->bHasEmptyJunction = true;
                    }
                    else
                    {
                        EvaluationData->AllTeamEvaluationData[AllTeamEvaluationDataIdx]->bHasJunction = true;
                    }
                }

                if (PotentialJunctionGroups[PotentialJunctionGroupsIdx].BoardSpaces[K - 1] == PotentialJunctionGroups[OtherPotentialJunctionGroupsIdx].BoardSpaces[K - 1])
                {
                    if (PotentialJunctionGroups[PotentialJunctionGroupsIdx].BoardSpaces[K - 1]->ClaimedTeamIdx == -1)
                    {
                        EvaluationData->AllTeamEvaluationData[AllTeamEvaluationDataIdx]->bHasEmptyJunction = true;
                    }
                    else
                    {
                        EvaluationData->AllTeamEvaluationData[AllTeamEvaluationDataIdx]->bHasJunction = true;
                    }
                }

                if (PotentialJunctionGroups[PotentialJunctionGroupsIdx].BoardSpaces[0] == PotentialJunctionGroups[OtherPotentialJunctionGroupsIdx].BoardSpaces[K - 1])
                {
                    if (PotentialJunctionGroups[PotentialJunctionGroupsIdx].BoardSpaces[0]->ClaimedTeamIdx == -1)
                    {
                        EvaluationData->AllTeamEvaluationData[AllTeamEvaluationDataIdx]->bHasEmptyJunction = true;
                    }
                    else
                    {
                        EvaluationData->AllTeamEvaluationData[AllTeamEvaluationDataIdx]->bHasJunction = true;
                    }
                }
            }
        }
    }
}

TArray<FIntPoint> ABoard_GravityOff::GetPossibleFutureMoves()
{
    TArray<FIntPoint> PossibleFutureMoves;

    TArray<ABoardSpaceBase*> FilledSpaces;
    for (int RowNumIdx = 0; RowNumIdx < RowNum; RowNumIdx++)
    {
        for (int ColumnNumIdx = 0; ColumnNumIdx < ColumnNum; ColumnNumIdx++)
        {
            ABoardSpaceBase* BoardSpace = BoardSpaces[RowNumIdx][ColumnNumIdx];
            if (BoardSpace->ClaimedTeamIdx != -1)
            {
                FilledSpaces.Add(BoardSpace);
            }
        }
    }

    for (ABoardSpaceBase* FilledSpace : FilledSpaces)
    {
        for (int RowOffsetIdx = -1; RowOffsetIdx < 2; RowOffsetIdx++)
        {
            if (FilledSpace->GetRowIdx() + RowOffsetIdx < 0 || FilledSpace->GetRowIdx() + RowOffsetIdx >= RowNum)
            {
                continue;
            }

            for (int ColumnOffsetIdx = -1; ColumnOffsetIdx < 2; ColumnOffsetIdx++)
            {
                if (FilledSpace->GetColumnIdx() + ColumnOffsetIdx < 0 || FilledSpace->GetColumnIdx() + ColumnOffsetIdx >= ColumnNum)
                {
                    continue;
                }

                if (BoardSpaces[FilledSpace->GetRowIdx() + RowOffsetIdx][FilledSpace->GetColumnIdx() + ColumnOffsetIdx]->ClaimedTeamIdx == -1)
                {
                    PossibleFutureMoves.AddUnique(FIntPoint(FilledSpace->GetColumnIdx() + ColumnOffsetIdx, FilledSpace->GetRowIdx() + RowOffsetIdx));
                }
            }
        }
    }


    if (PossibleFutureMoves.Num() == 0)
    {
        PossibleFutureMoves.Add(FIntPoint(ColumnNum/2, RowNum/2));
    }

    return PossibleFutureMoves;
}

int ABoard_GravityOff::GetColumnNum()
{
    return ColumnNum;
}

int ABoard_GravityOff::GetRowNum()
{
    return RowNum;
}

int ABoard_GravityOff::GetK()
{
    return K;
}

int ABoard_GravityOff::GetRemainingSpaces()
{
    return RemainingSpaces;
}

void ABoard_GravityOff::ClaimBoardSpace(int RowIdx, int ColumnIdx, int ClaimingTeamIdx)
{
    ABoardSpaceBase* BoardSpace = GetBoardSquare(RowIdx, ColumnIdx);
    BoardSpace->ClaimedTeamIdx = ClaimingTeamIdx;

    RemainingSpaces--;

    FBoardAnimData AnimData;
    SendAnimData(InitAnimData(AnimData, RowIdx, ColumnIdx, ClaimingTeamIdx));
}

void ABoard_GravityOff::SendAnimData_Implementation(const FBoardAnimData& AnimData)
{
    AnimDataQueue.Enqueue(AnimData);

    if (!bPlayingAnim)
    {
        bPlayingAnim = true;
        PlayAnim();
    }
}

void ABoard_GravityOff::PlayAnim()
{
    if (!AnimDataQueue.IsEmpty())
    {
        AnimFlashOn();
    }
    else
    {
        bPlayingAnim = false;
    }
}

void ABoard_GravityOff::AnimFlashOn()
{
    ABoardSpaceBase* BoardSpace = GetBoardSquare(AnimDataQueue.Peek()->RowIdx, AnimDataQueue.Peek()->ColumnIdx);
    BoardSpace->SetColorByTeam(AnimDataQueue.Peek()->ClaimingTeamIdx);

    if (AnimShouldStop())
    {
        AnimDataQueue.Pop();
        PlayAnim();
        return;
    }

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ABoard_GravityOff::AnimFlashOff, GetAnimFlashDuration(), false);
}

void ABoard_GravityOff::AnimFlashOff()
{
    ABoardSpaceBase* BoardSpace = GetBoardSquare(AnimDataQueue.Peek()->RowIdx, AnimDataQueue.Peek()->ColumnIdx);
    BoardSpace->ClearColor();

    UpdateAnimData();

    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ABoard_GravityOff::AnimFlashOn, GetAnimFlashDuration(), false);
}

float ABoard_GravityOff::GetAnimFlashDuration()
{
    return 0.2f;
}

void ABoard_GravityOff::UpdateAnimData()
{
    AnimDataQueue.Peek()->Flashes--;
}

bool ABoard_GravityOff::AnimShouldStop()
{
    return AnimDataQueue.Peek()->Flashes == 0;
}

FBoardAnimData& ABoard_GravityOff::InitAnimData(FBoardAnimData& AnimData, int RowIdx, int ColumnIdx, int ClaimingTeamIdx)
{
    AnimData.RowIdx = RowIdx;
    AnimData.ColumnIdx = ColumnIdx;
    AnimData.ClaimingTeamIdx = ClaimingTeamIdx;
    AnimData.Flashes = 1;

    return AnimData;
}