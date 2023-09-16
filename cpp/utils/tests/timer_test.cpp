// eta/hsm/timer_test.cpp

#include "eta/catch/catch.hpp"
#include "eta/chrono/FakeClock.hpp"
#include "eta/event/Timer.hpp"
#include "eta/logger/Logger.hpp"
#include "eta/varz/enum.hpp"

namespace eta {
namespace event {
namespace testing {
namespace {

ETA_DEFINE_LOGGER(logger, "event_test");

enum class Event
{
    eNone,
    eOne,
    eTwo,
    eThree,
    eMax
};

ETA_VARZ_ENUM( State, uint
    , eNone
    , eRed
    , eGreen
    , eBlue
);

enum class UniqueId
{
    eNone,
    eAlpha,
    eBravo,
    eCharlie,
    eDelta,
    eEcho,
    eFoxtrot,
};

FEATURE("Timer", "[utils]")
{
    GIVEN("A clock")
    {
        using Clock = eta::chrono::FakeClock<>;
        auto epoch = std::chrono::time_point<Clock>();

        SCENARIO("Timer expires when it is supposed to")
        {
            auto timestamp = epoch + std::chrono::seconds(1234);

            Timer<TimerTraits<Clock, Event, State>> timer(Event::eOne, State::eRed, timestamp);

            CHECK(timer.event() == Event::eOne);
            CHECK(timer.groupId() == State::eRed);

            CHECK(!timer.expired(timestamp - std::chrono::seconds(30)));
            CHECK(timer.expired(timestamp + std::chrono::seconds(30)));
        }

        SCENARIO("Timers sort in the correct order")
        {
            event::Timer<TimerTraits<Clock, Event, State>> firstTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(15));
            event::Timer<TimerTraits<Clock, Event, State>> secondTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(30));

            CHECK(secondTimer > firstTimer);
            CHECK(!(secondTimer < firstTimer));
        }
    }
}

FEATURE("TimerBank", "[utils]")
{
    GIVEN("A TimerBank")
    {
        using Clock = eta::chrono::FakeClock<>;
        auto epoch = std::chrono::time_point<Clock>();
        TimerBank<TimerTraits<Clock, Event, State>> bank;

        SCENARIO("Bank starts out empty and returns eNone events")
        {
            CHECK(bank.empty());
            CHECK(bank.checkForSingleFiredEvent(epoch) == Event::eNone);
        }

        SCENARIO("Requesting to clear an non-existent timer does not fail")
        {
            CHECK(bank.empty());
            bank.clearTimer(Event::eTwo, State::eBlue);
            CHECK(true);
        }

        SCENARIO("Timers go off in expiration order, even if added out of order")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(100));
            bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eThree, State::eGreen, epoch + std::chrono::seconds(150));

            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(25)) == Event::eNone);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)) == Event::eTwo);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)) == Event::eNone);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)) == Event::eOne);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)) == Event::eNone);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(175)) == Event::eThree);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(175)) == Event::eNone);
        }

        SCENARIO("Checking bank after multiple timers have expired only fires the first, second remains for next check")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(75));

            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(25)) == Event::eNone);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eOne);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eTwo);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eNone);
        }

        SCENARIO("Two timers with same expiration come out one at a time, order not defined")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(50));

            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(25)) == Event::eNone);
            Event firstEvent = bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100));
            Event secondEvent = bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100));
            CHECK( firstEvent != Event::eNone );
            CHECK( secondEvent != Event::eNone );
            CHECK( firstEvent != secondEvent );
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eNone);
        }

        SCENARIO("Clearing timer prevents it from firing")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(75));
            bank.clearTimer(Event::eOne, State::eRed);

            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(25)) == Event::eNone);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eTwo);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eNone);
        }

        SCENARIO("Duplicating timer overwrites first timer")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(100));

            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)) == Event::eNone);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)) == Event::eOne);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)) == Event::eNone);
        }

        SCENARIO("Same utils associated with different states handled independently")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eOne, State::eBlue, epoch + std::chrono::seconds(100));

            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)) == Event::eOne);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(75)) == Event::eNone);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)) == Event::eOne);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(125)) == Event::eNone);
        }
    }
}

