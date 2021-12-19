// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Patrol.h"
#include "AIController.h"

#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"


UBTTask_Patrol::UBTTask_Patrol() {
	NodeName = TEXT("Patrol");
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {

	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	AAIController* AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!AIController) {
		return EBTNodeResult::Failed;
	}

	AEnemy* Enemy= Cast<AEnemy>(AIController->GetPawn());
	if (!Enemy) {
		return EBTNodeResult::Failed;
	}
	
	AIController->MoveToLocation(GetRandomPointInRadius(Enemy), 100.0f);

	return EBTNodeResult::Succeeded;
}

FVector UBTTask_Patrol::GetRandomPointInRadius(ACharacter* EnemyParam) {

	FVector Origin = EnemyParam->GetActorLocation();
	FNavLocation Result;

	UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	if (!NavSystem) {
		return Origin;
	}

	bool bFoundPath = NavSystem->GetRandomReachablePointInRadius(Origin, 1000.0f, Result);

	return Result.Location;
}
