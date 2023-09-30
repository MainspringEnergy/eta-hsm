#include <gtest/gtest.h>

#include "../examples/cd_player/CDPlayer.hpp"

class CdPlayerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    eta_hsm::examples::cd_player::Player player_;
};

TEST_F(CdPlayerTest, InitialConditionsTest)
{
    EXPECT_EQ(player_.identify(), eta_hsm::examples::cd_player::CdState::eStopped);
}
