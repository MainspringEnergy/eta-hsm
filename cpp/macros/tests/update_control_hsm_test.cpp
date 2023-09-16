#include "../example/UpdateControlHsm.hpp"

#include <gtest/gtest.h>

class ExampleControlTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    eta::hsm::update_control::UpdateControlHsm update_control_hsm_;
};


TEST_F(ExampleControlTest, InitialConditionsTest)
{
    EXPECT_EQ(update_control_hsm_.identify(), eta::hsm::update_control::UpdateControlHsmStateEnum::eReady);
}