
#include "eta/hsm/testing/HsmTestRunner.hpp"
#include "eta/hsm/tests/UpdateControlHsm.hpp"

#include <vector>

namespace eta::hsm::update_control {
namespace testing {

using StateEnum = UpdateControlHsmStateEnum;
using EventEnum = UpdateControlHsmEventEnum;

using Transition = eta::hsm::testing::TransitionHsm<UpdateControlHsm>;
const std::vector<Transition> eventResponseTests
{
    {StateEnum::eOff, EventEnum::eOn, StateEnum::eOff},
    {StateEnum::eOff, EventEnum::eOn, StateEnum::eWarming,
        Transition::ControlUpdater
        {
            [](UpdateControlHsm& control, Input& input)
            {
                input.voltage = 4;
                control.update(input);
            }
        }
    },
    {StateEnum::eWarming, EventEnum::eOff, StateEnum::eOff},
    {StateEnum::eWarming, EventEnum::eFault, StateEnum::eFault},
    {StateEnum::eWarming, EventEnum::eWarm, StateEnum::eReady},
    {StateEnum::eReady, EventEnum::eOff, StateEnum::eOff},
    {StateEnum::eReady, EventEnum::eFault, StateEnum::eFault},
    {StateEnum::eFault, EventEnum::eClear, StateEnum::eOff},
};

FEATURE("Control State Machine Event Test", "[bus_control_events]")
{
    runHsmStateTests(eventResponseTests, stateTestAttributesUpdateControlHsm);
}

} // namespace testing
} // namespace eta::hsm::update_control