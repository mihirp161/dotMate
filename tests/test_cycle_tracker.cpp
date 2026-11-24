#include "dotmate/cycle_tracker.hpp"

#include <cstdio>
#include <fstream>
#include <set>
#include <sstream>

#include "micro_test.hpp"

using namespace dotmate;
using namespace std::chrono;

namespace {
year_month_day ymd(int y, unsigned m, unsigned d) {
    return year_month_day{year{y}, month{m}, day{d}};
}

// Minimal CSV line reader (handles "quoted, cells" and doubled quotes) so the
// tests can check what a spreadsheet would show rather than raw text.
std::vector<std::string> parse_csv_line(const std::string& line) {
    std::vector<std::string> cells;
    std::string current;
    bool quoted = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (quoted) {
            if (c == '"' && i + 1 < line.size() && line[i + 1] == '"') { current += '"'; ++i; }
            else if (c == '"') quoted = false;
            else current += c;
        } else if (c == '"') {
            quoted = true;
        } else if (c == ',') {
            cells.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    cells.push_back(current);
    return cells;
}

std::string joined(const std::vector<std::string>& cells) {
    std::string out;
    for (const auto& c : cells) out += (out.empty() ? "" : " ") + c;
    return out;
}

// Non-empty lines after the 32-row grid (the header row plus days 1-31).
std::vector<std::string> lines_after_grid(const std::string& csv) {
    std::vector<std::string> lines;
    std::istringstream in(csv);
    std::string line;
    int n = 0;
    while (std::getline(in, line)) {
        if (++n > 32 && !line.empty()) lines.push_back(line);
    }
    return lines;
}
}  // namespace

void run_cycle_tracker_tests() {
    // Fewer than 2 records -> no result, not a crash or a garbage average.
    {
        const auto avg = CycleTracker::average_cycle_length_days({});
        CHECK(!avg.has_value());
        const auto avg_one = CycleTracker::average_cycle_length_days({{ymd(2026, 1, 1)}});
        CHECK(!avg_one.has_value());
        const auto pred = CycleTracker::predict({{ymd(2026, 1, 1)}});
        CHECK(!pred.has_value());
    }

    // Three dates exactly 28 days apart -- Jan 1 -> Jan 29 -> Feb 26,
    // 2026 (not a leap year, so February has 28 days) -- average is
    // exactly 28, regardless of the order they're supplied in
    // (predict() sorts). Deliberately crosses a month boundary with a
    // hand-verified gap, since that's exactly the kind of arithmetic
    // <chrono> exists to get right instead of a hand-rolled offset.
    {
        std::vector<CycleRecord> history = {
            {ymd(2026, 2, 26)}, {ymd(2026, 1, 1)}, {ymd(2026, 1, 29)}};
        const auto avg = CycleTracker::average_cycle_length_days(history);
        CHECK(avg.has_value());
        if (avg) CHECK(*avg == 28.0);
    }

    // Prediction is last date + average gap -- and there's no hidden
    // offset folded in anywhere.
    {
        std::vector<CycleRecord> history = {{ymd(2026, 1, 1)}, {ymd(2026, 1, 29)}};
        const auto pred = CycleTracker::predict(history);
        CHECK(pred.has_value());
        if (pred) {
            CHECK(pred->average_cycle_length_days == 28.0);
            CHECK(pred->next_period_estimate == ymd(2026, 2, 26));  // Jan 29 + 28 days
            CHECK(!pred->length_is_unusual);
        }
    }

    // Statelessness: two independent predict() calls in a row must not
    // influence each other. This is a direct regression test for the
    // original bug, where invoking the calendar feature twice in one
    // run silently accumulated state from the first call into the
    // second.
    {
        std::vector<CycleRecord> first = {{ymd(2026, 1, 1)}, {ymd(2026, 1, 29)}};   // 28-day gap
        std::vector<CycleRecord> second = {{ymd(2026, 5, 1)}, {ymd(2026, 6, 1)}};   // 31-day gap

        const auto pred1 = CycleTracker::predict(first);
        const auto pred2 = CycleTracker::predict(second);
        const auto pred1_again = CycleTracker::predict(first);

        CHECK(pred1.has_value() && pred2.has_value() && pred1_again.has_value());
        if (pred1 && pred2 && pred1_again) {
            CHECK(pred1->average_cycle_length_days == 28.0);
            CHECK(pred2->average_cycle_length_days == 31.0);
            // Calling predict() with `first` again gives the exact same
            // answer as the first time, uncontaminated by the call to
            // predict(second) in between.
            CHECK(pred1_again->average_cycle_length_days == pred1->average_cycle_length_days);
            CHECK(pred1_again->next_period_estimate == pred1->next_period_estimate);
        }
    }

    // Unusual-length flagging at and around the [21, 35] boundary.
    CHECK(!CycleTracker::is_unusual_cycle_length(21));
    CHECK(!CycleTracker::is_unusual_cycle_length(28));
    CHECK(!CycleTracker::is_unusual_cycle_length(35));
    CHECK(CycleTracker::is_unusual_cycle_length(20));
    CHECK(CycleTracker::is_unusual_cycle_length(36));
    CHECK(CycleTracker::is_unusual_cycle_length(10));

    // CSV export produces a file.
    {
        std::vector<CycleRecord> history = {{ymd(2026, 1, 1)}, {ymd(2026, 1, 29)}};
        const auto pred = CycleTracker::predict(history);
        CHECK(pred.has_value());
        if (pred) {
            const std::string path = "test_calendar_export.csv";
            const bool ok = CycleTracker::export_calendar_csv(*pred, path);
            CHECK(ok);
            std::ifstream check(path);
            CHECK(check.good());
            check.close();
            std::remove(path.c_str());
        }
    }

    // The export ends with the quotes header and exactly ONE quote, like the
    // original -- not the whole list -- split at word boundaries across cells.
    {
        const auto pred = CycleTracker::predict({{ymd(2026, 1, 1)}, {ymd(2026, 1, 29)}});
        CHECK(pred.has_value());
        if (pred) {
            CHECK(CycleTracker::quote_count() == 7);
            const std::vector<std::string> expected = {
                "Last time, one man said to a woman that she can't fly a plane, then she flew one around the world.",
                "Men are without a vagina, therefore there's a $20B market for the Sanitary Pads.",
                "If you want to compare genders then let me tell you: Women are not only competing, they're beating you!",
                "So you were told to shut up. To close your eyes & to pop your eardrums....my question is: Will you?",
                "My body can be put on chains. But my soul will never be able to comply.",
                "God gave us hands to love and to work and most importantly- to slap when the need arises.",
                "Duty for my country is first, but duty for you comes later.",
            };
            std::set<std::string> valid_lines;
            for (std::size_t i = 0; i < expected.size(); ++i) {
                const auto lines = lines_after_grid(CycleTracker::render_calendar_csv(*pred, i));
                CHECK(lines.size() == 2);  // header + one quote
                if (lines.size() != 2) continue;
                CHECK(joined(parse_csv_line(lines[0])) == "Today's quotes:");
                const auto cells = parse_csv_line(lines[1]);
                CHECK(joined(cells) == expected[i]);  // nothing lost, nothing split mid-word
                for (const auto& cell : cells) CHECK(!cell.empty() && cell.size() <= 14);
                valid_lines.insert(lines[1]);
            }
            CHECK(valid_lines.size() == 7);
            // An index past the end wraps around.
            CHECK(CycleTracker::render_calendar_csv(*pred, 7) == CycleTracker::render_calendar_csv(*pred, 0));
            // Unspecified: always one of the seven, and it does vary.
            std::set<std::string> seen;
            for (int i = 0; i < 200; ++i) {
                const auto lines = lines_after_grid(CycleTracker::render_calendar_csv(*pred));
                CHECK(lines.size() == 2);
                if (lines.size() == 2) {
                    CHECK(valid_lines.count(lines[1]) == 1);
                    seen.insert(lines[1]);
                }
            }
            CHECK(seen.size() >= 2);
        }
    }
}
