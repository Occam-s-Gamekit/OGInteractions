// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/OGInteractableComponent_BehaviorSet.h"

bool FOGInteractableComponent_BehaviorSet_Base::TryExecuteDelegate_CanInteract(AActor* Interactor) const
{
	if (ensureAlwaysMsgf(CanInteractDelegate.IsBound(), TEXT("FOGInteractableComponent_BehaviorSet_Base::TryExecuteDelegate_CanInteract - Delegate for %s has not been set"), *AssociatedComponentId.ToString()))
	{
		return CanInteractDelegate.Execute(Interactor);
	}
	return false;
}

DEFINE_INTERACTION_DELEGATE_IMPLEMENTATION(FOGInteractableComponent_BehaviorSet_Triggered, Succeeded, true);
DEFINE_INTERACTION_DELEGATE_IMPLEMENTATION(FOGInteractableComponent_BehaviorSet_Triggered, Failed, false);


