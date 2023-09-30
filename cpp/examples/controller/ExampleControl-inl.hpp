#pragma once

#include "../../utils/TestLog.hpp"

namespace eta_hsm {
namespace examples {
namespace controller {

/// Templatized state-specific update functions
template <>
inline void ExampleControl::stateUpdate<ExampleState::eSober>()
{
    // Can use mpInput-> here to access inputs
    utils::TestLog::instance() << "stateUpdate<eSober> " << std::endl;

    // Possibly check for fault-like situations
    if (getBac() > 0.35)
    {  // We can queue events directly from within the state machine if we want to
        addEvent(ExampleEvent::ePassOut);
    }
    // Do something reasonable
    increaseBac(-0.01);  // metabolize alcohol
}

template <>
inline void ExampleControl::stateUpdate<ExampleState::eDrunk>()
{
    // Can use mpInput-> here to access inputs
    utils::TestLog::instance() << "stateUpdate<eDrunk> " << std::endl;

    // Possibly check for fault-like situations
    if (getBac() > 0.35)
    {
        addEvent(ExampleEvent::ePassOut);
    }
    // Do something unreasonable
    increaseBac(-0.01);  // metabolize alcohol
}

template <>
inline void ExampleControl::stateUpdate<ExampleState::eUnconcious>()
{
    utils::TestLog::instance() << "stateUpdate<eDead> " << std::endl;
    // do nothing...
}

template <>
inline void ExampleControl::stateUpdate<ExampleState::eBored>()
{
    utils::TestLog::instance() << "stateUpdate<eBored> " << std::endl;
    // do nothing...
}

template <>
inline void ExampleControl::entry<ExampleState::eAwake>()
{
    // Yes, we can also now test awake-ness by querying state, but I'm keeping this around as an example
    // of how to manipulate controller-continuous-state with state-specialized entry/exit functions.
    utils::TestLog::instance() << "enter_Awake " << std::endl;
    mAccumultedEntryExit += static_cast<int>(ExampleState::eAwake);
    mAwake = true;
}

template <>
inline void ExampleControl::exit<ExampleState::eAwake>()
{
    utils::TestLog::instance() << "exit_Awake " << std::endl;
    mAccumultedEntryExit -= static_cast<int>(ExampleState::eAwake);
    mAwake = false;
}

}  // namespace controller
}  // namespace examples
}  // namespace eta_hsm
