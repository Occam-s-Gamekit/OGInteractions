// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/OGInteractableComponent_Base.h"

#include "Components/ShapeComponent.h"
#include "Interactor/OGInteractorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Utilities/OGInteractions_FunctionLibrary.h"
#include "Utilities/OGInteractions_Types.h"

UOGInteractableComponent_Base::UOGInteractableComponent_Base()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UOGInteractableComponent_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOGInteractableComponent_Base, bDisabled);
}

void UOGInteractableComponent_Base::Initialize(FName Id, UShapeComponent* InQueryVolume, UMeshComponent* InPhysicalRepresentation,
	const FOGInteractableComponent_VisualDelegates& VisualDelegates
) {
	// These are initialized via inputs always
	QueryVolume = InQueryVolume;
	PhysicalRepresentation = InPhysicalRepresentation;

	ComponentId = Id;
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

		if (PhysicalRepresentation)
		{
			PhysicalRepresentation->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		}
	}
	else if (InPhysicalRepresentation)
	{
		PhysicalRepresentation->SetCollisionResponseToChannel(OG_ECC_INTERACTABLE, ECR_Block);
		
		PhysicalRepresentation->ComponentTags.Add(OccamsGamkit::Interactions::InteractableComponent::QueryVolume.GetTag().GetTagName());
		PhysicalRepresentation->ComponentTags.AddUnique(IdToUse);
	}

	ensureAlwaysMsgf(QueryVolume || PhysicalRepresentation, TEXT("UOGInteractableComponent_Base::Initialize - Needs either a volume or a mesh"));

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
	InitializeDelegates(
		VisualDelegates.OnDisabledChanged,
		VisualDelegates.OnUIStateChanged,
		VisualDelegates.GetUIState_OnHover,
		VisualDelegates.GetUIState_OnFocus,
		VisualDelegates.GetUIState_DefaultState
	);

	WhenInitialized->Fulfill();
}

void UOGInteractableComponent_Base::InitializeDelegates(
	FOnChangeStateNotificationDelegate OnDisabledChanged, FOnUIStateChangedDelegate OnUIStateChanged, FGetUIStateDelegate GetUIState_OnHover, FGetUIStateDelegate GetUIState_OnFocus, FGetUIStateDelegate GetUIState_DefaultState)
{
	if (OnDisabledChanged.IsBound())			{ SetOnDisabledChangedDelegate(OnDisabledChanged); }
	if (OnUIStateChanged.IsBound())				{ SetOnUIStateChangedDelegate(OnUIStateChanged); }
	if (GetUIState_OnHover.IsBound())			{ SetOnHoverDelegate(GetUIState_OnHover); }
	if (GetUIState_OnFocus.IsBound())			{ SetOnFocusDelegate(GetUIState_OnFocus); }
	if (GetUIState_DefaultState.IsBound())		{ SetGetDefaultStateDelegate(GetUIState_DefaultState); }

	if (GetDefaultStateDelegate.IsBound() && OnUIStateChangedDelegate.IsBound())
	{
		TriggerUIStateDefaultRefresh();
	}
}

void UOGInteractableComponent_Base::TriggerHover(const AActor* InInstigator)
{
	SetUIState(GetHoverStateFor(InInstigator)); // TODO: Why did we have hover prioritized?
}
void UOGInteractableComponent_Base::TriggerHoverEnd(const AActor* InInstigator)
{
	SetUIState(GetDefaultStateForLocalPlayer());
}

void UOGInteractableComponent_Base::TriggerFocus(const AActor* InInstigator)
{
	SetUIState(GetFocusStateFor(InInstigator));
}
void UOGInteractableComponent_Base::TriggerFocusEnd(const AActor* InInstigator)
{
	SetUIState(GetDefaultStateForLocalPlayer());
}

