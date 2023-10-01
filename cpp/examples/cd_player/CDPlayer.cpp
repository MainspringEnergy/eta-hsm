// CDPlayer.cpp

#include "CDPlayer.hpp"

#include <chrono>

#include "../../utils/TestLog.hpp"

namespace eta_hsm {
namespace examples {
namespace cd_player {

// The missing constructor
Player::Player()
{
    // We are responsible for initializing mState to something useful
    // In order to make sure we hit all of the correct entry methods
    // (and avoid duplicating initial/default states) the easist way
    // to do this is to kick off a self-transition from Top to Top.
    eta_hsm::Transition<Top, Top, Top> t(*this);
}

}  // namespace cd_player
}  // namespace examples
}  // namespace eta_hsm
