#pragma once

#include "../../utils/TestLog.hpp"

namespace eta_hsm {
namespace examples {
namespace cd_player {

/// We could have defined custom entry/exit actions by directly overriding
/// the STATE's entry/exit functions in CdPlayer-hsm.hpp.  However, it is
/// typically easier to have eta_hsm::StateMachine auto-generate default
/// entry/exit MEMBER functions by specifying DefaultActions in StateMachineTraits.
template <>
inline void Player::entry<CdState::eTop>()
{
    utils::TestLog::instance() << "(enter) TopState" << std::endl;
}

template <>
inline void Player::entry<CdState::eStopped>()
{
    utils::TestLog::instance() << "(enter) Stopped" << std::endl;
}

template <>
inline void Player::entry<CdState::eOpen>()
{
    utils::TestLog::instance() << "(enter) Open" << std::endl;
}

template <>
inline void Player::entry<CdState::eEmpty>()
{
    utils::TestLog::instance() << "(enter) Empty" << std::endl;
}

template <>
inline void Player::entry<CdState::ePlaying>()
{
    utils::TestLog::instance() << "(enter) Playing" << std::endl;
}

template <>
inline void Player::entry<CdState::ePaused>()
{
    utils::TestLog::instance() << "(enter) Entry" << std::endl;
}

// exit actions
template <>
inline void Player::exit<CdState::eTop>()
{
    utils::TestLog::instance() << "(exit) TopState" << std::endl;
}

template <>
inline void Player::exit<CdState::eStopped>()
{
    utils::TestLog::instance() << "(exit) Stopped" << std::endl;
}

template <>
inline void Player::exit<CdState::eOpen>()
{
    utils::TestLog::instance() << "(exit) Open" << std::endl;
}

template <>
inline void Player::exit<CdState::eEmpty>()
{
    utils::TestLog::instance() << "(exit) Empty" << std::endl;
}

template <>
inline void Player::exit<CdState::ePlaying>()
{
    utils::TestLog::instance() << "(exit) Playing" << std::endl;
}

template <>
inline void Player::exit<CdState::ePaused>()
{
    utils::TestLog::instance() << "(exit) Entry" << std::endl;
}

}  // namespace cd_player
}  // namespace examples
}  // namespace eta_hsm