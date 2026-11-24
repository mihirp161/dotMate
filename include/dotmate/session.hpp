#pragma once

#include <optional>
#include <string>
#include <vector>

#include "dotmate/cycle_tracker.hpp"
#include "dotmate/matcher.hpp"
#include "dotmate/returning_user_store.hpp"

namespace dotmate {

// Everything a Session needs from the outside world. The terminal app,
// the unit tests, and the WebAssembly build each supply their own, so
// Session itself never touches iostreams, files, or the browser.
class SessionIo {
public:
    virtual ~SessionIo() = default;

    // Text for the person to read. May contain ANSI colour codes.
    virtual void write(const std::string& text) = 0;

    // Offers a generated file (the calendar CSV). Returns a short phrase
    // for where it went, appended to "Saved to <name>" (for example
    // " in the current folder"), or nullopt if it couldn't be saved.
    virtual std::optional<std::string> save_file(const std::string& name,
                                                 const std::string& contents) = 0;
};

// One conversation, driven one line at a time: start() prints the
// greeting, then each line the person types goes to on_line(), which
// prints the reply and the next prompt.
//
// This used to be a blocking loop around std::getline. A browser tab
// can't block waiting for input, so the loop was turned inside out
// into an explicit state machine -- which also makes the whole
// conversation unit-testable (tests/test_session.cpp).
class Session {
public:
    Session(const Matcher& matcher, ReturningUserStore& store, SessionIo& io);

    void start();
    void on_line(const std::string& line);
    bool finished() const { return state_ == State::Done; }

private:
    enum class State { AskName, AskRemember, Main, CalendarDates, CalendarExport, Done };

    void say(const std::string& text) { io_.write(text); }
    void begin_main();
    void handle_name(const std::string& line);
    void handle_remember(const std::string& line);
    void handle_main(const std::string& line);
    void answer_question(const std::string& line);
    void start_calendar();
    void handle_calendar_date(const std::string& line);
    void finish_calendar_dates();
    void handle_calendar_export(const std::string& line);

    const Matcher& matcher_;
    ReturningUserStore& store_;
    SessionIo& io_;

    State state_ = State::AskName;
    std::string display_name_;  // empty if the person skipped giving one
    std::string pending_hash_;
    std::vector<CycleRecord> history_;
    std::optional<CyclePrediction> prediction_;
};

}  // namespace dotmate
