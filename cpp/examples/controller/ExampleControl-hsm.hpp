// eta/hsm/ExampleControl-hsm.hpp

#pragma once

// *********************** Begin State Machine Defintion in eta::hsm namespace ******************
// Like it or not, C++ requires us to specialize functions in the same namespce that they are declared in
// The only thing that we should be adding below are eventHandler() and during() for each state.
namespace eta_hsm {

// Do NOT open up eta::example_control namespace in the header

template <>
template <typename Current>
inline void examples::controller::Top::handleEvent(examples::controller::ExampleControl& stateMachine,
                                                   const Current& current, Event event) const
{
    switch (event)
    {
        // We can handle events here if we want them to have default
        // behaviors that can then be overridden in specific states.
        default:
            break;
    }
    return;  // Top has no parent
}

template <>
template <typename Current>
inline void examples::controller::Alive::handleEvent(examples::controller::ExampleControl& stateMachine,
                                                     const Current& currentState, Event event) const
{
    utils::TestLog::instance() << "Alive::handleEvent()" << std::endl;
    switch (event)
    {
        case examples::controller::ExampleEvent::eDrinkBeer:
        {
            // We can cause state transitions based upon events, but we can
            // also take non-transitioning actions as well.
            utils::TestLog::instance() << "Alive and drinking beer!" << std::endl;
            stateMachine.increaseBac(0.025);
            return;
        }
        case examples::controller::ExampleEvent::eDrinkWiskey:
        {
            // This is the strong stuff, so we take a different action
            utils::TestLog::instance() << "Alive and drinking whiskey!" << std::endl;
            stateMachine.increaseBac(0.05);
            return;
        }
        case examples::controller::ExampleEvent::eDie:
        {  // Note: order of these case statements does NOT matter as the
           // StateMachine
            //       is only ever dispatched with a single utils.
            // In other words, utils prioritization happens long before we get here.
            utils::TestLog::instance() << "Party over..." << std::endl;
            Transition<Current, ThisState, examples::controller::Dead> t(stateMachine);
            return;
        }
        case examples::controller::ExampleEvent::eStartWatch:
        {
            // We can now also set timers to fire events at specified points in the future
            utils::TestLog::instance() << "setting timer to look at watch in 2 units of time" << std::endl;
            stateMachine.eventScheduler().addTimer(examples::controller::ExampleEvent::eLookAtWatch, kState,
                                                   std::chrono::milliseconds(2000));
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template <>
template <typename Current>
inline void examples::controller::Sober::handleEvent(examples::controller::ExampleControl& stateMachine,
                                                     const Current& currentState, Event event) const
{
    utils::TestLog::instance() << "Sober::handleEvent()" << std::endl;
    switch (event)
    {
        // By choosing to handle an utils in the substate, we can override the behavior
        // declared in the superstate (Alive)
        case examples::controller::ExampleEvent::eDrinkBeer:
        {
            utils::TestLog::instance() << "Sober and drinking beer!" << std::endl;
            stateMachine.increaseBac(0.025);
            if (stateMachine.getBac() >= 0.08)
            {
                Transition<Current, ThisState, examples::controller::Drunk> t(stateMachine);
            }  // entrance actions of new state actually occur when Transition object goes out of scope
            return;
        }
        case examples::controller::ExampleEvent::eDrinkWiskey:
        {
            utils::TestLog::instance() << "Sober and drinking whiskey!" << std::endl;
            stateMachine.increaseBac(0.05);
            if (stateMachine.getBac() >= 0.08)
            {
                Transition<Current, ThisState, examples::controller::Drunk> t(stateMachine);
            }
            return;
        }
        case examples::controller::ExampleEvent::eLookAtWatch:
        {
            utils::TestLog::instance() << "Sober and looking at watch" << std::endl;
            Transition<Current, ThisState, examples::controller::Bored> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template <>
template <typename Current>
inline void examples::controller::Drunk::handleEvent(examples::controller::ExampleControl& stateMachine,
                                                     const Current& currentState, Event event) const
{
    utils::TestLog::instance() << "Drunk::handleEvent()" << std::endl;
    switch (event)
    {
        // By choosing to handle an utils in the substate, we can override the behavior
        // declared in the superstate (Alive)
        case examples::controller::ExampleEvent::eLookAtWatch:
        {
            utils::TestLog::instance() << "Drunk and looking at watch" << std::endl;
            utils::TestLog::instance() << "Keep partying..." << std::endl;
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
template <>
inline void examples::controller::Alive::init(examples::controller::ExampleControl& stateMachine)
{
    // This declares which substate we default into
    Init<examples::controller::Sober> i(stateMachine);
    utils::TestLog::instance() << "init_Alive" << std::endl;
}

template <>
inline void examples::controller::Top::init(examples::controller::ExampleControl& stateMachine)
{
    // This declares which substate we default into
    Init<examples::controller::Alive> i(stateMachine);
    utils::TestLog::instance() << "init_Top" << std::endl;
}

// Entry, Exit, and During actions can now be given a more useful default behavior by passing an `hsm::DefaultActions`
// as an optional parameter to `ExampleControl`, so we do not need to implement them all here.
// We still have the option of specializing some of them by specific state here if we want to.
// template<> inline void Dead::during(ExampleControl& stateMachine) const { stateMachine.doSomethingDifferent(); }

}  // namespace eta_hsm
