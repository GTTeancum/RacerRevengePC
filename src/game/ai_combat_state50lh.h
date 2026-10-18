#pragma once
// Authenticated retail170480 and state1/state3 continuations.
#include "ai_state_prefix50iv.h"
#include "body_contact_list48r.h"
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace rr::game {
// Call-time borrow of authentic self340+238/+23C and target1008->340.
// The count is copied from root.callbackState238; it is NOT body20C. The
// identity slots are the same canonical list populated by1654C0 and cleared
// (count only) by1654B0. No +210 list, distance test, or hit boolean may stand in.
// A native target token must be resolved from the retained manager identity
// in1008 against the SAME stable body registry used by those slots. A reused
// serial execution-arena address is not a body identity.
struct AiCombatContactView50LH {
    std::uint32_t count238Bits=0;
    const BodyContactIdentity48R* slots23C=nullptr;
    std::size_t slotCount=0;
    bool targetBody340Bound=false;
    BodyContactIdentity48R targetBody340{};
};

// Original170480..1704D8. A completed false source return is different from a
// missing-owner boundary (native false). count<=0 returns before consuming
// target1008, slots23C, or profile1060. Positive count consumes slots in order
// and stops at the first identity match. Each reached slot is bounds checked;
// do not reject a later inaccessible slot before a source-earlier match.
// This leaf performs no FP arithmetic and copies1060 bits exactly on a hit.
// On native failure the cooldown/output remain unchanged; no source write has
// yet been reached. View/input/output storage must not alias one another.
inline bool checkOriginalAiAttackContact17048050LH(
    const AiCombatContactView50LH& view,const float& waitToRepairTime1060,
    float& cooldown1018,bool& sourceReturnedOne,AiPrefixDiagnostic50IV& d) noexcept {
    using namespace ai_prefix_detail50IV;d={};
    std::int32_t count;std::memcpy(&count,&view.count238Bits,sizeof count);
    if(count<=0){sourceReturnedOne=false;return true;}
    for(std::int32_t index=0;index<count;++index){
        const auto slot=static_cast<std::size_t>(index);
        //17049C loads target1008,1704A0 loads the list slot, then1704A4
        // dereferences target340. A null/unresolved target is NOT a miss.
        if(!view.slots23C||slot>=view.slotCount)
            return fail(d,AiPrefixError50IV::Input,0x1704a0u);
        const auto other=view.slots23C[slot];
        if(!view.targetBody340Bound)
            return fail(d,AiPrefixError50IV::Input,0x1704a4u);
        if(other.value==view.targetBody340.value){
            std::memcpy(&cooldown1018,&waitToRepairTime1060,sizeof cooldown1018); //1704BC
            sourceReturnedOne=true;return true;
        }
    }
    sourceReturnedOne=false;return true;
}

//16EBF4..16EC3C, called ONLY after170480 returned exactly1. Keep the source
// stores visible in order. A later multiply-domain failure does not roll back
// the already-completed target clear/state change/raw timer copy, nor the
// helper's earlier1018 write. The inactive native1008 index payload is kept.
inline bool completeOriginalAiAttackHit50LH(bool& target1008Present,
    std::uint32_t& aiStateFE8,float& timer100C,const float& attackRecoveryTime1068,
    std::int32_t placeCCC,std::uint32_t fieldFE4,AiPrefixDiagnostic50IV& d) noexcept {
    using namespace ai_prefix_detail50IV;d={};
    target1008Present=false;                                  //16EBF8
    aiStateFE8=3;                                             //16EBFC
    std::memcpy(&timer100C,&attackRecoveryTime1068,sizeof timer100C); //16EC04
    if(placeCCC<3||fieldFE4){                                  //16EC0C..16EC1C
        if(!nearest(d,0x16ec34u))return false;
        Math f;const float scaled=f.mul(timer100C,10.f);        //16EC34
        if(!f.valid)return fail(d,AiPrefixError50IV::Arithmetic,0x16ec34u);
        timer100C=scaled;                                     //16EC3C
    }
    return true;
}

//16EC4C..16EC68, called ONLY after the state1 invocation of1704E0 COMPLETED.
// Missing-owner/arithmetic boundaries from the scan must never reach this
// helper. Any nonzero scan return preserves state1 and its target/mode writes.
inline void completeOriginalAiState1Scan50LH(std::uint32_t sourceReturn,
    bool& target1008Present,float& field104C,std::uint32_t& aiStateFE8) noexcept {
    if(sourceReturn)return;                                  //16EC4C
    target1008Present=false;                                 //16EC54
    field104C=-1.f;                                         //16EC5C
    aiStateFE8=2;                                           //16EC68
}

//16EC6C..16EC9C. Deliberately preserve the original counterintuitive branch:
// old timer<=0 => store old-dt, remain3; old timer>0 => set2, preserve timer.
// SUB16EC84 executes as a delay slot on BOTH paths, even when discarded.
// Do not process this immediately after a state1 hit in the same16EB00 call:
// the source switch is entered only once. No clamp or conventional cooldown.
inline bool advanceOriginalAiState3Timer50LH(float preparedFactor,
    float& timer100C,std::uint32_t& aiStateFE8,AiPrefixDiagnostic50IV& d) noexcept {
    using namespace ai_prefix_detail50IV;if(!nearest(d,0x16ec6cu))return false;
    Math f;const float old=f.read(timer100C);
    if(!f.valid)return fail(d,AiPrefixError50IV::Arithmetic,0x16ec78u);
    const bool nonpositive=old<=0.f;                         //16EC78
    const float reduced=f.sub(old,preparedFactor);           //16EC84, always
    if(!f.valid)return fail(d,AiPrefixError50IV::Arithmetic,0x16ec84u);
    if(nonpositive)timer100C=reduced;                        //16EC9C
    else aiStateFE8=2;                                      //16EC90
    return true;
}
} // namespace rr::game
