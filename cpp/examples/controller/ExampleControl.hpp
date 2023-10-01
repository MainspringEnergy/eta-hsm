#pragma once

#include <chrono>
#include <memory>
#include <queue>

#include "../../AutoLoggedStateMachine.hpp"
#include "../../utils/TestLog.hpp"
#include "../../utils/Timer.hpp"
#include "wise_enum/wise_enum.h"

namespace eta_hsm {
namespace examples {
namespace controller {

/// Example of a "hybrid state machine" that integrates a "controller"
/// for "continuous state" with a traditional discrete state machine.

using Real = float;

WISE_ENUM_CLASS((ExampleEvent, int32_t),
                eLookAtWatch,
                eStartWatch,
                ePassOut,
                eDrinkWiskey,
                eDrinkBeer,
                eNone)

WISE_ENUM_CLASS((ExampleState, int32_t),
                eTop,
                eAwake,
                eSober,
                eDrunk,
                eBored,
                eUnconcious)

struct Logger {};  // placeholder

struct Input {
    float voltage{0};
};

struct ExampleControlTraits {
    using Clock = std::chrono::steady_clock;
    using Event = ExampleEvent;
    using StateEnum = ExampleState;
    static constexpr DefaultActions kDefaultActions = eta_hsm::DefaultActions::eControlUpdate;
    static constexpr bool kClearTimersOnExit = true;
};

/// With eta-hsm, the top-level controller can BE the state machine
class ExampleControl : public eta_hsm::AutoLoggedStateMachine<ExampleControl, ExampleControlTraits, utils::TestLog> {
public:
    using Input = EmptyType;

    ExampleControl();
    virtual ~ExampleControl();

    // using Parent = eta_hsm::StateMachine<ExampleControl, ExampleControlTraits>;

    friend class TopState<ExampleControl>;

    using Clock = std::chrono::system_clock;
    using EventScheduler = eta_hsm::utils::TimerBank<eta_hsm::utils::TimerTraits<Clock, Event, StateEnum>>;

    /// Get a reference to the "limited" EventBucket interface for collecting events
    utils::EventBucket<Event>& eventBucket() { return mEventBucket; }
    /// Get a reference to the EventScheduler interface for scheduling events
    EventScheduler& eventScheduler() { return mEventScheduler; }

    /// generic top-level update
    void update(const controller::Input& input);

    /// Templatized state-specific update functions
    template <ExampleState state>
    void stateUpdate();  // will use internal mInput reference

    template <ExampleState state>
    void entry()  // If called by hsm within "update", input is available as mInput
    {
        utils::TestLog::instance() << "enter State " << wise_enum::to_string(state) << std::endl;
        mAccumultedEntryExit += static_cast<int>(state);
    }

    template <ExampleState state>
    void exit()  // If called by hsm within "update", input is available as mInput
    {
        utils::TestLog::instance() << "exit State " << wise_enum::to_string(state) << std::endl;
        mAccumultedEntryExit -= static_cast<int>(state);
    }

    // Add events for the state machine to process
    // We could eventually discern priority internally based upon the event,
    // but for simplicity, let's just assume for now that the user knows urgency.
    void addEvent(ExampleEvent evt) { mEventBucket.addEvent(evt); }

    // Silly stuff for the initial example to demonstrate control of "continous state"
    void increaseBac(Real amt);
    Real getBac() { return mBac; }

    // Accessor for some controller status
    void awake(bool status) { mAwake = status; }
    bool awake() const { return mAwake; }

    // Accessor to support testing
    int accumulatedEntryExit() { return mAccumultedEntryExit; }

private:
    /// A place to hold events.  Derived StateMachines can choose what type of bucket to use.
    utils::PrioritizedEventBucket<Event> mEventBucket{};

    /// Hold potentially several timers that will set off events when they expire
    EventScheduler mEventScheduler{};

    // This is non-standard for Update API, but we may need to hold a reference to the input
    // in order to avoid passing it around through all of the states.
    const controller::Input* mpInput{};

    // Additional stateful data
    Real mBac{0.0};
    bool mAwake{true};

    // Just using this as a silly example and to support unit testing
    int mAccumultedEntryExit{0};
};

template <ExampleState kState>
using ExampleTraits = StateTraits<ExampleControl, ExampleState, kState>;

/// Declare the states that exist in this example Hsm (and their relationships) here
using Top = eta_hsm::TopState<ExampleTraits<ExampleState::eTop>>;
using Awake = eta_hsm::CompState<ExampleTraits<ExampleState::eAwake>, Top>;
using Sober = eta_hsm::LeafState<ExampleTraits<ExampleState::eSober>, Awake>;
using Drunk = eta_hsm::LeafState<ExampleTraits<ExampleState::eDrunk>, Awake>;
using Bored = eta_hsm::LeafState<ExampleTraits<ExampleState::eBored>, Awake>;
using Unconcious = eta_hsm::LeafState<ExampleTraits<ExampleState::eUnconcious>, Top>;

}  // namespace controller
}  // namespace examples
}  // namespace eta_hsm

#include "ExampleControl-hsm.hpp"
#include "ExampleControl-inl.hpp"
