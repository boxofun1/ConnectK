// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamBoardEvaluationData.h"

void UTeamBoardEvaluationData::Init()
{
	UnsharedOddThreatsNum = 0;
	UnsharedEvenThreatsNum = 0;

	SharedOddThreatsNum = 0;
	SharedEvenThreatsNum = 0;

	bHasJunction = false;
	bHasEmptyJunction = false;

	while (IsGarbageCollecting());
	PotentialWinningGroups.Empty();
	while (IsGarbageCollecting());
	ThreatSpaces.Empty();
}