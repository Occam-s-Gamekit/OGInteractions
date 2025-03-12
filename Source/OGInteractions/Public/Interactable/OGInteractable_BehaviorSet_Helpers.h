// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
// #include "OGInteractable_BehaviorSet_Helpers.generated.h"


// Macro in the event you want to make several different BehaviorSets, e.g., to cover the various EnhancedInput trigger types
#define DECLARE_INTERACTION_EVENT_DELEGATE_DEFINITION(DelegateName) \
	void TryExecuteDelegate_##DelegateName(AActor* Interactor) const;

// Macro in the event you want to make several different BehaviorSets, e.g., to cover the various EnhancedInput trigger types
#define DEFINE_INTERACTION_DELEGATE_IMPLEMENTATION(StructName, DelegateName, bRequireDefinition) \
    void StructName::TryExecuteDelegate_OnInteract_##DelegateName(AActor* Interactor) const \
    { \
        if (OnInteract_##DelegateName##Delegate.IsBound()) \
        { \
            return OnInteract_##DelegateName##Delegate.Execute(Interactor); \
        } \
        else if (ensureAlwaysMsgf(!bRequireDefinition, TEXT("##StructName::TryExecuteDelegate_##DelegateName - Delegate for %s has not been set"), *AssociatedComponentId.ToString())) \
		{}\
    };
