// eta/hsm/HsmMacrosImpl.hpp

#pragma once

#include "../Hsm.hpp"
//#include "eta/once/once.hpp"

#include <functional>
#include <map>
#include <type_traits>

namespace eta {
namespace hsm {

template <class T>
struct isLeaf
{
    template <typename C>
    static constexpr decltype(std::declval<C>().instanceForTestingOnly(), bool()) test(int)
    {
        return true;
    }

    template <typename C>
    static constexpr bool test(...) { return false; }

    static constexpr bool value = test<T>(int());
};

#define ETA_HSM_DECLARE_TOP_STATE_IMPL(controller, name, state_enum)\
template<state_enum kState>\
using controller ## StateMachineTraits = hsm::StateTraits<controller, state_enum, kState>;\
\
template<state_enum state, typename parent>\
using CompState = hsm::CompState<controller ## StateMachineTraits<state>, parent>;\
\
template<state_enum state, typename parent>\
using LeafState = hsm::LeafState<controller ## StateMachineTraits<state>, parent>;\
\
using name = eta::hsm::TopState<controller ## StateMachineTraits<state_enum::e ## name>>;\
\
namespace testing {\
\
using controller ## StateAttributes = eta::hsm::testing::StateAttributes<controller>;\
\
template <typename State>\
auto setState(controller&)-> std::enable_if_t<!eta::hsm::isLeaf<State>::value>\
{}\
\
template <typename State>\
auto setState(controller& sm) -> std::enable_if_t<eta::hsm::isLeaf<State>::value>\
{ sm.directlySetStateForTestingOnly<State>(); }\
\
static auto stateTestAttributes ## controller = std::map<state_enum, controller ## StateAttributes>\
{\
    {state_enum::eNone, {false, [](controller&){}}},\
    {state_enum::e ## name, {eta::hsm::isLeaf<name>::value, setState<name>}}\
};\
\
} /* namespace testing */

#define HSM_ADD_STATE_TO_TEST_ATTRIBUTES(controller, name, state_enum)\
namespace testing {\
\
ETA_ONCE(stateTestAttributes ## controller.insert(\
    std::pair<state_enum, controller ## StateAttributes>(\
        state_enum::e ## name, {eta::hsm::isLeaf<name>::value, setState<name>}\
    )\
));\
\
} /* namespace testing */

#define ETA_HSM_DECLARE_COMP_STATE_IMPL(controller, name, parent, state_enum)\
using name = CompState<state_enum::e ## name, parent>;\
HSM_ADD_STATE_TO_TEST_ATTRIBUTES(controller, name, state_enum);

#define ETA_HSM_DECLARE_LEAF_STATE_IMPL(controller, name, parent, state_enum)\
using name = LeafState<state_enum::e ## name, parent>;\
HSM_ADD_STATE_TO_TEST_ATTRIBUTES(controller, name, state_enum);

} // namespace hsm
} // namespace eta
