#include <stdio.h>
#include "core_suite.h"
int main() {
    const CoreTestResult r = run_core_suite();
    printf("core-check pointer_bits=%u checks=%u failures=%u first_failure_line=%u digest=%08x\n",
        unsigned(sizeof(void*) * 8), r.checks, r.failures, r.first_failure_line, r.digest);
    return r.failures ? 1 : 0;
}
