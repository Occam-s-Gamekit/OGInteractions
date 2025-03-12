// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utilities/OGInteractionTags.h"
#include "OGInteractorComponent.generated.h"

class UOGInteractableComponent_Base;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OGINTERACTIONS_API UOGInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOGInteractorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InteractionTriggerType = OccamsGamkit::Interactions::Raycast;

	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="InteractionTriggerType == InteractionSystem::InteractionTrigger::Raycast()"))
	float RaycastRange = 600.f;

	virtual UOGInteractableComponent_Base* GetInteractionFocus() { return InteractionFocus; };
	virtual void SetInteractionFocus(UOGInteractableComponent_Base* NewInteractable);
	virtual void RemoveInteractionFocus(UOGInteractableComponent_Base* OldInteractable);
	virtual UOGInteractableComponent_Base* GetInteractionFocus() const;
	virtual void ClearInteractionFocus();

	virtual UOGInteractableComponent_Base* GetInteractionCandidate() { return InteractionCandidate; };
	virtual void SetInteractionCandidate(UOGInteractableComponent_Base* NewInteractable);
	virtual void RemoveInteractionCandidate(UOGInteractableComponent_Base* OldInteractable);
	virtual UOGInteractableComponent_Base* GetInteractionCandidate() const;
	virtual void ClearInteractionCandidate();

protected:
	// What is currently "Selected" or "Focused" in UI Parlance
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UOGInteractableComponent_Base> InteractionFocus = nullptr;
	// What is currently "Hovered" in UI Parlance
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UOGInteractableComponent_Base> InteractionCandidate = nullptr;
};
