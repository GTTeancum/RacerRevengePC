#include "../../src/game/original_water_read_scratch50la.h"
#include <cassert>
#include <cstdio>
#include <limits>

int main(){
    using S=rr::game::OriginalWaterReadScratch50LA;
    S s;
    assert(!s.word()&&!s.signedHalfword());
    assert(s.copyPrefix({})&&!s.word()&&!s.signedHalfword());
    const std::array<std::uint8_t,4> first{0,0x80,0x34,0x12};
    assert(s.copyPrefix(std::span(first).first(1))&&!s.signedHalfword());
    assert(s.byte(0)==0&&!s.byte(1));
    assert(s.copyPrefix(std::span(first).first(2))&&s.signedHalfword()==-32768&&!s.word());
    assert(s.copyPrefix(first)&&s.word()==0x12348000u);
    const std::array<std::uint8_t,1> low{0xff};
    assert(s.copyPrefix(low)&&s.signedHalfword()==-32513&&s.word()==0x123480ffu);
    const auto previous=s.word();
    assert(s.copyPrefix({})&&s.word()==previous); //EOF retains earlier iteration.
    const std::array<std::uint8_t,5> tooLong{1,2,3,4,5};
    assert(!s.copyPrefix(tooLong)&&s.word()==previous);
    assert(!s.byte(4)&&!s.byte(std::numeric_limits<std::size_t>::max()));
    //Every signed low-halfword value; upper stack bytes remain unknown.
    for(std::int32_t value=-32768;value<=32767;++value){
        const auto bits=std::uint32_t(value)&65535u;
        const std::array<std::uint8_t,2> encoded{std::uint8_t(bits),std::uint8_t(bits>>8u)};
        S word;assert(word.copyPrefix(encoded)&&word.signedHalfword()==value&&!word.word());
    }
    //A nested count must not acquire known upper bytes from its bookkeeping.
    S count;const std::array<std::uint8_t,4> n{7,0,0,0};
    assert(count.copyPrefix(std::span(n).first(3))&&!count.word());
    assert(count.copyPrefix(n)&&count.word()==7u);
    const std::array<std::uint8_t,1> next{2};
    assert(count.copyPrefix(next)&&count.word()==2u);
    std::puts("Water scratch: all65536 signed halves, unknown suffix, short-read reuse, EOF and bounds passed");
}
