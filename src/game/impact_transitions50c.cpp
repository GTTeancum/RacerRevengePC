#include "impact_transitions50c.h"
#include <cfenv>
#include <cstring>
namespace rr::game {
namespace {
float word(std::uint32_t b) noexcept { float f;std::memcpy(&f,&b,4);return f; }
struct Read {
    std::uint32_t invalidPC=0;
    float scalar(const float& f,std::uint32_t pc) noexcept {
        std::uint32_t b;std::memcpy(&b,&f,4);const auto e=b&0x7f800000u;
        if(e==0x7f800000u||(!e&&(b&0x7fffffu))){if(!invalidPC)invalidPC=pc;return 0.0f;}
        return word(b);
    }
    bool below(const float& f,float limit,std::uint32_t pc) noexcept {return scalar(f,pc)<limit;}
};
}
ImpactTransitionResult50C inspectImpactTransition50C(const ImpactTransitionInput50C& i) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return {ImpactTransitionStatus50C::arithmetic,0x225c50,0,0};
    Read r;const float high=word(0x3f7c28f6),low=word(0x3f666666);
    auto selected=[&](std::uint32_t pc,std::uint32_t slot,std::uint32_t target) noexcept {
        if(r.invalidPC)return ImpactTransitionResult50C{ImpactTransitionStatus50C::arithmetic,r.invalidPC,0,0};
        return ImpactTransitionResult50C{ImpactTransitionStatus50C::unsupportedTransition,pc,slot,target};
    };
    bool force=false;
    if(!r.below(i.damage7D0[1],high,0x225c80)||!r.below(i.damage7D0[3],high,0x225c94)||
       !r.below(i.damage7D0[0],high,0x225ca8)||!r.below(i.damage7D0[2],high,0x225cbc)) {
        const float impact=r.scalar(i.impactBA8,0x225cd4);
        std::uint32_t b;std::memcpy(&b,&impact,4);b&=0x7fffffffu;
        force=!(word(b)<=r.scalar(i.threshold50,0x225ce4));
    }
    if(force||(i.flagsB9D&1))return selected(0x225d10,0x88,0x2261b0);
    if(!r.below(i.damage7D0[1],high,0x225d34)||!r.below(i.damage7D0[3],high,0x225d48)) {
        if(!r.below(i.damage7D0[0],low,0x225d6c)||!r.below(i.damage7D0[2],low,0x225d80))
            return selected(0x225d9c,0x8c,0x226750);
    }
    if(!r.below(i.damage7D0[0],high,0x225dc0)||!r.below(i.damage7D0[2],high,0x225dd4)) {
        if(!r.below(i.damage7D0[1],low,0x225df8)||!r.below(i.damage7D0[3],low,0x225e0c))
            return selected(0x225e28,0x8c,0x226750);
    }
    if(!r.below(i.damage7D0[1],high,0x225e4c)||!r.below(i.damage7D0[3],high,0x225e60))
        return selected(0x225e7c,0x90,0x227370);
    if(!r.below(i.damage7D0[0],high,0x225e90)||!r.below(i.damage7D0[2],high,0x225ea4))
        return selected(0x225ec0,0x94,0x227a40);
    if(r.invalidPC)return {ImpactTransitionStatus50C::arithmetic,r.invalidPC,0,0};
    return {ImpactTransitionStatus50C::noTransition,0x2261a4,0,0};
}
}
