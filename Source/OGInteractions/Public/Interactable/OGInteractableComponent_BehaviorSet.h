// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "OGInteractableComponent_BehaviorSet.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FCanInteractDelegate, const AActor*, Interactor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInteractionEventDelegate, AActor*, Interactor);

/*
 * BehaviorSets define callbacks that you should call as necessary for your logic.
 */
USTRUCT(BlueprintType)
struct OGINTERACTIONS_API FOGInteractableComponent_BehaviorSet
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCanInteractDelegate CanInteractDelegate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnInteractionEventDelegate OnInteractSucceededDelegate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FOnInteractionEventDelegate OnInteractFailedDelegate;

public:
	bool TryExecuteDelegate_CanInteract(AActor* Interactor) const;
	void TryExecuteDelegate_OnInteractSucceeded(AActor* Interactor) const;
	void TryExecuteDelegate_OnInteractFailed(AActor* Interactor) const;

private:
	friend class UOGInteractableComponent_Base;
	FName AssociatedComponentId;
};
