#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include "mmi_suite47k.h"
#if !defined(__i386__) || defined(__SSE2__) || defined(__AVX__)
#error Xbox MMI validation requires Pentium III without SSE2
#endif
int main() {
    XVideoSetMode(640,480,32,REFRESH_DEFAULT);
    const auto r=run_mmi_suite47k();
    debugPrint("Borrowed PCSX2 scalar MMI validation; not the game\n");
    debugPrint("checks=%u failures=%u first_line=%u digest=%08x\n",
               r.checks,r.failures,r.first_failure_line,r.digest);
    for(;;) Sleep(1000);
}
