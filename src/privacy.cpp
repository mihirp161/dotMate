#include "dotmate/privacy.hpp"

#include <iomanip>
#include <sstream>

#include "dotmate/text_utils.hpp"

namespace dotmate::privacy {

std::string pseudonymize(const std::string& name) {
    // FNV-1a, 64-bit.
    std::uint64_t hash = 14695981039346656037ULL;
    constexpr std::uint64_t prime = 1099511628211ULL;

    const std::string normalized = text::normalize(name);
    for (unsigned char ch : normalized) {
        hash ^= ch;
        hash *= prime;
    }

    std::ostringstream out;
    out << std::hex << std::setw(16) << std::setfill('0') << hash;
    return out.str();
}

}  // namespace dotmate::privacy
