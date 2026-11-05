#include "../../src/game/original_water_index_storage50la.h"
#include <cassert>
#include <cstdio>
int main(){
    using S=rr::game::OriginalWaterIndexStorage50LA;
    const std::array<std::uint8_t,4> bytes{0x78,0x56,0x34,0x12};
    for(std::size_t copied=0;copied<=4;++copied){
        S s;assert(s.prepare(3)==S::Allocation::ready&&s.readCount()==2);
        assert(s.copyPrefix(0,bytes,copied)&&s.matches(0,bytes,copied));
        assert(s.matches(1,{},0)&&s.matches(2,{},0));
        assert(!s.copyPrefix(2,bytes,4)); //T-1: final allocated word stays unknown.
        assert(copied==4?s.knownWord(0)==0x12345678u:!s.knownWord(0));
        assert(!s.knownWord(1)&&!s.knownWord(2));
        if(copied){
            auto changed=bytes;changed[0]^=255u;
            assert(!s.copyPrefix(0,changed,4)&&s.matches(0,bytes,copied));
            assert(!s.matches(0,changed,copied));
        }
        assert(!s.matches(3,{},0)&&!s.knownWord(3));
        assert(!s.copyPrefix(1,bytes,5)&&s.matches(1,{},0));
        assert(s.prepare(3)==S::Allocation::alreadyAttempted);
    }
    S one;assert(one.prepare(1)==S::Allocation::ready&&one.readCount()==0);
    assert(!one.copyPrefix(0,bytes,0)&&!one.knownWord(0));
    S empty;const auto zero=empty.prepare(0);
    assert(zero==S::Allocation::ready||zero==S::Allocation::payload);
    assert(empty.count()==0&&empty.readCount()==0&&!empty.knownWord(0));
    S large;assert(large.prepare(16384)==S::Allocation::ready&&large.readCount()==16383);
    assert(large.copyPrefix(16382,bytes,4)&&large.knownWord(16382)==0x12345678u&&!large.knownWord(16383));
    S refused;assert(refused.prepare(16385)==S::Allocation::capacity&&!refused.ready());
    std::puts("Water indices: full/short/EOF prefix knownness, T-1 extent, zero and capacity passed");
}
