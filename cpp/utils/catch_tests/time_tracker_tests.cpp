#include <fstream>
#include <iostream>
#include <string>

#include "eta/catch/catch.hpp"
#include "eta/ctrl/define.hpp"
#include "eta/logger/Logger.hpp"
#include "eta/testing/getDataFile.hpp"
#include "eta/time_tracker/TimeTracker.hpp"
#include "eta/varz/asCsvData.hpp"
#include "eta/varz/asCsvHeader.hpp"

#define CSV_IO_NO_THREAD
#include <csv/csv.h>

namespace eta {
namespace time_tracker {
namespace {

const char kDelim{','};

ETA_DEFINE_LOGGER(logger, "time_tracker_tests");

ETA_VARZ_ENUM(TestEnum, ctrl::Uint, eAlpha, eBravo, eCharlie);

FEATURE("TimeTracker", "[time_tracker]")
{
    GIVEN("A TimeTracker")
    {
        using LocalClock = eta::chrono::FakeLocalClock<ctrl::DataClock::rep, ctrl::DataClock::period>;
        auto clock = LocalClock{};

        TimeTracker<TestEnum, LocalClock> tracker(clock);

        SCENARIO("upon initialization, all states return 0")
        {
            CHECK(tracker.timeInState(TestEnum::eAlpha) == std::chrono::seconds{0});
            CHECK(tracker.timeInState(TestEnum::eBravo) == std::chrono::seconds{0});
            CHECK(tracker.timeInState(TestEnum::eCharlie) == std::chrono::seconds{0});
        }

        SCENARIO("enter and exit a series of states")
        {
            tracker.enter(TestEnum::eAlpha);
            clock.advance(std::chrono::seconds{2});
            CHECK(tracker.timeInState(TestEnum::eAlpha) == std::chrono::seconds{2});

            tracker.enter(TestEnum::eBravo);
            clock.advance(std::chrono::seconds{3});
            CHECK(tracker.timeInState(TestEnum::eAlpha) == std::chrono::seconds{5});
            CHECK(tracker.timeInState(TestEnum::eBravo) == std::chrono::seconds{3});

            tracker.enter(TestEnum::eCharlie);
            clock.advance(std::chrono::seconds{7});
            CHECK(tracker.timeInState(TestEnum::eAlpha) == std::chrono::seconds{12});
            CHECK(tracker.timeInState(TestEnum::eBravo) == std::chrono::seconds{10});
            CHECK(tracker.timeInState(TestEnum::eCharlie) == std::chrono::seconds{7});

            tracker.exit(TestEnum::eCharlie);
            clock.advance(std::chrono::seconds{5});
            CHECK(tracker.timeInState(TestEnum::eAlpha) == std::chrono::seconds{17});
            CHECK(tracker.timeInState(TestEnum::eBravo) == std::chrono::seconds{15});
            CHECK(tracker.timeInState(TestEnum::eCharlie) == std::chrono::seconds{0});

            tracker.exit(TestEnum::eBravo);
            clock.advance(std::chrono::seconds{1});
            CHECK(tracker.timeInState(TestEnum::eAlpha) == std::chrono::seconds{18});
            CHECK(tracker.timeInState(TestEnum::eBravo) == std::chrono::seconds{0});
            CHECK(tracker.timeInState(TestEnum::eCharlie) == std::chrono::seconds{0});

            tracker.exit(TestEnum::eAlpha);
            clock.advance(std::chrono::seconds{10});
            CHECK(tracker.timeInState(TestEnum::eAlpha) == std::chrono::seconds{0});
            CHECK(tracker.timeInState(TestEnum::eBravo) == std::chrono::seconds{0});
            CHECK(tracker.timeInState(TestEnum::eCharlie) == std::chrono::seconds{0});
        }
    }
}

}  // namespace
}  // namespace time_tracker
}  // namespace eta
