// eta/hsm/Hsm.hpp

#pragma once

#include <type_traits>
#include <iostream>
#include <variant>

namespace eta_hsm {

enum class Semantics
{
    eLocal,
    eExternal,
};

enum class DefaultActions
{
    eNothing,
    eControlUpdate, // entry, exit, and during
    eEntryExitOnly,
};

using EmptyType = std::monostate;

// There is always one and only one TopState at the top of the hierarchy
// Host is the class that contains the state machine.
template<typename Traits>
struct TopState
{
    using Input = typename Traits::Host::Input;
public:
    static constexpr typename Traits::StateEnum kState = Traits::kState;

    virtual void eventHandler(typename Traits::Host&, typename Traits::Host::Event event) const = 0;
    virtual void during(typename Traits::Host&) const = 0;
    virtual void during(typename Traits::Host&, const Input&) const = 0;

    /// Identify yourself with a run-time usable enum
    /// Not intended for use in non-test code
    virtual typename Traits::StateEnum identify() const = 0;

    /// Test for membership of some parent super-state (ncluding yourself)
    /// Definitely not intended for use in non-test code
    virtual bool isSubstateOf(typename Traits::StateEnum queryState) const
    {
        if (Traits::kState == queryState)
        {
            return true;
        }
        return false;
    }

    template<typename Current, typename Source, typename Target, Semantics>
    friend class Transition;
    template<typename Target>
    friend class Init;

protected:
    // make these using declarations protected instead of private to avoid replicating below
    using Host = typename Traits::Host;
    using Event = typename Traits::Host::Event;

    template<typename Current>
    void handleEvent(typename Traits::Host& host, const Current& current, typename Traits::Host::Event event) const { }

private:
    /// There is intentionally no default implementation for init.  This way the compiler will enforce the definition
    /// of init for any non-leaf state.
    static void init(typename Traits::Host&);

    /// Default implementation of entry/exit can now be suggested via optional kDefaultAction argument to StateMachine,
    /// but can still be arbitrarily specialized by state.  Unfortunately, I think that these need to be repeated for
    /// Top, Comp, and Leaf states to avoid a specialized action for a parent state overriding the default action of
    /// a child state.
    static void entry(typename Traits::Host& host)
    {
        if constexpr (Traits::Host::kDefaultActions == DefaultActions::eControlUpdate ||
                      Traits::Host::kDefaultActions == DefaultActions::eEntryExitOnly)
        {
            host.template entry<Traits::kState>();
        }
        // else, do nothing
    }
    static void exit(typename Traits::Host& host)
    {
        if constexpr (Traits::Host::kDefaultActions == DefaultActions::eControlUpdate ||
                      Traits::Host::kDefaultActions == DefaultActions::eEntryExitOnly)
        {
            host.template exit<Traits::kState>();
        }
        // else, do nothing
    }

    using ParentState = TopState; // We are our own parent?
};

// Composite states are states that contain other child states.
// In addition to depending upon the Host type, they also depend upon their parent state
// and use a Tag to ensure uniqueness of types (in the case of sibling states which
// have the same Parent).
template<typename Traits, typename Parent_>
struct CompState : Parent_
{
public:
    template<typename Current, typename Source, typename Target, Semantics>
    friend class Transition;
    template<typename Target>
    friend class Init;

    static constexpr typename Traits::StateEnum kState = Traits::kState;

    /// Test for membership of some parent super-state (including yourself)
    /// Definitely not intended for use in non-test code
    bool isSubstateOf(typename Traits::StateEnum queryState) const override
    {
        if (Traits::kState == queryState)
        {
            return true;
        }
        // else
        return ParentState::isSubstateOf(queryState);
    }

protected:
    template<typename Current>
    void handleEvent(typename Traits::Host& host, const Current& current, typename Traits::Host::Event event) const
    {
        // std::cout << "default CompState::handleEvent()" << std::endl;
        Parent_::handleEvent(host, current, event);
    }

private:
    /// There is intentionally no default implementation for init.  This way the compiler will enforce the definition
    /// of init for any non-leaf state.
    static void init(typename Traits::Host&);

