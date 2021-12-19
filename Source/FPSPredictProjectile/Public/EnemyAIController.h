// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

/**
 * 
 */
UCLASS()
class FPSPREDICTPROJECTILE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
    AEnemyAIController();

protected:

    UBehaviorTreeComponent* BehaviorComp = nullptr;

    UBlackboardComponent* BlackboardComp = nullptr;
 
    virtual void OnPossess(APawn* PawnParam) override;

public:

    FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }

    FORCEINLINE UBehaviorTreeComponent* GetBehaviorTreeComponent() const { return BehaviorComp; }
	
};
