#pragma once
#include "body_contact_list48r.h"
#include "static_impact49y.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace rr::game {

// All records below are bounded call projections of existing canonical owners.
// They must never become an alternative persistent D34/D28/B9D0/pose-history owner.
// Peer observations are freshly collected at this racer's serial impact point.
struct DynamicImpactPeer50IW {
    bool present=false;
    std::int32_t managerIndex=-1;
    BodyContactIdentity48R mainBody340{};
    std::uint32_t vehicle40=0,vehicle58=0,partnerStateD34=0;
    std::uint8_t flagsCD8=0;
    float smoothedChannel0D28=0,worldAngularVelocityA4=0,cachedSpeed34=0;
    bool liveInputsBound=false;
};
struct DynamicImpactManagerView50IW {
    const DynamicImpactPeer50IW* peers=nullptr; // indexed by source manager index
    std::size_t peerCount=0;
    std::int32_t total30=0,humanCount34=0;
    bool bound=false;
};
struct DynamicImpactState50IW {
    std::uint32_t vehicleCFC=0,vehicleD00=0,fieldD04=0xffffffffu;
    // BasePoseHistoryState50Y owns both words; D2C is stored as binary32 bits.
    std::uint32_t timerD2CBits=0,markerD30=0xffffffffu;
    std::uint32_t partnerId=0xffffffffu,partnerState=0;
    float partnerTimer08=0,partnerAmount0C=0,partnerDuration10=0;
    // Bit-only projection of the canonical frame owner; upper bits absent.
    bool flagB9D0=false;
};
struct DynamicImpactSelf50IW {
    std::uint32_t vehicle58=0;
    float rawB5C=0,smoothedChannel0D28=0,worldAngularVelocityA4=0;
    float cachedSpeed34=0,reductionCF0=0;
    float damage7D0=0,damage7D4=0,damage7DC=0;
    bool canonicalOwnersBound=false;
};
struct DynamicImpactTuning50IW {
    float thresholds28[5]{},points3C[5]{};
    float minGrindingSpeed54=0,aiPointsMultiplier58=0,aiMultiplierDecrement5C=0;
    bool bound=false; // authenticated selected-track PodDamage authoring
    float podGrindingPoints50=0; // Original C0+50; bound by the same authoring reader.
};
struct DynamicImpactProfile50IW {
    // Exact application2DC fields, not RR_RACE_MODE or a guessed mode predicate.
    std::uint32_t game34=0,game10FC4=0,game10FE0=0;
    std::uint8_t track1B1=0,game1125F=0;
    std::uint8_t record100C4[13]{};
    std::uint32_t record100E8[13]{};
    std::uint32_t managerB4Field50=0;
    bool bound=false; // game34/10FC4/10FE0 and track1B1 are real bound owners
    bool raceAdjustmentBound=false; // B4+50 and all thirteen race records
};
struct DynamicImpactInput50IW {
    DynamicImpactSelf50IW self{};
    BodyContactListState48R contacts{};
    DynamicImpactManagerView50IW manager338{},managerB0{};
    DynamicImpactTuning50IW tuning{};
    StaticImpactTuning49Y environmentTuning{};
    bool environmentTuningBound=false;
    DynamicImpactProfile50IW profile{};
    // Already computed by the original223030 scalar/VU prefix, in source order.
    float worldMagnitude=0,normalizedLocalX=0,normalizedLocalZ=0;
};
enum class DynamicImpactStatus50IW {
    completed,needsFirstContact22A400,unsupported,arithmetic
};
struct DynamicImpactResult50IW {
    DynamicImpactStatus50IW status=DynamicImpactStatus50IW::unsupported;
    std::uint32_t sourcePC=0;
    float severity=0; // meaningful only for completed
};

