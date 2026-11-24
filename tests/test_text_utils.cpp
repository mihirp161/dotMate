#include "dotmate/text_utils.hpp"

#include "micro_test.hpp"

using namespace dotmate;

void run_text_utils_tests() {
    // normalize: lowercases, drops punctuation/digits, collapses whitespace
    CHECK(text::normalize("What IS a period?") == "what is a period");
    CHECK(text::normalize("  extra   spaces  ") == "extra spaces");
    CHECK(text::normalize("v2 super-absorbent!!") == "v super absorbent");
    CHECK(text::normalize("") == "");

    // tokenize
    {
        const auto tokens = text::tokenize("what is a period");
        CHECK(tokens.size() == 4);
        CHECK(tokens[0] == "what");
        CHECK(tokens[3] == "period");
    }
    CHECK(text::tokenize("").empty());

    // stopwords: question words must survive; filler words must not
    CHECK(!text::is_stopword("what"));
    CHECK(!text::is_stopword("when"));
    CHECK(!text::is_stopword("how"));
    CHECK(!text::is_stopword("why"));
    CHECK(text::is_stopword("a"));
    CHECK(text::is_stopword("the"));
    CHECK(text::is_stopword("is"));

    {
        const auto filtered = text::remove_stopwords({"what", "is", "a", "period"});
        CHECK(filtered.size() == 2);
        CHECK(filtered[0] == "what");
        CHECK(filtered[1] == "period");
    }
    // If every token is a stopword, remove_stopwords should give back
    // the original tokens rather than an empty vector -- an empty
    // token list would make every candidate score zero, which is worse
    // than falling back to unfiltered matching.
    {
        const auto filtered = text::remove_stopwords({"is", "a", "the"});
        CHECK(filtered.size() == 3);
    }

    // Levenshtein distance: known textbook values
    CHECK(text::levenshtein_distance("kitten", "sitting") == 3);
    CHECK(text::levenshtein_distance("", "") == 0);
    CHECK(text::levenshtein_distance("abc", "") == 3);
    CHECK(text::levenshtein_distance("", "abc") == 3);
    CHECK(text::levenshtein_distance("period", "period") == 0);
    CHECK(text::levenshtein_distance("period", "periodd") == 1);  // one extra letter
    CHECK(text::levenshtein_distance("what", "whatt") == 1);

    // token_similarity: normalized, symmetric, 1.0 for identical strings
    CHECK(text::token_similarity("period", "period") == 1.0);
    CHECK(text::token_similarity("period", "periodd") > 0.8);
    CHECK(text::token_similarity("what", "whatt") > 0.7);
    CHECK(text::token_similarity("period", "banana") < 0.5);
    CHECK(text::token_similarity("abc", "xyz") == text::token_similarity("xyz", "abc"));
}
