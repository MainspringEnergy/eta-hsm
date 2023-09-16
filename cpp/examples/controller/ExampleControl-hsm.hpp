// eta/hsm/ExampleControl-hsm.hpp

#pragma once

// *********************** Begin State Machine Defintion in eta::hsm namespace ******************
// Like it or not, C++ requires us to specialize functions in the same namespce that they are declared in
// The only thing that we should be adding below are eventHandler() and during() for each state.
namespace eta {
namespace hsm {

// Do NOT open up eta::example_control namespace in the header

template<>
template<typename Current>
inline void example_control::Top::handleEvent(example_control::ExampleControl& stateMachine, const Current& current, Event event) const
{
    switch (event)
    {
        // We can handle events here if we want them to have default
        // behaviors that can then be overridden in specific states.
        default:
            break;
    }
    return; // Top has no parent
}

template<>
template<typename Current>
inline void example_control::Alive::handleEvent(example_control::ExampleControl& stateMachine, const Current& currentState, Event event) const
{
    TestLog::instance() << "Alive::handleEvent()" << std::endl;
    switch (event)
    {
        case example_control::ExampleEvent::eDrinkBeer:
        {
            // We can cause state transitions based upon events, but we can
            // also take non-transitioning actions as well.
            TestLog::instance() << "Alive and drinking beer!" << std::endl;
            stateMachine.increaseBac(0.025);
            return;
        }
        case example_control::ExampleEvent::eDrinkWiskey:
        {
            // This is the strong stuff, so we take a different action
            TestLog::instance() << "Alive and drinking whiskey!" << std::endl;
            stateMachine.increaseBac(0.05);
            return;
        }
        case example_control::ExampleEvent::eDie:
        {   // Note: order of these case statements does NOT matter as the StateMachine
            //       is only ever dispatched with a single utils.
            // In other words, utils prioritization happens long before we get here.
            TestLog::instance() << "Party over..." << std::endl;
            Transition<Current, ThisState, example_control::Dead> t(stateMachine);
            return;
        }
        case example_control::ExampleEvent::eStartWatch:
        {
            // We can now also set timers to fire events at specified points in the future
            TestLog::instance() << "setting timer to look at watch in 2 units of time" << std::endl;
            stateMachine.eventScheduler().addTimer(example_control::ExampleEvent::eLookAtWatch, kState, std::chrono::milliseconds(2000));
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>
template<typename Current>
inline void example_control::Sober::handleEvent(example_control::ExampleControl& stateMachine, const Current& currentState, Event event) const
{
    TestLog::instance() << "Sober::handleEvent()" << std::endl;
    switch (event)
    {
        // By choosing to handle an utils in the substate, we can override the behavior
        // declared in the superstate (Alive)
        case example_control::ExampleEvent::eDrinkBeer:
        {
            TestLog::instance() << "Sober and drinking beer!" << std::endl;
            stateMachine.increaseBac(0.025);
            if(stateMachine.getBac() >= 0.08)
            {
                Transition<Current, ThisState, example_control::Drunk> t(stateMachine);
            } // entrance actions of new state actually occur when Transition object goes out of scope
            return;
        }
        case example_control::ExampleEvent::eDrinkWiskey:
        {
            TestLog::instance() << "Sober and drinking whiskey!" << std::endl;
            stateMachine.increaseBac(0.05);
            if(stateMachine.getBac() >= 0.08)
            {
                Transition<Current, ThisState, example_control::Drunk> t(stateMachine);
            }
            return;
        }
        case example_control::ExampleEvent::eLookAtWatch:
        {
            TestLog::instance() << "Sober and looking at watch" << std::endl;
            Transition<Current, ThisState, example_control::Bored> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>
template<typename Current>
inline void example_control::Drunk::handleEvent(example_control::ExampleControl& stateMachine, const Current& currentState, Event event) const
{
    TestLog::instance() << "Drunk::handleEvent()" << std::endl;
    switch (event)
    {
        // By choosing to handle an utils in the substate, we can override the behavior
        // declared in the superstate (Alive)
        case example_control::ExampleEvent::eLookAtWatch:
        {
            TestLog::instance() << "Drunk and looking at watch" << std::endl;
            TestLog::instance() << "Keep partying..." << std::endl;
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

// We don't have to implement State::handleEvent at all if it does not do anything different than superstate
// e.g. Bored::handleEvent() just does the same (default) behavior implemented in Alive::handleEvent()
// e.g. Dead::handleEvent() doesn't have to do anything at all!


// We do have to declare the initialization behavior of any composite states
// This can optionally take additional acctions, but at a minimum, they must declare
// initialization of their default substate.
// Note: They must be declared in bottom-up order.
template<> inline void example_control::Alive::init(example_control::ExampleControl& stateMachine)
{
    // This declares which substate we default into
    Init<example_control::Sober> i(stateMachine);
    TestLog::instance() << "init_Alive" << std::endl;
}

template<> inline void example_control::Top::init(example_control::ExampleControl& stateMachine)
{
    // This declares which substate we default into
    Init<example_control::Alive> i(stateMachine);
    TestLog::instance() << "init_Top" << std::endl;
}

// Entry, Exit, and During actions can now be given a more useful default behavior by passing an `hsm::DefaultActions`
// as an optional parameter to `ExampleControl`, so we do not need to implement them all here.
// We still have the option of specializing some of them by specific state here if we want to.
// template<> inline void Dead::during(ExampleControl& stateMachine) const { stateMachine.doSomethingDifferent(); }

} // namespace hsm
} // namespace eta
