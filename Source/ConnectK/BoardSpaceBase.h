// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardSpaceBase.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class CONNECTK_API ABoardSpaceBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoardSpaceBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	float GetBoardSpaceWidth();

	UFUNCTION()
	float GetBoardSpaceHeight();

	UFUNCTION(BlueprintCallable)
	int GetRowIdx() const;

	UFUNCTION(BlueprintCallable)
	int GetColumnIdx() const;

	UFUNCTION(BlueprintCallable)
	bool IsSpaceAvailable();

	UFUNCTION(BlueprintImplementableEvent)
	void SetColorByTeam(int TeamIdx);

	UFUNCTION(BlueprintImplementableEvent)
	void ClearColor();

	void SetPosition(int InRowIdx, int InColumnIdx);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int ClaimedTeamIdx = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	UPROPERTY()
	UBoxComponent *SpaceBorders;

	UPROPERTY(Replicated)
	int RowIdx;
	UPROPERTY(Replicated)
	int ColumnIdx;
};
