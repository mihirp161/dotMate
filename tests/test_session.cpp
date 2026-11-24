#include "dotmate/session.hpp"

#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

#include "dotmate/privacy.hpp"
#include "micro_test.hpp"
#include "test_support.hpp"

using namespace dotmate;

namespace {

class RecordingIo : public SessionIo {
public:
    void write(const std::string& text) override { output += text; }
    std::optional<std::string> save_file(const std::string& name,
                                         const std::string& contents) override {
        if (fail_saves) return std::nullopt;
        files.emplace_back(name, contents);
        return std::string(" in tests");
    }

    std::string output;
    std::vector<std::pair<std::string, std::string>> files;
    bool fail_saves = false;
};

bool has(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

// One conversation wired to a synthetic knowledge base, an in-memory
// store, and a recorder -- no terminal, no disk, no browser.
struct Harness {
    KnowledgeBase kb = test_support::load_synthetic_kb();
    Matcher matcher{kb};
    MemoryReturningUserStore store;
    RecordingIo io;
    Session session{matcher, store, io};

    void type(std::initializer_list<const char*> lines) {
        for (const char* line : lines) session.on_line(line);
    }
};

}  // namespace

void run_session_tests() {
    // Greeting asks for a name.
    {
        Harness h;
        h.session.start();
        CHECK(has(h.io.output, "What should I call you?"));
        CHECK(!h.session.finished());
    }

    // Skipping the name goes straight to the main prompt and stores nothing.
    {
        Harness h;
        h.session.start();
        h.type({""});
        CHECK(has(h.io.output, "No problem"));
        CHECK(has(h.io.output, "Ask a question"));
        CHECK(h.store.size() == 0);
    }

    // New name + "y": only a hash -> name pair is stored, never the name as a key.
    {
        Harness h;
        h.session.start();
        h.type({"Alex", "y"});
        CHECK(has(h.io.output, "Nice to meet you, Alex!"));
        CHECK(has(h.io.output, "Got it"));
        const auto stored = h.store.lookup(privacy::pseudonymize("Alex"));
        CHECK(stored.has_value() && *stored == "Alex");
        CHECK(h.store.size() == 1);
        CHECK(!has(h.store.to_json(), "\"Alex\":"));
    }

    // New name + "n": nothing is stored.
    {
        Harness h;
        h.session.start();
        h.type({"Sam", "n"});
        CHECK(has(h.io.output, "stay between us"));
        CHECK(h.store.size() == 0);
    }

    // A remembered name is recognised regardless of case or punctuation.
    {
        Harness h;
        h.store.remember(privacy::pseudonymize("Alex"), "Alex");
        h.session.start();
        h.type({"  alex!  "});
        CHECK(has(h.io.output, "Welcome back, Alex!"));
    }

    // Questions: confident answer in green with its source, a clear miss,
    // and a blank line just re-prompts.
    {
        Harness h;
        h.session.start();
        h.type({"", "What is a period?"});
        CHECK(has(h.io.output, "\033[32mAnswer about periods."));
        CHECK(has(h.io.output, "(source: Test Source)"));
        h.io.output.clear();
        h.type({"purple elephants dancing on the moon"});
        CHECK(has(h.io.output, "I don't have a good answer"));
        h.io.output.clear();
        h.type({""});
        CHECK(h.io.output == "> ");
    }

    // Privacy: what someone asks never reaches the store.
    {
        Harness h;
        h.session.start();
        h.type({"Alex", "y", "What is a period?", "define period"});
        CHECK(h.store.size() == 1);
        CHECK(!has(h.store.to_json(), "period"));
    }

    // Calendar, happy path: a bad line is rejected, the estimate is right,
    // and the export hands the CSV to the I/O layer.
    {
        Harness h;
        h.session.start();
        h.type({"", "calendar", "2026-01-01", "not a date", "2026-01-29", "2026-02-26", "", "y"});
        CHECK(has(h.io.output, "Couldn't read that as YYYY-MM-DD"));
        CHECK(has(h.io.output, "about 28 days"));
        CHECK(has(h.io.output, "Estimated next period: 2026-03-26"));
        CHECK(!has(h.io.output, "outside the typical"));
        CHECK(h.io.files.size() == 1);
        if (h.io.files.size() == 1) {
            CHECK(h.io.files[0].first == "period_calendar.csv");
            CHECK(has(h.io.files[0].second, "26,,,X"));
        }
        CHECK(has(h.io.output, "Saved to period_calendar.csv in tests."));
        CHECK(!h.session.finished());
    }

    // Declining the export saves nothing; a 40-day gap gets the doctor note.
    {
        Harness h;
        h.session.start();
        h.type({"", "calendar", "2026-01-01", "2026-02-10", "", "n"});
        CHECK(has(h.io.output, "outside the typical 21-35 day range"));
        CHECK(h.io.files.empty());
    }

    // Too few dates: say so, and the conversation carries on.
    {
        Harness h;
        h.session.start();
        h.type({"", "calendar", "2026-01-01", ""});
        CHECK(has(h.io.output, "Need at least two dates"));
        h.io.output.clear();
        h.type({"What is ovulation?"});
        CHECK(has(h.io.output, "Answer about ovulation."));
    }

    // A failed save is reported rather than ignored.
    {
        Harness h;
        h.io.fail_saves = true;
        h.session.start();
        h.type({"", "calendar", "2026-01-01", "2026-01-29", "", "y"});
        CHECK(has(h.io.output, "Couldn't write that file"));
    }

    // Regression for the original bug: using the calendar twice in one
    // run must not let the first estimate leak into the second.
    {
        Harness h;
        h.session.start();
        h.type({"", "calendar", "2026-01-01", "2026-01-29", "", "n"});
        CHECK(has(h.io.output, "Estimated next period: 2026-02-26"));
        h.io.output.clear();
        h.type({"calendar", "2026-05-01", "2026-06-01", "", "n"});
        CHECK(has(h.io.output, "about 31 days"));
        CHECK(has(h.io.output, "Estimated next period: 2026-07-02"));
    }

    // Natural phrasing still reaches the calendar.
    {
        Harness h;
        h.session.start();
        h.type({"", "I want to predict my next period"});
        CHECK(has(h.io.output, "Let's estimate your next period"));
    }

    // Exit, with a name and without; later input is ignored; start() resets.
    {
        Harness h;
        h.session.start();
        h.type({"Alex", "n", "bye"});
        CHECK(h.session.finished());
        CHECK(has(h.io.output, "Take care, Alex!"));
        const auto size_after_exit = h.io.output.size();
        h.type({"hello"});
        CHECK(h.io.output.size() == size_after_exit);
        h.session.start();
        CHECK(!h.session.finished());
    }
    {
        Harness h;
        h.session.start();
        h.type({"", "exit"});
        CHECK(h.session.finished());
        CHECK(has(h.io.output, "Take care!\n"));
    }

    // Stray carriage returns (Windows line endings) don't count as input.
    {
        Harness h;
        h.session.start();
        h.type({"\r"});
        CHECK(has(h.io.output, "No problem"));
    }
}
