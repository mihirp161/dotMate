#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace dotmate {

struct Source {
    std::string name;
    std::string url;
};

struct Entry {
    std::string id;
    std::string question;
    std::vector<std::string> aliases;
    std::string answer;
    std::vector<Source> sources;

    // Precomputed at load time so the matcher never re-tokenizes the
    // same knowledge-base text on every query. One entry per string in
    // "all_phrasings" (question, then each alias), in the same order.
    std::vector<std::string> all_phrasings;
    std::vector<std::vector<std::string>> phrasing_tokens;
};

class LoadError : public std::runtime_error {
public:
    explicit LoadError(const std::string& what) : std::runtime_error(what) {}
};

// Loads and validates the Q&A data file once at startup. The knowledge
// base is immutable after loading -- nothing in the matcher or session
// mutates it, so re-reading the file on every question (as the original
// did) is unnecessary rather than just slow.
class KnowledgeBase {
public:
    static KnowledgeBase load_from_file(const std::string& path);

    const std::vector<Entry>& entries() const { return entries_; }
    std::size_t size() const { return entries_.size(); }

private:
    std::vector<Entry> entries_;
};

}  // namespace dotmate
