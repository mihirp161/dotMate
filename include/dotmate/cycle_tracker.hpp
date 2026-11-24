#pragma once

#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace dotmate {

// One remembered period start date. That's the only thing a caller
// needs to supply -- cycle lengths are *derived* from consecutive
// dates rather than tracked as a separately-entered, separately-summed
// number, which is what let the original's running total drift.
struct CycleRecord {
    std::chrono::year_month_day period_start;
};

struct CyclePrediction {
    std::chrono::year_month_day next_period_estimate;
    double average_cycle_length_days = 0.0;
    bool length_is_unusual = false;  // outside [kMinNormalDays, kMaxNormalDays]
};

// All of this is pure, stateless computation over whatever history is
// passed in -- nothing here is a member variable that could carry a
// stale value from a previous, unrelated calculation into the next
// one. That statelessness is the actual fix for the original bug
// (invoking the calendar feature twice in one run silently compounded
// the second prediction on top of the first).
class CycleTracker {
public:
    // Adults: ACOG's commonly-cited normal range. A first-year-after-
    // menarche range is wider in reality; this app intentionally keeps
    // one simple range and says so, rather than branching on age.
    static constexpr int kMinNormalDays = 21;
    static constexpr int kMaxNormalDays = 35;

    // Requires at least 2 records (need at least one gap to measure).
    // Records do not need to be pre-sorted.
    static std::optional<double> average_cycle_length_days(
        std::vector<CycleRecord> history);

    static bool is_unusual_cycle_length(int days);

    // Returns std::nullopt if fewer than 2 records were supplied.
    static std::optional<CyclePrediction> predict(std::vector<CycleRecord> history);

    // The day-vs-month CSV grid as text (same content export_calendar_csv
    // writes), followed by a "Today's quotes:" line and ONE quote. The quote
    // is picked at random unless `quote_index` is given (tests do that); an
    // index past the end wraps around.
    static std::string render_calendar_csv(const CyclePrediction& prediction,
                                           std::optional<std::size_t> quote_index = std::nullopt);
    static std::size_t quote_count();

    // Writes a simple day-vs-month CSV grid marking the predicted date,
    // with one of the rotating quotes appended. Returns false if the
    // file couldn't be opened for writing.
    static bool export_calendar_csv(const CyclePrediction& prediction,
                                     const std::string& file_path);
};

}  // namespace dotmate
