// eta/hsm/HsmMacros.hpp

#pragma once

#include "HsmMacrosImpl.hpp"

namespace eta {
namespace hsm {

// Useful macros for declares states
#define ETA_HSM_DECLARE_TOP_STATE(controller, name, state_enum) \
    ETA_HSM_DECLARE_TOP_STATE_IMPL(controller, name, state_enum)
#define ETA_HSM_DECLARE_COMP_STATE(controller, name, parent, state_enum) \
    ETA_HSM_DECLARE_COMP_STATE_IMPL(controller, name, parent, state_enum)
#define ETA_HSM_DECLARE_LEAF_STATE(controller, name, parent, state_enum) \
    ETA_HSM_DECLARE_LEAF_STATE_IMPL(controller, name, parent, state_enum)

// shorthand macros for when the controller and state enum share the same prefix
#define ETA_HSM_TOP_STATE(controller, name) ETA_HSM_DECLARE_TOP_STATE(controller, name, controller##StateEnum)
#define ETA_HSM_COMP_STATE(controller, name, parent) \
    ETA_HSM_DECLARE_COMP_STATE(controller, name, parent, controller##StateEnum)
#define ETA_HSM_LEAF_STATE(controller, name, parent) \
    ETA_HSM_DECLARE_LEAF_STATE(controller, name, parent, controller##StateEnum)

namespace testing {

template <typename Controller>
struct StateAttributes {
    bool isLeaf;
    std::function<void(Controller&)> setState;
};

}  // namespace testing

}  // namespace hsm
}  // namespace eta