FEATURE("UniqueId", "[utils]")
{
    GIVEN("A TimerBank supporting Unique Ids on events")
    {
        using Clock = eta::chrono::FakeClock<>;
        auto epoch = std::chrono::time_point<Clock>();
        TimerBank<TimerTraits<Clock, Event, State, UniqueId>> bank;


        SCENARIO("Requesting to clear an non-existent timer does not fail")
        {
            CHECK(bank.empty());
            bank.clearTimer(Event::eTwo, State::eBlue, UniqueId::eBravo);
            CHECK(true);
        }

        SCENARIO("Clearing with unique id only clears labeled timer")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(25));
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50), UniqueId::eAlpha);
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(75), UniqueId::eBravo);
            bank.clearTimer(Event::eOne, State::eRed, UniqueId::eAlpha);

            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(30)) == Event::eOne);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(60)) == Event::eNone);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eOne);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eNone);
        }

        SCENARIO("Clearing without unique id does not clear labeled timers")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(25));
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50), UniqueId::eAlpha);
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(75), UniqueId::eBravo);
            bank.clearTimer(Event::eOne, State::eRed);

            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(30)) == Event::eNone);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(60)) == Event::eOne);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eOne);
            CHECK(bank.checkForSingleFiredEvent(epoch + std::chrono::seconds(100)) == Event::eNone);
        }
    }
}

FEATURE("StaticTimerBank", "[utils]")
{
    GIVEN("A StaticTimerBank and EventBucket")
    {
        using Clock = eta::chrono::FakeClock<>;
        auto epoch = std::chrono::time_point<Clock>();
        StaticTimerBank<TimerTraits<Clock, Event, State>> bank;
        OrderedEventBucket<Event> bucket;

        SCENARIO("Bank starts out empty and does not fire any events")
        {
            CHECK(bucket.empty());
            bank.checkTimers(epoch + std::chrono::seconds(10), bucket);
            CHECK(bucket.empty());
        }

        SCENARIO("Requesting to clear an unarmed timer does not fail")
        {
            bank.clearTimer(State::eBlue);
            CHECK(true);
        }

        SCENARIO("Timers go off in expiration order, even if added out of order")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(100));
            bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eThree, State::eGreen, epoch + std::chrono::seconds(150));

            CHECK(bucket.empty());
            bank.checkTimers(epoch + std::chrono::seconds(60), bucket);
            CHECK(bucket.getEvent() == Event::eTwo);
            CHECK(bucket.empty());

            bank.checkTimers(epoch + std::chrono::seconds(110), bucket);
            CHECK(bucket.getEvent() == Event::eOne);
            CHECK(bucket.empty());

            // Checking when no timers have expired should not produce events
            bank.checkTimers(epoch + std::chrono::seconds(120), bucket);
            CHECK(bucket.empty());

            bank.checkTimers(epoch + std::chrono::seconds(160), bucket);
            CHECK(bucket.getEvent() == Event::eThree);
            CHECK(bucket.empty());
        }

        SCENARIO("Checking bank after multiple timers have expired places both events in the bucket")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(75));

            CHECK(bucket.empty());
            bank.checkTimers(epoch + std::chrono::seconds(100), bucket);
            CHECK(bucket.size() == 2);
            // These will come out in state definition order
            CHECK(bucket.getEvent() == Event::eOne);
            CHECK(bucket.getEvent() == Event::eTwo);
            CHECK(bucket.empty());
        }

        SCENARIO("Clearing timer prevents it from firing")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eTwo, State::eBlue, epoch + std::chrono::seconds(75));
            bank.clearTimer(State::eRed);

            CHECK(bucket.empty());
            bank.checkTimers(epoch + std::chrono::seconds(100), bucket);
            CHECK(bucket.getEvent() == Event::eTwo);
            CHECK(bucket.empty());
        }

        SCENARIO("Duplicating state overwrites first timer")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eTwo, State::eRed, epoch + std::chrono::seconds(100));

            CHECK(bucket.empty());
            bank.checkTimers(epoch + std::chrono::seconds(60), bucket);
            // eOne utils did NOT fire
            CHECK(bucket.empty());

            CHECK(bucket.empty());
            bank.checkTimers(epoch + std::chrono::seconds(110), bucket);
            CHECK(bucket.getEvent() == Event::eTwo);
            CHECK(bucket.empty());
        }

        SCENARIO("Same utils associated with different states handled independently")
        {
            bank.addTimer(Event::eOne, State::eRed, epoch + std::chrono::seconds(50));
            bank.addTimer(Event::eOne, State::eBlue, epoch + std::chrono::seconds(100));

            CHECK(bucket.empty());
            bank.checkTimers(epoch + std::chrono::seconds(60), bucket);
            CHECK(bucket.getEvent() == Event::eOne);
            CHECK(bucket.empty());

            CHECK(bucket.empty());
            bank.checkTimers(epoch + std::chrono::seconds(110), bucket);
            CHECK(bucket.getEvent() == Event::eOne);
            CHECK(bucket.empty());
        }
    }
}


} // namespace
} // namespace testing
} // namespace hsm
} // namespace eta
