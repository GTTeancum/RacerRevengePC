#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include "core_suite.h"
#if !defined(__i386__) || defined(__SSE2__) || defined(__AVX__)
#error Xbox core must compile for 32-bit Pentium III without SSE2/AVX
#endif
static_assert(sizeof(void*) == 4, "Xbox pointers must be 32 bits");
int main() {
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
    const CoreTestResult r = run_core_suite();
    debugPrint("Racer Revenge shared-core validation\n");
    debugPrint("Not the game; no gameplay/FPS claim.\n");
    debugPrint("checks=%u failures=%u first_line=%u\n", r.checks, r.failures, r.first_failure_line);
    debugPrint("digest=%08x\n", r.digest);
    for (;;) Sleep(1000);
}
