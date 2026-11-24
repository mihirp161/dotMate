// WebAssembly entry points. Built only by Emscripten (see CMakeLists.txt).
//
// JavaScript talks to the C++ through two functions that each take and
// return plain strings:
//
//   dotmate_init(stored_users_json) -> reply   starts (or restarts) a conversation
//   dotmate_send_line(line)         -> reply   feeds in one line the person typed
//
// A reply is JSON: {"ok": true, "text": "...", "finished": false,
// "files": [{"name": "...", "contents": "..."}], "store": null | "<json>"}.
// "text" is what to print (ANSI colours included, which a terminal
// emulator renders natively); "files" are downloads to offer; "store" is
// the updated remembered-names JSON whenever it changed, for the page to
// keep in localStorage. On failure: {"ok": false, "error": "..."}.

#include <emscripten/emscripten.h>

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "dotmate/knowledge_base.hpp"
#include "dotmate/matcher.hpp"
#include "dotmate/returning_user_store.hpp"
#include "dotmate/session.hpp"
#include "nlohmann/json.hpp"

namespace {

// Collects what the session says during one call so it can be handed
// back to JavaScript in one piece.
class BufferIo : public dotmate::SessionIo {
public:
    void write(const std::string& text) override { text_ += text; }
    std::optional<std::string> save_file(const std::string& name,
                                         const std::string& contents) override {
        files_.emplace_back(name, contents);
        return std::string(" (check your downloads)");
    }

    std::string text_;
    std::vector<std::pair<std::string, std::string>> files_;
};

// A live conversation plus everything it borrows. Never moved after
// construction, so the references between members stay valid.
struct Engine {
    Engine(dotmate::KnowledgeBase loaded, const std::string& stored_users_json)
        : kb(std::move(loaded)),
          matcher(kb),
          store(stored_users_json),
          session(matcher, store, io) {}

    dotmate::KnowledgeBase kb;
    dotmate::Matcher matcher;
    dotmate::MemoryReturningUserStore store;
    BufferIo io;
    dotmate::Session session;
};

std::unique_ptr<Engine> g_engine;
std::string g_reply;  // owns the string returned to JavaScript until the next call

const char* finish(nlohmann::json reply) {
    g_reply = reply.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
    return g_reply.c_str();
}

const char* error_reply(const std::string& message) {
    return finish({{"ok", false}, {"error", message}});
}

const char* reply_from(Engine& engine) {
    nlohmann::json reply = {{"ok", true},
                            {"text", engine.io.text_},
                            {"finished", engine.session.finished()},
                            {"files", nlohmann::json::array()},
                            {"store", nullptr}};
    for (const auto& [name, contents] : engine.io.files_) {
        reply["files"].push_back({{"name", name}, {"contents", contents}});
    }
    if (engine.store.take_changed()) {
        reply["store"] = engine.store.to_json();
    }
    engine.io.text_.clear();
    engine.io.files_.clear();
    return finish(std::move(reply));
}

}  // namespace

extern "C" {

EMSCRIPTEN_KEEPALIVE
const char* dotmate_init(const char* stored_users_json) {
    try {
        g_engine.reset();
        g_engine = std::make_unique<Engine>(
            dotmate::KnowledgeBase::load_from_file("/data/menstrual_qa.json"),
            stored_users_json ? stored_users_json : "");
        g_engine->session.start();
        return reply_from(*g_engine);
    } catch (const std::exception& e) {
        return error_reply(e.what());
    }
}

EMSCRIPTEN_KEEPALIVE
const char* dotmate_send_line(const char* line) {
    if (!g_engine) return error_reply("dotmate_init has not been called");
    try {
        g_engine->session.on_line(line ? line : "");
        return reply_from(*g_engine);
    } catch (const std::exception& e) {
        return error_reply(e.what());
    }
}

}  // extern "C"
