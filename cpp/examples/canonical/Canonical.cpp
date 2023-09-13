// Canonical.cpp
#include "Canonical.hpp"

namespace eta_hsm {
namespace examples {
namespace canonical {


// The missing constructor
Canonical::Canonical() {
    // Guessing wildly at how to initialize mState to something useful
    eta_hsm::Transition<Top, Top, Top> t(*this);
}

} // close canonical namespace because templates below must be specialized in their originally declared scope?
} // examples

template<>template<typename Current>
inline void examples::canonical::Top::handleEvent(examples::canonical::Canonical& stateMachine, const Current& current, Event event) const
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
inline void examples::canonical::S0::handleEvent(examples::canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case examples::canonical::CanonicalEvent::E:
        {
            Transition<Current, ThisState, examples::canonical::S211> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::E_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void examples::canonical::S1::handleEvent(examples::canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case examples::canonical::CanonicalEvent::A:
        {
            Transition<Current, ThisState, examples::canonical::S1> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::A_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S1, Semantics::eLocal> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::B:
        {
            Transition<Current, ThisState, examples::canonical::S11> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::B_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S11, Semantics::eLocal> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::C:
        {
            Transition<Current, ThisState, examples::canonical::S2> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::C_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S2, Semantics::eLocal> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::D:
        {
            Transition<Current, ThisState, examples::canonical::S0> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::D_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S0, Semantics::eLocal> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::F:
        {
            Transition<Current, ThisState, examples::canonical::S211> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::F_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void examples::canonical::S11::handleEvent(examples::canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case examples::canonical::CanonicalEvent::G:
        {
            Transition<Current, ThisState, examples::canonical::S211> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::G_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::Z:
        {   // Not part of normal example, but used to get us into new state to test auto-transition
            Transition<Current, ThisState, examples::canonical::S12> t(stateMachine);
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void examples::canonical::S2::handleEvent(examples::canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case examples::canonical::CanonicalEvent::C:
        {
            Transition<Current, ThisState, examples::canonical::S1> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::C_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S1, Semantics::eLocal> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::F:
        {
            Transition<Current, ThisState, examples::canonical::S11> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::F_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S11, Semantics::eLocal> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void examples::canonical::S21::handleEvent(examples::canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case examples::canonical::CanonicalEvent::B:
        {
            Transition<Current, ThisState, examples::canonical::S211> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::B_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::H:
        {
            Transition<Current, ThisState, examples::canonical::S21> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::H_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S21, Semantics::eLocal> t(stateMachine);
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

template<>template<typename Current>
inline void examples::canonical::S211::handleEvent(examples::canonical::Canonical& stateMachine, const Current& currentState, Event event) const
{
    switch(event)
    {
        case examples::canonical::CanonicalEvent::B:
        {
            Transition<Current, ThisState, examples::canonical::S21> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::B_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S21, Semantics::eLocal> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::D:
        {
            Transition<Current, ThisState, examples::canonical::S211> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::D_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S211, Semantics::eLocal> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::G:
        {
            Transition<Current, ThisState, examples::canonical::S0> t(stateMachine);
            return;
        }
        case examples::canonical::CanonicalEvent::G_LOCAL:
        {
            Transition<Current, ThisState, examples::canonical::S0, Semantics::eLocal> t(stateMachine);
            return;
        }
        // TODO: Guard
        default:
            break;
    }
    return ParentState::handleEvent(stateMachine, currentState, event);
}

// init actions (note the reverse ordering!)
template<> inline void examples::canonical::S21::init(examples::canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<examples::canonical::S211> i(h);
    utils::TestLog::instance() << "init_S21 " << std::endl;
}

template<> inline void examples::canonical::S2::init(examples::canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<examples::canonical::S21> i(h);
    utils::TestLog::instance() << "init_S2 " << std::endl;
}

template<> inline void examples::canonical::S1::init(examples::canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<examples::canonical::S11> i(h);
    utils::TestLog::instance() << "init_S1 " << std::endl;
}

template<> inline void examples::canonical::S0::init(examples::canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<examples::canonical::S1> i(h);
    utils::TestLog::instance() << "init_S0 " << std::endl;
}

template<> inline void examples::canonical::Top::init(examples::canonical::Canonical& h)
{
    // This declares which substate we default into
    Init<examples::canonical::S0> i(h);
    utils::TestLog::instance() << "init_Top " << std::endl;
}

// entry actions
template<> inline void examples::canonical::Top::entry(examples::canonical::Canonical&) { utils::TestLog::instance() << "enter_Top " << std::endl; }
template<> inline void examples::canonical::S0::entry(examples::canonical::Canonical&) { utils::TestLog::instance() << "enter_S0 " << std::endl; }
template<> inline void examples::canonical::S1::entry(examples::canonical::Canonical&) { utils::TestLog::instance() << "enter_S1 " << std::endl; }
template<> inline void examples::canonical::S11::entry(examples::canonical::Canonical&) { utils::TestLog::instance() << "enter_S11 " << std::endl; }
template<> inline void examples::canonical::S12::entry(examples::canonical::Canonical&) { utils::TestLog::instance() << "enter_S12 " << std::endl; }
template<> inline void examples::canonical::S2::entry(examples::canonical::Canonical&) { utils::TestLog::instance() << "enter_S2 " << std::endl; }
template<> inline void examples::canonical::S21::entry(examples::canonical::Canonical&) { utils::TestLog::instance() << "enter_S21 " << std::endl; }
template<> inline void examples::canonical::S211::entry(examples::canonical::Canonical&) { utils::TestLog::instance() << "enter_S211 " << std::endl; }

// exit actions
template<> inline void examples::canonical::Top::exit(examples::canonical::Canonical&) { utils::TestLog::instance() << "exit_Top " << std::endl; }
template<> inline void examples::canonical::S0::exit(examples::canonical::Canonical&) { utils::TestLog::instance() << "exit_S0 " << std::endl; }
template<> inline void examples::canonical::S1::exit(examples::canonical::Canonical&) { utils::TestLog::instance() << "exit_S1 " << std::endl; }
template<> inline void examples::canonical::S11::exit(examples::canonical::Canonical&) { utils::TestLog::instance() << "exit_S11 " << std::endl; }
template<> inline void examples::canonical::S12::exit(examples::canonical::Canonical&) { utils::TestLog::instance() << "exit_S12 " << std::endl; }
template<> inline void examples::canonical::S2::exit(examples::canonical::Canonical&) { utils::TestLog::instance() << "exit_S2 " << std::endl; }
template<> inline void examples::canonical::S21::exit(examples::canonical::Canonical&) { utils::TestLog::instance() << "exit_S21 " << std::endl; }
template<> inline void examples::canonical::S211::exit(examples::canonical::Canonical&) { utils::TestLog::instance() << "exit_S211 " << std::endl; }

// during actions
template<> inline void examples::canonical::S11::during(examples::canonical::Canonical&) const { utils::TestLog::instance() << "during_S11 " << std::endl; }
template<> inline void examples::canonical::S211::during(examples::canonical::Canonical&) const { utils::TestLog::instance() << "during_S211 " << std::endl; }

// during action to implement a guarded auto-transition
template<> inline void examples::canonical::S12::during(examples::canonical::Canonical& stateMachine) const
{
    utils::TestLog::instance() << "during_S12 " << std::endl;
    if(true)
    {
        Transition<ThisState, ThisState, examples::canonical::S11> t(stateMachine);
    }
    // be careful to not perform anything else here, because you
    // may no longer be in the same state!

    // if you want to be really cheeky, you could try something like the following
    // to hit the new state, just watch out for recursion!
    // stateMachine.during();
}

} // namespace eta_hsm
