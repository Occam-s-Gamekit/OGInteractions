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

UOGInteractorComponent* UOGInteractions_FunctionLibrary::GetInteractorComponent(AActor* Actor)
{
	return GetInteractorComponent(Cast<APawn>(Actor));
}

UOGInteractorComponent* UOGInteractions_FunctionLibrary::GetInteractorComponent(APlayerController* PlayerController)
{
	return GetInteractorComponent(PlayerController->GetPawn());
}

UOGInteractorComponent* UOGInteractions_FunctionLibrary::GetInteractorComponent(APawn* Pawn)
{
	if (const auto* AsInteractor = Cast<IOGInteractorInterface>(Pawn))
		return AsInteractor->GetInteractorComponent();

	UE_LOG(LogOccamsGamekit_Interactions, Warning, TEXT("OGInteractions_FunctionLibrary::GetInteractorComponent: Pawn does not implement IOGInteractor"));
	return nullptr;
}
