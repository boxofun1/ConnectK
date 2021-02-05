// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardSpaceBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABoardSpaceBase::ABoardSpaceBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpaceBorders = CreateDefaultSubobject<UBoxComponent>(FName("SpaceBorders"));
	SetRootComponent(SpaceBorders);

	FVector BoxExtent(50.0f, 50.0f, 25.0f);
	SpaceBorders->InitBoxExtent(BoxExtent);

	//Replication
	//SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	bOnlyRelevantToOwner = false;
}

// Called when the game starts or when spawned
void ABoardSpaceBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoardSpaceBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABoardSpaceBase::GetBoardSpaceWidth()
{
	return SpaceBorders->GetScaledBoxExtent().X * 2.0f;
}

float ABoardSpaceBase::GetBoardSpaceHeight()
{
	return SpaceBorders->GetScaledBoxExtent().Y * 2.0f;
}

void ABoardSpaceBase::SetPosition(int InRowIdx, int InColumnIdx)
{
	RowIdx = InRowIdx;
	ColumnIdx = InColumnIdx;
}

int ABoardSpaceBase::GetRowIdx() const
{
	return RowIdx;
}

int ABoardSpaceBase::GetColumnIdx() const
{
	return ColumnIdx;
}

void ABoardSpaceBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABoardSpaceBase, RowIdx);
	DOREPLIFETIME(ABoardSpaceBase, ColumnIdx);
	DOREPLIFETIME(ABoardSpaceBase, ClaimedTeamIdx);
}

bool ABoardSpaceBase::IsSpaceAvailable()
{
	return ClaimedTeamIdx != -1;
}