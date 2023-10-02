// CdPlayer-hsm.hpp

#pragma once

// *********************** Begin State Machine Defintion in eta::hsm namespace ******************
// Like it or not, C++ requires us to specialize functions in the same namespce that they are declared in
// The only thing that we should be adding below are eventHandler() and during() for each state.
namespace eta_hsm {

// Do NOT open up eta_hsm::examples::cd_player namespace in the header

template <>
template <typename Current>
inline void examples::cd_player::Top::handleEvent(examples::cd_player::Player& player, const Current& current,
                                                  Event event) const
{
    switch (event)
    {
        // We can handle events here if we want them to have default
        // behaviors that can then be overridden in specific states.
        default:
            break;
    }
    return;  // TopState has no parent
}

template <>
template <typename Current>
inline void examples::cd_player::Stopped::handleEvent(examples::cd_player::Player& player, const Current& current,
                                                      Event event) const
{
    switch (event)
    {
        case examples::cd_player::CdEvent::ePlay:
        {
            Transition<Current, ThisState, examples::cd_player::Playing> t(player);
            player.start_playback();
            return;
        }
        case examples::cd_player::CdEvent::eOpenClose:
        {
            Transition<Current, ThisState, examples::cd_player::Open> t(player);
            player.open_drawer();
            return;
        }
        case examples::cd_player::CdEvent::eStop:
        {
            // no transition
            player.stopped_again();
            return;
        }
        default:
            break;
    }

    return ParentState::handleEvent(player, current, event);
}

template <>
template <typename Current>
inline void examples::cd_player::Open::handleEvent(examples::cd_player::Player& player, const Current& current,
                                                   Event event) const
{
    switch (event)
    {
        case examples::cd_player::CdEvent::eOpenClose:
        {
            Transition<Current, ThisState, examples::cd_player::Empty> t(player);
            player.close_drawer();
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(player, current, event);
}

template <>
template <typename Current>
inline void examples::cd_player::Empty::handleEvent(examples::cd_player::Player& player, const Current& current,
                                                    Event event) const
{
    switch (event)
    {
        case examples::cd_player::CdEvent::eCdDetected:
        {
            // TODO: check for disk tye
            Transition<Current, ThisState, examples::cd_player::Stopped> t(player);
            player.store_cd_info();
            return;
        }
        case examples::cd_player::CdEvent::eOpenClose:
        {
            Transition<Current, ThisState, examples::cd_player::Open> t(player);
            player.open_drawer();
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(player, current, event);
}

template <>
template <typename Current>
inline void examples::cd_player::Playing::handleEvent(examples::cd_player::Player& player, const Current& current,
                                                      Event event) const
{
    switch (event)
    {
        case examples::cd_player::CdEvent::eStop:
        {
            Transition<Current, ThisState, examples::cd_player::Stopped> t(player);
            player.stop_playback();
            return;
        }
        case examples::cd_player::CdEvent::ePause:
        {
            Transition<Current, ThisState, examples::cd_player::Paused> t(player);
            player.pause_playback();
            return;
        }
        case examples::cd_player::CdEvent::eOpenClose:
        {
            Transition<Current, ThisState, examples::cd_player::Open> t(player);
            player.stop_and_open();
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(player, current, event);
}

template <>
template <typename Current>
inline void examples::cd_player::Paused::handleEvent(examples::cd_player::Player& player, const Current& current,
                                                     Event event) const
{
    switch (event)
    {
        case examples::cd_player::CdEvent::eStop:
        {
            Transition<Current, ThisState, examples::cd_player::Stopped> t(player);
            player.stop_playback();
            return;
        }
        case examples::cd_player::CdEvent::eEndPause:
        {
            Transition<Current, ThisState, examples::cd_player::Playing> t(player);
            player.resume_playback();
            return;
        }
        case examples::cd_player::CdEvent::eOpenClose:
        {
            Transition<Current, ThisState, examples::cd_player::Open> t(player);
            player.stop_and_open();
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(player, current, event);
}

// We don't have to implement State::handleEvent at all if it does not do anything
// different from the superstate

// We do have to declare the initialization behavior of any composite states
// This can optionally take additional actions, but at a minimum, they must declare
// initialization of their default substate.
// Note: They must be declared in bottom-up order.
template <>
inline void examples::cd_player::Top::init(examples::cd_player::Player& player)
{
    // This declares which substate we default into
    Init<examples::cd_player::Stopped> i(player);
    // QUESTION: How often does this get run?
    utils::TestLog::instance() << "(init) TopState" << std::endl;
}

}  // namespace eta_hsm