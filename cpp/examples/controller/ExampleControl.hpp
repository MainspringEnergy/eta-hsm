#pragma once

#include <chrono>
#include <memory>
#include <queue>

#include "../../Hsm.hpp"
#include "../../utils/TestLog.hpp"
#include "../../utils/Timer.hpp"

namespace eta_hsm {
namespace examples {
namespace controller {

/// Top-level controller for APU control process

using Real = float;

enum class ExampleEvent {
    eLookAtWatch,
    eStartWatch,
    eDie,
    eDrinkWiskey,
    eDrinkBeer,
    eNone,
};

enum class ExampleState {
    eTop,
    eAlive,
    eSober,
    eDrunk,
    eBored,
    eDead,
};

struct Logger {};  // placeholder

struct Input {
    float voltage{0};
};

struct ExampleControlTraits {
    using Clock = std::chrono::steady_clock;
    using Event = ExampleEvent;
    using StateEnum = ExampleState;

    //    ETA_NAMED_EVENT(StateTransition, "StateTransitionEventName", (Event, utils), (StateEnum, from), (StateEnum,
    //    to));

    static constexpr DefaultActions kDefaultActions = eta_hsm::DefaultActions::eControlUpdate;
    static constexpr bool kClearTimersOnExit = true;
};

/// With eta::hsm, the top-level controller can BE the state machine
class ExampleControl : public eta_hsm::StateMachine<ExampleControl, ExampleControlTraits> {
public:
    using Input = EmptyType;  // rely on empty during() as an example of legacy control

    ExampleControl();
    virtual ~ExampleControl();

    using Parent = eta_hsm::StateMachine<ExampleControl, ExampleControlTraits>;

    friend class TopState<ExampleControl>;

    using Clock = std::chrono::system_clock;
    using EventScheduler = eta_hsm::utils::TimerBank<eta_hsm::utils::TimerTraits<Clock, Event, StateEnum>>;

    /// Get a reference to the "limited" EventBucket interface for collecting events
    event::EventBucket<Event>& eventBucket() { return mEventBucket; }
    /// Get a reference to the EventScheduler interface for scheduling events
    EventScheduler& eventScheduler() { return mEventScheduler; }

    /// geneirc top-level update
    void update(const controller::Input& input);

    /// Templatized state-specific update functions
    template <ExampleState state>
    void stateUpdate();  // will use internal mInput reference

    template <ExampleState state>
    void entry()  // If called by hsm within "update", input is available as mInput
    {
        // TestLog::instance() << "enter State " << wise_enum::to_string(state) << std::endl;
        mAccumultedEntryExit += static_cast<int>(state);
    }

    template <ExampleState state>
    void exit()  // If called by hsm within "update", input is available as mInput
    {
        // TestLog::instance() << "exit State " << wise_enum::to_string(state) << std::endl;
        mAccumultedEntryExit -= static_cast<int>(state);
    }

    // Add events for the state machine to process
    // We could eventually discern priority internally based upon the utils,
    // but for simplicity, let's just assume for now that the user knows urgency.
    void addEvent(ExampleEvent evt) { mEventBucket.addEvent(evt); }

    // Silly stuff for the initial example
    void increaseBac(Real amt);
    Real getBac() { return mBac; }

    // Accessor for some controller status
    void alive(bool status) { mAlive = status; }
    bool alive() const { return mAlive; }

    // Accessor to support testing
    int accumulatedEntryExit() { return mAccumultedEntryExit; }

private:
    /// A place to hold events.  Derived StateMachines can choose what type of bucket to use.
    utils::PrioritizedEventBucket<Event> mEventBucket {};

    /// Hold potentially several timers that will set off events when they expire
    EventScheduler mEventScheduler{};

    // This is non-standard for Update API, but we may need to hold a reference to the input
    // in order to avoid passing it around through all of the states.
    const controller::Input* mpInput{};

    // Additional stateful data
    Real mBac{0.0};
    bool mAlive{true};

    // Just using this as a silly example and to support unit testing
    int mAccumultedEntryExit{0};
};

template <ExampleState kState>
using ExampleTraits = StateTraits<ExampleControl, ExampleState, kState>;

/// Declare the states that exist in this example Hsm (and their relationships) here
using Top = eta_hsm::TopState<ExampleTraits<ExampleState::eTop>>;
using Alive = eta_hsm::CompState<ExampleTraits<ExampleState::eAlive>, Top>;
using Sober = eta_hsm::LeafState<ExampleTraits<ExampleState::eSober>, Alive>;
using Drunk = eta_hsm::LeafState<ExampleTraits<ExampleState::eDrunk>, Alive>;
using Bored = eta_hsm::LeafState<ExampleTraits<ExampleState::eBored>, Alive>;
using Dead = eta_hsm::LeafState<ExampleTraits<ExampleState::eDead>, Top>;

}  // namespace controller
}  // namespace examples
}  // namespace eta_hsm

#include "ExampleControl-hsm.hpp"
#include "ExampleControl-inl.hpp"
