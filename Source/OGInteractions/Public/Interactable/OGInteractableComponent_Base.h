// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "OGFuture.h"
#include "Components/ActorComponent.h"
#include "Utilities/OGInteractionTags.h"
#include "OGFuture.h"
#include "OGInteractableComponent_Base.generated.h"

// TODO: Perhaps it would be best to leave *_Base as-is, then create a derived class: InteractableComponent_InputBinding
// _No_, how else would you use it? Handling should be tied to the rest of the binding
// Perhaps call it action-handling? Or, rather than stick in on here directly, we put it on a child class for example
// That one can have the input binding logic, and otherwise leave users to create their own input handling systems
// Perhaps, they will query in their input component for an InteractableComponent, then call a bespoke function or interface
// on the actor if it CanInteract? I don't know exactly how or what they'd do, but leaving my implementation off of base
// would probably be beneficial

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FGameplayTag, FGetUIStateDelegate, const AActor*, Interactor);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUIStateChangedDelegate, const FGameplayTag&, NewUIState);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnChangeStateNotificationDelegate, bool, bNewState);

/*
 * Container for visual delegates
 * All are only called on the client.
 * Ensure any properties that you are generating visuals for are up-to-date on the client
 */
USTRUCT(BlueprintType)
struct FOGInteractableComponent_VisualDelegates
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FOnChangeStateNotificationDelegate OnDisabledChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FOnUIStateChangedDelegate OnUIStateChanged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FGetUIStateDelegate GetUIState_OnHover;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FGetUIStateDelegate GetUIState_OnFocus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	FGetUIStateDelegate GetUIState_DefaultState;
};

/*
 * The intent of this Component is to listen to various vectors of interaction (raycast/mouse/overlap) and surface the outcomes
 * through to an identical API via UI-style naming conventions:
 *	 Hover - Your interaction vector is engaging with this component
 *	 Focus - (if implemented) your Pawn has selected this (like a row in Excel)
 *	 Default - No interactions are occuring with this component
 *
 *	The InteractorComponent runs locally only, and interacts with the Trigger functions.
 *	These call the OnUIStateChange functions, which only run locally:
 *		- GetOnHoverState, GetOnFocusState, GetGetDefaultState
 *	
 */
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OGINTERACTIONS_API UOGInteractableComponent_Base : public UPrimitiveComponent
{
	GENERATED_BODY()

protected:
	UOGInteractableComponent_Base();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/**
	 * This is left Protected so you can implement your own Init function in BP, or call it in C++ if your implementation requires other setup.
	 * @brief Set the parameters for querying and updating your Interactable. These will be available when updating conveyance.
	 * @param Id A Unique tag to identify this OGInteractableComponent from others on the same actor. Will be added to the queried volume to back-reference this component
	 * @param InQueryVolume The volume you use to query this interactable. If provided, will always be used to query.
	 * @param InPhysicalRepresentation A mesh, skeletal mesh, etc. that represents this interactable. If the only option provided, it will be used to query. Otherwise it will be available when updating conveyance.
	 * @param VisualDelegates Container for optional visual to pass into the InteractionComponent.
	 */
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="VisualDelegates", AdvancedDisplay="VisualDelegates", BlueprintProtected))
	virtual void Initialize(FName Id, UShapeComponent* InQueryVolume, UMeshComponent* InPhysicalRepresentation,
		const FOGInteractableComponent_VisualDelegates& VisualDelegates
	);

	/**
	 * @brief Sets only Delegates. This is helpful for creating default handling in a base BP Implementation of this component
	 * @param OnDisabledChanged called when disabled changes
	 * @param OnUIStateChanged called whenever the visual conveyance state has changed  
	 * @param GetUIState_OnHover called when the user is hovering this
	 * @param GetUIState_OnFocus called when the user has focused this
	 * @param GetUIState_DefaultState called when the user is not pending an interaction
	 */
	UFUNCTION(BlueprintCallable, meta=(AdvancedDisplay="CanInteract, OnInteract, OnInteractFailed, OnDisabledChanged, OnUIStateChanged, GetUIState_OnHover, GetUIState_OnFocus, GetUIState_DefaultState"))
	void InitializeDelegates(
		FOnChangeStateNotificationDelegate OnDisabledChanged,
		FOnUIStateChangedDelegate OnUIStateChanged,
		FGetUIStateDelegate GetUIState_OnHover,
		FGetUIStateDelegate GetUIState_OnFocus,
		FGetUIStateDelegate GetUIState_DefaultState
	);

	UFUNCTION(BlueprintPure)
	bool GetIsDisabled() const { return bDisabled; };

	// TODO: This should be a global setting, not a per-interactable?
	// Override in game file with expected behavior
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InteractionTriggerType = OccamsGamkit::Interactions::Raycast;

	UFUNCTION(BlueprintCallable)
	FOGFuture GetWhenInitialized() { return WhenInitialized;}

