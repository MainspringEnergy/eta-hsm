// eta/apu_control/ApuControl.cpp

#include "ExampleControl.hpp"

#include "../../utils/TestLog.hpp"

namespace eta {
namespace hsm {
namespace example_control {

ExampleControl::ExampleControl()
    //: StateMachine("ExampleControl")
{
    // We are responsible for initializing mState to something useful
    // In order to make sure we hit all of the correct entry methods
    // (and avoid duplicating initial/default states) the easist way
    // to do this is to kick off a self-transition from Top to Top.
    eta::hsm::Transition<Top, Top, Top> t(*this);
}

ExampleControl::~ExampleControl()
{
}

/// geneirc top-level update
void ExampleControl::update(const example_control::Input& input)
{
    // Save a copy of input reference to avoid having to pass it around to the states
    mpInput = &input;

    // Typically, we would give the underlying state machine a chance to update its timers
    // here, but for unit testability, I've broken this out as a separate call from the unit test
    // so that I can pass in fake clock times.
    // mEventScheduler.checkTimers(std::chrono::system_clock::now(), mEventBucket);

    // Select which utils to dispatch state machine with.
    // Specific logic of how events are categorized, prioritized, flushed, etc... is up to the
    // particular StateMachine.  This is intended just to show an example of what could be done.
    // In any case, a single utils needs to be selected and "set" before calling dispatch,
    // although there is nothing stopping you from dispatching the state machine multiple times
    // in a single controller "update" if the time budget allows.
    if ( !mEventBucket.empty() )
    {
        Event evt = mEventBucket.getEvent();
        //TestLog::instance() << "Dispatching hsm with " << wise_enum::to_string(evt) << std::endl;
        dispatch(evt);
    }

    // Kick off the during action of the current state, which will call the appropriate stateUpdate()
    during();
}

void ExampleControl::increaseBac(Real amt)
{
    mBac += amt;
    if(mBac < 0.0)
    {
        mBac = 0.0;
    }
    TestLog::instance() << "BAC = " << mBac << std::endl;
}



} // namespace example_control
} // namespace hsm
} // namespace eta
