// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/OGInteractableComponent_BehaviorSet.h"


bool FOGInteractableComponent_BehaviorSet::TryExecuteDelegate_CanInteract(AActor* Interactor) const
{
	if (ensureAlwaysMsgf(CanInteractDelegate.IsBound(), TEXT("FOGInteractableComponent_BehaviorSet::TryExecuteDelegate_CanInteract - Delegate for %s has not been set"), *AssociatedComponentId.ToString()))
	{
		return CanInteractDelegate.Execute(Interactor);
	}
	return false;
}

void FOGInteractableComponent_BehaviorSet::TryExecuteDelegate_OnInteractSucceeded(AActor* Interactor) const
{
	if (ensureAlwaysMsgf(OnInteractSucceededDelegate.IsBound(), TEXT("FOGInteractableComponent_BehaviorSet::TryExecuteDelegate_OnInteractSucceeded - Delegate for %s has not been set"), *AssociatedComponentId.ToString()))
	{
		OnInteractSucceededDelegate.Execute(Interactor);
	}
}

void FOGInteractableComponent_BehaviorSet::TryExecuteDelegate_OnInteractFailed(AActor* Interactor) const
{
	if (OnInteractFailedDelegate.IsBound())
	{
		OnInteractFailedDelegate.Execute(Interactor);
	}
}
