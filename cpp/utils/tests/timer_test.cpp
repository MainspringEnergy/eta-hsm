// timer_test.cpp

#include "../Timer.hpp"

#include <gtest/gtest.h>

#include <chrono>

#include "../FakeClock.hpp"

namespace eta_hsm {
namespace utils {
namespace tests {

enum class Event { eNone, eOne, eTwo, eThree, eMax };

WISE_ENUM_CLASS((State, uint32_t), eNone, eRed, eGreen, eBlue)

enum class UniqueId {
    eNone,
    eAlpha,
    eBravo,
    eCharlie,
    eDelta,
    eEcho,
    eFoxtrot,
};

using Clock = FakeClock;
auto epoch = FakeClock::time_point();

TEST(TimerTest, TimerExpiration)
{
    // Timer expires when it is supposed to
    auto timestamp = epoch + std::chrono::seconds(1234);
    Timer<TimerTraits<Clock, Event, State>> timer(Event::eOne, State::eRed, timestamp);

    EXPECT_EQ(timer.event(), Event::eOne);
    EXPECT_EQ(timer.groupId(), State::eRed);

    EXPECT_FALSE(timer.expired(timestamp - std::chrono::seconds(30)));
    EXPECT_TRUE(timer.expired(timestamp + std::chrono::seconds(30)));
}

TEST(TimerTest, TimerSorting)
{
    // Timers sort in the correct order
    Timer<TimerTraits<Clock, Event, State>> firstTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(15));
    Timer<TimerTraits<Clock, Event, State>> secondTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(30));

    EXPECT_TRUE(secondTimer > firstTimer);
    EXPECT_FALSE(secondTimer < firstTimer);
}

TEST(TimerTest, TimerBank)
{
    // GIVEN("A TimerBank")
    TimerBank<TimerTraits<Clock, Event, State>> bank;

    // SCENARIO("Bank starts out empty and returns eNone events")
    EXPECT_TRUE(bank.empty());
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch), Event::eNone);

    // SCENARIO("Requesting to clear a non-existent timer does not fail")
    EXPECT_TRUE(bank.empty());
    bank.clearTimer(Event::eTwo, State::eBlue);
    EXPECT_TRUE(true);

    // SCENARIO("Timers go off in expiration order, even if added out of order")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(100));
    bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eThree, State::eGreen, epoch + std::chrono::seconds(150));

    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(25)), Event::eNone);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)), Event::eTwo);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)), Event::eNone);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)), Event::eOne);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)), Event::eNone);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(175)), Event::eThree);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(175)), Event::eNone);

    // SCENARIO("Checking bank after multiple timers have expired only fires the first, second remains for next check")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(75));

    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(25)), Event::eNone);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eOne);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eTwo);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eNone);

    // SCENARIO("Two timers with same expiration come out one at a time, order not defined")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(50));

    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(25)), Event::eNone);
    Event firstEvent = bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100));
    Event secondEvent = bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100));
    EXPECT_NE(firstEvent, Event::eNone);
    EXPECT_NE(secondEvent, Event::eNone);
    EXPECT_NE(firstEvent, secondEvent);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eNone);

    // SCENARIO("Clearing timer prevents it from firing")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(75));
    bank.clearTimer(Event::eOne, State::eRed);

    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(25)), Event::eNone);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eTwo);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eNone);

    // SCENARIO("Duplicating timer overwrites first timer")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(100));

    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)), Event::eNone);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)), Event::eOne);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)), Event::eNone);

    // SCENARIO("Same events associated with different states handled independently")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eOne, State::eBlue, epoch + std::chrono::seconds(100));

    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)), Event::eOne);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)), Event::eNone);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)), Event::eOne);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)), Event::eNone);
}

TEST(TimerTest, UniqueId)
{
    // GIVEN("A TimerBank supporting Unique Ids on events")
    TimerBank<TimerTraits<Clock, Event, State, UniqueId>> bank;

    // SCENARIO("Requesting to clear an non-existent timer does not fail")
    EXPECT_TRUE(bank.empty());
    bank.clearTimer(Event::eTwo, State::eBlue, UniqueId::eBravo);
    EXPECT_TRUE(true);

    // SCENARIO("Clearing with unique id only clears labeled timer")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(25));
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50), UniqueId::eAlpha);
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(75), UniqueId::eBravo);
    bank.clearTimer(Event::eOne, State::eRed, UniqueId::eAlpha);

    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(30)), Event::eOne);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(60)), Event::eNone);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eOne);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eNone);

    //  SCENARIO("Clearing without unique id does not clear labeled timers")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(25));
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50), UniqueId::eAlpha);
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(75), UniqueId::eBravo);
    bank.clearTimer(Event::eOne, State::eRed);

    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(30)), Event::eNone);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(60)), Event::eOne);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eOne);
    EXPECT_EQ(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)), Event::eNone);
}

