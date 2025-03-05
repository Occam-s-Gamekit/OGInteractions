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
	
	// Set your default UIState in your overriden constructor
	// UIState = EOG_UIState::None;
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

	// TODO: Only raycast has been implemented
	// The handlers for these exist to call into the same Hover functions
	// The Raycast implementation directly calls Hover/EndHover from the InteractorComponent
	
	// if (InteractionTriggerType == InteractionSystem::InteractionTrigger::PawnOverlap())
	// {
	// 	GetExpectedOuter()->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOverlapBegin);
	// 	GetExpectedOuter()->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleOverlapEnd);
	//
	// 	// GetExpectedOuter()->SetCollisionResponseToChannel(CG_ECC_INTERACTABLE, ECR_Block);
	// }
	// else if (InteractionTriggerType == InteractionSystem::InteractionTrigger::MouseOver())
	// {
	// 	// https://forums.unrealengine.com/t/use-box-collision-with-cursor-over/375072/2
	// 	GetExpectedOuter()->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
	// 	GetExpectedOuter()->OnBeginCursorOver.AddDynamic(this, &ThisClass::HandleCursorOverBegin);
	// 	GetExpectedOuter()->OnEndCursorOver.AddDynamic(this, &ThisClass::HandleCursorOverEnd);
	//
	// 	// Mouse events occur on visibility channel
	// 	// GetExpectedOuter()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	// }
}

/* This should live on the input component, not here?
// TODO: This should probably be "TryInteract" and return bool
// Then we can pop up or respond to failure from the asker, instead of silently fail here
// Oh, we do. HandleInteractFailed. Huh.
// Ok, well this ends up meaning that I wrap this call with "CanInteract", so we call it twice.
// We should prepend it with "Try" so we know what it's actually doing
void UOGInteractableComponent_Base::Interact(APawn* InteractorPawn)
{
	if (auto AsInteractor = UOGInteractions_FunctionLibrary::GetInteractorComponent(InteractorPawn))
	{
		if (IOGInteractableInterface::Execute_CanInteract(GetOwner(), InteractorPawn, GetExpectedOuter()))
		{
			AsInteractor->SetInteractionFocus(this);
			IOGInteractableInterface::Execute_HandleInteract(GetOwner(), InteractorPawn, GetExpectedOuter());
		}
		else
		{
			AsInteractor->SetInteractionFocus(this);
			IOGInteractableInterface::Execute_HandleInteractFailed(GetOwner(), InteractorPawn, GetExpectedOuter());
		}
	}
}
*/

void UOGInteractableComponent_Base::TriggerHover_Implementation(AActor* InInstigator)
{
	SetUIState(GetHoverState(InInstigator)); // TODO: Why did we have hover prioritized?
}
void UOGInteractableComponent_Base::TriggerHoverEnd_Implementation(AActor* InInstigator)
{
	SetUIState(GetDefaultState());
}

