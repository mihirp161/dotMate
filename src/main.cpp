#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

#include "dotmate/knowledge_base.hpp"
#include "dotmate/matcher.hpp"
#include "dotmate/returning_user_store.hpp"
#include "dotmate/session.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

// Looks for the data file next to the executable first, then falls
// back to a path relative to the current working directory -- so it
// works whether you run it from the build folder or from an install
// layout.
std::string locate_data_file(const char* argv0) {
    namespace fs = std::filesystem;
    const fs::path candidates[] = {
        fs::path(argv0).parent_path() / "data" / "menstrual_qa.json",
        fs::path("data/menstrual_qa.json"),
        fs::path("../data/menstrual_qa.json"),
    };
    for (const auto& candidate : candidates) {
        if (fs::exists(candidate)) {
            return candidate.string();
        }
    }
    return "data/menstrual_qa.json";  // let KnowledgeBase::load_from_file report the error
}

// The terminal app's side of Session: stdout for text, a file in the
// current folder for the calendar export.
class StdioIo : public dotmate::SessionIo {
public:
    void write(const std::string& text) override {
        std::cout << text;
        std::cout.flush();
    }
    std::optional<std::string> save_file(const std::string& name,
                                         const std::string& contents) override {
        std::ofstream out(name);
        if (!out) return std::nullopt;
        out << contents;
        if (!out) return std::nullopt;
        return std::string(" in the current folder");
    }
};

#ifdef _WIN32
// The answers contain UTF-8 (em dashes) and ANSI colour codes; older
// Windows consoles need to be told to expect both.
void prepare_windows_console() {
    SetConsoleOutputCP(CP_UTF8);
    const HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (out != INVALID_HANDLE_VALUE && GetConsoleMode(out, &mode)) {
        SetConsoleMode(out, mode | 0x0004);  // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    }
}
#endif

}  // namespace

int main(int argc, char** argv) {
#ifdef _WIN32
    prepare_windows_console();
#endif

    const std::string data_path = locate_data_file(argc > 0 ? argv[0] : "");

    dotmate::KnowledgeBase kb;
    try {
        kb = dotmate::KnowledgeBase::load_from_file(data_path);
    } catch (const dotmate::LoadError& e) {
        std::cerr << "Failed to load knowledge base: " << e.what() << "\n";
        return 1;
    }

    const dotmate::Matcher matcher(kb);
    dotmate::FileReturningUserStore store("returning_users.json");
    StdioIo io;
    dotmate::Session session(matcher, store, io);

    session.start();
    std::string line;
    while (!session.finished() && std::getline(std::cin, line)) {
        session.on_line(line);
    }
    return 0;
}
