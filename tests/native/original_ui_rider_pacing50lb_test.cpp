#include "original_ui_rider_pacing50lb.h"
#include "original_ui_elapsed50lh.h"
#include <cstdint>
#include <cstdio>
#include <limits>
namespace {
unsigned checks=0,failures=0;
void check(bool value,const char* label) {
    ++checks;if(!value){++failures;std::fprintf(stderr,"FAIL %s\n",label);}
}
// Fake clock exists only in this arithmetic executable. It cannot be bound to
// Frontend, a renderer, audio, a source clock or a window presentation owner.
struct FakeClock {
    std::uint64_t value=0;
    std::uint64_t operator()()const noexcept{return value;}
};
}
int main() {
    using namespace original_ui_rider_pacing50LB;
    constexpr auto period=periodNanoseconds;
    constexpr auto maximum=std::numeric_limits<std::uint64_t>::max();
    FakeClock clock;Deadline deadline;
    check(deadline.observe(clock()).error==Error::phase,"observe before begin is rejected");
    check(deadline.finishWait(clock())==Error::phase,"finish before begin is rejected");
    check(deadline.beginWait(clock())==Error::none,"first observed zero is a valid numeric timestamp");
    check(deadline.beginWait(clock())==Error::phase,"overlapping waits are rejected");
    auto observed=deadline.observe(clock());
    check(observed.error==Error::none&&!observed.due&&observed.sleepNanoseconds==maximumSleepSliceNanoseconds,
        "first wait requests only one millisecond, not synthetic advancement");
    clock.value=period-1;observed=deadline.observe(clock());
    check(!observed.due&&observed.sleepNanoseconds==1,"last nanosecond is still a real wait");
    check(deadline.finishWait(clock())==Error::early,"early completion is rejected");
    clock.value=period;observed=deadline.observe(clock());
    check(observed.error==Error::none&&observed.due&&observed.sleepNanoseconds==0,"deadline equality is due");
    check(deadline.finishWait(clock())==Error::none,"observed due wait finishes once");
    check(deadline.finishWait(clock())==Error::phase,"double finish is rejected");
    check(deadline.observe(clock()).error==Error::phase,"finished wait cannot emit another admission");
    check(deadline.beginWait(clock())==Error::none,"next sample can begin");
    observed=deadline.observe(clock());
    check(!observed.due&&observed.sleepNanoseconds==maximumSleepSliceNanoseconds,"next deadline is in the future");
    clock.value=4*period;observed=deadline.observe(clock());
    check(observed.due&&observed.sleepNanoseconds==0,"late service uses actual observed time");
    check(deadline.finishWait(clock())==Error::none,"late service reanchors one future deadline");
    check(deadline.beginWait(clock())==Error::none,"following sample begins after late service");
    observed=deadline.observe(clock());
    check(!observed.due,"missed slots do not produce catch-up admissions");
    clock.value=5*period-1;observed=deadline.observe(clock());
    check(!observed.due&&observed.sleepNanoseconds==1,"reanchored deadline preserves its exact remaining nanosecond");
    check(deadline.observe(clock()-1).error==Error::backwards,"backwards clock is rejected");
    check(deadline.finishWait(clock()-1)==Error::backwards,"backwards finish is rejected");
    clock.value=5*period;check(deadline.finishWait(clock())==Error::none,"backwards rejection did not damage valid state");
    check(deadline.beginWait(clock()-1)==Error::backwards,"backwards next begin is rejected");
    deadline.reset();check(deadline.observe(clock()).error==Error::phase,"reset cancels arithmetic wait ownership");
    check(deadline.beginWait(maximum-period+1)==Error::overflow,"first deadline overflow is rejected");
    check(deadline.beginWait(maximum-period)==Error::none,"largest representable first deadline is admitted");
    observed=deadline.observe(maximum);check(observed.due,"maximum deadline is observed without wrap");
    check(deadline.finishWait(maximum)==Error::overflow,"future-deadline overflow never wraps to ready");
    deadline.reset();clock.value=1234;
    check(deadline.beginWait(clock())==Error::none,"reset after overflow starts an independent numeric sequence");
    clock.value+=period;check(deadline.observe(clock()).due&&deadline.finishWait(clock())==Error::none,
        "fresh numeric wait works after cancelled overflow");
    // Deadline arithmetic cannot hide/credit a real stall. Runtime still uses
    // this unchanged audio interval gate before admitting a waited sample.
    constexpr std::uint64_t base=1000;
    const auto oneSecond=original_ui_elapsed50LH::project(base,base+1000000000ull,0);
    const auto stall=original_ui_elapsed50LH::project(base,base+1000000001ull,0);
    check(oneSecond.error==original_ui_elapsed50LH::Error::none&&oneSecond.activeNanoseconds==1000000000ull,
        "existing exact one-second active boundary remains accepted");
    check(stall.error==original_ui_elapsed50LH::Error::stall&&stall.excludedColdNanoseconds==0,
        "one-second-plus-one active wait still fails, with no cold credit");
    std::printf("rider_pacing_arithmetic checks=%u failures=%u real_clock_or_display_tested=0\n",checks,failures);
    return failures?1:0;
}
