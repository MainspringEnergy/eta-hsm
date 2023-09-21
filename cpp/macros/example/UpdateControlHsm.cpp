
#include "UpdateControlHsm.hpp"

namespace eta::hsm::update_control {

UpdateControlHsm::UpdateControlHsm()
    : StateMachine()
      //, mTimeTracker {clock()}
      ,
      mEventScheduler{},
      mBusVoltage{}
{
    // We are responsible for initializing mState to something useful
    // In order to make sure we hit all of the correct entry methods
    // (and avoid duplicating initial/default states) the easist way
    // to do this is to kick off a self-transition from Top to Top.
    eta::hsm::Transition<Top, Top, Top> t(*this);
}

void UpdateControlHsm::update(const Input& input)
{
    mBusVoltage = input.voltage;

    // Check timers that might generate events that the state machine will need to process below
    mEventScheduler.checkTimers(clock().now(), eventPool());

    // Let the controllers check for utils conditions in their data streams (will populate mEventBucket)
    // Not entirely obvious, but this is kicked off by using eta::hsm's "during" mechanism.
    during(input);  // ends up back at GeneratorControl::stateUpdate<state>()

    // Select which utils to dispatch state machine with.
    // Specific logic of how events are categorized, prioritized, flushed, etc... is still being debated.
    if (!eventPool().empty())
    {
        const auto event{eventPool().getEvent()};  // removal from bucket is included
        dispatch(event);
    }
    else
    {}
}

}  // namespace eta::hsm::update_control
