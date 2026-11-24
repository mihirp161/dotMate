#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace dotmate::text {

// Lowercases, strips punctuation/digits, and collapses whitespace.
// This is deliberately simple ASCII-range normalization -- good enough
// for a small hand-curated FAQ, not a general Unicode text pipeline.
std::string normalize(const std::string& input);

// Splits an already-normalized string on whitespace.
std::vector<std::string> tokenize(const std::string& normalized_input);

// True if `word` is common enough to be dropped when scoring similarity.
// Deliberately keeps question words (what/when/how/why/who/which) --
// those carry most of the actual intent of a question. This is the
// specific mistake the original matcher made: it stripped whitespace
// and compared whole sentences, so it had no notion of "important"
// vs. "filler" words at all.
bool is_stopword(const std::string& word);

// Returns `tokens` with stopwords removed. If that would remove every
// token (e.g. the query *is* "what is this"), returns the original
// tokens unfiltered rather than an empty vector.
std::vector<std::string> remove_stopwords(const std::vector<std::string>& tokens);

// Classic Levenshtein (edit) distance between two strings: the minimum
// number of single-character insertions, deletions, or substitutions
// to turn `a` into `b`. O(n*m) time, O(min(n,m)) space.
//
// This is the one piece of the original project kept deliberately
// unchanged in spirit -- it's still the right tool for catching typos
// at the individual-word level. What changed is *what* it's applied to:
// see Matcher, which runs this per-token instead of over an entire
// squashed-together sentence.
std::size_t levenshtein_distance(const std::string& a, const std::string& b);

// Similarity in [0.0, 1.0] derived from edit distance, normalized by
// the length of the longer string so a threshold means the same thing
// for a 3-letter word and a 12-letter word. 1.0 = identical.
double token_similarity(const std::string& a, const std::string& b);

}  // namespace dotmate::text
