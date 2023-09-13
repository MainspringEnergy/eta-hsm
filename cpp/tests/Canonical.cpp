// Canonical.cpp
#include "eta/hsm/tests/Canonical.hpp"

namespace eta {
namespace hsm {
namespace canonical {


// The missing constructor
Canonical::Canonical()
{
    // Guessing wildly at how to initialize mState to something useful
    eta::hsm::Transition<Top, Top, Top> t(*this);
}

} // close canonical namespace because templates below must be specialized in their originally declared scope?

template<>template<typename Current>
inline void canonical::Top::handleEvent(canonical::Canonical& stateMachine, const Current& current, Event event) const
{
    switch(event)
    {
        // We can handle events here if we want them to have default
        // behaviors that can then be overridden in specific states.
        default:
            break;
    }
    return; // TopState has no parent
}

template<>template<typename Current>
inline void canonical::S0::handleEvent(canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case canonical::CanonicalEvent::E:
        {
            Transition<Current, ThisState, canonical::S211> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::E_LOCAL:
        {
            Transition<Current, ThisState, canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void canonical::S1::handleEvent(canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case canonical::CanonicalEvent::A:
        {
            Transition<Current, ThisState, canonical::S1> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::A_LOCAL:
        {
            Transition<Current, ThisState, canonical::S1, Semantics::eLocal> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::B:
        {
            Transition<Current, ThisState, canonical::S11> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::B_LOCAL:
        {
            Transition<Current, ThisState, canonical::S11, Semantics::eLocal> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::C:
        {
            Transition<Current, ThisState, canonical::S2> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::C_LOCAL:
        {
            Transition<Current, ThisState, canonical::S2, Semantics::eLocal> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::D:
        {
            Transition<Current, ThisState, canonical::S0> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::D_LOCAL:
        {
            Transition<Current, ThisState, canonical::S0, Semantics::eLocal> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::F:
        {
            Transition<Current, ThisState, canonical::S211> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::F_LOCAL:
        {
            Transition<Current, ThisState, canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void canonical::S11::handleEvent(canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case canonical::CanonicalEvent::G:
        {
            Transition<Current, ThisState, canonical::S211> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::G_LOCAL:
        {
            Transition<Current, ThisState, canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::Z:
        {   // Not part of normal example, but used to get us into new state to test auto-transition
            Transition<Current, ThisState, canonical::S12> t(stateMachine);
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void canonical::S2::handleEvent(canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case canonical::CanonicalEvent::C:
        {
            Transition<Current, ThisState, canonical::S1> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::C_LOCAL:
        {
            Transition<Current, ThisState, canonical::S1, Semantics::eLocal> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::F:
        {
            Transition<Current, ThisState, canonical::S11> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::F_LOCAL:
        {
            Transition<Current, ThisState, canonical::S11, Semantics::eLocal> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void canonical::S21::handleEvent(canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case canonical::CanonicalEvent::B:
        {
            Transition<Current, ThisState, canonical::S211> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::B_LOCAL:
        {
            Transition<Current, ThisState, canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::H:
        {
            Transition<Current, ThisState, canonical::S21> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::H_LOCAL:
        {
            Transition<Current, ThisState, canonical::S21, Semantics::eLocal> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void canonical::S211::handleEvent(canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case canonical::CanonicalEvent::B:
        {
            Transition<Current, ThisState, canonical::S21> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::B_LOCAL:
        {
            Transition<Current, ThisState, canonical::S21, Semantics::eLocal> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::D:
        {
            Transition<Current, ThisState, canonical::S211> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::D_LOCAL:
        {
            Transition<Current, ThisState, canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::G:
        {
            Transition<Current, ThisState, canonical::S0> t(stateMachine);
            return;
        }
        case canonical::CanonicalEvent::G_LOCAL:
        {
            Transition<Current, ThisState, canonical::S0, Semantics::eLocal> t(stateMachine);
            return;
        }
        // TODO: Guard
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

// init actions (note the reverse ordering!)
template<> inline void canonical::S21::init(canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<canonical::S211> i(h);
    TestLog::instance() << "init_S21 " << std::endl;
}

template<> inline void canonical::S2::init(canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<canonical::S21> i(h);
    TestLog::instance() << "init_S2 " << std::endl;
}

template<> inline void canonical::S1::init(canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<canonical::S11> i(h);
    TestLog::instance() << "init_S1 " << std::endl;
}

template<> inline void canonical::S0::init(canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<canonical::S1> i(h);
    TestLog::instance() << "init_S0 " << std::endl;
}

template<> inline void canonical::Top::init(canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<canonical::S0> i(h);
    TestLog::instance() << "init_Top " << std::endl;
}

// entry actions
template<> inline void canonical::Top::entry(canonical::Canonical&) { TestLog::instance() << "enter_Top " << std::endl; }
template<> inline void canonical::S0::entry(canonical::Canonical&) { TestLog::instance() << "enter_S0 " << std::endl; }
template<> inline void canonical::S1::entry(canonical::Canonical&) { TestLog::instance() << "enter_S1 " << std::endl; }
template<> inline void canonical::S11::entry(canonical::Canonical&) { TestLog::instance() << "enter_S11 " << std::endl; }
template<> inline void canonical::S12::entry(canonical::Canonical&) { TestLog::instance() << "enter_S12 " << std::endl; }
template<> inline void canonical::S2::entry(canonical::Canonical&) { TestLog::instance() << "enter_S2 " << std::endl; }
template<> inline void canonical::S21::entry(canonical::Canonical&) { TestLog::instance() << "enter_S21 " << std::endl; }
template<> inline void canonical::S211::entry(canonical::Canonical&) { TestLog::instance() << "enter_S211 " << std::endl; }

// exit actions
template<> inline void canonical::Top::exit(canonical::Canonical&) { TestLog::instance() << "exit_Top " << std::endl; }
template<> inline void canonical::S0::exit(canonical::Canonical&) { TestLog::instance() << "exit_S0 " << std::endl; }
template<> inline void canonical::S1::exit(canonical::Canonical&) { TestLog::instance() << "exit_S1 " << std::endl; }
template<> inline void canonical::S11::exit(canonical::Canonical&) { TestLog::instance() << "exit_S11 " << std::endl; }
template<> inline void canonical::S12::exit(canonical::Canonical&) { TestLog::instance() << "exit_S12 " << std::endl; }
template<> inline void canonical::S2::exit(canonical::Canonical&) { TestLog::instance() << "exit_S2 " << std::endl; }
template<> inline void canonical::S21::exit(canonical::Canonical&) { TestLog::instance() << "exit_S21 " << std::endl; }
template<> inline void canonical::S211::exit(canonical::Canonical&) { TestLog::instance() << "exit_S211 " << std::endl; }

// during actions
template<> inline void canonical::S11::during(canonical::Canonical&) const { TestLog::instance() << "during_S11 " << std::endl; }
template<> inline void canonical::S211::during(canonical::Canonical&) const { TestLog::instance() << "during_S211 " << std::endl; }

// during action to implement a guarded auto-transition
template<> inline void canonical::S12::during(canonical::Canonical& stateMachine) const
{
    TestLog::instance() << "during_S12 " << std::endl;
    if(true)
    {
        Transition<ThisState, ThisState, canonical::S11> t(stateMachine);
    }
    // be careful to not perform anything else here, because you
    // may no longer be in the same state!

    // if you want to be really cheeky, you could try something like the following
    // to hit the new state, just watch out for recursion!
    // stateMachine.during();
}

} // namespace hsm
} // namespace eta
