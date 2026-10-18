#include "original_ui_elapsed50lh.h"
#include <cstdint>
#include <iostream>
#include <limits>

namespace {
unsigned checks=0;
bool check(bool accepted,const char* name) {
    ++checks;if(!accepted)std::cerr<<"FAIL: "<<name<<'\n';return accepted;
}
}
int main() {
    using namespace original_ui_elapsed50LH;
    bool okay=true;
    constexpr std::uint64_t begin=100,second=1000000000ull;
    std::uint64_t last=begin,cold=1500000000ull;
    const auto excluded=consume(last,begin+cold+16000000ull,cold);
    okay&=check(excluded.error==Error::none&&excluded.wallNanoseconds==1516000000ull&&
        excluded.excludedColdNanoseconds==1500000000ull&&excluded.activeNanoseconds==16000000ull,
        "actual >1s cold work excluded, active 16ms retained");
    okay&=check(cold==0&&last==begin+1516000000ull,"update credit consumed once");
    const auto next=consume(last,last+1500000000ull,cold);
    okay&=check(next.error==Error::stall&&next.excludedColdNanoseconds==0&&next.activeNanoseconds==1500000000ull,
        "previous credit cannot hide later active GPU/render/swap stall");
    const auto mixed=project(begin,begin+1500000000ull+second+1,1500000000ull);
    okay&=check(mixed.error==Error::stall&&mixed.activeNanoseconds==second+1,
        "genuine >1s active work fails even beside long cold work");
    okay&=check(project(begin,begin+second,0).error==Error::none&&
        project(begin,begin+second+1,0).error==Error::stall,"unchanged exact one-second boundary");
    last=100;cold=20;const auto backwards=consume(last,99,cold);
    okay&=check(backwards.error==Error::backwards&&last==100&&cold==20,"backwards interval leaves state intact");
    const auto overCredit=consume(last,119,cold);
    okay&=check(overCredit.error==Error::coldExceedsWall&&last==100&&cold==20,
        "cold cannot exceed measured wall interval");
    last=0;cold=123;const auto first=consume(last,500,cold);
    okay&=check(first.error==Error::none&&first.activeNanoseconds==0&&last==500&&cold==0,
        "first/resumed update preserves no-catch-up convention");
    std::uint64_t update=0,material=0;
    okay&=check(creditCold(900,update,material)&&creditCold(100,update,material)&&update==1000&&material==1000,
        "separate nonoverlapping real cold intervals add once per consumer");
    last=100;const auto checkpoint=project(last,1200,update);
    okay&=check(checkpoint.error==Error::none&&checkpoint.activeNanoseconds==100&&last==100&&update==1000&&material==1000,
        "audio checkpoint projection leaves state intact before output succeeds");
    // These are exactly the host's post-success stores; failure performs none.
    update=0;last=1200;
    okay&=check(material==1000&&project(last,1300,update).activeNanoseconds==100,
        "successful checkpoint consumes only update credit, leaves material credit");
    const auto materialInterval=project(100,1300,material);material=0;
    okay&=check(materialInterval.error==Error::none&&materialInterval.activeNanoseconds==200&&
        project(1300,1400,material).activeNanoseconds==100,"material credit independently consumed once");
    const auto maximum=std::numeric_limits<std::uint64_t>::max();update=maximum-5;material=4;
    okay&=check(!creditCold(6,update,material)&&update==maximum-5&&material==4,
        "update overflow changes neither counter");
    update=4;material=maximum-5;
    okay&=check(!creditCold(6,update,material)&&update==4&&material==maximum-5,
        "material overflow changes neither counter");
    okay&=check(creditCold(5,update,material)&&update==9&&material==maximum,"representable maximum accepted");
    if(okay)std::cout<<"original_ui_elapsed50lh: "<<checks<<" checks passed\n";
    return okay?0:1;
}
