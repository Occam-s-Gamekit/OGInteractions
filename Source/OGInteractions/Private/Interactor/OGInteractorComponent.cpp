// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactor/OGInteractorComponent.h"

#include "Camera/CameraComponent.h"
#include "Interactable/OGInteractableComponent_Base.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utilities/OGInteractions_Types.h"
#include "Utilities/OGInteractionTags.h"


UOGInteractorComponent::UOGInteractorComponent()
{
	// Only tick for Raycast Interactions
	PrimaryComponentTick.bCanEverTick = InteractionTriggerType == OccamsGamkit::Interactions::Raycast;
}

void UOGInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Only Raycast locally
	auto* Owner = Cast<APawn>(GetOwner());
	if (Owner && Owner->IsLocallyControlled())
	{
		if (const UCameraComponent* OwnerCamera = GetOwner()->FindComponentByClass<UCameraComponent>())
		{
			const FVector StartTrace = OwnerCamera->GetComponentLocation();
			const FVector EndTrace = StartTrace + (OwnerCamera->GetForwardVector() * RaycastRange);
			FHitResult HitResult;
			
			if (UKismetSystemLibrary::LineTraceSingle(
				GetWorld(), StartTrace, EndTrace, UEngineTypes::ConvertToTraceType(OG_ECC_INTERACTABLE), false, {},
				EDrawDebugTrace::None, HitResult, true
			))
			{
				if (HitResult.Component->ComponentHasTag(OccamsGamkit::Interactions::InteractableComponent::QueryVolume.GetTag().GetTagName()))
				{
					// If a component is tagged with the QueryVolume tag, as above, it should also have an ID tag.
					auto IdTag = HitResult.Component->ComponentTags.FindByPredicate([](FName Tag)
					{
						return Tag.ToString().StartsWith(OccamsGamkit::Interactions::InteractableComponent::ComponentId.GetTag().ToString());
					});

					if (IdTag)
					{
						auto FoundComps = HitResult.GetActor()->GetComponentsByTag(UOGInteractableComponent_Base::StaticClass(), *IdTag);
						
						if (UOGInteractableComponent_Base* AsInteractableComp = FoundComps.IsValidIndex(0)
							? Cast<UOGInteractableComponent_Base>(FoundComps[0])
							: nullptr)
						{
							SetInteractionCandidate(AsInteractableComp);
						}
					}
				}
				else if (InteractionCandidate)
				{
					// Hit result exists, and isn't interactable
					ClearInteractionCandidate();
				}
			}
			else if (InteractionCandidate)
			{
				// No hit result
				ClearInteractionCandidate();
			}
		}
	}
}

void UOGInteractorComponent::SetInteractionFocus(UOGInteractableComponent_Base* NewInteractable)
{
	const bool bAreSame = InteractionFocus == NewInteractable;
	if (InteractionFocus && !bAreSame)
	{
		InteractionFocus->TriggerFocusEnd(GetOwner());
	}
	if (!bAreSame)
	{
		InteractionFocus = NewInteractable;
		InteractionFocus->TriggerFocus(GetOwner());
	}
}

void UOGInteractorComponent::RemoveInteractionFocus(UOGInteractableComponent_Base* OldInteractable)
{
	if (InteractionFocus == OldInteractable)
	{
		InteractionFocus->TriggerFocusEnd(GetOwner());
		InteractionFocus = nullptr;
	}
}

UOGInteractableComponent_Base* UOGInteractorComponent::GetInteractionFocus() const
{
	return InteractionFocus;
}

void UOGInteractorComponent::ClearInteractionFocus()
{
	if (InteractionFocus)
	{
		InteractionFocus->TriggerFocusEnd(GetOwner());
		InteractionFocus = nullptr;
	}
}

void UOGInteractorComponent::SetInteractionCandidate(UOGInteractableComponent_Base* NewInteractable)
{
	const bool bAreSame = InteractionCandidate == NewInteractable;
	if (InteractionCandidate && !bAreSame)
	{
		InteractionCandidate->TriggerHoverEnd(GetOwner());
	}
	if (!bAreSame)
	{
		InteractionCandidate = NewInteractable;
		InteractionCandidate->TriggerHover(GetOwner());
	}
}

void UOGInteractorComponent::RemoveInteractionCandidate(UOGInteractableComponent_Base* OldInteractable)
{
	if (InteractionCandidate == OldInteractable)
	{
		InteractionCandidate->TriggerHoverEnd(GetOwner());
		InteractionCandidate = nullptr;
	}
}

UOGInteractableComponent_Base* UOGInteractorComponent::GetInteractionCandidate() const
{
	return InteractionCandidate;
}

void UOGInteractorComponent::ClearInteractionCandidate()
{
	if (InteractionCandidate)
	{
		InteractionCandidate->TriggerHoverEnd(GetOwner());
		InteractionCandidate = nullptr;
	}
}
