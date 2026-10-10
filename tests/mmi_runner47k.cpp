#include "mmi_suite47k.h"
#include <stdio.h>
int main() {
    const auto r=run_mmi_suite47k();
    printf("MMI47K checks=%u failures=%u first_line=%u digest=%08x\n",
           r.checks,r.failures,r.first_failure_line,r.digest);
    return r.failures?1:0;
}
