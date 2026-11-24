#include <iostream>

#include "micro_test.hpp"

void run_text_utils_tests();
void run_matcher_tests();
void run_cycle_tracker_tests();
void run_session_tests();

int main() {
    run_text_utils_tests();
    run_matcher_tests();
    run_cycle_tracker_tests();
    run_session_tests();

    const int total = micro_test::total_count();
    const int failed = micro_test::failure_count();
    std::cout << (total - failed) << "/" << total << " checks passed\n";
    return failed == 0 ? 0 : 1;
}