#pragma region Triggers
	////////////////////////////////////////
	//// Triggers to change UI State, called from InteractorComponent
	///		Triggers are only visual refreshes (not networked).
	///		If you want a different visual outcome from passive player interactions change rebind
	///		the Get(Hover/Focus/ReturnToDefault)State Delegates for your client

	// Hover, mirrors UI "Hover", as in your "mouse" (i.e., interaction vector) has intersected the interactable
	UFUNCTION(BlueprintCallable)
	void TriggerHover(const AActor* InInstigator);
	UFUNCTION(BlueprintCallable)
	void TriggerHoverEnd(const AActor* InInstigator);

	// Hover, mirrors UI "Focus", as in you have "clicked" (i.e., interacted) with an interactable that can be selected
	// (This is not wired up by default, your input component will have to hook into this logic)
	UFUNCTION(BlueprintCallable)
	void TriggerFocus(const AActor* InInstigator);
	UFUNCTION(BlueprintCallable)
	void TriggerFocusEnd(const AActor* InInstigator);

	UFUNCTION(BlueprintCallable)
	void TriggerUIStateDefaultRefresh();

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
	virtual FGameplayTag GetHoverStateFor(const AActor* Interactor) const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: GetUIState_OnHover"))
	void SetOnHoverDelegate(const FGetUIStateDelegate& GetUIState_OnHover);

	// Logic for determining what UI state should be set on Focus
	UPROPERTY()
	FGetUIStateDelegate OnFocusDelegate;
	UFUNCTION(BlueprintPure)
	virtual FGameplayTag GetFocusStateFor(const AActor* Interactor) const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: GetUIState_OnFocus"))
	void SetOnFocusDelegate(const FGetUIStateDelegate& GetUIState_OnFocus);

	// Logic for determining what UI state should be set when neither Hovered or Focused
	UPROPERTY()
	FGetUIStateDelegate GetDefaultStateDelegate;
	UFUNCTION(BlueprintPure)
	virtual FGameplayTag GetDefaultStateForLocalPlayer() const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: GetUIState_DefaultState"))
	void SetGetDefaultStateDelegate(const FGetUIStateDelegate& GetUIState_DefaultState);

	// Logic for determining what UI state should be set when neither Hovered or Focused
	UPROPERTY()
	FOnUIStateChangedDelegate OnUIStateChangedDelegate;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: OnUIStateChanged"))
	virtual void SetOnUIStateChangedDelegate(const FOnUIStateChangedDelegate& OnUIStateChanged);

	//// End Interaction Candidate handles
	//////////////////////////////////////
#pragma endregion UIStateChange_Delegates

#pragma region Behavior_Delegates
	////////////////////////////////////////
	//// Begin Behavior Delegates, CanInteract, & Interaction Outcomes

	UPROPERTY()
	FOnChangeStateNotificationDelegate OnDisabledChangedDelegate;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: OnDisabledChanged"))
	void SetOnDisabledChangedDelegate(const FOnChangeStateNotificationDelegate& OnDisabledChanged);
	
	//// End Behavior Delegates, CanInteract, & Interaction Outcomes
	////////////////////////////////////////
#pragma endregion Behavior_Delegates

#pragma endregion Delegates

	/**
	 * @brief (Server) Extend this in your own code to handle the Disabled case
	 *		  I would expect this to involve disabling collision, etc.
	 * @param bInDisabled 
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable)
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
	FName ComponentId;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing="OnRep_OnDisabledChanged")
	bool bDisabled = false;

	// We need to reliably trigger bDisabledUpdates, so it is the only OnRep controlled property
	UFUNCTION()
	void OnRep_OnDisabledChanged();

private:
	TOGPromise<void> WhenInitialized;
	
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
	
protected:
	FGameplayTag TryExecuteGetterDelegate(const FGetUIStateDelegate& InDelegate, const AActor* Interactor, FString CallingFunction) const;

};
