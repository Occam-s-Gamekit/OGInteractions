# OGInteractions
This system is meant to allow multiple interactions per actor,
as well as to allow a high ease of declaring the conveyance of the interaction.

It is composed of a few components:

### InteractorComponent
Add this to your Player Character, and implement the InteractorInterface

### InteractableComponent
This component drives the Client-side visual representation of your interactable.

- You must call `Initialize` on it in order for the interactable to function. This ties this component to an actor.
- You may call `InitializeDelegates` in order to define the visual handlers separately from Initializing onto an actor.
For instance you may wish to do this in order to extend this component into a BP version that handles default Visuals
- The Base component does _not_ include any behavior handling

#### OGInteractableComponent_DevelopmentInputPassthrough
This got named "DevelopmentInputPassthrough" as I was unsure how I felt about having the Actor/Components bind to an input action tag.
The more I thought about it, however, I think it is ok, as this system is for Engineers who will create an API for designers to interact with.
As such, I'm using a version similar to this in my own code:
- In C++ I handle any bespoke logic (e.g., I have some logic for ongoing interactions)
- In a BP class I implement the default visual handlers
- In a BP Base class (e.g., InteractableActor) I init my Interactable component and bind functions to the correct delegates
  - From here, I can create BP Classes that extend this base class. This hides the implementation details of the interactable system from downstream users.
It also means that downstream users can override the functions you've already bound.

The examples in the level/included in this repo don't fully show this flow, rather each one inits on its own and makes the whole process appear more complex
than it necessarily is. However how I'm using this system is still a WIP, so if you want to use this repo let me know and I'll show you my current best practices.
