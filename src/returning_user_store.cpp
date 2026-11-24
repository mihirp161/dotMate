#include "dotmate/returning_user_store.hpp"

#include <fstream>
#include <sstream>

#include "nlohmann/json.hpp"

namespace dotmate {

namespace {

std::string read_all(const std::string& path) {
    std::ifstream in(path);
    if (!in) return {};
    std::ostringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

}  // namespace

MemoryReturningUserStore::MemoryReturningUserStore(const std::string& json_text) {
    if (json_text.empty()) return;
    try {
        const auto root = nlohmann::json::parse(json_text);
        if (!root.is_object()) return;
        for (auto it = root.begin(); it != root.end(); ++it) {
            const auto& entry = it.value();
            if (entry.is_object() && entry.contains("display_name") &&
                entry["display_name"].is_string()) {
                users_[it.key()] = entry["display_name"].get<std::string>();
            }
        }
    } catch (const nlohmann::json::exception&) {
        users_.clear();  // unreadable data: start fresh rather than fail the session
    }
}

std::optional<std::string> MemoryReturningUserStore::lookup(const std::string& hash) const {
    const auto it = users_.find(hash);
    if (it == users_.end()) return std::nullopt;
    return it->second;
}

void MemoryReturningUserStore::remember(const std::string& hash, const std::string& display_name) {
    users_[hash] = display_name;
    changed_ = true;
}

std::string MemoryReturningUserStore::to_json() const {
    nlohmann::json root = nlohmann::json::object();
    for (const auto& [hash, name] : users_) {
        root[hash] = {{"display_name", name}};
    }
    return root.dump(2, ' ', false, nlohmann::json::error_handler_t::replace);
}

bool MemoryReturningUserStore::take_changed() {
    const bool was = changed_;
    changed_ = false;
    return was;
}

std::optional<std::string> FileReturningUserStore::lookup(const std::string& hash) const {
    return MemoryReturningUserStore(read_all(path_)).lookup(hash);
}

void FileReturningUserStore::remember(const std::string& hash, const std::string& display_name) {
    MemoryReturningUserStore store(read_all(path_));
    store.remember(hash, display_name);
    std::ofstream out(path_);
    if (out) out << store.to_json();
}

}  // namespace dotmate
