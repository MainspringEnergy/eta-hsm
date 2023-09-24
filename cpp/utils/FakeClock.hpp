#pragma once

#include <chrono>
#include <cstdint>

// Satisfies the TrivialClock concept (maybe? just barely?)
// https://en.cppreference.com/w/cpp/named_req/TrivialClock

namespace eta_hsm {
namespace utils {

class FakeClock {
public:
    using rep = uint64_t;
    using period = std::ratio<1l, 1000000000l>;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<FakeClock>;

    void advance(duration d) noexcept { now_us_ += d; }

    void reset_to_epoch() noexcept { now_us_ -= (now_us_ - time_point()); }

    time_point now() const noexcept { return now_us_; }

private:
    time_point now_us_;
};

}  // namespace utils
}  // namespace eta_hsm
