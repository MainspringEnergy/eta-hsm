#include <gtest/gtest.h>

#include "../ExampleControl.hpp"

namespace eta_hsm {
namespace examples {
namespace controller {
namespace tests {

class ExampleControlTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    ExampleControl example_control_hsm_;
};

TEST_F(ExampleControlTest, InitialConditionsTest)
{
    EXPECT_EQ(example_control_hsm_.identify(), ExampleState::eSober);
}

TEST_F(ExampleControlTest, TransitionLoggingTest)
{
    EXPECT_EQ(example_control_hsm_.identify(), ExampleState::eSober);
    example_control_hsm_.dispatch(ExampleEvent::eDrinkWiskey);
    example_control_hsm_.dispatch(ExampleEvent::eDrinkWiskey);
    example_control_hsm_.dispatch(ExampleEvent::eDrinkWiskey);
    EXPECT_EQ(example_control_hsm_.identify(), ExampleState::eDrunk);
}

}  // namespace tests
}  // namespace controller
}  // namespace examples
}  // namespace eta_hsm