#pragma once

namespace eta::hsm {

// *********************** Begin State Machine Defintion in eta::hsm namespace ******************
// Like it or not, C++ requires us to specialize functions in the same namespace that they are declared in
// The only thing that we should be adding below are handleEvent() and init() for each state

template <>
template <typename Current>
inline void update_control::Top::handleEvent(update_control::UpdateControlHsm& stateMachine, const Current& current,
                                             Event event) const
{
    switch (event)
    {
        // We can handle events here if we want them to have default
        // behaviors that can then be overridden in specific states.
        default:
            break;
    }
    return;  // Top has no parent
}

template <>
template <typename Current>
inline void update_control::Off::handleEvent(update_control::UpdateControlHsm& stateMachine, const Current& current,
                                             Event event) const
{
    switch (event)
    {
        case update_control::EventEnum::eOn:
        {
            if (stateMachine.isVoltageStable())
            {
                Transition<Current, ThisState, update_control::Warming> t(stateMachine);
            }
            return;
        }
        default:
            break;
    }
    ParentState::handleEvent(stateMachine, current, event);
}

template <>
template <typename Current>
inline void update_control::On::handleEvent(update_control::UpdateControlHsm& stateMachine, const Current& current,
                                            Event event) const
{
    switch (event)
    {
        case update_control::EventEnum::eFault:
        {
            Transition<Current, ThisState, update_control::Fault> t(stateMachine);
            return;
        }
        case update_control::EventEnum::eOff:
        {
            Transition<Current, ThisState, update_control::Off> t(stateMachine);
            return;
        }
        default:
            break;
    }
    ParentState::handleEvent(stateMachine, current, event);
}

template <>
template <typename Current>
inline void update_control::Warming::handleEvent(update_control::UpdateControlHsm& stateMachine, const Current& current,
                                                 Event event) const
{
    switch (event)
    {
        case update_control::EventEnum::eWarm:
        {
            Transition<Current, ThisState, update_control::Ready> t(stateMachine);
            return;
        }
        default:
            break;
    }
    ParentState::handleEvent(stateMachine, current, event);
}

template <>
template <typename Current>
inline void update_control::Fault::handleEvent(update_control::UpdateControlHsm& stateMachine, const Current& current,
                                               Event event) const
{
    switch (event)
    {
        case update_control::EventEnum::eClear:
        {
            Transition<Current, ThisState, update_control::Off> t(stateMachine);
            return;
        }
        default:
            break;
    }
    ParentState::handleEvent(stateMachine, current, event);
}

// *********************************************************************************************************************
//                                     *****  State Initialization  *****
//
// We have to declare the initialization behavior of any composite states.  This can optionally take additional actions,
// but at a minimum, they must declare initialization of their default substate.
// Note: They must be declared in bottom-up order.
// *********************************************************************************************************************
template <>
inline void update_control::On::init(update_control::UpdateControlHsm& stateMachine)
{
    // This declares which substate we default into
    Init<update_control::Warming> i(stateMachine);
}

template <>
inline void update_control::Top::init(update_control::UpdateControlHsm& stateMachine)
{
    // This declares which substate we default into
    Init<update_control::Off> i(stateMachine);
}

}  // namespace eta::hsm