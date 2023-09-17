#include "../examples/controller/ExampleControl.hpp"

#include <gtest/gtest.h>

class ExampleControlTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    eta_hsm::examples::controller::ExampleControl example_control_hsm_;
};


TEST_F(ExampleControlTest, InitialConditionsTest)
{
    EXPECT_EQ(example_control_hsm_.identify(), eta::hsm::example_control::ExampleState::eSober);
}
