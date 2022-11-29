// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/AI/Hunter/AIHunterController.h"

#include "GCFPS/Player/FPCharacter.h"
//#include "../../FPCharacter.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

//#include "AIEnforcer.h"
#include "GCFPS/AI/Enforcer/AIEnforcer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Kismet/GameplayStatics.h"

#include "Runtime/Engine/Classes/Engine/Engine.h"

#include "GenericTeamAgentInterface.h"