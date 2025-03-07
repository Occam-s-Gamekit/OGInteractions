// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/OGInteractions_FunctionLibrary.h"

#include "Interactor/OGInteractorComponent.h"
#include "Interactor/OGInteractorInterface.h"
#include "Utilities/OGInteractions_Types.h"


APlayerController* UOGInteractions_FunctionLibrary::GetLocalPlayerController(const UObject* WorldContextObject)
{
	const auto* World = WorldContextObject->GetWorld();
	const auto* Player = World ? World->GetFirstLocalPlayerFromController() : nullptr;
	return Player ? Player->PlayerController : nullptr;
}

UOGInteractorComponent* UOGInteractions_FunctionLibrary::GetInteractorComponent(const AActor* Actor)
{
	return GetInteractorComponent(Cast<APawn>(Actor));
}

UOGInteractorComponent* UOGInteractions_FunctionLibrary::GetInteractorComponent(const APlayerController* PlayerController)
{
	return GetInteractorComponent(PlayerController->GetPawn());
}

UOGInteractorComponent* UOGInteractions_FunctionLibrary::GetInteractorComponent(const APawn* Pawn)
{
	if (Pawn && Pawn->Implements<UOGInteractorInterface>())
		return IOGInteractorInterface::Execute_GetInteractorComponent(Pawn);

	ensureAlwaysMsgf(!Pawn, TEXT("OGInteractions_FunctionLibrary::GetInteractorComponent: Pawn %s does not implement IOGInteractor"), *GetNameSafe(Pawn));
	return nullptr;
}
