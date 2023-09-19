
#include "wise_enum/wise_enum.h"
#include "../FakeClock.hpp"
#include "../TimeTracker.hpp"
#include <gtest/gtest.h>

#include <string>

namespace eta_hsm {
namespace utils {
namespace tests {

WISE_ENUM_CLASS((TestEnum, int64_t), eAlpha, eBravo, eCharlie)

TEST(TimeTrackerTest, TimeTrackerTest)
{
    auto clock = FakeClock{};

    TimeTracker<TestEnum, FakeClock> tracker(clock);

    // upon initialization, all states return 0
    EXPECT_EQ(tracker.timeInState(TestEnum::eAlpha), std::chrono::seconds{0});
    EXPECT_EQ(tracker.timeInState(TestEnum::eBravo), std::chrono::seconds{0});
    EXPECT_EQ(tracker.timeInState(TestEnum::eCharlie), std::chrono::seconds{0});

    // enter and exit a series of states
    tracker.enter(TestEnum::eAlpha);
    clock.advance(std::chrono::seconds{2});
    EXPECT_EQ(tracker.timeInState(TestEnum::eAlpha), std::chrono::seconds{2});

    tracker.enter(TestEnum::eBravo);
    clock.advance(std::chrono::seconds{3});
    EXPECT_EQ(tracker.timeInState(TestEnum::eAlpha), std::chrono::seconds{5});
    EXPECT_EQ(tracker.timeInState(TestEnum::eBravo), std::chrono::seconds{3});

    tracker.enter(TestEnum::eCharlie);
    clock.advance(std::chrono::seconds{7});
    EXPECT_EQ(tracker.timeInState(TestEnum::eAlpha), std::chrono::seconds{12});
    EXPECT_EQ(tracker.timeInState(TestEnum::eBravo), std::chrono::seconds{10});
    EXPECT_EQ(tracker.timeInState(TestEnum::eCharlie), std::chrono::seconds{7});

    tracker.exit(TestEnum::eCharlie);
    clock.advance(std::chrono::seconds{5});
    EXPECT_EQ(tracker.timeInState(TestEnum::eAlpha), std::chrono::seconds{17});
    EXPECT_EQ(tracker.timeInState(TestEnum::eBravo), std::chrono::seconds{15});
    EXPECT_EQ(tracker.timeInState(TestEnum::eCharlie), std::chrono::seconds{0});

    tracker.exit(TestEnum::eBravo);
    clock.advance(std::chrono::seconds{1});
    EXPECT_EQ(tracker.timeInState(TestEnum::eAlpha), std::chrono::seconds{18});
    EXPECT_EQ(tracker.timeInState(TestEnum::eBravo), std::chrono::seconds{0});
    EXPECT_EQ(tracker.timeInState(TestEnum::eCharlie), std::chrono::seconds{0});

    tracker.exit(TestEnum::eAlpha);
    clock.advance(std::chrono::seconds{10});
    EXPECT_EQ(tracker.timeInState(TestEnum::eAlpha), std::chrono::seconds{0});
    EXPECT_EQ(tracker.timeInState(TestEnum::eBravo), std::chrono::seconds{0});
    EXPECT_EQ(tracker.timeInState(TestEnum::eCharlie), std::chrono::seconds{0});
}

} // namespace tests
} // namespace utils
} // namespace eta_hsm
