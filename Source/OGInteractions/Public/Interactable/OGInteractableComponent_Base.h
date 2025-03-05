// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Utilities/OGInteractionTags.h"
#include "OGInteractableComponent_Base.generated.h"

	/*
	TODO: So here is where we would instead call Execute_GetUIStateForActor_OnHover(GetOwner(), Instigator, GetExpectedOuter());
	 * Do we have a matching "GetUIStateFor_EndHover"? Focus/EndFocus?
	 * Is "GetDefaultState() instead something that we use when hover ends? (That's what we're doing now)
	 * Either you can interact/cannot interact.
	 *
	 * Or rather, its default state should be set to:
	 *		- Callout_Strong           (strong callout that you _should_ use this) (Some games)
	 *		- Callout_Light (Enabled)  (light callout that you _could_ use this) (UI default)
	 *		- None                     (we don't add a callout unless you hover it) (Many games)
	 *  	- Disabled				   (but this would be derived from the element not being interactable.
	 *  	- Invalid_Light  (Enabled's counterpart?) Let's players know that they can use this, just not right now?
	 *  	- Invalid_Strong (OnHover Invalid?)       Probably the OnHover, No-go vis. You Definitely can't use this and we need you to know. Why would this be used as a default state?
	 *  												A like, "Defense mission, hit the button to clear the area when the timer is up".
	 *  												We want you to know for sure what the button is, and that you can't use it right now the the door when you've defended for 30s. You know where the button
	 *  	- Unnecessary_Light    You could interact with this, something will happen, but you don't _need_ to
	 *  	- Unnecessary_Strong   The Hover version
	 *  	This begs the question though, a Red highlight for NO, a yellow one for "unnecessary"? Like when you hover the charging handle after it's been charged.
	 *  	Maybe a gray? We want a neutral color.
	 *  		E.g., setting disabled would override the default setting.
	 *
	 *  THEN, we just fallback on the default when EndHover? (Does it depend on CanInteract? _Probably_
	 *		_BUT_ this again could be dependent not on the element being ABLE to be interacted with, but the actor's state
	 *		E.g., you may be able to interact with something, but...
	 *		CanInteract -> F is _Disabled_, right? We shouldn't have a...
	 *		No, CanInteract-> is _Invalid_, it means you _could_ interact if something else was different.
	 *			This would be used to imply that you _should_ interact with it soon, but something else needs to be done
	 *			Or RATHER, OnHover, that you can't interact with it _now_ but could later.
	 *				This may just always be preferable to Disabled? You want players to know what they could interact with
	 *
	 *	So it's SetDefaultCalloutState(Enum CalloutLevelCanInteract, Enum UIStateCannotInteract)?
	 *
	 *	Yes, `(Get/Set)DefaultCalloutState` as the "what it looks like normally".
	 *	This is based on LocalClient (we don't need the interactor?) and runs based on its own/world state.
	 *	`UpdateDefaultCalloutState()` whenever something happens (e.g., in its state) that would require an update.
	 *
	 *	OnDefaultCalloutActive() -> This is the OnHover/OnFocus equivalent for when you mouse-off or whatever.
	 *		We can get a DefaultCalloutState on a per-component basis, and use the actor's state to inform this.
	 *
	 *	SetUIState(GetCalloutState_Default(LocalClient, Component))
	 *		So GDCS runs (up to a) CanInteract with the local client to determine what the DefaultCalloutState is, and then sets it.
	 *
	 *	SetUIState(GetCalloutState_OnHover(Interactor, Component)
	 *	SetUIState(GetCalloutState_OnFocus(Interactor, Component)
	 *
	 *	On(Hover/Focus)End() -> SetUIState(GetCalloutState_Default(LocalClient, Component))
	 *
	 *	IS THIS ENTIRE INTERACTION SYSTEM GETTING UNNECESSARILY COMPLEX?
	 *		Most games don't have this good of conveyance
	 *		We're not most games, we're asking for a lot of manual interactions, and they need to be clear and intuitive
	 *		Sure, but "Invalid/Unnecessary" callouts may just clutter the UI, when we should just have the "suggested" interactions with the Callout_Light and everything else should be none
	 *		Sure, but we still need more refined control over the UI states of things in the Big Cannon example.
	 *			Currently the shell gets a none hover, when what we want is an "Unnecessary" hover unless you're holding the ramrod.
	 *			We'd want the ramrods to get a callout, but only if they were within a certain distance of the cannon, and were not being held, and there was a shell on the tray.
	 *			But like, is this getting too complex? Should the ramrod just be part of the overall skeletal mesh, so you can't lose it?
	 *			A third thing to slide into place that you can't lose?
	 *
	 *
	 *	TODO:
	 *	TODO:
	 *	TODO:
	 *	TODO:
	 *	TODO:
	 *
	 *	So what this entails:
	 *		- Update the `InteractableInterface` in order to respond to:
	 *			- FGameplayTag GetUIState_Hover(SpecificInteractor, SpecificComponent)
	 *			- FGameplayTag GetUIState_Focus(SpecificInteractor, SpecificComponent)
	 *			- FGameplayTag OnReturnToDefault(LocalClient, SpecificComponent)
	 *
	 *		- Update the InteractableComponent_Base to have Hover/Focus/EndHover/EndFocus call up to those functions then set state based on the return value
	 *
	 *		Then,
	 *			We set the state based on the return value
	 *			We do that part in C++ so the BP user doesn't need to think about it
	 */

// enum class EOG_UIState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUIStateChange, const FGameplayTag&, NewState);

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
	
	// TODO: This should be a global setting, not a per-interactable?
	// Override in game file with expected behavior
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InteractionTriggerType = OccamsGamkit::Interactions::Raycast;

	// UPROPERTY(BlueprintAssignable)
	// FUIStateChange OnUIStateChangeEvent;

	// TODO: Should CanInteract live here and be implemented the same way?

	////////////////////////////////////////
	//// Begin Interaction Candidate handles, using a UI-based naming convention

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

	// Logic for determining what UI state should be set on Hover
	UPROPERTY()
	FGetUIStateDelegate GetHoverStateDelegate;
	UFUNCTION(BlueprintPure)
	FGameplayTag GetHoverState(AActor* Interactor) const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: GetUIState_OnHover"))
	void SetOnHoverDelegate(const FGetUIStateDelegate& GetUIState_OnHover);
	
	// Logic for determining what UI state should be set on Focus
	UPROPERTY()
	FGetUIStateDelegate GetFocusStateDelegate;
	UFUNCTION(BlueprintPure)
	FGameplayTag GetFocusState(AActor* Interactor) const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: GetUIState_OnFocus"))
	void SetOnFocusDelegate(const FGetUIStateDelegate& GetUIState_OnFocus);
	
	// Logic for determining what UI state should be set when neither Hovered or Focused
	UPROPERTY()
	FGetUIStateDelegate GetDefaultStateDelegate;
	UFUNCTION(BlueprintPure)
	FGameplayTag GetDefaultState() const;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: GetUIState_OnReturnToDefault"))
	void SetOnReturnToDefaultDelegate(const FGetUIStateDelegate& GetUIState_OnReturnToDefault);
	
	// Logic for determining what UI state should be set when neither Hovered or Focused
	UPROPERTY()
	FOnUIStateChangedDelegate OnUIStateChangedDelegate;
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Delegate: OnUIStateChanged"))
	void SetOnUIStateChangedDelegate(const FOnUIStateChangedDelegate& OnUIStateChanged);

	//// End Interaction Candidate handles
	//////////////////////////////////////

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
};

