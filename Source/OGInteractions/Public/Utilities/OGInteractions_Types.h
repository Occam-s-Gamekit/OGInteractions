#pragma once

DECLARE_LOG_CATEGORY_EXTERN(LogOccamsGamekit_Interactions, Warning, All);

static auto OG_ECC_INTERACTABLE = ECC_GameTraceChannel1;

/*
 * This example is the reason why this system was created. Many games need less thorough interaction conveyance,
 * but in the event you need many potential states, and reasons to have them, this system enables easier handling of it
 *
 * You should create your own definition of EOG_UIState for your own project.
 * Do not take the example below as how it "should" be used, as it is clearly overkill for most games.
 *
 * Feel free to edit this plugin locally with your own EOG_UIStates.
 * I could not find a clean way to template this enum into my components, or to allow extension/subsetting of the enum itself
 */
// UENUM(BlueprintType)
// enum class EOG_UIState : uint8
// {
// 	None,						// No conveyance
//
// 	Disabled,					// Cannot be interacted with (grayed out, if it was a UI button)
// 								// (I would also disable the ability to interact in this case, removing collision from the actor.)
//
// 	Hover,						// Valid hover state (user is overlapping/looking at. This is UI's Mouseover equivalent)
// 								// Unless there's something wrong with the actor, I'd always use this if the user's interaction vector is looking at the interactable
// 								// (Unnecessary_Strong/Invalid_Strong would be as applicable otherwise)
//
// 	Focus,						// Valid focus state (equivalent to a user "clicking" an entry, selecting a row, etc...)
// 								// Generally, use this if your users need to click on something in order to get more specific actions
// 								// E.g., A specific unit in an RTS that has been clicked
//
// 	// In my mind for the below options, "Light" would be used as a default state if you're trying to convey something about an actor.
// 	// Whereas "Strong" would be used if the user is actively trying to interact with something and needs stronger conveyance.
//
// 	// Callout:	Used to draw player attention
// 	//			This is equivalent to the "Enabled" ui state, but we may not want to convey "Enabled" via callouts, as it could clutter the world
// 	//			I prefer "None" for the default "Enabled" state, and use Callouts when trying to draw the player's attention somewher specific
// 	Callout_Light,				// A lighter outline, "enabled".
// 	Callout_Heavy,				// A heavier outline, "You should interact with this".
// 								//	 I would use Hover if the user was actually mouse-overing this, but this callout if I want the user to use this.
//
//
// 	// Unnecessary: This is likely specific only to the project that this system was designed for
// 	//				It implies that you _could_ interact with this, but you don't need to
// 	Unnecessary_Light,			// A lighter, probably yellow, outline.
// 	Unnecessary_Heavy,			// A heavier outline.
//
// 	// Invalid: This _cannot_ be interacted with _right now_ (i.e., not because it's disabled, but because relevant state is not correct)
// 	//			If the user is hovering something Invalid, I may put Strong on that element, and Light on the associate elements causing the issues.
// 	//			This is equivalent to "Invalid" in UI Parlance. Think, "I hit submit, but I didn't enter an email, so the form yells at me"
// 	Invalid_Light,				// a lighter outline, red.
// 	Invalid_Heavy,				// a heavier outline, red.
// };
//
