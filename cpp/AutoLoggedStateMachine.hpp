// AutoLoggedStateMachine.hpp

#pragma once

#include "Hsm.hpp"
#include "utils/TestLog.hpp"
#include "wise_enum/wise_enum.h"

namespace eta_hsm {

// using Format = char[eta::target_log::kFormatMax-1];

template<typename SM, typename StateMachineTraits, typename Logger>
class AutoLoggedStateMachine : public eta_hsm::StateMachine<SM, StateMachineTraits>
{
public:
    AutoLoggedStateMachine(const std::string& name, Logger* logger=nullptr)
        : mName{name}
        , mpLogger{logger}
    {
//        std::strcpy(mTransitionFormat, name.c_str());
//        std::strcat(mTransitionFormat, " HSM transitioning from %s to %s due to %s");
    }

    virtual ~AutoLoggedStateMachine() {}

    /// Dispatch (step) state machine directly with a named event.
    void dispatch(typename StateMachineTraits::Event evt)
    {
        // Save the event that we are dispatching so that it can be used for logging in next()
        mEventDispatched = evt;
        // Let the base class peform the actual dispatch normally
        StateMachine<SM, StateMachineTraits>::dispatch(evt);
    }

    /// Friend the LeafState so that it can access `next` below without exposing it to the world
    template<typename Traits, typename Parent>
    friend struct LeafState;

private:
    /// States use this function to set the next (current) state of the state machine
    void next(const eta_hsm::TopState<StateTraits<SM, typename StateMachineTraits::StateEnum, StateMachineTraits::StateEnum::eTop>>& state)
    {
        // Most instances of hsm::StateMachine use something like "Transition<Top, Top, Top>"
        // in their constructors to initialize the mState pointer.  This is not ideal, but arguably
        // better than them having to know how to instantiate a specific leaf state, which is the
        // only thing that this pointer can actually point to (composite states are abstract).
        // Therefore, in order to avoid a bunch of yuckiness elsehwere, we put in a little bit of a
        // hack here to catch this first initialization and avoid dereferencing the unitialized pointer.
        if (mStateInitialized)
        {
            typename StateMachineTraits::StateEnum originState = AutoLoggedStateMachine::mState->identify();
            typename StateMachineTraits::StateEnum destinationState = state.identify();

            if (mpLogger)
            {
                static_assert(wise_enum::is_wise_enum_v<typename StateMachineTraits::StateEnum>, "Ignorant State Enum");
                static_assert(wise_enum::is_wise_enum_v<typename StateMachineTraits::Event>, "Ignorant Event Enum");

                // mpLogger->printf(mTransitionFormat, originState, destinationState, mEventDispatched);
                *mpLogger << mName << " HSM transitioning from " << originState << " to " << destinationState << " due to " << mEventDispatched << std::endl;
            }
        }

        // This is the line that actually changes the current state of the state machine
        AutoLoggedStateMachine::mState = &state;
        mStateInitialized = true;
    }

    bool mStateInitialized {false};
    typename StateMachineTraits::Event mEventDispatched {StateMachineTraits::Event::eNone};
    // Format mTransitionFormat {};
    std::string mName {};
    Logger* mpLogger;
};

} // namespace eta-hsm

