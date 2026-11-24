#include "dotmate/matcher.hpp"

#include "dotmate/knowledge_base.hpp"
#include "micro_test.hpp"
#include "test_support.hpp"

using namespace dotmate;

void run_matcher_tests() {
    const KnowledgeBase kb = test_support::load_synthetic_kb();
    const Matcher matcher(kb);

    // Exact phrasing -> confident, correct entry.
    {
        const auto r = matcher.find("What is a period?");
        CHECK(r.confidence == Confidence::Confident);
        CHECK(r.best != nullptr);
        if (r.best) CHECK(r.best->id == "period");
    }

    // Single-character typos in two different words -> still confident.
    // (This is the whole point of keeping Levenshtein: it should shrug
    // off exactly this kind of mistake.)
    {
        const auto r = matcher.find("Whatt is a periodd?");
        CHECK(r.confidence == Confidence::Confident);
        if (r.best) CHECK(r.best->id == "period");
    }

    // Word order scrambled -> the original's whole-string comparison
    // would have fallen apart here; token-based scoring shouldn't.
    {
        const auto r = matcher.find("period a what is");
        CHECK(r.confidence == Confidence::Confident);
        if (r.best) CHECK(r.best->id == "period");
    }

    // Extra wrapping words around a real match -> at least a
    // suggestion, ideally still confident, and never confused for a
    // *different* entry.
    {
        const auto r = matcher.find("so I was just wondering, what is a period exactly");
        CHECK(r.confidence != Confidence::None);
        if (r.confidence == Confidence::Confident) {
            CHECK(r.best->id == "period");
        } else {
            CHECK(!r.suggestions.empty());
            CHECK(r.suggestions.front().entry->id == "period");
        }
    }

    // A curated alias should match exactly as well as the canonical question.
    {
        const auto r = matcher.find("define period");
        CHECK(r.confidence == Confidence::Confident);
        if (r.best) CHECK(r.best->id == "period");
    }

    // Best match wins even when a different entry would have been
    // encountered first in the underlying list -- this is the direct
    // fix for the original returning the first candidate under a
    // threshold rather than the closest one.
    {
        const auto r = matcher.find("average age of menopause");
        CHECK(r.confidence == Confidence::Confident);
        if (r.best) CHECK(r.best->id == "menopause");
    }

    // Genuinely unrelated input -> no match.
    {
        const auto r = matcher.find("purple elephants dancing on the moon");
        CHECK(r.confidence == Confidence::None);
    }
}