// 22A280 ->222F00 ->221F30/2218F0 ->2224F0, with source222770 fallback.
// Output may alias state. On unsupported/arithmetic it is unchanged. A
// needsFirstContact22A400 result commits the exact source prefix to output:
// contact classification and D34{id=D04,state=1,gap=+0}; amount/duration retain
// their prior bits. Commit that prefix once, execute the real shared RNG/sound
// continuation, and only then call resumeDynamicImpactAfter22A40050IW.
// There is deliberately no optional callback or boolean "sound succeeded".
// The resume entry must not be called merely because a request was enqueued.
// It may only follow a completed22A400 cursor for this same call, with no racer
// frame/interleaving between prefix/effects/tail. New unjoined effects stop at
// their original PC. This header neither consumes impact1C0 nor applies common
// 22313C maxima nor the later222B20/2228D0 damage/event consumers.
// Numerical domain: FE_TONEAREST, normal binary32/signed zero consumed operands
// and intermediates, no fast math or contraction, as static49Y. Copy-only state
// is not numerically validated. No claim of full EE extended-float fidelity.
inline DynamicImpactResult50IW beginDynamicImpact50IW(
    const DynamicImpactInput50IW&,const DynamicImpactState50IW&,
    DynamicImpactState50IW&) noexcept;
inline DynamicImpactResult50IW resumeDynamicImpactAfter22A40050IW(
    const DynamicImpactInput50IW&,const DynamicImpactState50IW&,
    DynamicImpactState50IW&) noexcept;

