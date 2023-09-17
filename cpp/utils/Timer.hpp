// eta/hsm/Timer.hpp
#pragma once

#include "EventBucket.hpp"
#include "wise_enum/wise_enum.h"
#include <chrono>
#include <set>

namespace eta_hsm {
namespace utils {

/// Provide a default unique enum type so that users only have to provide their own if they want
/// to utilize the additional functionality of being able to schedule multiple timers from the same
/// state with the same utils.
enum class DefaultUniqueEnum
{
    eNone
};

/// Declaring this struct here as an example of what Timer expects and as a convenience
template <typename Clock_, typename Event_, typename GroupEnum_, typename UniqueEnum_=DefaultUniqueEnum>
struct TimerTraits
{
    using Clock = Clock_;
    using Event = Event_;
    using GroupEnum = GroupEnum_;
    using UniqueEnum = UniqueEnum_;
};


/// This timer module (for use in eta::hsm) intentionally does NOT interact with any clocks itself.
/// In order to make this functionality portable across state machines executing on widely different
/// processor architectures, any sense of time will be passed in explicitly.
// TODO: Template on time_point instead of Clock
template <typename Traits_>
class Timer
{
public:
    using Clock = typename Traits_::Clock;
    using Event = typename Traits_::Event;
    using GroupEnum = typename Traits_::GroupEnum;
    using UniqueEnum = typename Traits_::UniqueEnum;

    Timer()
        : mEvent{Event::eNone}
        , mGroupId{GroupEnum::eNone}
        , mExpiration{}
        , mUniqueId{UniqueEnum::eNone}
        , mArmed{false}
    {}

    Timer(Event evt, GroupEnum groupId, std::chrono::time_point<Clock> expiration, UniqueEnum uniqueId=UniqueEnum::eNone)
        : mEvent{evt}
        , mGroupId{groupId}
        , mExpiration{expiration}
        , mUniqueId{uniqueId}
        , mArmed{true}
    {}

    /// Public accessor for utils that will be fired when timer expires
    Event event() const { return mEvent; }
    GroupEnum groupId() const { return mGroupId; }
    UniqueEnum uniqueId() const { return mUniqueId; }

    /// Check whether or not this particular timer has expired
    bool expired(const std::chrono::time_point<Clock>& now) const
    {
        if(now > mExpiration)
        {
            return true;
        }
        return false;
    }

    /// Reset a timer to a new expiration time
    void reset(Event evt, GroupEnum groupId, std::chrono::time_point<Clock> expiration, UniqueEnum uniqueId=UniqueEnum::eNone)
    {
        mEvent = evt;
        mGroupId = groupId;
        mExpiration = expiration;
        mUniqueId = uniqueId;
        mArmed = true;
    }

    bool armed() const { return mArmed; }
    void disarm() { mArmed = false; }

    // TODO: Is this enough to allow multiset to sort?
    bool operator<( const Timer& rhs ) const
    {
        return mExpiration < rhs.mExpiration;
    }
    bool operator>( const Timer& rhs ) const
    {
        return mExpiration > rhs.mExpiration;
    }

protected:

private:
    Event mEvent;
    GroupEnum mGroupId;
    std::chrono::time_point<Clock> mExpiration {};
    UniqueEnum mUniqueId;
    bool mArmed;
};

/// The controller (which is an hsm::StateMachine) will have to hold potentially several timers running simultaneously,
/// and I was unsure which STL container (if any) would end up working best for indexing by state and utils,
/// much less one that would be appropriate for the RPU from a memory allocation standpoint.
/// Hence, I'm going to declare our own custom container, and we can
/// worry about optimizing the implementation behind this interface if/when needed.

/// Assumption: kMaxTimers will be a small enough number that efficiency doesn't matter much

/// Guarantee: If multiple timers have expired since last check, the earliest one will return first
///            so that we maintain the order of events.

/// Note: Event/State pairs must be unique.  Setting an Event/State pair that already exists will clear the old timer.

template<typename Traits_>
class TimerBank
{
public:
    using Clock = typename Traits_::Clock;
    using Event = typename Traits_::Event;
    using GroupEnum = typename Traits_::GroupEnum;
    using UniqueEnum = typename Traits_::UniqueEnum;

    TimerBank() = default;

    bool empty() { return mTimers.empty(); }

    /// create (set) timer to expire at a specified time_point in the future
    void addTimer(Event event, GroupEnum groupId, std::chrono::time_point<Clock> expiration, UniqueEnum uniqueId=UniqueEnum::eNone)
    {
        // TODO: Question: Do we want to allow exact duplicates?
        clearTimer(event, groupId, uniqueId);
        // multiset will take care of sorting by expiration
        mTimers.emplace(Timer<Traits_>(event, groupId, expiration, uniqueId));
    }

    /// create (set) timer to expire a specified duration from now
    void addTimer(Event event, GroupEnum groupId, std::chrono::milliseconds duration_ms, UniqueEnum uniqueId=UniqueEnum::eNone)
    {
        // TODO: Question: Do we want to allow exact duplicates?
        clearTimer(event, groupId, uniqueId);
        // multiset will take care of sorting by expiration
        mTimers.emplace(Timer<Traits_>(event, groupId, mLastTimeValue + duration_ms, uniqueId));
    }

    /// clear (remove) a specific timer
    void clearTimer(Event event, GroupEnum groupId, UniqueEnum uniqueId=UniqueEnum::eNone)
    {
        // brute force search and then remove
        for (auto it = mTimers.begin(); it != mTimers.end(); )
        {
            if(it->event() == event && it->groupId() == groupId && it->uniqueId() == uniqueId)
            {
                it = mTimers.erase(it);
                // keep searching since there might be multiples
            }
            else
            {
                // We have to increment the iterator ourselves to avoid the for loop trying to
                // increment the iterator we just removed above.
                ++it;
            }
        }
    }

