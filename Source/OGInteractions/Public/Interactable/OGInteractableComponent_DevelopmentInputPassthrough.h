// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OGInteractableComponent_Base.h"
#include "Components/ActorComponent.h"
#include "OGInteractableComponent_DevelopmentInputPassthrough.generated.h"

/*
 * This is an example of a way you can extend the visual input component to handle behavior.
 * It is meant as a simple input-passthrough to aid quick development.
 * Naturally, it would be preferable to interact with each actor/component only through behavior api hooks and not direct input-handling
 * so as to maintain a separation of concerns
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OGINTERACTIONS_API UOGInteractableComponent_DevelopmentInputPassthrough : public UOGInteractableComponent_Base
{
	GENERATED_BODY()

public:
	// Surface the Initialize function to Public so it can be called via BP.
	// (Implementations that require parameters may want to surface a different initialize function)
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="VisualDelegates", AdvancedDisplay="VisualDelegates"))
	void Init(FName Id, UShapeComponent* InQueryVolume, UMeshComponent* InPhysicalRepresentation,
		const FOGInteractableComponent_VisualDelegates& VisualDelegates
	);
	
	/**
	 * @brief (Server) Tests if CanInteract returns true, and interacts if so.
	 * Triggers (Server) OnInteract or (Server) OnInteractFailed
	 * TODO: Consider deprecating in preference of users implementing their own logic? But triggered should always behave this way, it's the Ongoing ones that are _super bespoke_
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void TryInteract(AActor* Interactor, const FGameplayTag& InputAction);

	UFUNCTION(BlueprintCallable)
	void BindTriggeredAction(FGameplayTag InputAction, FOGInteractableComponent_BehaviorSet_Triggered TriggeredBinding);

protected:
	// In the event you want to bind different types of interactions (e.g., Ongoing) you'll need a second storage solution
	TMap<FGameplayTag, FOGInteractableComponent_BehaviorSet_Triggered> InteractBehaviors;
};
