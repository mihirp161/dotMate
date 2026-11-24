#pragma once

#include <cstdio>
#include <fstream>
#include <string>

#include "dotmate/knowledge_base.hpp"

namespace test_support {

// A tiny three-entry knowledge base, written to a temp file and loaded
// through the real loader, so tests control exactly what's in it.
inline dotmate::KnowledgeBase load_synthetic_kb() {
    const std::string path = "test_synthetic_kb.json";
    {
        std::ofstream out(path);
        out << R"JSON({
          "entries": [
            {
              "id": "period",
              "question": "What is a period?",
              "aliases": ["define period"],
              "answer": "Answer about periods.",
              "sources": [{"name": "Test Source", "url": "https://example.org"}]
            },
            {
              "id": "ovulation",
              "question": "What is ovulation?",
              "aliases": [],
              "answer": "Answer about ovulation.",
              "sources": []
            },
            {
              "id": "menopause",
              "question": "When does menopause happen?",
              "aliases": ["average age of menopause"],
              "answer": "Answer about menopause.",
              "sources": []
            }
          ]
        })JSON";
    }
    dotmate::KnowledgeBase kb = dotmate::KnowledgeBase::load_from_file(path);
    std::remove(path.c_str());
    return kb;
}

}  // namespace test_support
