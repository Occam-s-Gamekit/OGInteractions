// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/OGInteractableComponent_Base.h"

#include "Components/ShapeComponent.h"
#include "Interactable/OGInteractableInterface.h"
#include "Interactor/OGInteractorComponent.h"
#include "Utilities/OGInteractions_FunctionLibrary.h"
#include "Utilities/OGInteractions_Types.h"

UOGInteractableComponent_Base::UOGInteractableComponent_Base()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UOGInteractableComponent_Base::Initialize(FName Id, UShapeComponent* InQueryVolume, UMeshComponent* InPhysicalRepresentation)
{
	// We expect the actor that owns the InteractableComponent to implement the CGInteractable interface, this ensures the component can use any of the interface functions on any actor
	ensure(GetOwner() && GetOwner()->Implements<UOGInteractableInterface>());

	// These are initialized via inputs always
	QueryVolume = InQueryVolume;
	PhysicalRepresentation = InPhysicalRepresentation;
	
	const FString IdPrefix = OccamsGamkit::Interactions::InteractableComponent::ComponentId.GetTag().GetTagName().ToString();
	const FString IdString = FString::Printf(TEXT("%s_%s"), *IdPrefix, *Id.ToString());
	const FName IdToUse = FName(*IdString);
	ComponentTags.AddUnique(IdToUse);

	// We only set one of the two to be queryable
	if (InQueryVolume)
	{
		QueryVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		QueryVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
		QueryVolume->SetCollisionResponseToChannel(OG_ECC_INTERACTABLE, ECR_Block);

		QueryVolume->ComponentTags.Add(OccamsGamkit::Interactions::InteractableComponent::QueryVolume.GetTag().GetTagName());
		QueryVolume->ComponentTags.AddUnique(IdToUse);
	}
	else if (InPhysicalRepresentation)
	{
		PhysicalRepresentation->SetCollisionResponseToChannel(OG_ECC_INTERACTABLE, ECR_Block);
		
		PhysicalRepresentation->ComponentTags.Add(OccamsGamkit::Interactions::InteractableComponent::QueryVolume.GetTag().GetTagName());
		PhysicalRepresentation->ComponentTags.AddUnique(IdToUse);
	}

	/*
	 * TODO: Only raycast has been implemented
	 * The handlers for these exist to call into the same Hover functions
	 * The Raycast implementation directly calls Hover/EndHover from the InteractorComponent

	if (InteractionTriggerType == InteractionSystem::InteractionTrigger::PawnOverlap())
	{
	}
	else if (InteractionTriggerType == InteractionSystem::InteractionTrigger::MouseOver())
	{
		// https://forums.unrealengine.com/t/use-box-collision-with-cursor-over/375072/2
	}
	*/
}

// TODO: This is a basic interact, but we may want to designate a separate path for different keys
// Should this implementation...
// Well, what does "interact" mean to an actor?
bool UOGInteractableComponent_Base::TryInteract(AActor* Interactor)
{
	if (CanInteract(Interactor))
	{
		TryExecuteInteractEventDelegate(OnInteractDelegate, Interactor, "OnInteract");
		return true;
	}
	
	TryExecuteInteractEventDelegate(OnInteractFailedDelegate, Interactor, "OnInteractFailed");
	return false;
}

void UOGInteractableComponent_Base::TriggerHover_Implementation(AActor* InInstigator)
{
	SetUIState(GetHoverStateFor(InInstigator)); // TODO: Why did we have hover prioritized?
}
void UOGInteractableComponent_Base::TriggerHoverEnd_Implementation(AActor* InInstigator)
{
	SetUIState(GetDefaultStateForLocalPlayer());
}

void UOGInteractableComponent_Base::TriggerFocus_Implementation(AActor* InInstigator)
{
	SetUIState(GetFocusStateFor(InInstigator));
}
void UOGInteractableComponent_Base::TriggerFocusEnd_Implementation(AActor* InInstigator)
{
	SetUIState(GetDefaultStateForLocalPlayer());
}

void UOGInteractableComponent_Base::SetDisabled_Implementation(bool bInDisabled)
{
	bDisabled = bInDisabled;
	// if (bDisabled)
	// {
	// 	SetUIState(EOG_UIState::Disabled);
	// }
	// else
	// {
	// 	SetUIState(EOG_UIState::None);
	// }
}

// void UOGInteractableComponent_Base::SetHoverStateFunction(TFunction<EOG_UIState()> InFunction)
// {
// 	GetHoverStateDelegate.
// 	//  = InFunction;
// 	// GetHoverStateDelegate.BindLambda([InFunction]()
// 	// {
// 	// 	return InFunction();
// 	// });
// }

FGameplayTag UOGInteractableComponent_Base::GetHoverStateFor(AActor* Interactor) const
{
	return TryExecuteGetterDelegate(OnHoverDelegate, Interactor, "OnHover");
}

void UOGInteractableComponent_Base::SetOnHoverDelegate(const FGetUIStateDelegate& GetUIState_OnHover)
{
	OnHoverDelegate = GetUIState_OnHover;
}

FGameplayTag UOGInteractableComponent_Base::GetFocusStateFor(AActor* Interactor) const
{
	return TryExecuteGetterDelegate(OnFocusDelegate, Interactor, "OnFocus");
}
void UOGInteractableComponent_Base::SetOnFocusDelegate(const FGetUIStateDelegate& GetUIState_OnFocus)
{
	OnFocusDelegate = GetUIState_OnFocus;
}

