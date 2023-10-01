// CDPlayer.hpp

#include <chrono>

#include "../../Hsm.hpp"
#include "../../utils/TestLog.hpp"

namespace eta_hsm {
namespace examples {
namespace cd_player {

enum class CdEvent {
    ePlay,
    eOpenClose,
    eStop,
    eCdDetected,
    ePause,
    eEndPause
};

enum class CdState {
    eTop,
    eStopped,
    eOpen,
    eEmpty,
    ePlaying,
    ePaused,
};

using PlayerTraits = eta_hsm::StateMachineTraits<CdEvent,
                                                 CdState,
                                                 std::chrono::steady_clock,
                                                 DefaultActions::eEntryExitOnly>;

/// While it is possible to declare and manipulate states directly (see examples::cd_player_basic),
/// inheriting from eta_hsm::StateMachine makes it somewhat easier and provides more
/// advanced capabilities that will be useful for more complex situations (see examples::controller).
class Player : public eta_hsm::StateMachine<Player, PlayerTraits> {
public:
    using Input = EmptyType;  // should this really be necessary if we are using eEntryExitOnly?

    Player();

    // Dummy action functions
    // I don't like these being public, but they have to be callable from state objects
    void start_playback() { utils::TestLog::instance() << "(action) start_playback" << std::endl; }
    void open_drawer() { utils::TestLog::instance() << "(action) open_drawer" << std::endl; }
    void close_drawer() { utils::TestLog::instance() << "(action) close_drawer" << std::endl; }
    void store_cd_info() { utils::TestLog::instance() << "(action) store_cd_info" << std::endl; }
    void stop_playback() { utils::TestLog::instance() << "(action) stop_playback()" << std::endl; }
    void pause_playback() { utils::TestLog::instance() << "(action) pause_playback" << std::endl; }
    void resume_playback() { utils::TestLog::instance() << "(action) resume_playback" << std::endl; }
    void stop_and_open() { utils::TestLog::instance() << "(action) stop_and_open" << std::endl; }
    void stopped_again() { utils::TestLog::instance() << "(action) stopped_again" << std::endl; }

    template <CdState state>
    void entry() {}

    template <CdState state>
    void exit() {}

private:
};

/// This intermediate template is just to make the state tree declaration below more compact
template <CdState kState>
using CdTraits = StateTraits<Player, CdState, kState>;

/// This is where we declare what states exist and the relationships between them.
using Top = eta_hsm::TopState<CdTraits<CdState::eTop>>;
using Stopped = eta_hsm::LeafState<CdTraits<CdState::eStopped>, Top>;
using Open = eta_hsm::LeafState<CdTraits<CdState::eOpen>, Top>;
using Empty = eta_hsm::LeafState<CdTraits<CdState::eEmpty>, Top>;
using Playing = eta_hsm::LeafState<CdTraits<CdState::ePlaying>, Top>;
using Paused = eta_hsm::LeafState<CdTraits<CdState::ePaused>, Top>;

}  // namespace cd_player
}  // namespace examples
}  // namespace eta_hsm

#include "CdPlayer-hsm.hpp"
#include "CdPlayer-inl.hpp"