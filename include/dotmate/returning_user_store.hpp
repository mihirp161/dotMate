#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <utility>

namespace dotmate {

// Where the opt-in "remember my name" feature keeps its data. By design
// the only thing that ever goes in here is a name hash and the display
// name -- never anything the person asked.
class ReturningUserStore {
public:
    virtual ~ReturningUserStore() = default;
    virtual std::optional<std::string> lookup(const std::string& hash) const = 0;
    virtual void remember(const std::string& hash, const std::string& display_name) = 0;
};

// Keeps everything in memory and round-trips through JSON text in the
// same format the file store writes: {"<hash>": {"display_name": "..."}}.
// Used by the tests and by the WebAssembly build, where the page keeps
// that JSON in localStorage.
class MemoryReturningUserStore : public ReturningUserStore {
public:
    MemoryReturningUserStore() = default;
    // Malformed or empty text is treated as "nobody remembered yet".
    explicit MemoryReturningUserStore(const std::string& json_text);

    std::optional<std::string> lookup(const std::string& hash) const override;
    void remember(const std::string& hash, const std::string& display_name) override;

    std::string to_json() const;
    // True (once) if something was remembered since the last call.
    bool take_changed();
    std::size_t size() const { return users_.size(); }

private:
    std::map<std::string, std::string> users_;
    bool changed_ = false;
};

// A JSON file on disk (native build). Same format as the original app.
class FileReturningUserStore : public ReturningUserStore {
public:
    explicit FileReturningUserStore(std::string path) : path_(std::move(path)) {}
    std::optional<std::string> lookup(const std::string& hash) const override;
    void remember(const std::string& hash, const std::string& display_name) override;

private:
    std::string path_;
};

}  // namespace dotmate
