#include "../examples/canonical/Canonical.hpp"

#include <gtest/gtest.h>

class CanonicalTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    eta_hsm::examples::canonical::Canonical canonical_hsm_;
};


TEST_F(CanonicalTest, InitialConditionsTest)
{
    EXPECT_EQ(canonical_hsm_.identify(), eta_hsm::examples::canonical::CanonicalState::eS11);
}
