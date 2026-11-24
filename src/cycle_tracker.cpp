#include "dotmate/cycle_tracker.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <numeric>
#include <random>
#include <sstream>

namespace dotmate {

namespace {

using std::chrono::sys_days;
using std::chrono::year_month_day;

// The original quotes, kept verbatim -- these are the author's own
// family "wisdom" quotes from the original project, not something to
// rewrite. (The original stored each one pre-split into comma-separated
// fragments so it would land across CSV cells; that splitting is a
// presentation detail, done in wrap_for_csv() below, not part of the
// quote text itself.) Like the original, an export gets the header line
// and ONE quote picked at random -- not all of them.
const std::string kQuoteHeader = "Today's quotes:";
const std::array<std::string, 7> kQuotes = {
    "Tell a woman she can't fly, and she'll map the whole sky just to prove you wrong.",
    "It takes a lot of nerve for people who never bleed to price-gouge the ones who do.",
    "Stop keeping score between genders—you're already behind",
    "They handed you a blindfold, earplugs, and a gag. Only you can decide to put them on.",
    "My body can be put in chains, but my soul will never comply.",
    "These hands were made for loving, working, and delivering emergency attitude adjustments.",
    "You're a chapter in my life. My country is the whole book.",
};

constexpr std::array<const char*, 12> kMonthNames = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December",
};

sys_days to_sys_days(const year_month_day& d) { return sys_days{d}; }

// One CSV cell: quoted (inner quotes doubled) only when it has to be.
std::string csv_cell(const std::string& text) {
    if (text.find_first_of(",\"\n") == std::string::npos) return text;
    std::string quoted = "\"";
    for (const char c : text) {
        if (c == '"') quoted += '"';
        quoted += c;
    }
    return quoted + "\"";
}

// Splits a quote at word boundaries into short chunks (about two words
// each) so it reads across a row of CSV cells, the way the original's
// pre-chunked strings did.
std::vector<std::string> wrap_for_csv(const std::string& quote, std::size_t max_len = 14) {
    std::vector<std::string> chunks;
    std::istringstream words(quote);
    std::string word, current;
    while (words >> word) {
        if (!current.empty() && current.size() + 1 + word.size() > max_len) {
            chunks.push_back(current);
            current.clear();
        }
        if (!current.empty()) current += ' ';
        current += word;
    }
    if (!current.empty()) chunks.push_back(current);
    return chunks;
}

std::string csv_row(const std::vector<std::string>& cells) {
    std::string row;
    for (std::size_t i = 0; i < cells.size(); ++i) {
        if (i) row += ',';
        row += csv_cell(cells[i]);
    }
    return row;
}

std::size_t random_quote_index() {
    std::seed_seq seed{std::random_device{}(),
                       static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())};
    std::mt19937 rng(seed);
    return std::uniform_int_distribution<std::size_t>(0, kQuotes.size() - 1)(rng);
}

}  // namespace

std::optional<double> CycleTracker::average_cycle_length_days(std::vector<CycleRecord> history) {
    if (history.size() < 2) {
        return std::nullopt;
    }
    std::sort(history.begin(), history.end(), [](const CycleRecord& a, const CycleRecord& b) {
        return a.period_start < b.period_start;
    });

    long long total_days = 0;
    for (std::size_t i = 1; i < history.size(); ++i) {
        const auto gap = to_sys_days(history[i].period_start) - to_sys_days(history[i - 1].period_start);
        total_days += gap.count();
    }
    const auto gap_count = static_cast<double>(history.size() - 1);
    return static_cast<double>(total_days) / gap_count;
}

bool CycleTracker::is_unusual_cycle_length(int days) {
    return days < kMinNormalDays || days > kMaxNormalDays;
}

std::optional<CyclePrediction> CycleTracker::predict(std::vector<CycleRecord> history) {
    const auto avg = average_cycle_length_days(history);
    if (!avg) {
        return std::nullopt;
    }

    std::sort(history.begin(), history.end(), [](const CycleRecord& a, const CycleRecord& b) {
        return a.period_start < b.period_start;
    });
    const auto& most_recent = history.back().period_start;

    // No mystery offset here: the estimate is simply the last known
    // start date plus the average gap between starts, rounded to the
    // nearest whole day. If there was a real reason for the original's
    // unexplained +/-2-day fudge factor, it belongs here, named and
    // covered by a test -- not carried forward silently.
    const auto rounded_days = static_cast<int>(std::lround(*avg));
    const sys_days next = to_sys_days(most_recent) + std::chrono::days{rounded_days};

    CyclePrediction result;
    result.next_period_estimate = year_month_day{next};
    result.average_cycle_length_days = *avg;
    result.length_is_unusual = is_unusual_cycle_length(rounded_days);
    return result;
}

std::string CycleTracker::render_calendar_csv(const CyclePrediction& prediction,
                                              std::optional<std::size_t> quote_index) {
    std::ostringstream out;

    const auto& d = prediction.next_period_estimate;
    const unsigned target_month = static_cast<unsigned>(d.month());
    const unsigned target_day = static_cast<unsigned>(d.day());

    // Header row: blank corner cell, then one column per month.
    out << "day";
    for (const auto* name : kMonthNames) out << "," << name;
    out << "\n";

    for (int day = 1; day <= 31; ++day) {
        out << day;
        for (unsigned month = 1; month <= 12; ++month) {
            const bool is_target = (month == target_month && static_cast<unsigned>(day) == target_day);
            out << "," << (is_target ? "X" : "");
        }
        out << "\n";
    }

    const std::size_t pick = (quote_index ? *quote_index : random_quote_index()) % kQuotes.size();
    out << "\n" << csv_row(wrap_for_csv(kQuoteHeader)) << "\n"
        << csv_row(wrap_for_csv(kQuotes[pick])) << "\n";
    return out.str();
}

std::size_t CycleTracker::quote_count() { return kQuotes.size(); }

bool CycleTracker::export_calendar_csv(const CyclePrediction& prediction, const std::string& file_path) {
    std::ofstream out(file_path);
    if (!out) {
        return false;
    }
    out << render_calendar_csv(prediction);
    return static_cast<bool>(out);
}

}  // namespace dotmate