FGameplayTag UOGInteractableComponent_Base::GetDefaultStateForLocalPlayer() const
{
	const auto* LocalPC = UOGInteractions_FunctionLibrary::GetLocalPlayerController(this);
	return TryExecuteGetterDelegate(OnReturnToDefaultDelegate, LocalPC ? LocalPC->GetPawn() : nullptr, "GetDefaultState");
}
void UOGInteractableComponent_Base::SetOnReturnToDefaultDelegate(const FGetUIStateDelegate& GetUIState_OnReturnToDefault)
{
	OnReturnToDefaultDelegate = GetUIState_OnReturnToDefault;
}

void UOGInteractableComponent_Base::SetOnUIStateChangedDelegate(const FOnUIStateChangedDelegate& OnUIStateChanged)
{
	OnUIStateChangedDelegate = OnUIStateChanged;
}

bool UOGInteractableComponent_Base::CanInteract(const AActor* Interactor) const
{
	if (ensureAlwaysMsgf(CanInteractDelegate.IsBound(), TEXT("UOGInteractableComponent_Base::CanInteract - Delegate for %s has not been set"), *GetNameSafe(GetOwner())))
	{
		return CanInteractDelegate.Execute(Interactor);
	}
	return false;
}
void UOGInteractableComponent_Base::SetCanInteractDelegate(const FCanInteractDelegate& CanInteract)
{
	CanInteractDelegate = CanInteract;
}

void UOGInteractableComponent_Base::SetOnInteractDelegate(const FOnInteractionEventDelegate& OnInteract)
{
	OnInteractDelegate = OnInteract;
}

void UOGInteractableComponent_Base::SetOnInteractFailedDelegate(const FOnInteractionEventDelegate& OnInteractFailed)
{
	OnInteractFailedDelegate = OnInteractFailed;
}

const FGameplayTag& UOGInteractableComponent_Base::SetUIState(const FGameplayTag& NewState)
{
	// if (bDisabled)
	// {
	// 	if (GetUIState() != EOG_UIState::Disabled)
	// 	{
	// 		UIState = EOG_UIState::Disabled;
	// 	}
	// }
	// TODO: What was the case where we had this check-state? Hover doesn't override Focus?
	// You shouldn't be able to HOVER on DISABLED
	// else if (CheckUIStatePriority(UIState, Priority))
	// {
	if (UIState != NewState)
	{
		UIState = NewState;
		OnUIStateChange();
	}
	return UIState;
}


////////////////////////////////////////
///// Begin Listeners

void UOGInteractableComponent_Base::HandleOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (auto* AsInteractor = UOGInteractions_FunctionLibrary::GetInteractorComponent(OtherActor))
	{
		AsInteractor->SetInteractionCandidate(this);
	}
}

void UOGInteractableComponent_Base::HandleOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (auto* AsInteractor = UOGInteractions_FunctionLibrary::GetInteractorComponent(OtherActor))
	{
		AsInteractor->RemoveInteractionCandidate(this);
	}
}

void UOGInteractableComponent_Base::HandleCursorOverBegin(UPrimitiveComponent* TouchedComponent)
{
	if (auto* AsInteractor = UOGInteractions_FunctionLibrary::GetInteractorComponent(UOGInteractions_FunctionLibrary::GetLocalPlayerController(this)))
	{
		AsInteractor->SetInteractionCandidate(this);
	}
}

void UOGInteractableComponent_Base::HandleCursorOverEnd(UPrimitiveComponent* TouchedComponent)
{
	if (auto* AsInteractor = UOGInteractions_FunctionLibrary::GetInteractorComponent(UOGInteractions_FunctionLibrary::GetLocalPlayerController(this)))
	{
		AsInteractor->RemoveInteractionCandidate(this);
	}
}

void UOGInteractableComponent_Base::OnUIStateChange() const
{
	if (ensureAlwaysMsgf(OnUIStateChangedDelegate.IsBound(), TEXT("UOGInteractableComponent_Base::OnUIStateChange - Delegate for %s has not been set"), *GetNameSafe(this)))
	{
		OnUIStateChangedDelegate.Execute(GetUIState());
	}
}

///// End Listeners
////////////////////////////////////////

////////////////////////////////////////
//// Begin Helpers
	
FGameplayTag UOGInteractableComponent_Base::TryExecuteGetterDelegate(const FGetUIStateDelegate& InDelegate, AActor* Interactor, FString CallingFunction) const
{
	if (ensureAlwaysMsgf(InDelegate.IsBound(), TEXT("UOGInteractableComponent_Base::GetterDelegate - %s Delegate for %s has not been set"), *CallingFunction, *GetNameSafe(this)))
	{
		return InDelegate.Execute(Interactor);
	}
	return FGameplayTag::EmptyTag;
}

void UOGInteractableComponent_Base::TryExecuteInteractEventDelegate(const FOnInteractionEventDelegate& InDelegate, AActor* Interactor, FString CallingFunction) const
{
	if (ensureAlwaysMsgf(InDelegate.IsBound(), TEXT("UOGInteractableComponent_Base::TryExecuteInteractEvent - %s Delegate for %s has not been set"), *CallingFunction, *GetNameSafe(this)))
	{
		InDelegate.Execute(Interactor);
	}
}

//// End Helpers
////////////////////////////////////////
