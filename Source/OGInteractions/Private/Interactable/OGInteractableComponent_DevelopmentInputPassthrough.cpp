// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactable/OGInteractableComponent_DevelopmentInputPassthrough.h"

#include "Interactable/OGInteractableComponent_BehaviorSet.h"


void UOGInteractableComponent_DevelopmentInputPassthrough::Init(FName Id, UShapeComponent* InQueryVolume, UMeshComponent* InPhysicalRepresentation, const FOGInteractableComponent_VisualDelegates& VisualDelegates)
{
	Initialize(Id, InQueryVolume, InPhysicalRepresentation, VisualDelegates);
}

void UOGInteractableComponent_DevelopmentInputPassthrough::TryInteract_Implementation(AActor* Interactor, const FGameplayTag& InputAction)
{
	if (!Interactor)
		return;

	if (const auto* TriggerBehavior = InteractBehaviors.Find(InputAction))
	{
		if (TriggerBehavior->TryExecuteDelegate_CanInteract(Interactor))
		{
			TriggerBehavior->TryExecuteDelegate_OnInteract_Succeeded(Interactor);
		}
		else
		{
			TriggerBehavior->TryExecuteDelegate_OnInteract_Failed(Interactor);
		}
	}
}

void UOGInteractableComponent_DevelopmentInputPassthrough::BindTriggeredAction(FGameplayTag InputAction, FOGInteractableComponent_BehaviorSet_Triggered TriggeredBinding)
{
	InteractBehaviors.Add(InputAction, TriggeredBinding);
	TriggeredBinding.AssociatedComponentId = ComponentId;
}
