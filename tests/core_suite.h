#pragma once
#include <stdint.h>
struct CoreTestResult {
    uint32_t checks;
    uint32_t failures;
    uint32_t first_failure_line;
    uint32_t digest;
};
CoreTestResult run_core_suite() noexcept;
