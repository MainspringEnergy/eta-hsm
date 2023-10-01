#include <gtest/gtest.h>

#include "../CDPlayer.hpp"

namespace eta_hsm {
namespace examples {
namespace cd_player {
namespace tests {

class CdPlayerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    Player player_;
};

TEST_F(CdPlayerTest, InitialConditionsTest)
{
    EXPECT_EQ(player_.identify(), CdState::eStopped);
}

}  // namespace tests
}  // namespace cd_player
}  // namespace examples
}  // namespace eta_hsm