namespace dynamic_impact_detail50iw {
using Status=DynamicImpactStatus50IW;
inline DynamicImpactResult50IW fail(std::uint32_t pc) noexcept {
    return {Status::unsupported,pc,0.0f};
}
struct Math {
    std::uint32_t badPC=0;
    float read(float x,std::uint32_t pc) noexcept {
        std::uint32_t bits;std::memcpy(&bits,&x,sizeof(bits));
        if(!badPC&&(!std::isfinite(x)||
            ((bits&0x7f800000u)==0&&(bits&0x7fffffffu)!=0)))badPC=pc;
        return x;
    }
    float add(float x,float y,std::uint32_t pc) noexcept {
        read(x,pc);read(y,pc);volatile float z=x+y;return read(z,pc);
    }
    float sub(float x,float y,std::uint32_t pc) noexcept {
        read(x,pc);read(y,pc);volatile float z=x-y;return read(z,pc);
    }
    float mul(float x,float y,std::uint32_t pc) noexcept {
        read(x,pc);read(y,pc);volatile float z=x*y;return read(z,pc);
    }
    float div(float x,float y,std::uint32_t pc) noexcept {
        read(x,pc);read(y,pc);volatile float z=x/y;return read(z,pc);
    }
    float maximum(float x,float y,std::uint32_t pc) noexcept {
        read(x,pc);read(y,pc);return detail::scalarMax48O(x,y);
    }
    float signedWord(std::uint32_t x,std::uint32_t pc) noexcept {
        const std::int64_t value=x<=0x7fffffffu?std::int64_t(x):
            std::int64_t(x)-0x100000000ll;
        volatile float z=static_cast<float>(value);return read(z,pc);
    }
};
inline float fromBits(std::uint32_t word) noexcept {
    float result;std::memcpy(&result,&word,sizeof(result));return result;
}
inline DynamicImpactResult50IW arithmetic(const Math& f) noexcept {
    return {Status::arithmetic,f.badPC,0.0f};
}
inline bool managerShape(const DynamicImpactManagerView50IW& m) noexcept {
    return m.bound&&m.total30>=0&&m.humanCount34>=0&&
        m.humanCount34<=m.total30&&std::size_t(m.total30)<=m.peerCount&&
        (m.total30==0||m.peers!=nullptr);
}
// A null slot is a source null return only for222F00/221F30. The classifier
// dereferences every visited slot, so its corresponding null is unsupported.
inline const DynamicImpactPeer50IW* lookup(
    const DynamicImpactManagerView50IW& m,std::uint32_t index,
    std::uint32_t& failurePC,std::uint32_t callPC) noexcept {
    if(!managerShape(m)){failurePC=callPC;return nullptr;}
    if(index>=std::uint32_t(m.total30)){failurePC=0x1af458u;return nullptr;}
    const auto& p=m.peers[index];
    if(p.managerIndex!=std::int32_t(index)){failurePC=callPC;return nullptr;}
    return p.present?&p:nullptr;
}
inline DynamicImpactResult50IW identifyContacts(
    const DynamicImpactInput50IW& in,DynamicImpactState50IW& s) noexcept {
    if(in.contacts.count20C<=0||in.contacts.count20C>
        std::int32_t(BodyContactCapacity48R))return fail(0x223100u);
    if(!managerShape(in.manager338))return fail(0x22a2c4u);
    bool matched=false;
    for(std::int32_t contact=0;contact<in.contacts.count20C;++contact){
        // Original order: AI manager range, then human manager range; all
        // matches write, and the final matching write owns D04/D2C/D30.
        for(unsigned range=0;range<2;++range){
            const std::int32_t first=range==0?in.manager338.humanCount34:0;
            const std::int32_t end=range==0?in.manager338.total30:
                in.manager338.humanCount34;
            for(std::int32_t index=first;index<end;++index){
                const auto& p=in.manager338.peers[std::size_t(index)];
                if(!p.present||p.managerIndex!=index)
                    return fail(range==0?0x22a2d8u:0x22a34cu);
                if(p.vehicle40!=0||(p.flagsCD8&1u)||
                    p.mainBody340.value!=in.contacts.slots210[
                        std::size_t(contact)].value)continue;
                matched=true;
                if(range==0){
                    s.vehicleD00=1;s.fieldD04=std::uint32_t(index);
                    s.timerD2CBits=0;s.markerD30=0xffffffffu;
                }else{
                    s.vehicleCFC=1;s.fieldD04=std::uint32_t(index);
                    s.timerD2CBits=0x40000000u;s.markerD30=std::uint32_t(index);
                }
            }
        }
    }
    if(!matched)s.fieldD04=0xffffffffu;
    return {Status::completed,0x22a3f0u,0.0f};
}

struct Classification { bool value;float ratio; };
inline Classification classify2218F0(float u,float v,bool positive,
                                     Math& f) noexcept {
    f.read(u,positive?0x22190cu:0x221c1cu);
    const bool firstGroup=positive?u<=0.0f:u>=0.0f;
    f.read(v,positive?(firstGroup?0x22191cu:0x221a9cu):
        (firstGroup?0x221c2cu:0x221dacu));
    const bool secondGroup=positive?v<=0.0f:v>=0.0f;
    const float a=std::fabs(u),b=std::fabs(v);
    if(firstGroup&&secondGroup)return {true,a<=b?1.0f:0.75f};
    if(firstGroup&&!secondGroup)return {a>=b,0.5f};
    if(!firstGroup&&secondGroup)return {a<=b,0.0f};
    return {false,a>=b?1.0f:0.75f};
}
inline float damageMaximum(const DynamicImpactSelf50IW& self,Math& f) noexcept {
    const float a=f.read(self.damage7D0,0x2234e0u);
    const float b=f.read(self.damage7D4,0x2234e4u);
    const float pair=f.maximum(b,a,0x2234f0u);
    const float c=f.read(self.damage7DC,0x2234fcu);
    const float other=f.maximum(c,c,0x223508u);
    return f.maximum(pair,other,0x22351cu);
}
inline DynamicImpactResult50IW partnerAmount(
    const DynamicImpactInput50IW& in,DynamicImpactState50IW& s,Math& f) noexcept {
    if(s.partnerId==0xffffffffu)return {Status::completed,0x221f64u,0.0f};
    std::uint32_t failurePC=0;
    const auto* other=lookup(in.managerB0,s.fieldD04,failurePC,0x221f7cu);
    if(failurePC)return fail(failurePC);
    if(!other)return {Status::completed,0x221f90u,0.0f};
    if(!other->liveInputsBound||other->vehicle58>1u)return fail(0x221f98u);
    if(in.self.vehicle58==1&&other->vehicle58==1){
        const float maximum=damageMaximum(in.self,f);
        // Source311038 conversion is exact in this admitted binary32 domain;
        // 2EAEA0 compares to binary64 0x3feccccccccccccd, rather than float .9.
        const std::uint64_t thresholdBits=0x3feccccccccccccdull;
        double threshold;std::memcpy(&threshold,&thresholdBits,sizeof(threshold));
        s.partnerAmount0C=static_cast<double>(maximum)>threshold?1.0f:0.75f;
        return {Status::completed,0x222008u,0.0f};
    }
    const float x=f.read(in.normalizedLocalX,0x222010u);
    const float z=f.read(in.normalizedLocalZ,0x222024u);
    float candidate=0.0f;
    if(std::fabs(x)<std::fabs(z)){
        if(z<0.0f&&(other->vehicle58==1||in.self.vehicle58==1))candidate=1.0f;
    }else{
        const bool positive=x>0.0f;
        if(s.partnerState==3){
            const float raw=f.read(in.self.rawB5C,positive?0x222080u:0x2220acu);
            if((positive&&raw<fromBits(0xbf19999au))||
               (!positive&&raw>fromBits(0x3f19999au)))s.partnerState=2;
            else{s.partnerAmount0C=1.0f;return {Status::completed,0x2220ecu,0.0f};}
        }else if(in.self.vehicle58==0){
            const float raw=f.read(in.self.rawB5C,positive?0x222108u:0x222134u);
            if((positive&&raw>fromBits(0x3f19999au))||
               (!positive&&raw<fromBits(0xbf19999au))){
                s.partnerState=3;return {Status::completed,0x222164u,0.0f};
            }
        }
        if(in.self.vehicle58==1&&other->vehicle58==0&&
            s.partnerId!=0xffffffffu&&other->partnerStateD34==3){
            s.partnerAmount0C=0.0f;return {Status::completed,0x2221b0u,0.0f};
        }
        Classification a=classify2218F0(in.self.smoothedChannel0D28,
            other->smoothedChannel0D28,positive,f);
        Classification b=classify2218F0(in.self.worldAngularVelocityA4,
            other->worldAngularVelocityA4,positive,f);
        if(a.value&&b.value){
            const float sum=f.add(a.ratio,b.ratio,0x222220u);
            const float average=f.div(sum,2.0f,0x22222cu);
            const float centered=f.sub(average,0.0f,0x222230u);
            const float scaled=f.mul(centered,0.25f,0x222234u);
            const float portion=f.div(scaled,1.0f,0x222244u);
            candidate=f.add(0.75f,portion,0x22224cu);
            if(candidate>1.0f)candidate=1.0f;
            else if(candidate<0.75f)candidate=0.75f;
        }else if(!a.value&&!b.value){
            const float sum=f.add(a.ratio,b.ratio,0x2222b0u);
            const float average=f.div(sum,2.0f,0x2222bcu);
            const float centered=f.sub(average,0.0f,0x2222c0u);
            const float scaled=f.mul(centered,-0.25f,0x2222c4u);
            const float portion=f.div(scaled,1.0f,0x2222d4u);
            candidate=f.add(0.25f,portion,0x2222dcu);
            if(candidate<0.0f)candidate=0.0f;
            else if(candidate>0.25f)candidate=0.25f;
        }else{
            if(a.value&&s.partnerState==2&&
                f.read(s.partnerDuration10,0x222330u)>0.5f){
                a.ratio=1.0f;b.ratio=0.0f;
            }
            const float difference=a.value?
                f.sub(a.ratio,b.ratio,0x22236cu):
                f.sub(b.ratio,a.ratio,0x222378u);
            const float centered=f.sub(difference,-1.0f,0x222388u);
            const float scaled=f.mul(centered,0.5f,0x2223a4u);
            const float portion=f.div(scaled,2.0f,0x2223a8u);
            candidate=f.add(0.25f,portion,0x2223b8u);
            if(candidate>0.75f)candidate=0.75f;
            else if(candidate<0.25f)candidate=0.25f;
        }
    }
    if(s.partnerState==1)s.partnerAmount0C=candidate;
    else{
        // EE scalar MULA then MADD; each admitted native product and add is
        // rounded separately, matching the existing ordinary49Y contract.
        const float oldPart=f.mul(0.75f,s.partnerAmount0C,0x22245cu);
        const float newPart=f.mul(0.25f,candidate,0x222460u);
        s.partnerAmount0C=f.add(oldPart,newPart,0x222460u);
    }
    if(in.self.vehicle58==0&&other->vehicle58==0){
        if(f.read(in.self.cachedSpeed34,0x22248cu)<50.0f)s.partnerAmount0C=0.0f;
        else if(f.read(other->cachedSpeed34,0x2224b0u)<50.0f)s.partnerAmount0C=1.0f;
    }
    return {Status::completed,0x2224d0u,0.0f};
}

inline DynamicImpactResult50IW environmentSeverity(
    const DynamicImpactInput50IW& in,const DynamicImpactState50IW& s,
    Math& f) noexcept {
    if(!in.environmentTuningBound)return fail(0x222798u);
    float chosen=0.0f;unsigned match=5;
    constexpr std::uint32_t comparePC[5]={0x22279cu,0x2227b8u,0x2227d4u,
                                        0x2227f0u,0x22280cu};
    constexpr std::uint32_t pointsPC[5]={0x2227b0u,0x2227ccu,0x2227e8u,
                                       0x222804u,0x22281cu};
    for(unsigned i=0;i<5;++i){
        const float threshold=f.read(in.environmentTuning.thresholds00[i],comparePC[i]);
        if(!(f.read(in.worldMagnitude,comparePC[i])<threshold)){
            chosen=f.read(in.environmentTuning.severities14[i],pointsPC[i]);
            match=i;break;
        }
    }
    if(in.self.vehicle58==1&&match==4&&s.partnerState!=0){
        if(damageMaximum(in.self,f)>fromBits(0x3f666666u))chosen=0.0f;
    }
    if(in.self.vehicle58==1&&s.vehicleCFC==0){
        if(!in.environmentTuning.aiEnvironmentMultiplierBound)
            return f.badPC?arithmetic(f):fail(0x222898u);
        chosen=f.mul(chosen,in.environmentTuning.aiEnvironmentMultiplier60,0x22289cu);
    }
    const float defense=f.sub(1.0f,in.self.reductionCF0,0x2228b0u);
    const float severity=f.mul(chosen,defense,0x2228b8u);
    return f.badPC?arithmetic(f):DynamicImpactResult50IW{
        Status::completed,0x222ff4u,severity};
}

inline DynamicImpactResult50IW podSeverity(
    const DynamicImpactInput50IW& in,DynamicImpactState50IW& s,Math& f) noexcept {
    if(!in.tuning.bound)return fail(0x222528u);
    float chosen=0.0f;
    constexpr std::uint32_t comparePC[5]={0x222530u,0x22254cu,0x222568u,
                                        0x222584u,0x2225a0u};
    constexpr std::uint32_t pointsPC[5]={0x222544u,0x222560u,0x22257cu,
                                       0x222598u,0x2225dcu};
    for(unsigned i=0;i<5;++i){
        const float threshold=f.read(in.tuning.thresholds28[i],comparePC[i]);
        if(!(f.read(in.worldMagnitude,comparePC[i])<threshold)){
            if(i<4)chosen=f.read(in.tuning.points3C[i],pointsPC[i]);
            else{
                const float minimum=f.read(in.tuning.minGrindingSpeed54,0x2225c0u);
                const float speed=f.mul(fromBits(0x3f2e8bc1u),
                    in.self.cachedSpeed34,0x2225c8u);
                if(speed>minimum)chosen=f.read(in.tuning.points3C[4],0x2225dcu);
            }
            break;
        }
    }
    if(in.self.vehicle58==1&&s.vehicleCFC!=0){
        float multiplier=f.read(in.tuning.aiPointsMultiplier58,0x222600u);
        if(!in.profile.bound)return f.badPC?arithmetic(f):fail(0x222608u);
        const bool modeOverride=in.profile.game34!=2&&in.profile.game10FC4!=0;
        if(modeOverride)multiplier=1.5f;
        if(in.profile.track1B1&2u){
            if(!in.profile.raceAdjustmentBound)return f.badPC?arithmetic(f):fail(0x222648u);
            if(!managerShape(in.managerB0))return f.badPC?arithmetic(f):fail(0x222650u);
            unsigned record=0;
            for(unsigned i=0;i<13;++i){
                if(in.profile.record100C4[i]==(in.profile.game1125F&15u)){
                    record=i;break;
                }
            }
            // SUBU, SUBU, ADDIU define modulo2^32 before the signed BLEZ.
            std::uint32_t count=std::uint32_t(in.managerB0.total30)-
                in.profile.managerB4Field50;
            count=count-in.profile.record100E8[record]+std::uint32_t(1);
            if(count!=0&&(count&0x80000000u)==0){
                const float n=f.signedWord(count,modeOverride?0x2226b0u:0x2226dcu);
                float decrement;
                if(modeOverride)decrement=f.mul(fromBits(0x3dcccccdu),n,0x2226c0u);
                else decrement=f.mul(n,in.tuning.aiMultiplierDecrement5C,0x2226ecu);
                multiplier=f.sub(multiplier,decrement,modeOverride?0x2226c8u:0x2226f0u);
            }
        }
        chosen=f.mul(chosen,multiplier,0x2226fcu);
        if(in.profile.game34!=2&&in.profile.game10FE0!=0)
            s.flagB9D0=true; //222714..222728 sets only source B9D bit0.
    }
    const float remainingShare=f.sub(1.0f,s.partnerAmount0C,0x22273cu);
    const float remainingDefense=f.sub(1.0f,in.self.reductionCF0,0x22274cu);
    const float scaled=f.mul(chosen,remainingDefense,0x222754u);
    const float severity=f.mul(remainingShare,scaled,0x22275cu);
    return f.badPC?arithmetic(f):DynamicImpactResult50IW{
        Status::completed,0x22301cu,severity};
}
inline DynamicImpactResult50IW tail(
    const DynamicImpactInput50IW& in,DynamicImpactState50IW& s) noexcept {
    Math f;
    const auto amount=partnerAmount(in,s,f);
    if(f.badPC)return arithmetic(f);
    if(amount.status!=Status::completed)return amount;
    return podSeverity(in,s,f);
}
} // namespace dynamic_impact_detail50iw

