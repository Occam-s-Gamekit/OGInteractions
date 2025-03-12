// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OGInteractorInterface.generated.h"

UINTERFACE(MinimalAPI)
class UOGInteractorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Expected on the PlayerCharacter
 * Needed for Interactable interactions to work
 */
class OGINTERACTIONS_API IOGInteractorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	class UOGInteractorComponent* GetInteractorComponent() const;
};