TEST(TimerTest, StaticTimerBank)
{
    // GIVEN("A StaticTimerBank and EventBucket")
    StaticTimerBank<TimerTraits<Clock, Event, State>> bank;
    OrderedEventBucket<Event> bucket;

    // SCENARIO("Bank starts out empty and does not fire any events")
    EXPECT_TRUE(bucket.empty());
    bank.checkTimers(epoch + std::chrono::seconds(10), bucket);
    EXPECT_TRUE(bucket.empty());

    // SCENARIO("Requesting to clear an unarmed timer does not fail")
    bank.clearTimer(State::eBlue);
    EXPECT_TRUE(true);

    // SCENARIO("Timers go off in expiration order, even if added out of order")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(100));
    bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eThree, State::eGreen, epoch + std::chrono::seconds(150));

    EXPECT_TRUE(bucket.empty());
    bank.checkTimers(epoch + std::chrono::seconds(60), bucket);
    EXPECT_EQ(bucket.getEvent(), Event::eTwo);
    EXPECT_TRUE(bucket.empty());

    bank.checkTimers(epoch + std::chrono::seconds(110), bucket);
    EXPECT_EQ(bucket.getEvent(), Event::eOne);
    EXPECT_TRUE(bucket.empty());

    // Checking when no timers have expired should not produce events
    bank.checkTimers(epoch + std::chrono::seconds(120), bucket);
    EXPECT_TRUE(bucket.empty());

    bank.checkTimers(epoch + std::chrono::seconds(160), bucket);
    EXPECT_EQ(bucket.getEvent(), Event::eThree);
    EXPECT_TRUE(bucket.empty());

    // SCENARIO("Checking bank after multiple timers have expired places both events in the bucket")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(75));

    EXPECT_TRUE(bucket.empty());
    bank.checkTimers(epoch + std::chrono::seconds(100), bucket);
    EXPECT_EQ(bucket.size(), 2);
    // These will come out in state definition order
    EXPECT_EQ(bucket.getEvent(), Event::eOne);
    EXPECT_EQ(bucket.getEvent(), Event::eTwo);
    EXPECT_TRUE(bucket.empty());

    // SCENARIO("Clearing timer prevents it from firing")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(75));
    bank.clearTimer(State::eRed);

    EXPECT_TRUE(bucket.empty());
    bank.checkTimers(epoch + std::chrono::seconds(100), bucket);
    EXPECT_EQ(bucket.getEvent(), Event::eTwo);
    EXPECT_TRUE(bucket.empty());

    // SCENARIO("Duplicating state overwrites first timer")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eTwo, State::eRed, epoch + std::chrono::seconds(100));

    EXPECT_TRUE(bucket.empty());
    bank.checkTimers(epoch + std::chrono::seconds(60), bucket);
    // eOne utils did NOT fire
    EXPECT_TRUE(bucket.empty());

    EXPECT_TRUE(bucket.empty());
    bank.checkTimers(epoch + std::chrono::seconds(110), bucket);
    EXPECT_EQ(bucket.getEvent(), Event::eTwo);
    EXPECT_TRUE(bucket.empty());

    // SCENARIO("Same utils associated with different states handled independently")
    bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
    bank.addTimer(Event::eOne, State::eBlue, epoch + std::chrono::seconds(100));

    EXPECT_TRUE(bucket.empty());
    bank.checkTimers(epoch + std::chrono::seconds(60), bucket);
    EXPECT_EQ(bucket.getEvent(), Event::eOne);
    EXPECT_TRUE(bucket.empty());

    EXPECT_TRUE(bucket.empty());
    bank.checkTimers(epoch + std::chrono::seconds(110), bucket);
    EXPECT_EQ(bucket.getEvent(), Event::eOne);
    EXPECT_TRUE(bucket.empty());
}

}  // namespace tests
}  // namespace utils
}  // namespace eta_hsm
