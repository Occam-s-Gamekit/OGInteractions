﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "OGInteractions_FunctionLibrary.generated.h"

class UOGInteractorComponent;

/**
 * 
 */
UCLASS()
class OGINTERACTIONS_API UOGInteractions_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="PlayerController", meta=(DefaultToSelf="WorldContextObject"))
	static APlayerController* GetLocalPlayerController(const UObject* WorldContextObject);

    static UOGInteractorComponent* GetInteractorComponent(AActor* Actor);
    static UOGInteractorComponent* GetInteractorComponent(APlayerController* PlayerController);
    static UOGInteractorComponent* GetInteractorComponent(APawn* Pawn);
};