void UOGInteractableComponent_Base::TriggerFocus_Implementation(AActor* InInstigator)
{
	SetUIState(GetFocusState(InInstigator));
}
void UOGInteractableComponent_Base::TriggerFocusEnd_Implementation(AActor* InInstigator)
{
	SetUIState(GetDefaultState());
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

FGameplayTag UOGInteractableComponent_Base::GetHoverState(AActor* Interactor) const
{
	if (ensureMsgf(GetHoverStateDelegate.IsBound(), TEXT("UOGInteractableComponent_Base::GetHoverState - Delagate for %s has not been set"), *GetNameSafe(GetOwner())))
	{
		return GetHoverStateDelegate.Execute(Interactor);
	}
	return FGameplayTag::EmptyTag;
}
void UOGInteractableComponent_Base::SetOnHoverDelegate(const FGetUIStateDelegate& GetUIState_OnHover)
{
	GetHoverStateDelegate = GetUIState_OnHover;
}

FGameplayTag UOGInteractableComponent_Base::GetFocusState(AActor* Interactor) const
{
	if (ensureMsgf(GetFocusStateDelegate.IsBound(), TEXT("UOGInteractableComponent_Base::GetFocusState - Delagate for %s has not been set"), *GetNameSafe(GetOwner())))
	{
		return GetFocusStateDelegate.Execute(Interactor);
	}
	return FGameplayTag::EmptyTag;
}
void UOGInteractableComponent_Base::SetOnFocusDelegate(const FGetUIStateDelegate& GetUIState_OnFocus)
{
	GetFocusStateDelegate = GetUIState_OnFocus;
}

FGameplayTag UOGInteractableComponent_Base::GetDefaultState() const
{
	if (ensureMsgf(GetDefaultStateDelegate.IsBound(), TEXT("UOGInteractableComponent_Base::GetDefaultState - Delagate for %s has not been set"), *GetNameSafe(GetOwner())))
	{
		auto* LocalPC = UOGInteractions_FunctionLibrary::GetLocalPlayerController(this);
		return GetDefaultStateDelegate.Execute(LocalPC ? LocalPC->GetPawn() : nullptr);
	}
	return FGameplayTag::EmptyTag;
}
void UOGInteractableComponent_Base::SetOnReturnToDefaultDelegate(const FGetUIStateDelegate& GetUIState_OnReturnToDefault)
{
	GetDefaultStateDelegate = GetUIState_OnReturnToDefault;
}

void UOGInteractableComponent_Base::SetOnUIStateChangedDelegate(const FOnUIStateChangedDelegate& OnUIStateChanged)
{
	OnUIStateChangedDelegate = OnUIStateChanged;
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

// bool UOGInteractableComponent_Base::CheckUIStatePriority(const EOG_UIState& OriginalState, const EOG_UIState& Priority)
// {
// 	return Priority >= OriginalState;
// }

// void UOGInteractableComponent_Base::SetDefaultStates(EOG_UIState CanInteract, EOG_UIState CannotInteract)
// {
// 	UIState_CanInteract = CanInteract;
// 	UIState_CannotInteract = CannotInteract;
// }

// EOG_UIState UOGInteractableComponent_Base::GetDefaultState(AActor* InInstigator) const
// {
// 	return IOGInteractableInterface::Execute_CanInteract(GetOwner(), InInstigator, GetExpectedOuter())
// 		? UIState_CanInteract
// 		: UIState_CannotInteract;
// }


///////////////////////////////////////////////////////////////////////
/////////////////////Begin Listeners///////////////////////////////////
///////////////////////////////////////////////////////////////////////

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
	// if (GetOwner()->GetClass()->ImplementsInterface(UOGInteractableInterface::StaticClass()))
	// {
	// 	IOGInteractableInterface::Execute_HandleUIStateChange(GetOwner(), GetUIState(), GetExpectedOuter());
	// }
	if (ensureMsgf(OnUIStateChangedDelegate.IsBound(), TEXT("UOGInteractableComponent_Base::OnUIStateChange - Delagate for %s has not been set"), *GetNameSafe(GetOwner())))
	{
		OnUIStateChangedDelegate.Execute(GetUIState());
	}
}

// ==============================================
// ================= BP Exposed =================
// ==============================================

// UCG_InteractableComponent_Mesh::UCG_InteractableComponent_Mesh()
// {
// 	SetCollisionObjectType(ECC_WorldDynamic);
// 	SetCollisionResponseToChannel(CG_ECC_INTERACTABLE, ECR_Block);
// }
//
// void UCG_InteractableComponent_Mesh::BeginPlay()
// {
// 	Super::BeginPlay();
// 	SpawnInteractableComponent();
// }
//
// void UCG_InteractableComponent_Mesh::SpawnInteractableComponent()
// {
// 	InteractableComponent = NewObject<UOGInteractableComponent_Base>(this, UOGInteractableComponent_Base::StaticClass(), FName("InteractableComponentLogic"));
// 	InteractableComponent->Initialize();
// }
//
// UOGInteractableComponent_Base* UCG_InteractableComponent_Mesh::GetInteractableLogicManager_Implementation() const
// {
// 	return InteractableComponent;
// }
//
// // void UCG_InteractableComponent_Mesh::RefreshUIState_Implementation()
// // {
// // 	if (InteractableComponent)
// // 	{
// // 		InteractableComponent->SetUIState(InteractableComponent->GetDefaultState(UCG_FunctionLibrary::GetLocalPlayerController(this)));
// // 	}
// // }
//
// // ======
//
// UCG_InteractableComponent_Box::UCG_InteractableComponent_Box()
// {
// 	SetCollisionObjectType(ECC_WorldDynamic);
// 	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
// 	SetCollisionResponseToAllChannels(ECR_Ignore);
// 	SetCollisionResponseToChannel(CG_ECC_INTERACTABLE, ECR_Block);
// }
//
// void UCG_InteractableComponent_Box::BeginPlay()
// {
// 	Super::BeginPlay();
// 	SpawnInteractableComponent();
// }
//
// void UCG_InteractableComponent_Box::SpawnInteractableComponent()
// {
// 	InteractableComponent = NewObject<UOGInteractableComponent_Base>(this, UOGInteractableComponent_Base::StaticClass(), FName("InteractableComponentLogic"));
// 	InteractableComponent->Initialize();
// }
//
// UOGInteractableComponent_Base* UCG_InteractableComponent_Box::GetInteractableLogicManager_Implementation() const
// {
// 	return InteractableComponent;
// }
//
// /*
// void UCG_InteractableComponent_Box::RefreshUIState_Implementation()
// {
// 	if (InteractableComponent)
// 	{
// 		InteractableComponent->SetUIState(InteractableComponent->GetDefaultState(UCG_FunctionLibrary::GetLocalPlayerController(this)));
// 	}
// }
// */
//
