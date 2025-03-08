#include "Utilities/OGInteractionTags.h"

namespace OccamsGamkit
{
	namespace Interactions
	{
		UE_DEFINE_GAMEPLAY_TAG(Raycast,						"OccamsGamekit.Interactions.Raycast")

		namespace InteractableComponent
		{
			UE_DEFINE_GAMEPLAY_TAG(QueryVolume,				"OccamsGamekit.Interactions.InteractableComponent.QueryVolume") // Used to tag components that are being monitored
			UE_DEFINE_GAMEPLAY_TAG(ComponentId,				"OccamsGamekit.Interactions.InteractableComponent.ComponentId") // Prefix to find IDs
		}
		
		namespace Examples
		{
			namespace UIState
			{
				UE_DEFINE_GAMEPLAY_TAG(None,					"OccamsGamekit.Interactions.Examples.UIState.None");
				UE_DEFINE_GAMEPLAY_TAG(Hover,					"OccamsGamekit.Interactions.Examples.UIState.Hover");
				UE_DEFINE_GAMEPLAY_TAG(Callout,					"OccamsGamekit.Interactions.Examples.UIState.Callout");
				UE_DEFINE_GAMEPLAY_TAG(Invalid,					"OccamsGamekit.Interactions.Examples.UIState.Invalid");
			}

			namespace InputActions
			{
				UE_DEFINE_GAMEPLAY_TAG(Interact_E,				"OccamsGamekit.Interactions.Examples.E");
				UE_DEFINE_GAMEPLAY_TAG(Interact_F,				"OccamsGamekit.Interactions.Examples.F");
			}
		}
	}
}