void UOGInteractableComponent_Base::TriggerUIStateDefaultRefresh()
{
	// This should check local player for Candidate/Focus and then Set to the correct state
	const auto* LocalPlayer = UOGInteractions_FunctionLibrary::GetLocalPlayerController(this);
	const auto* LocalPawn = LocalPlayer ? LocalPlayer->GetPawn() : nullptr;
	if (const auto* Interactor = UOGInteractions_FunctionLibrary::GetInteractorComponent(LocalPawn))
	{
		if (Interactor->GetInteractionFocus() == this)
		{
			SetUIState(GetFocusStateFor(LocalPawn));
		}
		else if (Interactor->GetInteractionCandidate() == this)
		{
			SetUIState(GetHoverStateFor(LocalPawn));
		}
		else
		{
			SetUIState(GetDefaultStateForLocalPlayer());
		}
	}
}

void UOGInteractableComponent_Base::SetDisabled_Implementation(bool bInDisabled)
{
	if (bInDisabled == bDisabled)
		return;

	bDisabled = bInDisabled;
	if (GetOwnerRole() == ROLE_Authority)
	{
		OnRep_OnDisabledChanged();
	}
}

FGameplayTag UOGInteractableComponent_Base::GetHoverStateFor(const AActor* Interactor) const
{
	return TryExecuteGetterDelegate(OnHoverDelegate, Interactor, "OnHover");
}

void UOGInteractableComponent_Base::SetOnHoverDelegate(const FGetUIStateDelegate& GetUIState_OnHover)
{
	OnHoverDelegate = GetUIState_OnHover;
}

FGameplayTag UOGInteractableComponent_Base::GetFocusStateFor(const AActor* Interactor) const
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
	return TryExecuteGetterDelegate(GetDefaultStateDelegate, LocalPC ? LocalPC->GetPawn() : nullptr, "GetDefaultState");
}
void UOGInteractableComponent_Base::SetGetDefaultStateDelegate(const FGetUIStateDelegate& GetUIState_DefaultState)
{
	GetDefaultStateDelegate = GetUIState_DefaultState;
}

void UOGInteractableComponent_Base::SetOnUIStateChangedDelegate(const FOnUIStateChangedDelegate& OnUIStateChanged)
{
	OnUIStateChangedDelegate = OnUIStateChanged;
}

void UOGInteractableComponent_Base::SetOnDisabledChangedDelegate(const FOnChangeStateNotificationDelegate& OnDisabledChanged)
{
	OnDisabledChangedDelegate = OnDisabledChanged;
}

const FGameplayTag& UOGInteractableComponent_Base::SetUIState(const FGameplayTag& NewState)
{
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
	
FGameplayTag UOGInteractableComponent_Base::TryExecuteGetterDelegate(const FGetUIStateDelegate& InDelegate, const AActor* Interactor, FString CallingFunction) const
{
if (ensureAlwaysMsgf(InDelegate.IsBound(), TEXT("UOGInteractableComponent_Base::GetterDelegate - %s Delegate for %s has not been set"), *CallingFunction, *GetNameSafe(this)))
	{
		return InDelegate.Execute(Interactor);
	}
	return FGameplayTag::EmptyTag;
}


void UOGInteractableComponent_Base::OnRep_OnDisabledChanged()
{
	UPrimitiveComponent* InteractionQueryTarget = QueryVolume;
	if (!InteractionQueryTarget) { InteractionQueryTarget = PhysicalRepresentation; }

	if (InteractionQueryTarget)
	{
		if (bDisabled)
		{
			InteractionQueryTarget->SetCollisionResponseToChannel(OG_ECC_INTERACTABLE, ECR_Ignore);
		}
		else
		{
			InteractionQueryTarget->SetCollisionResponseToChannel(OG_ECC_INTERACTABLE, ECR_Block);
		}
	}

	if (OnDisabledChangedDelegate.IsBound())
	{
		OnDisabledChangedDelegate.Execute(bDisabled);
	}
	TriggerUIStateDefaultRefresh();
}

//// End Helpers
////////////////////////////////////////
