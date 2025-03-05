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
		
		namespace UIStateExamples
		{
			UE_DEFINE_GAMEPLAY_TAG(None,					"OccamsGamekit.Interactions.UIStateExamples.None");
			UE_DEFINE_GAMEPLAY_TAG(Hover,					"OccamsGamekit.Interactions.UIStateExamples.Hover");
			UE_DEFINE_GAMEPLAY_TAG(Callout,					"OccamsGamekit.Interactions.UIStateExamples.Callout");
			UE_DEFINE_GAMEPLAY_TAG(Invalid,					"OccamsGamekit.Interactions.UIStateExamples.Invalid");
		}
	}
}
