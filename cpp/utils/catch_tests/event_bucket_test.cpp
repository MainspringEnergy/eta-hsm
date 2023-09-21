// eta/hsm/event_bucket_test.cpp

#include "eta/catch/catch.hpp"
#include "eta/event/EventBucket.hpp"
#include "eta/logger/Logger.hpp"

namespace eta {
namespace event {
namespace testing {
namespace {

ETA_DEFINE_LOGGER(logger, "event_test");

enum class TestEnum {
    eNone,
    eOne,  // interpreted as lower priority
    eTwo,
    eThree,  // interpreted as higher priority
    eMax
};

FEATURE("EventBucket", "[utils]")
{
    GIVEN("An OrderedEventBucket and an EventBucket reference")
    {
        // The state machine itself will hold a concrete instance of some sort of derived EventBucket
        OrderedEventBucket<TestEnum> orderedBucket;
        // We will pass a reference of type "EventBucket" to the producers of events
        EventBucket<TestEnum>& bucket = orderedBucket;

        SCENARIO("Add and remove events")
        {
            bucket.addEvent(TestEnum::eOne);
            bucket.addEvent(TestEnum::eTwo);
            bucket.addEvent(TestEnum::eThree);

            CHECK(orderedBucket.size() == 3);
            CHECK(!orderedBucket.empty());
            CHECK(orderedBucket.back() == TestEnum::eThree);
            CHECK(orderedBucket.front() == TestEnum::eOne);
            CHECK(orderedBucket.size() == 3);
            CHECK(orderedBucket.getEvent() == TestEnum::eOne);
            CHECK(orderedBucket.size() == 2);
            CHECK(orderedBucket.getEvent() == TestEnum::eTwo);
            CHECK(orderedBucket.size() == 1);
            CHECK(orderedBucket.getEvent() == TestEnum::eThree);
            CHECK(orderedBucket.empty());
        }
    }

    GIVEN("An PrioritizedEventBucket and an EventBucket reference")
    {
        // The state machine itself will hold a concrete instance of some sort of derived EventBucket
        PrioritizedEventBucket<TestEnum> prioritizedBucket;
        // We will pass a reference of type "EventBucket" to the producers of events
        EventBucket<TestEnum>& bucket = prioritizedBucket;

        SCENARIO("Add and remove events")
        {
            bucket.addEvent(TestEnum::eTwo);
            bucket.addEvent(TestEnum::eThree);
            bucket.addEvent(TestEnum::eOne);

            CHECK(prioritizedBucket.size() == 3);
            CHECK(!prioritizedBucket.empty());
            CHECK(prioritizedBucket.top() == TestEnum::eOne);
            CHECK(prioritizedBucket.size() == 3);
            CHECK(prioritizedBucket.getEvent() == TestEnum::eOne);
            CHECK(prioritizedBucket.size() == 2);
            CHECK(prioritizedBucket.getEvent() == TestEnum::eTwo);
            CHECK(prioritizedBucket.size() == 1);
            CHECK(prioritizedBucket.getEvent() == TestEnum::eThree);
            CHECK(prioritizedBucket.empty());
        }

        SCENARIO("Add and clear events")
        {
            bucket.addEvent(TestEnum::eTwo);
            bucket.addEvent(TestEnum::eThree);
            bucket.addEvent(TestEnum::eOne);

            CHECK(prioritizedBucket.size() == 3);
            CHECK_NOTHROW(prioritizedBucket.clear());
            CHECK(prioritizedBucket.empty());
        }
    }

    GIVEN("A PooledEventBucket and an EventBucket reference")
    {
        // The state machine itself will hold a concrete instance of some sort of derived EventBucket
        PooledEventBucket<TestEnum> pooledBucket;
        // We will pass a reference of type "EventBucket" to the producers of events
        EventBucket<TestEnum>& bucket = pooledBucket;

        SCENARIO("Add and remove events")
        {
            bucket.addEvent(TestEnum::eTwo);
            bucket.addEvent(TestEnum::eThree);
            bucket.addEvent(TestEnum::eOne);

            CHECK(pooledBucket.size() == 3);
            CHECK(!pooledBucket.empty());
            CHECK(pooledBucket.getEvent() == TestEnum::eOne);
            CHECK(pooledBucket.size() == 2);
            CHECK(pooledBucket.getEvent() == TestEnum::eTwo);
            CHECK(pooledBucket.size() == 1);
            CHECK(pooledBucket.getEvent() == TestEnum::eThree);
            CHECK(pooledBucket.empty());
        }

        SCENARIO("Won't compile when enum exceed 64 bit")
        {
            // enum class Big : int
            // {
            //     eA = 0,
            //     eB = 63,
            //     eNone
            // };
            // PooledEventBucket<Big> bigPool;
        }
    }
}

}  // namespace
}  // namespace testing
}  // namespace event
}  // namespace eta
