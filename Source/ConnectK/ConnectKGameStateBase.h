// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Delegates/Delegate.h"
#include "ConnectKGameStateBase.generated.h"

class ABoard_GravityOff;
/**
 * 
 */
UCLASS(Blueprintable)
class CONNECTK_API AConnectKGameStateBase : public AGameState
{
	GENERATED_BODY()
	
public:
	AConnectKGameStateBase();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ClaimBoardSpace(int RowIdx, int ColumnIdx, int ClaimingTeamIdx);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsMyTurn(int PlayerTeamIdx);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClaimedSpaceDelegate);
	UPROPERTY(BlueprintCallable)
	FClaimedSpaceDelegate ClaimedSpaceDelegateProp;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBoardCreatedDelegate);
	UPROPERTY(BlueprintCallable)
	FBoardCreatedDelegate BoardCreatedDelegateProp;

	UPROPERTY(Replicated, BlueprintReadWrite)
	ABoard_GravityOff* Board;

	UPROPERTY(BlueprintReadWrite)
	int FirstPlayerTeam = -1;
};
