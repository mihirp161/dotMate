#pragma once

#include <iostream>
#include <string>

// A deliberately tiny test harness -- no framework dependency, just
// enough to make the test suite's intent obvious. Swap in Catch2 or
// doctest if this project grows past a few dozen cases; the point
// here is that a test suite exists and runs as part of the build at
// all, which the original project had none of.
namespace micro_test {

inline int& failure_count() {
    static int count = 0;
    return count;
}

inline int& total_count() {
    static int count = 0;
    return count;
}

inline void check(bool condition, const char* expr, const char* file, int line) {
    ++total_count();
    if (!condition) {
        ++failure_count();
        std::cerr << "FAIL: " << expr << "  (" << file << ":" << line << ")\n";
    }
}

}  // namespace micro_test

#define CHECK(cond) ::micro_test::check((cond), #cond, __FILE__, __LINE__)
