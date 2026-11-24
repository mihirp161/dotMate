#include "dotmate/text_utils.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_set>

namespace dotmate::text {

std::string normalize(const std::string& input) {
    std::string out;
    out.reserve(input.size());

    // Every non-letter character (digits, punctuation, and whitespace
    // alike) is treated as a word boundary rather than silently
    // deleted. That matters for things like hyphens and slashes:
    // "super-absorbent" should tokenize as two words, not fuse into
    // "superabsorbent" just because there was no literal space there.
    bool last_was_boundary = true;  // true so no leading space is emitted
    for (unsigned char ch : input) {
        if (std::isalpha(ch)) {
            out.push_back(static_cast<char>(std::tolower(ch)));
            last_was_boundary = false;
        } else if (!last_was_boundary) {
            out.push_back(' ');
            last_was_boundary = true;
        }
    }
    if (!out.empty() && out.back() == ' ') {
        out.pop_back();
    }
    return out;
}

std::vector<std::string> tokenize(const std::string& normalized_input) {
    std::vector<std::string> tokens;
    std::istringstream stream(normalized_input);
    std::string word;
    while (stream >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

bool is_stopword(const std::string& word) {
    // Intentionally small, and intentionally does NOT include
    // what/when/how/why/who/which/can/should/will -- those are the
    // words that actually distinguish one question from another.
    static const std::unordered_set<std::string> stopwords = {
        "a", "an", "the", "is", "are", "was", "were", "be", "been",
        "do", "does", "did", "of", "to", "in", "on", "at", "for",
        "i", "my", "me", "you", "your", "it", "its", "this", "that",
        "and", "or", "so", "up",
    };
    return stopwords.count(word) != 0;
}

std::vector<std::string> remove_stopwords(const std::vector<std::string>& tokens) {
    std::vector<std::string> filtered;
    filtered.reserve(tokens.size());
    for (const auto& t : tokens) {
        if (!is_stopword(t)) {
            filtered.push_back(t);
        }
    }
    return filtered.empty() ? tokens : filtered;
}

std::size_t levenshtein_distance(const std::string& a, const std::string& b) {
    const std::size_t n = a.size();
    const std::size_t m = b.size();
    if (n == 0) return m;
    if (m == 0) return n;

    // Two-row rolling DP table -- O(min(n,m)) space instead of the
    // original's full O(n*m) table, same recurrence otherwise.
    std::vector<std::size_t> prev(m + 1), curr(m + 1);
    for (std::size_t j = 0; j <= m; ++j) prev[j] = j;

    for (std::size_t i = 1; i <= n; ++i) {
        curr[0] = i;
        for (std::size_t j = 1; j <= m; ++j) {
            const std::size_t cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            curr[j] = std::min({
                prev[j] + 1,        // deletion
                curr[j - 1] + 1,    // insertion
                prev[j - 1] + cost  // substitution / match
            });
        }
        std::swap(prev, curr);
    }
    return prev[m];
}

double token_similarity(const std::string& a, const std::string& b) {
    const std::size_t longer = std::max(a.size(), b.size());
    if (longer == 0) return 1.0;  // both empty
    const std::size_t dist = levenshtein_distance(a, b);
    return 1.0 - static_cast<double>(dist) / static_cast<double>(longer);
}

}  // namespace dotmate::text
