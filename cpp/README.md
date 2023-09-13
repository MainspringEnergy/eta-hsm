
# `HsmMacros.hpp`
The `HsmMacros` module is at first blush an easier way to declare the HSM states via the following macros:

```cpp
#define ETA_HSM_TOP_STATE(controller, name)
#define ETA_HSM_COMP_STATE(controller, name, parent)
#define ETA_HSM_LEAF_STATE(controller, name, parent)
```

however the real power that these macros bring is it enables a much more streamlined testing procedure.

## How it works
Most of the macro power is within `ETA_HSM_TOP_STATE`. This macro declares the typedefs and sets up a `std::map` of `StateAttributes` so the user can make runtime calls to `directlySetStateForTestingOnly` and `isLeaf` as those "methods" were originally template types which are hard to work with during runtime. The following macros `ETA_HSM_COMP_STATE` and `ETA_HSM_LEAF_STATE` leverage the typedefs and add the state to the `std::map` of `StateAttributes` declared in the top state call.

## Pitfalls
1. You must declare the `ETA_HSM_TOP_STATE` ahead of other states as the top state declares typedefs and the testing `std::map` for the `StateAttributes` which the macros `ETA_HSM_COMP_STATE` and `ETA_HSM_LEAF_STATE` need.

# `testing::HsmTestRunner.hpp`
## Motivation
A big part of testing an HSM is ensuring that the transistions desired are functional **and** the state machine does not perform any action for a given state/event that is not desired. Testing all these scenarios is usually pretty complicated and is very easy to make a generic utility once `update_control` is employed.

## Setup
`HsmTestRunner` allows for a much easier testing process for an `Hsm` based off of `update_control`. A controller/HSM must inherit from the `hsm::StateMachine` and `update_control::DynamicElement*`. The former is obvious but the requirement of `update_control` is because `update_control` provides the ability to construct any controller with just its `Context`. This is essential in order to create a generic test runner. An example of a suitable controller is shown below:

```cpp
class Control
    : public eta::update_control::DynamicElementImpl<Traits, Context>
    , public eta::hsm::StateMachine<Control, StateMachineTraits>;
```

## Use
The test runner should be fairly easy to use. One is only required to create the `std::vector` of expected test transitions. An example is shown below:

```cpp
#include "eta/hsm/testing/HsmTestRunner.hpp"
// include the controller here

#include <vector>

using Transition = eta::hsm::testing::TransitionHsm<Control>;
const std::vector<Transition> eventResponseTests
{
    {StateEnum::eS1, EventEnum::eE1, StateEnum::eS2},
    {StateEnum::eS2, EventEnum::eE3, StateEnum::eS4,
        Transition::ControlUpdater{
            [](Control& control, Input& input)
            {
                input.x = 3;
                control.update(input); // dont forget the update!
            }
        }
    },
    {StateEnum::eS2, EventEnum::eE4, StateEnum::eS3},
};

FEATURE("Control State Machine Event Test", "[bus_control_events]")
{
    runHsmStateTests(eventResponseTests, stateTestAttributesControl);
}

```

## State Guards
The example above also shows a way to setup guard conditions which are a unique scenario where an event can have different ending states based on different `Input`. This is acheived by the `HsmTestRunner` calling the `ControlUpdater` prior to testing the event. Another nice thing about the `Transition` type is that it defaults the `updater` to not update the `Input` and call the control `update` once. This means one does not have to provide an `ControlUpdater` for every `Transition`.

## Pitfalls
1. You cannot test any states other than leaf states as the state machine can never reside in any state other than a leaf state. So if a parent contains the transition then you must **explicitly** test that transition for each child in the parent
1. The state and event enum must follow the pattern of `[ControllerName]StateEnum` or `[ControllerName]EventEnum`. The macros cannot deal with the event or state enums that do not matching this name pattern
1. the second arg to the `runHsmStateTests` must follow the pattern of `stateTestAttributes[ControllerName]`
1. the macros for declaring the state must be given the exact controller name as it is declared.