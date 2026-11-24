#include "dotmate/matcher.hpp"

#include <algorithm>

#include "dotmate/text_utils.hpp"

namespace dotmate {

namespace {

// How much of `from`'s tokens have a good-enough fuzzy match somewhere
// in `to` -- "precision-of-from-against-to", using soft credit (a
// near-perfect fuzzy match counts a little less than 1.0).
double coverage(const std::vector<std::string>& from, const std::vector<std::string>& to) {
    double matched = 0.0;
    for (const auto& a : from) {
        double best = 0.0;
        for (const auto& b : to) {
            best = std::max(best, text::token_similarity(a, b));
        }
        if (best >= Matcher::kTokenMatchThreshold) {
            matched += best;
        }
    }
    return matched / static_cast<double>(from.size());
}

// Score one token list against another as the harmonic mean (F1) of
// two directional coverage scores:
//   - precision: how much of the *query* is accounted for by the
//     candidate (penalizes rambling, off-topic extra words)
//   - recall: how much of the *candidate* is accounted for by the
//     query (rewards covering the candidate's actual key terms, even
//     inside a longer or reordered query)
// Order-independent by construction, and a query with a few extra
// filler words around a real match still scores well as long as it
// covers the candidate's key terms -- which is the specific failure
// mode the original's whole-string comparison had no way to avoid.
double score_token_lists(const std::vector<std::string>& query_tokens,
                          const std::vector<std::string>& candidate_tokens) {
    if (query_tokens.empty() || candidate_tokens.empty()) {
        return 0.0;
    }
    const double precision = coverage(query_tokens, candidate_tokens);
    const double recall = coverage(candidate_tokens, query_tokens);
    if (precision + recall == 0.0) {
        return 0.0;
    }
    return 2.0 * precision * recall / (precision + recall);
}

}  // namespace

double Matcher::score_against(const std::string& query, const Entry& entry) const {
    const auto query_tokens =
        text::remove_stopwords(text::tokenize(text::normalize(query)));

    double best = 0.0;
    for (const auto& phrasing_tokens : entry.phrasing_tokens) {
        best = std::max(best, score_token_lists(query_tokens, phrasing_tokens));
    }
    return best;
}

MatchResult Matcher::find(const std::string& query) const {
    std::vector<Candidate> scored;
    scored.reserve(kb_.size());
    for (const auto& entry : kb_.entries()) {
        scored.push_back({&entry, score_against(query, entry)});
    }

    std::sort(scored.begin(), scored.end(), [](const Candidate& a, const Candidate& b) {
        return a.score > b.score;
    });

    MatchResult result;
    if (scored.empty() || scored.front().score < kPossibleThreshold) {
        result.confidence = Confidence::None;
        return result;
    }

    if (scored.front().score >= kConfidentThreshold) {
        result.confidence = Confidence::Confident;
        result.best = scored.front().entry;
        return result;
    }

    result.confidence = Confidence::Possible;
    const std::size_t n = std::min<std::size_t>(3, scored.size());
    for (std::size_t i = 0; i < n && scored[i].score >= kPossibleThreshold; ++i) {
        result.suggestions.push_back(scored[i]);
    }
    return result;
}

}  // namespace dotmate
