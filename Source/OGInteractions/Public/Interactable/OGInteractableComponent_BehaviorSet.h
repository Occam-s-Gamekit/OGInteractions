// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactable/OGInteractable_BehaviorSet_Helpers.h"
#include "OGInteractableComponent_BehaviorSet.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FCanInteractDelegate, const AActor*, Interactor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInteractionEventDelegate, AActor*, Interactor);

USTRUCT(BlueprintType)
struct OGINTERACTIONS_API FOGInteractableComponent_BehaviorSet_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCanInteractDelegate CanInteractDelegate;
	
	bool TryExecuteDelegate_CanInteract(AActor* Interactor) const;

	FName AssociatedComponentId;
};

/*
 * This concept was an extension of the delegate system used for the visual callbacks
 * It is being used in the DevelopmentInputPassthrough as an easy way to pair inputs to behaviors
 * however, it's a little to open-ended to be surfaced to any user in a BP setting
 */

/*
 * Maps to the callbacks you would want to use if you were using a "pressed" input
 */
USTRUCT(BlueprintType)
struct OGINTERACTIONS_API FOGInteractableComponent_BehaviorSet_Triggered : public FOGInteractableComponent_BehaviorSet_Base
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnInteractionEventDelegate OnInteract_SucceededDelegate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FOnInteractionEventDelegate OnInteract_FailedDelegate;

public:
	DECLARE_INTERACTION_EVENT_DELEGATE_DEFINITION(OnInteract_Succeeded);
	DECLARE_INTERACTION_EVENT_DELEGATE_DEFINITION(OnInteract_Failed);
};
