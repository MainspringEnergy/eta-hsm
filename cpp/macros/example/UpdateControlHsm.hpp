
#pragma once

#include "../../Hsm.hpp"
#include "../HsmMacros.hpp"
#include "../event/EventBucket.hpp"
#include "../event/Timer.hpp"
//#include "../time_tracker/TimeTracker.hpp"

#include <chrono>
#include <cstddef>

namespace eta::hsm::update_control {

enum class UpdateControlHsmEventEnum {
    eOn,
    eOff,
    eFault,
    eWarm,
    eClear,
    eNone,
};

enum class UpdateControlHsmStateEnum {
    eNone,
    eTop,
    eOff,
    eFault,
    eOn,
    eWarming,
    eReady,
};

using UpdateControlEventBucket = event::PrioritizedEventBucket<UpdateControlHsmEventEnum>;

struct Input {
    float voltage{0};
};

struct Output {
    float dummy;
};

using Clock = std::chrono::system_clock;

struct StateMachineTraits {
    using Clock = update_control::Clock;
    using Event = UpdateControlHsmEventEnum;
    using StateEnum = UpdateControlHsmStateEnum;
    using Input = EmptyType;

    // ETA_NAMED_EVENT(StateTransition, "StateTransitionEventName", (Event, utils), (StateEnum, from), (StateEnum, to));

    static constexpr DefaultActions kDefaultActions = hsm::DefaultActions::eControlUpdate;
    static constexpr bool kClearTimersOnExit = true;
};

// struct Children
//{
//     BOOST_HANA_DEFINE_STRUCT(
//         Children
//     );
// };

// using EmittableEvents = eta::hsm::EmittableEvents<StateMachineTraits>;

// struct Traits : eta::update_control::DefaultTraits
//{
////    using Children = update_control::Children;
//    using Clock = update_control::Clock;
//    using EventPool = update_control::UpdateControlEventBucket;
//    using Input = update_control::Input;
//    using Output = update_control::Output;
//};

// using Context = eta::update_control::Context<Traits>;

using EventEnum = update_control::UpdateControlHsmEventEnum;
using StateEnum = update_control::UpdateControlHsmStateEnum;

class UpdateControlHsm : public eta::hsm::StateMachine<UpdateControlHsm, StateMachineTraits> {
public:
    UpdateControlHsm();
    using Input = EmptyType;  // rely on empty during() as an example of legacy control

    using Clock = update_control::Clock;
    using EventScheduler = event::TimerBank<event::TimerTraits<Clock, EventEnum, StateEnum>>;

    //    UpdateControlHsm(Context);

    void update(const Input&);

    template <StateEnum state>
    void stateUpdate(const Input&);

    template <StateEnum state>
    void entry();

    template <StateEnum state>
    void exit();

    EventScheduler& eventScheduler() { return mEventScheduler; }

    bool isVoltageStable() { return mBusVoltage > 10.0; }

private:
    // using TimeTracker = time_tracker::TimeTracker<StateEnum, Clock>;

    // Utility for keeping track of how long we've been in each state
    // TimeTracker mTimeTracker;

    // Hold potentially several timers that will set off events when they expire
    EventScheduler mEventScheduler;

    float mBusVoltage;

    // HSM automatically calls GeneratorControl::entry/exit<state>() which perform
    // updates of the controller and sub-controllers.  It then delegates back to
    // specialized forms of hsmEntry<state>() and hsmExit<state>() to implement
    // the entry/exit actions actually called out on the state diagram.
    // This seems a little bit backwards, but was the only way I could figure out
    // that did not involve a lot of error-prone code duplication.
    template <StateEnum State>
    void hsmEntry()
    {
        reset<State>();
    }

    template <StateEnum State>
    void hsmExit()
    {}

    template <StateEnum>
    void reset();
};

ETA_HSM_TOP_STATE(UpdateControlHsm, Top);
ETA_HSM_LEAF_STATE(UpdateControlHsm, Off, Top);
ETA_HSM_LEAF_STATE(UpdateControlHsm, Fault, Top);
ETA_HSM_COMP_STATE(UpdateControlHsm, On, Top);
ETA_HSM_LEAF_STATE(UpdateControlHsm, Warming, On);
ETA_HSM_LEAF_STATE(UpdateControlHsm, Ready, On);

}  // namespace eta::hsm::update_control

#include "UpdateControlHsm-hsm.hpp"
#include "UpdateControlHsm-inl.hpp"