    /// clear (remove) all timers associated with a particular state
    void clearAllTimersInGroup(GroupEnum groupId)
    {
        // brute force search and then remove
        for (auto it = mTimers.begin(); it != mTimers.end(); )
        {
            if(it->groupId() == groupId)
            {
                it = mTimers.erase(it);
                // keep searching since there might be multiples
            }
            else
            {
                // We have to increment the iterator ourselves to avoid the for loop trying to
                // increment the iterator we just removed above.
                ++it;
            }
        }
    }

    /// check timers and add all fired events to the utils bucket
    void checkTimers(std::chrono::time_point<Clock> now, event::EventBucket<Event>& eventBucket)
    {
        // To avoid interacting with clocks, we will hold the last time value that we have seen.
        mLastTimeValue = now;

        // For now, let's make sure we catch ALL expired timers
        // Note: they will come out in age order, but what we choose to then do with the events is independent
        Event event = checkForSingleFiredEvent(now);
        while(event != Event::eNone)
        {
            eventBucket.addEvent(event);
            event = checkForSingleFiredEvent(now);
        }
    }

    /// This is effectively part of the "old" interface to TimerBank.
    /// It could (should) be private but is still here for the time being because it is used
    /// by the existing unit tests.
    Event checkForSingleFiredEvent(const std::chrono::time_point<Clock>& now)
    {
        // To avoid interacting with clocks, we will hold the last time value that we have seen.
        mLastTimeValue = now;

        if(empty())
        {
            return Event::eNone;
        }
        else if(mTimers.begin()->expired(now))
        {
            // Note: we only have to check the first timer since they are already sorted by expiration
            Event firedEvent = mTimers.begin()->event();
            mTimers.erase(mTimers.begin());
            return firedEvent;
        }
        else
        {
            return Event::eNone;
        }
    }

protected:

private:
    std::multiset<Timer<Traits_>> mTimers {};
    /// Keep the "latest" timer value around for future use
    std::chrono::time_point<Clock> mLastTimeValue {};
};

/// A StaticTimerBank provides a subset of the functionality of TimerBank but does so without dynamically
/// allocated memory (after contruction).  It is intended for use within the simpler HSMs running on the RPUs
/// with stricter real-time requirements.
///
/// Specifically, StaticTimerBank only allows one timer to be declared per state.  You can still have timers
/// from multiple nested states running simultaneously, but there is no guarantee about the order that the
/// events will fire if multiple timers have expired since the last time they were checked.
///
/// StaticTimerBank and TimerBank share similar interfaces (and could thus both derive from an abstract base class)
/// but sinc they share effectively no common implementation, they are not currently strictly related.
template<typename Traits_>
class StaticTimerBank
{
public:
    using Clock = typename Traits_::Clock;
    using Event = typename Traits_::Event;
    using GroupEnum = typename Traits_::GroupEnum;

//    ETA_EVENT(StaticTimerBankOverwriteActiveTimer, (GroupEnum, groupId));
//    using SharedEventEmitter = std::shared_ptr<
//        eta::event_emitter::EventEmitter<
//            eta::event_emitter::EmittableEvents<StaticTimerBankOverwriteActiveTimer>,
//            eta::ctrl::DataClock
//        >
//    >;

    /// Default initializes all elements of mTimers array to "disarmed"
    StaticTimerBank()
    {}

    /// set timer to expire at a specified time_point in the future
    void addTimer(Event event, GroupEnum groupId, std::chrono::time_point<Clock> expiration)
    {
        // warn user on overwrite
//        if (mEventEmitter && mTimers.at(static_cast<size_t>(groupId)).armed())
//        {   // TODO: include events and state as enums in log message
//            mEventEmitter->emit(StaticTimerBankOverwriteActiveTimer{.groupId=groupId});
//        }
        mTimers.at(static_cast<size_t>(groupId)).reset(event, groupId, expiration);
    }

    /// set timer to expire a specified duration from now
    void addTimer(Event event, GroupEnum groupId, std::chrono::milliseconds duration_ms)
    {
        addTimer(event, groupId, mLastTimeValue + duration_ms);
    }

    /// clear a specific timer
    void clearTimer(GroupEnum groupId)
    {
        mTimers.at(static_cast<size_t>(groupId)).disarm();
    }

    /// Since StaticTimerBank only allows one timer per group, clearing all timers in the
    /// group is the same thing as clearing **the** timer for the group in the function above.
    /// This function exists just to maintain a similar interface to TimerBank
    void clearAllTimersInGroup(GroupEnum groupId)
    {
         clearTimer(groupId);
    }

    /// check timers and add all fired events to the utils bucket
    void checkTimers(std::chrono::time_point<Clock> now, event::EventBucket<Event>& eventBucket)
    {
        // To avoid interacting with clocks, we will hold the last time value that we have seen.
        mLastTimeValue = now;

        // For now, let's make sure we catch ALL expired timers
        // Note: they will come out in GroupEnum order, which is differnt from TimerBank
        for (auto& timer : mTimers)
        {
            if (timer.armed() && timer.expired(now))
            {
                eventBucket.addEvent(timer.event());
                timer.disarm();
            }
        }
    }

protected:

private:
    // Using a static array with one timer allowed per state
    std::array<Timer<Traits_>, wise_enum::size<GroupEnum>> mTimers {};
    /// Keep the "latest" timer value around for future use
    std::chrono::time_point<Clock> mLastTimeValue {};
};


} // namespace utils
} // namespace eta_hsm
