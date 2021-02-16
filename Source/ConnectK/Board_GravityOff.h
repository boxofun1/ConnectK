// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BoardSpaceBase.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Board_GravityOff.generated.h"

USTRUCT(BlueprintType)
struct FBoardAnimData
{
	GENERATED_BODY()

	UPROPERTY()
	int RowIdx;
	UPROPERTY()
	int ColumnIdx;
	UPROPERTY()
	int ClaimingTeamIdx;
	UPROPERTY()
	int BottomRowIdx;
	UPROPERTY()
	int Flashes;
};

USTRUCT()
struct FBoardRow
{
	GENERATED_BODY()

public:
	void Add(ABoardSpaceBase* BoardSpace)
	{
		BoardRows.Add(BoardSpace);
	}

	int Num()
	{
		return BoardRows.Num();
	}

	ABoardSpaceBase* operator[](int BoardRowIdx)
	{
		return BoardRows[BoardRowIdx];
	}

//private:
	UPROPERTY()
	TArray<ABoardSpaceBase*> BoardRows;
};

USTRUCT()
struct FSpaceGroup
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<ABoardSpaceBase*> SpaceGroup;
};

USTRUCT()
struct FSpaceGroups
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSpaceGroup> SpaceGroups;
};

USTRUCT()
struct FTeamEvaluationData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSpaceGroups> IndexedSpaceGroups;
};

USTRUCT(BlueprintType)
struct FBoardEvaluationData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int BoardWinner = -1;

	UPROPERTY()
	TArray<FTeamEvaluationData> AllTeamEvaluationData;

	FBoardEvaluationData()
	{
		AllTeamEvaluationData.Init(FTeamEvaluationData(), 2);
	}
};

class Board_LineEvaluator;
class Board_Line;
class AAIControllerBase_GravityOff;

UCLASS(BlueprintType)
class CONNECTK_API ABoard_GravityOff : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	friend class Board_LineEvaluator;
	friend class Board_Line;
	friend class AAIControllerBase_GravityOff;

	ABoard_GravityOff();
	UFUNCTION(BlueprintCallable)
	void GenerateNewBoard(int InRowNum, int InColumnNum, int InK);
	UFUNCTION(BlueprintCallable)
	ABoardSpaceBase* GetBoardSquare(int RowIdx, int ColumnIdx);

	UFUNCTION(BlueprintCallable)
	FBoardEvaluationData EvaluateBoard();
	virtual TArray<FIntPoint> GetPossibleFutureMoves();

	UFUNCTION(BlueprintCallable)
	int GetColumnNum();
	UFUNCTION(BlueprintCallable)
	int GetRowNum();
	UFUNCTION(BlueprintCallable)
	int GetK();
	UFUNCTION(BlueprintCallable)
	int GetRemainingSpaces();
	UFUNCTION(BlueprintCallable)
	void ClaimBoardSpace(int RowIdx, int ColumnIdx, int ClaimingTeamIdx);
	void SetBoardSpace(int RowIdx, int ColumnIdx, int SettingTeamIdx);
	void ClearBoardSpace(int RowIdx, int ColumnIdx);

	UFUNCTION(NetMulticast, Reliable)
	void SendAnimData(const FBoardAnimData& AnimData);
	void PlayAnim();
	void AnimFlashOn();
	void AnimFlashOff();
	virtual float GetAnimFlashDuration();
	virtual void UpdateAnimData();
	virtual bool AnimShouldStop();
	virtual FBoardAnimData& InitAnimData(FBoardAnimData& AnimData, int RowIdx, int ColumnIdx, int ClaimingTeamIdx);

	void InitBoard(int InRowNum, int InColumnNum, int InK);
	void ClearBoard();
	virtual void CreateBoardSpaces();
	void PositionBoardSpaces();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	const float BOARD_EXTENT_X = 1300.0f;
	const float BOARD_EXTENT_Y = 1000.0f;

	const float BOARD_SPACE_BUFFER_X = 30.0f;
	const float BOARD_SPACE_BUFFER_Y = 30.0f;

	const FVector BOARD_ORIGIN = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(Replicated)
	TArray<FBoardRow> BoardSpaces;

	UPROPERTY(BlueprintReadWrite, Category="Default", Meta=(ExposeOnSpawn=true))
	TSubclassOf<ABoardSpaceBase> BoardSpaceClass;

	UPROPERTY(BlueprintReadWrite, Category="Default", Meta=(ExposeOnSpawn=true))
	TSubclassOf<ABoardSpaceBase> ColumnSelectorClass;

	UPROPERTY(Replicated)
	int RowNum;

	UPROPERTY()
	int ColumnNum;
	UPROPERTY()
	int K;
	UPROPERTY()
	int RemainingSpaces;

	TQueue<FBoardAnimData> AnimDataQueue;
	bool bPlayingAnim;

	FCriticalSection Mutex;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