inline DynamicImpactResult50IW beginDynamicImpact50IW(
    const DynamicImpactInput50IW& in,const DynamicImpactState50IW& state,
    DynamicImpactState50IW& out) noexcept {
    using namespace dynamic_impact_detail50iw;
    if(!in.self.canonicalOwnersBound||in.self.vehicle58>1u)return fail(0x222f00u);
    if(std::fegetround()!=FE_TONEAREST)return {Status::arithmetic,0x222f00u,0.0f};
    DynamicImpactState50IW s=state;
    const auto identification=identifyContacts(in,s);
    if(identification.status!=Status::completed)return identification;
    if(s.fieldD04!=0xffffffffu){
        std::uint32_t failurePC=0;
        const auto* other=lookup(in.managerB0,s.fieldD04,failurePC,0x222f44u);
        if(failurePC)return fail(failurePC);
        if(!other){out=s;return {Status::completed,0x222f54u,0.0f};}
    }
    if(s.partnerState==0){
        s.partnerId=s.fieldD04;s.partnerState=1;s.partnerTimer08=0.0f;
        if(in.self.vehicle58==0){
            out=s;return {Status::needsFirstContact22A400,0x222f98u,0.0f};
        }
    }else{
        if(s.partnerId!=s.fieldD04){
            Math f;const auto result=environmentSeverity(in,s,f);
            if(result.status==Status::completed)out=s;
            return result;
        }
        s.partnerTimer08=0.0f;
        if(s.partnerState==1){s.partnerState=2;s.partnerDuration10=0.0f;}
    }
    const auto result=tail(in,s);
    if(result.status==Status::completed)out=s;
    return result;
}

inline DynamicImpactResult50IW resumeDynamicImpactAfter22A40050IW(
    const DynamicImpactInput50IW& in,const DynamicImpactState50IW& prefix,
    DynamicImpactState50IW& out) noexcept {
    using namespace dynamic_impact_detail50iw;
    if(!in.self.canonicalOwnersBound||in.self.vehicle58!=0||
        prefix.partnerState!=1||prefix.partnerId!=prefix.fieldD04||
        prefix.partnerTimer08!=0.0f)return fail(0x222fa0u);
    if(std::fegetround()!=FE_TONEAREST)return {Status::arithmetic,0x222ffcu,0.0f};
    DynamicImpactState50IW s=prefix;
    const auto result=tail(in,s);
    if(result.status==Status::completed)out=s;
    return result;
}

} // namespace rr::game
