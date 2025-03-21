﻿#pragma once
#include "NativeGameplayTags.h"

namespace OccamsGamkit
{
	namespace Interactions
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Raycast);

		namespace InteractableComponent
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(QueryVolume);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ComponentId);
		}

		namespace Examples
		{
			namespace UIState
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(None);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hover);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Callout);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Invalid);
			}

			namespace InputActions
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Interact_Primary);
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Interact_Secondary);
			}
		}
	}
}
