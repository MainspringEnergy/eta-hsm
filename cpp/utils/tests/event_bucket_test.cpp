// eta_hsm/utils/tests/event_bucket_test.cpp
#include <gtest/gtest.h>

#include "../EventBucket.hpp"
#include "../TestLog.hpp"

namespace eta_hsm {
namespace utils {
namespace tests {

enum class TestEnum {
    eNone,
    eOne,  // interpreted as lower priority
    eTwo,
    eThree,  // interpreted as higher priority
    eMax
};

TEST(EventBucketTest, OrderedEventBucketTEst)
{
    // The state machine itself will hold a concrete instance of some sort of derived EventBucket
    OrderedEventBucket<TestEnum> orderedBucket;
    // We will pass a reference of type "EventBucket" to the producers of events
    EventBucket<TestEnum>& bucket = orderedBucket;

    // Add and remove events
    bucket.addEvent(TestEnum::eOne);
    bucket.addEvent(TestEnum::eTwo);
    bucket.addEvent(TestEnum::eThree);

    EXPECT_EQ(orderedBucket.size(), 3);
    EXPECT_TRUE(!orderedBucket.empty());
    EXPECT_EQ(orderedBucket.back(), TestEnum::eThree);
    EXPECT_EQ(orderedBucket.front(), TestEnum::eOne);
    EXPECT_EQ(orderedBucket.size(), 3);
    EXPECT_EQ(orderedBucket.getEvent(), TestEnum::eOne);
    EXPECT_EQ(orderedBucket.size(), 2);
    EXPECT_EQ(orderedBucket.getEvent(), TestEnum::eTwo);
    EXPECT_EQ(orderedBucket.size(), 1);
    EXPECT_EQ(orderedBucket.getEvent(), TestEnum::eThree);
    EXPECT_TRUE(orderedBucket.empty());
}

TEST(EventBucketTest, PrioritizedEventBucketTest)
{
    // The state machine itself will hold a concrete instance of some sort of derived EventBucket
    PrioritizedEventBucket<TestEnum> prioritizedBucket;
    // We will pass a reference of type "EventBucket" to the producers of events
    EventBucket<TestEnum>& bucket = prioritizedBucket;

    // Add and remove events
    bucket.addEvent(TestEnum::eTwo);
    bucket.addEvent(TestEnum::eThree);
    bucket.addEvent(TestEnum::eOne);

    EXPECT_EQ(prioritizedBucket.size(), 3);
    EXPECT_FALSE(prioritizedBucket.empty());
    EXPECT_EQ(prioritizedBucket.top(), TestEnum::eOne);
    EXPECT_EQ(prioritizedBucket.size(), 3);
    EXPECT_EQ(prioritizedBucket.getEvent(), TestEnum::eOne);
    EXPECT_EQ(prioritizedBucket.size(), 2);
    EXPECT_EQ(prioritizedBucket.getEvent(), TestEnum::eTwo);
    EXPECT_EQ(prioritizedBucket.size(), 1);
    EXPECT_EQ(prioritizedBucket.getEvent(), TestEnum::eThree);
    EXPECT_TRUE(prioritizedBucket.empty());

    // Add and clear events
    bucket.addEvent(TestEnum::eTwo);
    bucket.addEvent(TestEnum::eThree);
    bucket.addEvent(TestEnum::eOne);

    EXPECT_EQ(prioritizedBucket.size(), 3);
    EXPECT_NO_THROW(prioritizedBucket.clear());
    EXPECT_TRUE(prioritizedBucket.empty());
}

}  // namespace tests
}  // namespace utils
}  // namespace eta_hsm
