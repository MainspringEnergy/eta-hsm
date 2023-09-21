// eta/time_tracker/TimeTracker.hpp

#pragma once
#include <array>
#include <chrono>

#include "wise_enum/wise_enum.h"

namespace eta_hsm {
namespace utils {

/// A utility to keep track of how long we have been in each (potentially nested) state.
template <typename StateEnum, typename LocalClock>
class TimeTracker {
public:
    TimeTracker(const LocalClock& clock) : mClock{clock} {}

    /// We will return durations with whatever precision the provided clock uses
    using Duration = std::chrono::duration<typename LocalClock::rep, typename LocalClock::period>;

    /// Record the time upon entry into each state
    void enter(StateEnum state)
    {
        mEntryTimes.at(static_cast<Underlying>(state)) = mClock.now();
        mInState.at(static_cast<Underlying>(state)) = true;
    }

    /// Record that we have exited each state so that we can avoid returning incorrect durations
    /// for states that we are no longer in
    void exit(StateEnum state) { mInState.at(static_cast<Underlying>(state)) = false; }

    /// Query how long we have been in any particular state
    /// If we are not currently in that state, return 0
    Duration timeInState(StateEnum state) const
    {
        if (!mInState.at(static_cast<Underlying>(state)))
        {
            return std::chrono::milliseconds{0};
        }
        return mClock.now() - mEntryTimes.at(static_cast<Underlying>(state));
    }

protected:
private:
    const LocalClock& mClock;

    /// I expected to be able to use enum.value() from wise_enum, but that didn't work for some reason
    /// so I will static_cast to the underlying integer type manually.
    using Underlying = typename std::underlying_type<StateEnum>::type;

    /// A list of entry times for each state
    std::array<std::chrono::time_point<LocalClock>, wise_enum::size<StateEnum>> mEntryTimes{};

    /// A simple boolean record of whether we are currently in a particular state.
    /// We could probably encode this information with some sort of intentionally invalid mEntryTime,
    /// but that seemed more difficult to understand and potentially more error prone.
    std::array<bool, wise_enum::size<StateEnum>> mInState{false};
};

}  // namespace utils
}  // namespace eta_hsm
