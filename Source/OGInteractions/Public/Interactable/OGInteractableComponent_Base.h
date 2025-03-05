// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Utilities/OGInteractionTags.h"
#include "OGInteractableComponent_Base.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUIStateChange, const FGameplayTag&, NewState);

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FCanInteractDelegate, const AActor*, Interactor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInteractionEventDelegate, AActor*, Interactor);

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FGameplayTag, FGetUIStateDelegate, AActor*, Interactor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUIStateChangedDelegate, const FGameplayTag&, NewUIState);

/*
 * The intent of this Component is to listen to various vectors of interaction (raycast/mouse/overlap) and surface the outcomes
 * through to a identical API via UI-style naming conventions:
 *	 Hover - Your interaction vector is engaging with this component
 *	 Focus - (if implemented) your Pawn has selected this (like a row in Excel)
 *	 Default - No interactions are occuring with this component
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OGINTERACTIONS_API UOGInteractableComponent_Base : public UPrimitiveComponent
{
	GENERATED_BODY()

protected:
	UOGInteractableComponent_Base();

public:
	/**
	 * @brief Set the parameters for querying and updating your Interactable. These will be available when updating conveyance.
	 * @param Id A Unique tag to identify this OGInteractableComponent from others on the same actor. Will be added to the queried volume to back-reference this component
	 * @param InQueryVolume The volume you use to query this interactable. If provided, will always be used to query.
	 * @param InPhysicalRepresentation A mesh, skeletal mesh, etc. that represents this interactable. If the only option provided, it will be used to query. Otherwise it will be available when updating conveyance.
	 */
	UFUNCTION(BlueprintCallable)
	void Initialize(FName Id, UShapeComponent* InQueryVolume = nullptr, UMeshComponent* InPhysicalRepresentation = nullptr);

	/**
	 * @brief Tests if CanInteract returns true, and interacts if so.
	 * Triggers OnInteract or OnInteractFailed
	 * @return whether interact was successful.
	 */
	UFUNCTION(BlueprintCallable)
	bool TryInteract(AActor* Interactor);

	// TODO: This should be a global setting, not a per-interactable?
	// Override in game file with expected behavior
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InteractionTriggerType = OccamsGamkit::Interactions::Raycast;

	// TODO: Should CanInteract live here and be implemented the same way?

#pragma region Triggers
	////////////////////////////////////////
	//// Triggers to change UI State, called from InteractorComponent

	// Hover, mirrors UI "Hover", as in your "mouse" (i.e., interaction vector) has intersected the interactable
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerHover(AActor* InInstigator);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerHoverEnd(AActor* InInstigator);

	// Hover, mirrors UI "Focus", as in you have "clicked" (i.e., interacted) with an interactable that can be selected
	// (This is not wired up by default, your input component will have to hook into this logic)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerFocus(AActor* InInstigator);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerFocusEnd(AActor* InInstigator);

	//// End Triggers
	////////////////////////////////////////
#pragma endregion Triggers

#pragma region Delegates

