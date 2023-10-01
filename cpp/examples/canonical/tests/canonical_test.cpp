#include "../Canonical.hpp"

#include <gtest/gtest.h>

namespace eta_hsm {
namespace examples {
namespace canonical {
namespace tests {

class CanonicalTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
    Canonical canonical_hsm_;
};

TEST_F(CanonicalTest, InitialConditionsTest) { EXPECT_EQ(canonical_hsm_.identify(), CanonicalState::eS11); }

}  // namespace tests
}  // namespace canonical
}  // namespace examples
}  // namespace eta_hsm