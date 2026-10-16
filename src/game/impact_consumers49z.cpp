#include "impact_consumers49z.h"
#include <cfenv>
#include <cstring>

namespace rr::game {
namespace {
struct Math {
    bool valid = true;
    float read(float x) noexcept {
        std::uint32_t b; std::memcpy(&b, &x, 4);
        const auto e = b & 0x7f800000u;
        if (e == 0x7f800000u || (e == 0 && (b & 0x7fffffu))) valid = false;
        return valid ? x : 0.0f;
    }
    float add(float a, float b) noexcept { volatile float r = read(a) + read(b); return read(r); }
    float sub(float a, float b) noexcept { volatile float r = read(a) - read(b); return read(r); }
    float mul(float a, float b) noexcept { volatile float r = read(a) * read(b); return read(r); }
    float div(float a, float b) noexcept { volatile float r = read(a) / read(b); return read(r); }
};
bool profileBlocked(const ImpactConsumerInput49Z& i) noexcept {
    // 188EC0 is called with a1=1: profile34=2 returns zero; otherwise10FAC.
    return i.profile34 != 2 && i.profile10FAC != 0;
}
void damage(const ImpactConsumerInput49Z& i, ImpactConsumerState49Z& s,
    unsigned mask, float amount, Math& m) noexcept {
    // 221754..76C gate before the component-specific arithmetic.
    if (i.vehicle58 != 1 && (i.vehicle58 != 0 || profileBlocked(i))) return;
    const unsigned indices[] = {1, 0};
    const unsigned flags[] = {1, 0x10};
    for (unsigned n = 0; n != 2; ++n) {
        const auto index = indices[n];
        if (!(mask & flags[n]) || !(i.componentPresence & (1u << index))) continue;
        bool increment = !i.vehicle3C;
        if (!i.vehicle3C && s.partnerState) {
            if (!(m.read(s.damage7D0[index]) < m.read(i.finalDamageThreshold64)) &&
                m.read(amount) <= m.read(i.finalDamageIncrement68)) increment = false;
        }
        if (increment) {
            s.damage7D0[index] = m.add(s.damage7D0[index], amount);
            if (!s.partnerState) {
                auto& total = index ? s.environmentalD38 : s.environmentalD3C;
                total = m.add(total, amount);
            }
        }
        const float value = m.read(s.damage7D0[index]);
        s.damage7D0[index] = value > 0.0f ? value : 0.0f;
        s.cooldown7E0 = 0.5f; // Includes a gated increment:221818/2218C0.
    }
}
void clearDirectional(ImpactConsumerState49Z& s) noexcept {
    for (unsigned j = 0; j != 6; ++j) s.directionalD08[j] = 0.0f;
    s.fieldD00 = 0; s.fieldCFC = 0; s.fieldD04 = 0xffffffffu;
}
void periodic(const ImpactConsumerInput49Z& i, ImpactConsumerState49Z& s, Math& m) noexcept {
    if (m.read(s.periodic7E8) > 0.0f) { s.periodic7E8 = m.sub(s.periodic7E8, i.factor); return; }
    if (i.vehicle58 == 1 || (i.vehicle58 == 0 && !profileBlocked(i))) {
        const unsigned order[] = {1, 0, 3, 2};
        for (auto j : order) if (i.componentPresence & (1u << j)) {
            const auto value = m.add(s.damage7D0[j], 0.02f);
            s.damage7D0[j] = value > 0.0f ? value : 0.0f;
        }
    }
    s.periodic7E8 = 0.1f;
}
}
static ImpactConsumerStatus49Z consumeImpactsImpl50KZ(const ImpactConsumerInput49Z& i,
    const ImpactConsumerState49Z& state, ImpactConsumerState49Z& output,
    ImpactConsumerEvent49Z* events, std::size_t capacity, std::size_t& count,
    ImpactPartnerEvents49Z* partnerEvents,bool afterFeedback) noexcept {
    if (!afterFeedback&&(!(i.managerFlags58 & 2) || ((i.flagsCD8 & 1) && !i.vehicle3C))) {
        std::memcpy(&output, &state, sizeof state); count = 0;if(partnerEvents)*partnerEvents={};
        return ImpactConsumerStatus49Z::skipped;
    }
    if (std::fegetround() != FE_TONEAREST) return ImpactConsumerStatus49Z::arithmetic;
    ImpactConsumerState49Z s; std::memcpy(&s, &state, sizeof s);
    ImpactConsumerEvent49Z pending[2]{}; std::size_t used = 0; Math m;
    ImpactPartnerEvents49Z partners{};
    if(!afterFeedback){
    for (auto& timer : s.timers820) if (m.read(timer) > 0.0f) timer = m.sub(timer, i.factor);
    if (m.read(s.cooldown7E0) > 0.0f) s.cooldown7E0 = m.sub(s.cooldown7E0, i.factor);
    if (s.partnerState == 2) s.partnerDuration10 = m.add(s.partnerDuration10, i.factor);
    if (m.read(s.partnerTimer08) > 0.5f) {
        s.partnerDuration10 = 0; s.partnerState = 0; s.partnerAmount0C = 0;
        s.partnerId = 0xffffffffu; s.partnerTimer08 = 0;
    }
    if (s.partnerId != 0xffffffffu && m.read(s.partnerAmount0C) < 0.1f) {
        if(!i.dynamicPartnerEventsBound||!partnerEvents)
            return m.valid ? ImpactConsumerStatus49Z::unsupportedPartner : ImpactConsumerStatus49Z::arithmetic;
        partners.values[partners.count++]={0x222cfcu,s.partnerId,0u};
    }
    const float lateral = m.add(s.directionalD08[6], s.directionalD08[7]);
    if (lateral > 0.0f) {
        s.timers820[4] = 0.25f;
        if (!i.vehicle58 && !i.vehicle3C && (i.profileFlags11050 & 4)) {
            pending[used++] = {i.stopBeforeControllerFeedback50IW&&!i.player5CBound50IW?
                0x222d70u:0x222d8cu, 0x40, i.player5C, 2, 0, 0};
            if(i.stopBeforeControllerFeedback50IW){
                if(!m.valid)return ImpactConsumerStatus49Z::arithmetic;
                if(used>capacity||!events)return ImpactConsumerStatus49Z::capacity;
                std::memcpy(&output,&s,sizeof s);std::memcpy(events,pending,used*sizeof *events);
                count=used;if(partnerEvents)*partnerEvents=partners;
                return ImpactConsumerStatus49Z::pendingControllerFeedback;
            }
            const float scale = m.mul(0.005f, lateral);
            pending[used++] = {0x222dec, 0x44, i.player5C, 2, 0, scale < 0.5f ? scale : 0.5f};
        }
    }
    } // Original222DF4 continuation; prefix/controller helpers already returned.
    if (m.read(s.cooldown7E0) <= 0.0f) {
        if (!(i.flagsCD8 & 1) && !i.vehicle40) {
            // Frozen sum is consumed only by the excluded partner route; retain
            // source arithmetic/domain even though admitted calls cannot use it.
            const auto pair = m.add(s.directionalD08[0], s.directionalD08[1]);
            const auto three = m.add(s.directionalD08[4], pair);
            const float damageSum=m.add(s.directionalD08[5], three);
            const unsigned indices[] = {0, 1, 4, 5};
            const unsigned masks[] = {0x10, 1, 0x11, 0x11};
            for (unsigned n = 0; n != 4; ++n) {
                const auto value = m.read(s.directionalD08[indices[n]]);
                if (value > 0.1f) damage(i, s, masks[n], m.div(value, n == 3 ? 200.0f : 100.0f), m);
            }
            if(s.partnerId!=0xffffffffu&&m.read(s.partnerAmount0C)<0.1f){
                if(!i.dynamicPartnerEventsBound||!partnerEvents)
                    return ImpactConsumerStatus49Z::unsupportedPartner;
                if(!i.partnerEventThresholdBound)return ImpactConsumerStatus49Z::unsupportedDynamicTuning;
                if(!(damageSum<=m.read(i.partnerEventThreshold48)))
                    partners.values[partners.count++]={0x222accu,s.partnerId,1u};
            }
        }
        clearDirectional(s);
    }
    if (m.read(s.directionalD08[7]) > 0.1f) s.timers820[1] = m.add(s.timers820[1], 0.025f);
    if (m.read(s.directionalD08[6]) > 0.1f) s.timers820[0] = m.add(s.timers820[0], 0.025f);
    if (i.vehicle64 == 2 && !i.vehicle3C) periodic(i, s, m);
    if (s.partnerState) s.partnerTimer08 = m.add(s.partnerTimer08, i.factor);
    s.directionalD08[6] = 0; s.directionalD08[7] = 0;
    if (!m.valid) return ImpactConsumerStatus49Z::arithmetic;
    if (used > capacity || (used && !events)) return ImpactConsumerStatus49Z::capacity;
    std::memcpy(&output, &s, sizeof s);
    if (used) std::memcpy(events, pending, used * sizeof *events);
    count = used;
    if(partnerEvents)*partnerEvents=partners;
    return ImpactConsumerStatus49Z::applied;
}

ImpactConsumerStatus49Z consumeImpacts49Z(const ImpactConsumerInput49Z& i,
    const ImpactConsumerState49Z& state,ImpactConsumerState49Z& output,
    ImpactConsumerEvent49Z* events,std::size_t capacity,std::size_t& count,
    ImpactPartnerEvents49Z* partners) noexcept {
    return consumeImpactsImpl50KZ(i,state,output,events,capacity,count,partners,false);
}
ImpactConsumerStatus49Z resumeImpactsAfterControllerFeedback50KZ(
    const ImpactConsumerInput49Z& i,const ImpactConsumerState49Z& state,
    ImpactConsumerState49Z& output,ImpactPartnerEvents49Z* partners) noexcept {
    std::size_t count=0;
    return consumeImpactsImpl50KZ(i,state,output,nullptr,0,count,partners,true);
}
}