#pragma region UIStateChange_Delegates
	////////////////////////////////////////
	//// Begin Interaction Candidate handles, using a UI-based naming convention

	// Logic for determining what UI state should be set on Hover
	UPROPERTY()
	FGetUIStateDelegate OnHoverDelegate;
	UFUNCTION(BlueprintPure)
	FGameplayTag GetHoverStateFor(AActor* Interactor) const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: GetUIState_OnHover"))
	void SetOnHoverDelegate(const FGetUIStateDelegate& GetUIState_OnHover);

	// Logic for determining what UI state should be set on Focus
	UPROPERTY()
	FGetUIStateDelegate OnFocusDelegate;
	UFUNCTION(BlueprintPure)
	FGameplayTag GetFocusStateFor(AActor* Interactor) const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: GetUIState_OnFocus"))
	void SetOnFocusDelegate(const FGetUIStateDelegate& GetUIState_OnFocus);

	// Logic for determining what UI state should be set when neither Hovered or Focused
	UPROPERTY()
	FGetUIStateDelegate OnReturnToDefaultDelegate;
	UFUNCTION(BlueprintPure)
	FGameplayTag GetDefaultStateForLocalPlayer() const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: GetUIState_OnReturnToDefault"))
	void SetOnReturnToDefaultDelegate(const FGetUIStateDelegate& GetUIState_OnReturnToDefault);

	// Logic for determining what UI state should be set when neither Hovered or Focused
	UPROPERTY()
	FOnUIStateChangedDelegate OnUIStateChangedDelegate;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: OnUIStateChanged"))
	void SetOnUIStateChangedDelegate(const FOnUIStateChangedDelegate& OnUIStateChanged);
	// DECLARE_BP_SETTABLE_DELEGATE_PROPERTY(FOnUIStateChangedDelegate, OnUIStateChanged, OnUIStateChanged);

	//// End Interaction Candidate handles
	//////////////////////////////////////
#pragma endregion UIStateChange_Delegates

#pragma region Behavior_Delegates
	////////////////////////////////////////
	//// Begin Behavior Delegates, CanInteract, & Interaction Outcomes
	// DECLARE_BP_SETTABLE_DELEGATE_PROPERTY(FCanInteractDelegate, CanInteract, GetCanInteract);
	UPROPERTY()
	FCanInteractDelegate CanInteractDelegate;
	UFUNCTION(BlueprintPure)
	bool CanInteract(const AActor* Interactor) const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: CanInteract"))
	void SetCanInteractDelegate(const FCanInteractDelegate& CanInteract);

	UPROPERTY()
	FOnInteractionEventDelegate OnInteractDelegate;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: OnInteract"))
	void SetOnInteractDelegate(const FOnInteractionEventDelegate& OnInteract);
	
	UPROPERTY()
	FOnInteractionEventDelegate OnInteractFailedDelegate;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: OnInteractFailed"))
	void SetOnInteractFailedDelegate(const FOnInteractionEventDelegate& OnInteractFailed);
	
	//// End Behavior Delegates, CanInteract, & Interaction Outcomes
	////////////////////////////////////////
#pragma endregion Behavior_Delegates

#pragma endregion Delegates

	// Extend this in your own code to handle the Disabled case
	// I would expect this to involve disabling collision, etc
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetDisabled(bool bInDisabled);

	// When setting FGameplayTag, priority determines what level of state you can clear to replace
	const FGameplayTag& SetUIState(const FGameplayTag& NewState);
	const FGameplayTag& GetUIState() const { return UIState; }

	UFUNCTION(BlueprintPure)
	UPrimitiveComponent* GetExpectedOuter() const { return Cast<UPrimitiveComponent>(GetOuter()); }

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UShapeComponent> QueryVolume;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMeshComponent> PhysicalRepresentation;

protected:
	bool bDisabled = false;

private:
	// This is not initialized until first hover
	FGameplayTag UIState;

	// Begin Unreal Listeners - Volume based interaction
	UFUNCTION()
	void HandleOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	);
	UFUNCTION()
	void HandleOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	// Begin Unreal Listeners - Mouse based interaction
	UFUNCTION()
	void HandleCursorOverBegin(UPrimitiveComponent* TouchedComponent);
	UFUNCTION()
	void HandleCursorOverEnd(UPrimitiveComponent* TouchedComponent);
	// End Unreal Listeners

	// Alerts owner and broadcasts to listeners
	void OnUIStateChange() const;

	FGameplayTag TryExecuteGetterDelegate(const FGetUIStateDelegate& InDelegate, AActor* Interactor, FString CallingFunction) const;
	void TryExecuteInteractEventDelegate(const FOnInteractionEventDelegate& InDelegate, AActor* Interactor, FString CallingFunction) const;
};
