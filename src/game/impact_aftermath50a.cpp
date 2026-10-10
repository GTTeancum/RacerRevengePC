#include "impact_aftermath50a.h"
#include <cfenv>
#include <cstring>
namespace rr::game {
namespace {
struct Math {
    bool valid=true;
    float read(float x) noexcept {
        std::uint32_t b;std::memcpy(&b,&x,4);const auto e=b&0x7f800000u;
        if (e==0x7f800000u || (!e && (b&0x7fffffu))) valid=false;
        return valid?x:0.0f;
    }
    float sub(float a,float b) noexcept {volatile float r=read(a)-read(b);return read(r);}
    float mul(float a,float b) noexcept {volatile float r=read(a)*read(b);return read(r);}
    float div(float a,float b) noexcept {volatile float r=read(a)/read(b);return read(r);}
    float abs(float a) noexcept {a=read(a);std::uint32_t b;std::memcpy(&b,&a,4);b&=0x7fffffffu;std::memcpy(&a,&b,4);return a;}
    float clamp(float a) noexcept {a=read(a);return a<=0.0f?0.0f:(a>=1.0f?1.0f:a);}
};
void pair(float& left,float& right,float budget,float difference,
    bool secondary,float mainLeft,Math& m) noexcept {
    if (budget<=difference) {
        if (m.read(left)<=m.read(right)) right=m.sub(right,budget);
        else left=m.sub(left,budget);
    } else {
        if (m.read(left)>m.read(right)) {budget=m.sub(budget,difference);left=right;}
        else if (m.read(right)>m.read(left)) {
            budget=m.sub(budget,difference);
            right=secondary?m.read(mainLeft):left; // Original224660/668, not a typo.
        }
        if (m.read(right)==m.read(left)) {
            const auto half=m.div(budget,2.0f);
            right=m.sub(right,half);left=m.sub(left,half);
        }
    }
    left=m.clamp(left);right=m.clamp(right);
}
}
ImpactAftermathStatus50A repairImpactDamage50A(const ImpactRepairInput50A& i,
    const ImpactRepairState50A& state,ImpactRepairState50A& output) noexcept {
    if (!(i.flagsB9C&2)) {std::memcpy(&output,&state,sizeof state);return ImpactAftermathStatus50A::skipped;}
    if (std::fegetround()!=FE_TONEAREST) return ImpactAftermathStatus50A::arithmetic;
    Math m;ImpactRepairState50A s;std::memcpy(&s,&state,sizeof s);
    if (m.read(s.timer7E4)>0.0f) s.timer7E4=m.sub(s.timer7E4,i.factor);
    else {
        s.timer7E4=0.1f;
        if ((i.componentPresence&3)==3) {
            const float oldLeft=m.read(s.damage7D0[1]),oldRight=m.read(s.damage7D0[0]);
            // Difference subtract precedes budget multiply; each scalar rounded.
            const auto delta=m.sub(oldLeft,oldRight);
            const auto budget=m.mul(2.0f,i.repairCF4);
            const auto difference=m.abs(delta);
            pair(s.damage7D0[1],s.damage7D0[0],budget,difference,false,0.0f,m);
            s.environmentalD38=m.sub(s.environmentalD38,m.sub(oldLeft,s.damage7D0[1]));
            s.environmentalD3C=m.sub(s.environmentalD3C,m.sub(oldRight,s.damage7D0[0]));
            s.environmentalD38=m.clamp(s.environmentalD38);
            s.environmentalD3C=m.clamp(s.environmentalD3C);
            if ((i.componentPresence&12)==12) {
                const auto secondDelta=m.sub(s.damage7D0[3],s.damage7D0[2]);
                const auto secondBudget=m.mul(2.0f,i.repairCF4);
                const auto secondDifference=m.abs(secondDelta);
                pair(s.damage7D0[3],s.damage7D0[2],secondBudget,secondDifference,true,s.damage7D0[1],m);
            }
        }
    }
    if (!m.valid) return ImpactAftermathStatus50A::arithmetic;
    std::memcpy(&output,&s,sizeof s);return ImpactAftermathStatus50A::applied;
}
}
