#pragma once

#include "../../utils/TestLog.hpp"

namespace eta {
namespace hsm {
namespace example_control {

/// Templatized state-specific update functions
template<>
inline void ExampleControl::stateUpdate<ExampleState::eSober>()
{
    // Can use mpInput-> here to access inputs
    TestLog::instance() << "stateUpdate<eSober> " << std::endl;

    // Possibly check for fault-like situations
    if(getBac() > 0.35)
    {   // We can queue events directly from within the state machine if we want to
        addEvent(ExampleEvent::eDie);
    }
    // Do something reasonable
    increaseBac(-0.01); // metabolize alcohol
}

template<>
inline void ExampleControl::stateUpdate<ExampleState::eDrunk>()
{
    // Can use mpInput-> here to access inputs
    TestLog::instance() << "stateUpdate<eDrunk> " << std::endl;

    // Possibly check for fault-like situations
    if(getBac() > 0.35)
    {
        addEvent(ExampleEvent::eDie);
    }
    // Do something unreasonable
    increaseBac(-0.01); // metabolize alcohol
}

template<>
inline void ExampleControl::stateUpdate<ExampleState::eDead>()
{
    TestLog::instance() << "stateUpdate<eDead> " << std::endl;
    // do nothing...
}

template<>
inline void ExampleControl::stateUpdate<ExampleState::eBored>()
{
    TestLog::instance() << "stateUpdate<eBored> " << std::endl;
    // do nothing...
}

template<>
inline void ExampleControl::entry<ExampleState::eAlive>()
{
    // Yes, we can also now test alive-ness by querying state, but I'm keeping this around as an example
    // of how to manipulate controller-continuous-state with state-specialized entry/exit functions.
    TestLog::instance() << "enter_Alive " << std::endl;
    mAccumultedEntryExit += static_cast<int>(ExampleState::eAlive);
    mAlive = true;
}

template<>
inline void ExampleControl::exit<ExampleState::eAlive>()
{
    TestLog::instance() << "exit_Alive " << std::endl;
    mAccumultedEntryExit -= static_cast<int>(ExampleState::eAlive);
    mAlive = false;
}

} // namespace example_control
} // namespace hsm
} // namespace eta

