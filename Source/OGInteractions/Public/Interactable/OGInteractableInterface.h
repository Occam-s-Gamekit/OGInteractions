// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OGInteractableInterface.generated.h"


UINTERFACE()
class UOGInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * An Interactable is an Actor that complies to this interface
 * This system expects that you have attached an OGInteractableComponent_(Mesh/Box) to it.
 * When a user engages in an interaction, this system will forward the Interactor, and the Component to the actor to determine the outcome.
 * This is to support multiple interactions on an Actor.
 * 
 * If you are not using more than 1 InteractionComponent, it is entirely safe to ignore the ActingComponent property
 */
class OGINTERACTIONS_API IOGInteractableInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanInteract(const AActor* Interactor, const TScriptInterface<IOGInteractableInterface>& ActingComponent);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleInteract(AActor* Interactor, const TScriptInterface<IOGInteractableInterface>& ActingComponent);
	
	// virtual void HandleInteract_Implementation(AActor* InInstigator) = 0;
	// TODO: Implement hold if we ever need it?
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	// void HandleInteractEnd(AActor* InInstigator);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleInteractFailed(AActor* Interactor, const TScriptInterface<IOGInteractableInterface>& ActingComponent);
};