    /// Default implementation of entry/exit can now be suggested via optional kDefaultAction argument to StateMachine,
    /// but can still be arbitrarily specialized by state.  Unfortunately, I think that these need to be repeated for
    /// Top, Comp, and Leaf states to avoid a specialized action for a parent state overriding the default action of
    /// a child state.
    static void entry(typename Traits::Host& host)
    {
        if constexpr (Traits::Host::kDefaultActions == DefaultActions::eControlUpdate ||
                      Traits::Host::kDefaultActions == DefaultActions::eEntryExitOnly)
        {
            host.template entry<Traits::kState>();
        }
        // else, do nothing
    }
    static void exit(typename Traits::Host& host)
    {
        // clear all timers associated with this state
        if constexpr(Traits::Host::kClearTimersOnExit)
        {
            host.template eventScheduler().clearAllTimersInGroup(Traits::kState);
        }

        if constexpr (Traits::Host::kDefaultActions == DefaultActions::eControlUpdate ||
                      Traits::Host::kDefaultActions == DefaultActions::eEntryExitOnly)
        {
            host.template exit<Traits::kState>();
        }
        // else, do nothing
    }

    using ParentState = Parent_; // This is so that the parent type is still accessible after the template has been specialized
    using ThisState = CompState<Traits, ParentState>;
};

// Leaf states are states with no more children.  While composite states can have entry and
// exit methods that get called as we pass through them, we only ever "exist" in leaf states.
template<typename Traits, typename Parent_>
struct LeafState : Parent_
{
    using Input = typename Traits::Host::Input;
public:
    template<typename Current, typename Source, typename Target, Semantics>
    friend
    class Transition;

    template<typename Target>
    friend
    class Init;

    static constexpr typename Traits::StateEnum kState = Traits::kState;

    /// eventHandler is public entry point into the state, only defined for leaf states.
    void eventHandler(typename Traits::Host& host, typename Traits::Host::Event event) const final
    {
        handleEvent(host, *this, event);
    }

    /// Default implementation of during can now be suggested via optional kDefaultDuring argument to StateMachine,
    /// but can still be arbitrarily specialized by state.
    void during(typename Traits::Host& host) const final { during(host, {}); }
    void during(typename Traits::Host& host, const Input& input) const final
    {
        if constexpr (Traits::Host::kDefaultActions == DefaultActions::eControlUpdate)
        {
            // Note the utterly perplexing "template" keyword location below.
            // This is required to disambiguate the syntax since the compiler doesn't know
            // what `host` actually is at this point, so it has no way to know that `stateUpdate`
            // is a template function.  If stateUpdate were a member variable of host, the first `<`
            // would be interpreted as a less-than comparison.
            if constexpr (std::is_same_v<Input, EmptyType>)
            {
                host.template stateUpdate<Traits::kState>();
            }
            else
            {
                host.template stateUpdate<Traits::kState>(input);
            }
        }
        // else, do nothing
    };

    /// Expose direct access to state instance for testing and simmulation only.
    /// WARNING: This exposes access to all sorts of stuff that you shouldn't be touching!
    static const LeafState& instanceForTestingOnly() { return mObj; }

    /// Identify yourself with a run-time usable enum
    /// Not intended for use in non-test code
    typename Traits::StateEnum identify() const final { return Traits::kState; }

    /// Test for membership of some parent super-state (including yourself)
    /// Definitely not intended for use in non-test code
    bool isSubstateOf(typename Traits::StateEnum queryState) const final
    {
        if (Traits::kState == queryState)
        {
            return true;
        }
        // else
        return ParentState::isSubstateOf(queryState);
    }

protected:
    template<typename Current>
    void handleEvent(typename Traits::Host& host, const Current& current, typename Traits::Host::Event event) const
    {
        // std::cout << "default LeafState::handleEvent()" << std::endl;
        Parent_::handleEvent(host, current, event);
    }

private:
    // Specific leaf states should not specialize this init behavior.
    static void init(typename Traits::Host& host) { host.next(mObj); }

