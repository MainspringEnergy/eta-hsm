// CDPlayer.cpp
#include "eta/hsm/Hsm.hpp"
#include "TestLog.hpp"
#include <chrono>

namespace eta {
namespace hsm {
namespace player {


enum class CdEvent
{
    PLAY,
    OPEN_CLOSE,
    STOP,
    CD_DETECTED,
    PAUSE,
    END_PAUSE
};

enum class CdState
{
    eTop,
    eStopped,
    eOpen,
    eEmpty,
    ePlaying,
    ePaused,
};

class Player
{
public:
    using Input = EmptyType;
    Player();
    using Event = CdEvent;
    static constexpr hsm::DefaultActions kDefaultActions = hsm::DefaultActions::eNothing;

    void next(const eta::hsm::TopState<StateTraits<Player, CdState, CdState::eTop>>& state) { mState = &state; }
    void dispatch(CdEvent evt) { mState->eventHandler(*this, evt); }

    // Dummy action functions
    // I don't like these being public, but they have to be callable from state objects
    void start_playback() { TestLog::instance() << "(action) start_playback" << std::endl; }
    void open_drawer() { TestLog::instance() << "(action) open_drawer" << std::endl; }
    void close_drawer() { TestLog::instance() << "(action) close_drawer" << std::endl; }
    void store_cd_info() { TestLog::instance() << "(action) store_cd_info" << std::endl; }
    void stop_playback() { TestLog::instance() << "(action) stop_playback()" << std::endl; }
    void pause_playback() { TestLog::instance() << "(action) pause_playback" << std::endl; }
    void resume_playback() { TestLog::instance() << "(action) resume_playback" << std::endl; }
    void stop_and_open() { TestLog::instance() << "(action) stop_and_open" << std::endl; }
    void stopped_again() { TestLog::instance() << "(action) stopped_again" << std::endl; }

private:
    const eta::hsm::TopState<StateTraits<Player, CdState, CdState::eTop>>* mState;
};

template<CdState kState>
using CdTraits = StateTraits<Player, CdState, kState>;

using Top = eta::hsm::TopState<CdTraits<CdState::eTop>>;
using Stopped = eta::hsm::LeafState<CdTraits<CdState::eStopped>, Top>;
using Open = eta::hsm::LeafState<CdTraits<CdState::eOpen>, Top>;
using Empty = eta::hsm::LeafState<CdTraits<CdState::eEmpty>, Top>;
using Playing = eta::hsm::LeafState<CdTraits<CdState::ePlaying>, Top>;
using Paused = eta::hsm::LeafState<CdTraits<CdState::ePaused>, Top>;

// The missing constructor
Player::Player()
{
    // Guessing wildly at how to initialize mState to something useful
    eta::hsm::Transition<Top, Top, Top> t(*this);
}

} // close player namespace because templates below must be specialized in their originally declared scope?
using namespace player;

template<>template<typename Current>
inline void Top::handleEvent(Player& player, const Current& current, Event event) const
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
inline void Stopped::handleEvent(Player& player, const Current& current, Event event) const
{
    switch(event)
    {
        case CdEvent::PLAY:
        {
            Transition<Current, ThisState, Playing> t(player);
            player.start_playback();
            return;
        }
        case CdEvent::OPEN_CLOSE:
        {
            Transition<Current, ThisState, Open> t(player);
            player.open_drawer();
            return;
        }
        case CdEvent::STOP:
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

template<>template<typename Current>
inline void Open::handleEvent(Player& player, const Current& current, Event event) const
{
    switch(event)
    {
        case CdEvent::OPEN_CLOSE:
        {
            Transition<Current, ThisState, Empty> t(player);
            player.close_drawer();
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(player, current, event);
}

template<>template<typename Current>
inline void Empty::handleEvent(Player& player, const Current& current, Event event) const
{
    switch(event)
    {
        case CdEvent::CD_DETECTED:
        {
            // TODO: check for disk tye
            Transition<Current, ThisState, Stopped> t(player);
            player.store_cd_info();
            return;
        }
        case CdEvent::OPEN_CLOSE:
        {
            Transition<Current, ThisState, Open> t(player);
            player.open_drawer();
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(player, current, event);
}

template<>template<typename Current>
inline void Playing::handleEvent(Player& player, const Current& current, Event event) const
{
    switch(event)
    {
        case CdEvent::STOP:
        {
            Transition<Current, ThisState, Stopped> t(player);
            player.stop_playback();
            return;
        }
        case CdEvent::PAUSE:
        {
            Transition<Current, ThisState, Paused> t(player);
            player.pause_playback();
            return;
        }
        case CdEvent::OPEN_CLOSE:
        {
            Transition<Current, ThisState, Open> t(player);
            player.stop_and_open();
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(player,current, event);
}

template<>template<typename Current>
inline void Paused::handleEvent(Player& player, const Current& current, Event event) const
{
    switch(event)
    {
        case CdEvent::STOP:
        {
            Transition<Current, ThisState, Stopped> t(player);
            player.stop_playback();
            return;
        }
        case CdEvent::END_PAUSE:
        {
            Transition<Current, ThisState, Playing> t(player);
            player.resume_playback();
            return;
        }
        case CdEvent::OPEN_CLOSE:
        {
            Transition<Current, ThisState, Open> t(player);
            player.stop_and_open();
            return;
        }
        default:
            break;
    }
    return ParentState::handleEvent(player, current, event);
}

// entry actions
template<> inline void Top::entry(Player&) { TestLog::instance() << "(enter) TopState" << std::endl; }
template<> inline void Stopped::entry(Player&) { TestLog::instance() << "(enter) Stopped" << std::endl; }
template<> inline void Open::entry(Player&) { TestLog::instance() << "(enter) Open" << std::endl; }
template<> inline void Empty::entry(Player&) { TestLog::instance() << "(enter) Empty" << std::endl; }
template<> inline void Playing::entry(Player&) { TestLog::instance() << "(enter) Playing" << std::endl; }
template<> inline void Paused::entry(Player&) { TestLog::instance() << "(enter) Entry" << std::endl; }

// exit actions
template<> inline void Top::exit(Player&) { TestLog::instance() << "(exit) TopState" << std::endl; }
template<> inline void Stopped::exit(Player&) { TestLog::instance() << "(exit) Stopped" << std::endl; }
template<> inline void Open::exit(Player&) { TestLog::instance() << "(exit) Open" << std::endl; }
template<> inline void Empty::exit(Player&) { TestLog::instance() << "(exit) Empty" << std::endl; }
template<> inline void Playing::exit(Player&) { TestLog::instance() << "(exit) Playing" << std::endl; }
template<> inline void Paused::exit(Player&) { TestLog::instance() << "(exit) Entry" << std::endl; }

// init actions (note the reverse ordering!)
template<> inline void player::Top::init(Player& h)
{
    // This declares which substate we default into
    Init<Stopped> i(h);
    // QUESTION: How often does this get run?
    TestLog::instance() << "(init) TopState" << std::endl;
}

} // namespace hsm
} // namespace eta
// Close remaining hsm::eta namesapces so that linker can find main()
using namespace eta::hsm::player;

int main()
{
    TestLog::instance() << "main: Hello Player!" << std::endl;
    Player p;

    // go to Open, call on_exit on Empty, then action, then on_entry on Open
    TestLog::instance() << "Inject open_close event" << std::endl;
    p.dispatch(CdEvent::OPEN_CLOSE);

    TestLog::instance() << "Inject open_close event" << std::endl;
    p.dispatch(CdEvent::OPEN_CLOSE);

    // will be rejected, wrong disk type
    TestLog::instance() << "Inject detect DVD" << std::endl;
    // TODO: add event properties
    //p.process_event(cd::cd_detected("louie, louie", cd::DISK_DVD));
    p.dispatch(CdEvent::CD_DETECTED);

    TestLog::instance() << "Inject detect CD" << std::endl;
    // TODO: add event properties
    //p.process_event(cd::cd_detected("louie, louie", cd::DISK_CD));
    p.dispatch(CdEvent::CD_DETECTED);

    // no need to call play() as the previous transition does it in its action method
    TestLog::instance() << "Inject play event MANUALLY" << std::endl;
    p.dispatch(CdEvent::PLAY);

    // at this point, Play is active
    TestLog::instance() << "Inject pause event" << std::endl;
    p.dispatch(CdEvent::PAUSE);

    // go back to Playing
    TestLog::instance() << "Inject end_pause event" << std::endl;
    p.dispatch(CdEvent::END_PAUSE);

    TestLog::instance() << "Inject pause event" << std::endl;
    p.dispatch(CdEvent::PAUSE);

    TestLog::instance() << "Inject stop event" << std::endl;
    p.dispatch(CdEvent::STOP);

    // event leading to the same state
    // no action method called as it is not present in the transition table
    TestLog::instance() << "Inject stop event" << std::endl;
    p.dispatch(CdEvent::STOP);

    TestLog::instance().disable();

    auto start_time = std::chrono::high_resolution_clock::now();
    auto N = 1E6;
    for(auto idx = 0; idx < N; idx++)
    {
        p.dispatch(CdEvent::PLAY);
        p.dispatch(CdEvent::STOP);
    }
    auto stop_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time);
    TestLog::instance().enable();
    TestLog::instance() << N << " play/stop cycles in " << duration.count() << " microseconds" << std::endl;

    return 0;;
} // main()
