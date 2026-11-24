#include "dotmate/knowledge_base.hpp"

#include <fstream>
#include <sstream>

#include "dotmate/text_utils.hpp"
#include "nlohmann/json.hpp"

namespace dotmate {

namespace {

Entry parse_entry(const nlohmann::json& j) {
    Entry e;
    e.id = j.at("id").get<std::string>();
    e.question = j.at("question").get<std::string>();
    e.answer = j.at("answer").get<std::string>();

    if (j.contains("aliases")) {
        for (const auto& alias : j.at("aliases")) {
            e.aliases.push_back(alias.get<std::string>());
        }
    }
    if (j.contains("sources")) {
        for (const auto& src : j.at("sources")) {
            Source s;
            s.name = src.value("name", "");
            s.url = src.value("url", "");
            e.sources.push_back(std::move(s));
        }
    }

    e.all_phrasings.push_back(e.question);
    for (const auto& alias : e.aliases) {
        e.all_phrasings.push_back(alias);
    }
    for (const auto& phrasing : e.all_phrasings) {
        auto tokens = text::tokenize(text::normalize(phrasing));
        e.phrasing_tokens.push_back(text::remove_stopwords(tokens));
    }
    return e;
}

}  // namespace

KnowledgeBase KnowledgeBase::load_from_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw LoadError("could not open knowledge base file: " + path);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    nlohmann::json root;
    try {
        root = nlohmann::json::parse(buffer.str());
    } catch (const nlohmann::json::parse_error& e) {
        throw LoadError("malformed knowledge base JSON in " + path + ": " + e.what());
    }

    if (!root.contains("entries") || !root.at("entries").is_array()) {
        throw LoadError("knowledge base file has no 'entries' array: " + path);
    }

    KnowledgeBase kb;
    for (const auto& entry_json : root.at("entries")) {
        kb.entries_.push_back(parse_entry(entry_json));
    }

    if (kb.entries_.empty()) {
        throw LoadError("knowledge base loaded but contains zero entries: " + path);
    }
    return kb;
}

}  // namespace dotmate