    /// Default implementation of entry/exit can now be suggested via optional kDefaultAction argument to StateMachine,
    /// but can still be arbitrarily specialized by state.  Unfortunately, I think that these need to be repeated for
    /// Top, Comp, and Leaf states to avoid a specialized action for a parent state overriding the default action of
    /// a child state.
    static void entry(typename Traits::Host& host)
    {
        if constexpr (Traits::Host::kDefaultActions == DefaultActions::eControlUpdate ||
                      Traits::Host::kDefaultActions == DefaultActions::eEntryExitOnly)
        {
            host.template entry<Traits::kState>();
        }
        // else, do nothing
    }
    static void exit(typename Traits::Host& host)
    {
        // clear all timers associated with this state
        if constexpr(Traits::Host::kClearTimersOnExit)
        {
            host.template eventScheduler().clearAllTimersInGroup(Traits::kState);
        }

        if constexpr(Traits::Host::kDefaultActions == DefaultActions::eControlUpdate ||
                     Traits::Host::kDefaultActions == DefaultActions::eEntryExitOnly)
        {
            host.template exit<Traits::kState>();
        }
        // else, do nothing
    }

    using ParentState = Parent_;
    using ThisState = LeafState<Traits, ParentState>;

    // Use a static member to initialize leaf states at construction time.
    static const LeafState mObj;
};

// Instantiate the actual leaf states via their static members
template<typename Traits, typename Parent_>
const LeafState<Traits, Parent_> LeafState<Traits, Parent_>::mObj;

// We need types that represent true or false
template<bool b>
struct Bool {};

// Transitions between states are accomplished by instantiating (and destroying) a Transition object
// Note:  Transitions can be interpreted using either "local" or "external" semantics.
//        "external" semantics are currently default because that is all that existed prior to UML2.
// See https://en.wikipedia.org/wiki/UML_state_machine#Local_versus_external_transitions
template<typename Current, typename Source, typename Target, Semantics TransitionSemantics=Semantics::eExternal>
struct Transition
{
    using Host = typename Current::Host;

    // Note: These exit/EntryStop expressions apply to whether or not we want the current state's PARENT to execute an
    //       entry or exit.  I am relatively confident that (especially with the inclusion of internal semantics) the
    //       overall logic could be simplified by shifting these evaluations to apply to the CURRENT state's entry/exit.
    //       This is made easier by some of the more modern language elements available such as `if constexpr`
    static constexpr bool exitStop { std::is_base_of<typename Current::ParentState, typename Target::ParentState>::value
                                     && std::is_base_of<Current, Source>::value };
    static constexpr bool entryStop { std::is_base_of<Current, Source>::value
                                      || ( std::is_base_of<typename Current::ParentState, Source>::value
                                         && !std::is_base_of<Source, Current>::value )};

    // Local vs external transition semantics only matter in the two following cases
    static constexpr bool sourceContainsTarget { std::is_base_of<Source, Target>::value };
    static constexpr bool targetContainsSource { std::is_base_of<Target, Source>::value };

    // intermediate logical variables to make the final expression more readable
    static constexpr bool atTarget { std::is_same<Current, Target>::value };
    static constexpr bool atSource { std::is_same<Current, Source>::value };

    // Local transition semantics imply that we skip one level of exit/entry in these scenarios
    static constexpr bool omitActionDueToLocalSemantics { TransitionSemantics==Semantics::eLocal
        && ( (sourceContainsTarget && atSource ) || (targetContainsSource && atTarget) )};

    // We use overloading to stop recursion. The more natural template specialization
    // method would require to specialize the inner template without specializing the
    // outer one, which is forbidden.
    static void exitActions(Host&, Bool<true>) {}
    static void exitActions(Host& host, Bool<false>)
    {
        if constexpr( !omitActionDueToLocalSemantics )
        {
            Current::exit(host);
        }
        Transition<typename Current::ParentState, Source, Target, TransitionSemantics>::exitActions(host, Bool<exitStop>());
    }

