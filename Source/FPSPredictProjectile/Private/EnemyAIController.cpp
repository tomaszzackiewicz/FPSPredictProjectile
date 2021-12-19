// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AEnemyAIController::AEnemyAIController(){

    BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

}

void AEnemyAIController::OnPossess(APawn* PawnParam)
{
    Super::OnPossess(PawnParam);

    AEnemy* Enemy = Cast<AEnemy>(PawnParam);

    if (Enemy)
    {
        if (Enemy->BehaviorTree->BlackboardAsset)
        {
            BlackboardComp->InitializeBlackboard(*(Enemy->BehaviorTree->BlackboardAsset));
        }
       
        BehaviorComp->StartTree(*Enemy->BehaviorTree);
    }
}