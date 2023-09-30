// main.cpp
#include "CDPlayer.hpp"

using namespace eta_hsm::examples::cd_player;
using namespace eta_hsm::utils;

int main()
{
    TestLog::instance() << "main: Hello Player!" << std::endl;
    Player player;

    // go to Open, call on_exit on Empty, then action, then on_entry on Open
    TestLog::instance() << "Inject open_close utils" << std::endl;
    player.dispatch(CdEvent::eOpenClose);

    TestLog::instance() << "Inject open_close utils" << std::endl;
    player.dispatch(CdEvent::eOpenClose);

    TestLog::instance() << "Inject detect CD" << std::endl;
    player.dispatch(CdEvent::eCdDetected);

    // no need to call play() as the previous transition does it in its action method
    TestLog::instance() << "Inject play utils MANUALLY" << std::endl;
    player.dispatch(CdEvent::ePlay);

    // at this point, Play is active
    TestLog::instance() << "Inject pause utils" << std::endl;
    player.dispatch(CdEvent::ePause);

    // go back to Playing
    TestLog::instance() << "Inject end_pause event" << std::endl;
    player.dispatch(CdEvent::eEndPause);

    TestLog::instance() << "Inject pause event" << std::endl;
    player.dispatch(CdEvent::ePause);

    TestLog::instance() << "Inject stop event" << std::endl;
    player.dispatch(CdEvent::STOP);

    // event leading to the same state
    // no action method called as it is not present in the transition table
    TestLog::instance() << "Inject stop event" << std::endl;
    player.dispatch(CdEvent::eStop);

    TestLog::instance().disable();

    auto start_time = std::chrono::high_resolution_clock::now();
    auto N = 1E6;
    for (auto idx = 0; idx < N; idx++)
    {
        player.dispatch(CdEvent::ePlay);
        player.dispatch(CdEvent::eStop);
    }
    auto stop_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time);
    TestLog::instance().enable();
    TestLog::instance() << N << " play/stop cycles in " << duration.count() << " microseconds" << std::endl;

    return 0;
}  // main()

