
#pragma once

namespace eta::hsm::update_control {

template <StateEnum State>
void UpdateControlHsm::stateUpdate(const Input& input)
{

}

template <StateEnum State>
void UpdateControlHsm::entry()
{
    // Keep track of how long we have been in each state
    // This is done here (instead of in hsm itself) because the hsm library has no sense of time
    mTimeTracker.enter(State);

    // Delegate the entry actions called out on the UML stateflow diagram
    hsmEntry<State>();

    reset<State>();
}

template <StateEnum State>
void UpdateControlHsm::exit()
{
    // Also record when we leave each state
    mTimeTracker.exit(State);

    // Delegate the exit actions called out on the UML stateflow diagram
    hsmExit<State>();
}

template <StateEnum State>
void UpdateControlHsm::reset()
{

}

} // namespace eta::hsm::update_control
