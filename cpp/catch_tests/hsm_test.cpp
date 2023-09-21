// eta/hsm/tests/test.cpp

#include "eta/hsm/Hsm.hpp"

#include "eta/hsm/tests/Canonical.hpp"

namespace eta {
namespace hsm {
namespace testing {
namespace {

FEATURE("Canonical HSM Example", "[hsm]")
{
    GIVEN("Canonical HSM Example in S11")
    {
        TestLog::instance() << "Given: Canonical HSM Example in S11" << std::endl;
        canonical::Canonical canonicalHsm;
        CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);

        SCENARIO("Ancestry of S11")
        {
            // S11 should be a "substate" of all of its ancestors (including itself)
            CHECK(canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS11));
            CHECK(canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS1));
            CHECK(canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS0));
            CHECK(canonicalHsm.isInSubstateOf(canonical::CanonicalState::eTop));

            // S11 shold NOT be a "substate" of any other state
            CHECK(!canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS211));
            CHECK(!canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS21));
            CHECK(!canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS2));
        }

        SCENARIO("Event A in state S11")
        {
            TestLog::instance() << "Scenario: Event A in state S11" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::A);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S11 exit_S1 enter_S1 init_S1 enter_S11 during_S11 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
        }

        SCENARIO("Event A_LOCAL in state S11")  // I'm not sure if this one actually makes sense or not
        {
            TestLog::instance() << "Scenario: Event A_LOCAL in state S11" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::A_LOCAL);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S11 init_S1 enter_S11 during_S11 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
        }

        SCENARIO("Event E in state S11")
        {
            TestLog::instance() << "Scenario: Event E in state S11" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::E);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() ==
                  "exit_S11 exit_S1 exit_S0 enter_S0 enter_S2 enter_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Event E_LOCAL in state S11")
        {
            TestLog::instance() << "Scenario: Event E_LOCAL in state S11" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::E_LOCAL);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S11 exit_S1 enter_S2 enter_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Event B in state S11")
        {
            TestLog::instance() << "Scenario: Event B in state S11" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::B);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S11 exit_S1 enter_S1 enter_S11 during_S11 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
        }

        SCENARIO("Event B_LOCAL in state S11")
        {
            TestLog::instance() << "Scenario: Event B_LOCAL in state S11" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::B_LOCAL);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S11 enter_S11 during_S11 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
        }

        SCENARIO("Event G in state S11")
        {
            TestLog::instance() << "Scenario: Event G in state S11" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::G);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S11 exit_S1 enter_S2 enter_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        // Local vs external semantics do not make a difference here.
        // Testing anyway to make sure they both give the same behavior.
        SCENARIO("Event G_LOCAL in state S11")
        {
            TestLog::instance() << "Scenario: Event G_LOCAL in state S11" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::G_LOCAL);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S11 exit_S1 enter_S2 enter_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Auto transition from S12 back to S11")
        {
            TestLog::instance() << "Auto transition from S12 back to S11" << std::endl;
            TestLog::instance().startCapture();
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
            canonicalHsm.dispatch(canonical::CanonicalEvent::Z);  // get us into S12
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS12);
            canonicalHsm.during();  // we should auto-transition back on first during
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
            canonicalHsm.during();
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S11 enter_S12 during_S12 exit_S12 enter_S11 during_S11 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
        }
    }

    GIVEN("Canonical HSM Example in S211")
    {
        TestLog::instance() << "Given: Canonical HSM Example in S211" << std::endl;
        canonical::Canonical canonicalHsm;
        // Event E should always take us to S211, so this is the "nicer" way to initialize state for testing
        // canonicalHsm.dispatch(canonical::CanonicalEvent::E);
        // If you really want to, you can now set the state directly.
        // NOTE: This bypasses entry and exit conditions, so please use with caution.
        canonicalHsm.directlySetStateForTestingOnly<canonical::S211>();
        CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);

        SCENARIO("Ancestry of S211")
        {
            // S211 should be a "substate" of all of its ancestors (including itself)
            CHECK(canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS211));
            CHECK(canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS21));
            CHECK(canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS2));
            CHECK(canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS0));
            CHECK(canonicalHsm.isInSubstateOf(canonical::CanonicalState::eTop));

            // S211 shold NOT be a "substate" of any other state
            CHECK(!canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS12));
            CHECK(!canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS11));
            CHECK(!canonicalHsm.isInSubstateOf(canonical::CanonicalState::eS1));
        }

        SCENARIO("Event E in state S211")
        {
            TestLog::instance() << "Scenario: Event E in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::E);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() ==
                  "exit_S211 exit_S21 exit_S2 exit_S0 enter_S0 enter_S2 enter_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Event E_LOCAL in state S211")
        {
            TestLog::instance() << "Scenario: Event E_LOCAL in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::E_LOCAL);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() ==
                  "exit_S211 exit_S21 exit_S2 enter_S2 enter_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Event A in state S211")
        {
            TestLog::instance() << "Scenario: Event A in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::A);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Event A_LOCAL in state S211")
        {
            TestLog::instance() << "Scenario: Event A_LOCAL in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::A_LOCAL);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Event H in state S211")
        {
            TestLog::instance() << "Scenario: Event H in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::H);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S211 exit_S21 enter_S21 init_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Event H_LOCAL in state S211")
        {
            TestLog::instance() << "Scenario: Event H_LOCAL in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::H_LOCAL);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S211 init_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Event G in state S211")
        {
            TestLog::instance() << "Scenario: Event G in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::G);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() ==
                  "exit_S211 exit_S21 exit_S2 exit_S0 enter_S0 init_S0 enter_S1 init_S1 enter_S11 during_S11 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
        }

        SCENARIO("Event G_LOCAL in state S211")
        {
            TestLog::instance() << "Scenario: Event G_LOCAL in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::G_LOCAL);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() ==
                  "exit_S211 exit_S21 exit_S2 init_S0 enter_S1 init_S1 enter_S11 during_S11 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS11);
        }

        SCENARIO("Event B in state S211")
        {
            TestLog::instance() << "Scenario: Event B in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::B);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S211 exit_S21 enter_S21 init_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }

        SCENARIO("Event B_LOCAL in state S211")
        {
            TestLog::instance() << "Scenario: Event B_LOCAL in state S211" << std::endl;
            TestLog::instance().startCapture();
            canonicalHsm.dispatch(canonical::CanonicalEvent::B_LOCAL);
            canonicalHsm.during();
            TestLog::instance().stopCapture();
            CHECK(TestLog::instance().getCaptured() == "exit_S211 init_S21 enter_S211 during_S211 ");
            CHECK(canonicalHsm.identify() == canonical::CanonicalState::eS211);
        }
    }
}

}  // namespace
}  // namespace testing
}  // namespace hsm
}  // namespace eta