    static void entryActions(Host&, Bool<true>) {}
    static void entryActions(Host& host, Bool<false>)
    {
        Transition<typename Current::ParentState, Source, Target, TransitionSemantics>::entryActions(host, Bool<entryStop>());
        if constexpr( !omitActionDueToLocalSemantics )
        {
            Current::entry(host);
        }
    }

    Transition(Host& h) : mHost(h)
    {
        exitActions(mHost, Bool<false>());
    }

    ~Transition()
    {
        Transition<Target, Source, Target, TransitionSemantics>::entryActions(mHost, Bool<false>());
        Target::init(mHost);
    }

    Host& mHost;
};

template<typename Target>
struct Init
{
    using Host = typename Target::Host;

    Init(Host& host) : mHost(host) {}

    ~Init()
    {
        Target::entry(mHost);
        Target::init(mHost);
    }

    Host& mHost;
};

template<typename Host_, typename StateEnum_, StateEnum_ kState_>
struct StateTraits
{
    using Host = Host_;
    using StateEnum = StateEnum_;
    static constexpr StateEnum kState = kState_;
};

/// Declaring this struct here as an example of StateMachine expects and as a convenience
/// for existing users of StateMachine.
template<typename Event_, typename StateEnum_, typename Clock_, DefaultActions kDefaultActions_=DefaultActions::eNothing>
struct StateMachineTraits
{
    using Clock = Clock_;  // used by EventEmittingStateMachine
    using Event = Event_;
    using StateEnum = StateEnum_;

    // ETA_NAMED_EVENT(StateTransition, "StateTransitionEventName", (Event, utils), (StateEnum, from), (StateEnum, to));

    static constexpr DefaultActions kDefaultActions = kDefaultActions_;
    static constexpr bool kClearTimersOnExit = false;  // could make default once all state machines support it
};

template<typename SM, typename StateMachineTraits>
class StateMachine
{
public:
    StateMachine() {}
    virtual ~StateMachine() {};

    /// Expose type of Event so that derived classes can see it
    using Event = typename StateMachineTraits::Event;
    using StateEnum = typename StateMachineTraits::StateEnum;
    //using StateTransition = typename StateMachineTraits::StateTransition;
    static constexpr DefaultActions kDefaultActions = StateMachineTraits::kDefaultActions;
    static constexpr bool kClearTimersOnExit = StateMachineTraits::kClearTimersOnExit;

    /// Dispatch (step) state machine directly with a named utils.
    virtual void dispatch(Event evt) { mState->eventHandler(*static_cast<SM*>(this), evt); }

    /// Kick off during action for current state
    void during() { mState->during(*static_cast<SM*>(this)); }

    // has to be templatized as SM is not resolved yet so cannot lift Input type
    template <typename Input>
    void during(const Input& input) { mState->during(*static_cast<SM*>(this), input); }

    /// Identify current state with a run-time usable enum
    /// Not intended for use in non-test code
    StateEnum identify() const { return mState->identify(); }

    /// Test for current state membership of some parent super-state (including itself)
    /// Definitely not intended for use in non-test code
    bool isInSubstateOf(StateEnum queryState) const { return mState->isSubstateOf(queryState); }

    /// Friend the LeafState so that it can access `next` below without exposing it to the world
    template<typename Traits, typename Parent>
    friend struct LeafState;

    /// Expose direct setting of state for testing and simmulation only.
    /// WARNING: This BYPASSES entry and exit methods
    template<typename State>
    void directlySetStateForTestingOnly() { mState = &State::instanceForTestingOnly(); }

protected:
    /// States use this function to set the next (current) state of the state machine
    virtual void next(const eta_hsm::TopState<StateTraits<SM, StateEnum, StateEnum::eTop>>& state) { mState = &state; }
    const eta_hsm::TopState<StateTraits<SM, StateEnum, StateEnum::eTop>>* mState{};
};

} // namespace hsm_hsm

#include "Hsm-inl.hpp"
