// Fill out your copyright notice in the Description page of Project Settings.


#include "ConnectKGameStateBase.h"
#include "Net/UnrealNetwork.h"

AConnectKGameStateBase::AConnectKGameStateBase()
{
	//replication
	bReplicates = true;
	bAlwaysRelevant = true;
	bOnlyRelevantToOwner = false;
}

void AConnectKGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConnectKGameStateBase, Board);
}