#pragma once

#include <string>
#include <vector>

#include "dotmate/knowledge_base.hpp"

namespace dotmate {

enum class Confidence { Confident, Possible, None };

struct Candidate {
    const Entry* entry;
    double score;  // in [0, 1]
};

struct MatchResult {
    Confidence confidence = Confidence::None;
    // Populated when confidence == Confident.
    const Entry* best = nullptr;
    // Populated (up to 3, best first) when confidence == Possible.
    std::vector<Candidate> suggestions;
};

// Tokenized, length-normalized fuzzy matching over a KnowledgeBase.
//
// This class has no I/O and touches nothing outside its constructor
// argument and its own scoring math, specifically so it can be unit
// tested directly (see tests/test_matcher.cpp) without spinning up a
// session, a terminal, or a file on disk.
//
// Scoring, in short: every query token is fuzzy-matched (via
// Levenshtein-based token_similarity) against every candidate token,
// and vice versa, giving a precision score (how much of the query the
// candidate accounts for) and a recall score (how much of the
// candidate the query accounts for). The final score is their harmonic
// mean (F1) -- order-independent, typo-tolerant per word, and it
// rewards a query that covers a candidate's key terms even when wrapped
// in extra filler words, rather than penalizing every extra word
// as harshly as a plain query-length normalization would.
// An entry's overall score is the best score across its question and
// all of its aliases. The best-scoring entry wins; it does not have to
// be the first one encountered.
//
// The specific thresholds below are a first-cut default, not a
// mathematically derived constant -- they're meant to be tuned once
// there's a real corpus of test queries to tune against.
class Matcher {
public:
    static constexpr double kConfidentThreshold = 0.75;
    static constexpr double kPossibleThreshold = 0.50;
    static constexpr double kTokenMatchThreshold = 0.70;

    explicit Matcher(const KnowledgeBase& kb) : kb_(kb) {}

    MatchResult find(const std::string& query) const;

    // Exposed for testing: the raw per-entry score a query gets against
    // one specific entry (its best phrasing), without the
    // confident/possible/none bucketing.
    double score_against(const std::string& query, const Entry& entry) const;

private:
    const KnowledgeBase& kb_;
};

}  // namespace dotmate
