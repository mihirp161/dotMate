#include "dotmate/session.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "dotmate/privacy.hpp"
#include "dotmate/text_utils.hpp"

namespace dotmate {

namespace {

namespace color {
constexpr auto reset = "\033[0m";
constexpr auto green = "\033[32m";
constexpr auto yellow = "\033[33m";
constexpr auto cyan = "\033[36m";
constexpr auto bold = "\033[1m";
}  // namespace color

std::string trim(const std::string& s) {
    const auto is_space = [](unsigned char c) { return std::isspace(c) != 0; };
    const auto first = std::find_if_not(s.begin(), s.end(), is_space);
    const auto last = std::find_if_not(s.rbegin(), s.rend(), is_space).base();
    return first < last ? std::string(first, last) : std::string();
}

bool contains(const std::vector<std::string>& tokens, const std::string& target) {
    return std::find(tokens.begin(), tokens.end(), target) != tokens.end();
}

bool wants_exit(const std::vector<std::string>& tokens) {
    return contains(tokens, "exit") || contains(tokens, "bye") || contains(tokens, "quit");
}

bool wants_calendar(const std::vector<std::string>& tokens) {
    if (contains(tokens, "calendar")) return true;
    const bool verb = contains(tokens, "predict") || contains(tokens, "track") ||
                      contains(tokens, "calculate") || contains(tokens, "estimate");
    const bool noun = contains(tokens, "period") || contains(tokens, "cycle");
    return verb && noun;
}

std::optional<std::chrono::year_month_day> parse_iso_date(const std::string& s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') return std::nullopt;
    try {
        const int y = std::stoi(s.substr(0, 4));
        const unsigned mo = static_cast<unsigned>(std::stoi(s.substr(5, 2)));
        const unsigned da = static_cast<unsigned>(std::stoi(s.substr(8, 2)));
        const std::chrono::year_month_day ymd{std::chrono::year{y}, std::chrono::month{mo},
                                              std::chrono::day{da}};
        if (!ymd.ok()) return std::nullopt;
        return ymd;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::string format_date(const std::chrono::year_month_day& d) {
    std::ostringstream out;
    out << static_cast<int>(d.year()) << '-' << std::setfill('0') << std::setw(2)
        << static_cast<unsigned>(d.month()) << '-' << std::setw(2)
        << static_cast<unsigned>(d.day());
    return out.str();
}

}  // namespace

Session::Session(const Matcher& matcher, ReturningUserStore& store, SessionIo& io)
    : matcher_(matcher), store_(store), io_(io) {}

void Session::start() {
    state_ = State::AskName;
    display_name_.clear();
    pending_hash_.clear();
    history_.clear();
    prediction_.reset();
    say(std::string(color::bold) + "dotMate -- a menstrual health guide" + color::reset + "\n");
    say("What should I call you? (press enter to skip)\n> ");
}

void Session::on_line(const std::string& raw) {
    const std::string line = trim(raw);
    switch (state_) {
        case State::AskName: handle_name(line); break;
        case State::AskRemember: handle_remember(line); break;
        case State::Main: handle_main(line); break;
        case State::CalendarDates: handle_calendar_date(line); break;
        case State::CalendarExport: handle_calendar_export(line); break;
        case State::Done: break;
    }
}

void Session::begin_main() {
    say("Ask a question, type \"calendar\" to estimate your next period, or \"exit\" to leave.\n\n> ");
    state_ = State::Main;
}

void Session::handle_name(const std::string& name) {
    if (name.empty()) {
        say("No problem -- ask me anything, or type \"calendar\" to estimate your next period.\n\n");
        begin_main();
        return;
    }

    pending_hash_ = privacy::pseudonymize(name);
    if (const auto existing = store_.lookup(pending_hash_)) {
        display_name_ = *existing;
        say("Welcome back, " + display_name_ + "!\n\n");
        begin_main();
        return;
    }

    display_name_ = name;
    say("Nice to meet you, " + name + "!\n"
        "Want me to remember your name for next time? Nothing else about this "
        "session is ever saved. (y/n)\n> ");
    state_ = State::AskRemember;
}

void Session::handle_remember(const std::string& reply) {
    const auto answer = text::normalize(reply);
    if (answer == "y" || answer == "yes") {
        store_.remember(pending_hash_, display_name_);
        say("Got it -- I'll remember your name next time.\n\n");
    } else {
        say("Sure -- this session will just stay between us.\n\n");
    }
    begin_main();
}

void Session::handle_main(const std::string& line) {
    if (line.empty()) {
        say("> ");
        return;
    }

    const auto tokens = text::tokenize(text::normalize(line));
    if (wants_exit(tokens)) {
        say(display_name_.empty() ? std::string("Take care!\n")
                                  : "Take care, " + display_name_ + "!\n");
        state_ = State::Done;
        return;
    }
    if (wants_calendar(tokens)) {
        start_calendar();
        return;
    }
    answer_question(line);
    say("> ");
}

void Session::answer_question(const std::string& line) {
    const MatchResult result = matcher_.find(line);
    std::ostringstream out;
    switch (result.confidence) {
        case Confidence::Confident:
            out << color::green << result.best->answer << color::reset << "\n";
            if (!result.best->sources.empty()) {
                out << color::cyan << "(source: " << result.best->sources.front().name << ")"
                    << color::reset << "\n";
            }
            break;
        case Confidence::Possible:
            out << color::yellow << "I'm not fully sure -- did you mean one of these?"
                << color::reset << "\n";
            for (const auto& candidate : result.suggestions) {
                out << color::yellow << "  - " << candidate.entry->question << color::reset << "\n";
            }
            break;
        case Confidence::None:
            out << "I don't have a good answer for that one yet -- try rephrasing, "
                   "or ask something else.\n";
            break;
    }
    out << "\n";
    say(out.str());
}

void Session::start_calendar() {
    history_.clear();
    say("\nLet's estimate your next period. Enter your last few period start "
        "dates, oldest first, as YYYY-MM-DD -- one per line, blank line when "
        "you're done (need at least 2):\n> ");
    state_ = State::CalendarDates;
}

void Session::handle_calendar_date(const std::string& line) {
    if (line.empty()) {
        finish_calendar_dates();
        return;
    }
    if (const auto parsed = parse_iso_date(line)) {
        history_.push_back({*parsed});
    } else {
        say("Couldn't read that as YYYY-MM-DD -- try again, or leave it blank to stop.\n");
    }
    say("> ");
}

void Session::finish_calendar_dates() {
    if (history_.size() < 2) {
        say("Need at least two dates to estimate anything -- maybe next time.\n\n> ");
        state_ = State::Main;
        return;
    }

    const auto prediction = CycleTracker::predict(history_);
    if (!prediction) {
        say("Something went wrong estimating from those dates -- double check them "
            "and try again.\n\n> ");
        state_ = State::Main;
        return;
    }

    std::ostringstream out;
    out << "\nBased on those dates, your average cycle is about "
        << prediction->average_cycle_length_days << " days.\n";
    out << "Estimated next period: " << format_date(prediction->next_period_estimate) << "\n";
    if (prediction->length_is_unusual) {
        out << color::yellow
            << "That's outside the typical 21-35 day range for adults -- if that's "
               "been consistent for you, it's worth mentioning to a doctor."
            << color::reset << "\n";
    }
    out << "Want a printable calendar file? (y/n)\n> ";
    say(out.str());

    prediction_ = *prediction;
    state_ = State::CalendarExport;
}

void Session::handle_calendar_export(const std::string& reply) {
    const auto answer = text::normalize(reply);
    if (answer == "y" || answer == "yes") {
        const std::string filename = "period_calendar.csv";
        const auto where = io_.save_file(filename, CycleTracker::render_calendar_csv(*prediction_));
        if (where) {
            say("Saved to " + filename + *where + ".\n\n");
        } else {
            say("Couldn't write that file -- check folder permissions and try again.\n\n");
        }
    } else {
        say("\n");
    }
    prediction_.reset();
    state_ = State::Main;
    say("> ");
}

}  // namespace dotmate
