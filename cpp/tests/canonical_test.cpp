#include "Canonical.hpp"

#include <gtest/gtest.h>

class CanonicalTest : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
    eta::hsm::canonical::Canonical canonical_hsm_;
};


TEST_F(CanonicalTest, InitialConditionsTest)
{
    EXPECT_EQ(canonical_hsm_.identify(), eta::hsm::canonical::CanonicalState::eS11);
}
