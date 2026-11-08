#pragma once
// UNACTIVATED native copied-input resource kernel. Not retail282CD0: no
// 3E1A70 existence/open/read/close service,443240 assertion or loader receipt.
// Storage/math donors are reused, never the V2 child/factory or its identity.
#include "original_ta_cut_child_candidate.h"
#include <cfloat>
#if defined(__linux__) && defined(__GLIBC__) && defined(__x86_64__) && defined(__SSE2__)
#include <xmmintrin.h>
#include <fpu_control.h>
#elif defined(_MSC_VER) && defined(_M_X64)
#include <xmmintrin.h>
#include <cfenv>
#include <limits>
#endif

namespace rr::game::original_ta_cut_resource_candidate_detail {
using Arrays=original_ta_cut_storage_candidate_detail::Arrays;
using V3=OriginalTaCutVector3Candidate;
inline bool arithmeticAdmitted()noexcept {
#if defined(__linux__) && defined(__GLIBC__) && defined(__x86_64__) && defined(__SSE2__) && FLT_EVAL_METHOD == 0
    if(sizeof(float)!=4||!std::numeric_limits<float>::is_iec559||
       std::numeric_limits<float>::radix!=2||std::numeric_limits<float>::digits!=24||
       std::numeric_limits<float>::max_exponent!=128||std::fegetround()!=FE_TONEAREST)return false;
    const auto csr=static_cast<std::uint32_t>(_mm_getcsr());
    fpu_control_t cw;_FPU_GETCW(cw);
    // Read-only actual state: no rounding setter, sticky-flag clear or floating
    // underflow probe. SSE nearest, FTZ/DAZ off, all exception masks set; x87
    // nearest/masked too, covering a possible libm use. No EE/VU parity claim.
    return (csr&0x0000e040u)==0&&(csr&0x00001f80u)==0x00001f80u&&
        (static_cast<unsigned>(cw)&0x0c00u)==0&&(static_cast<unsigned>(cw)&0x003fu)==0x003fu;
#elif defined(_MSC_VER) && defined(_M_X64)
    // Windows x64: every float operation is SSE (no x87 evaluation), so the
    // read-only MXCSR check is complete. Nearest and all exceptions masked;
    // FTZ/DAZ may be on: the game thread runs the PS2 no-subnormal rule.
    if(sizeof(float)!=4||!std::numeric_limits<float>::is_iec559||
       std::numeric_limits<float>::radix!=2||std::numeric_limits<float>::digits!=24||
       std::numeric_limits<float>::max_exponent!=128||std::fegetround()!=FE_TONEAREST)return false;
    const auto csr=static_cast<std::uint32_t>(_mm_getcsr());
    return (csr&0x00006000u)==0&&(csr&0x00001f80u)==0x00001f80u;
#else
    return false; // No unchecked portable/caller FP-mode promise.
#endif
}
inline std::unique_ptr<Arrays> reserveTa01()noexcept {
    // Explicit NATIVE failure-safe allocation policy, not retail allocator/OOM
    // behavior. No child field is touched until all seven buffers exist.
    try {
        auto next=std::unique_ptr<Arrays>(new Arrays);
        next->frameHeaders5C.resize(461);next->nodeIdentifiers58.resize(461);
        next->nodes60.resize(461);next->look20.resize(461);next->up24.resize(461);
        next->position28.resize(461);next->scalar50.resize(461);
        return next;
    }catch(...){return {};}
}
inline bool ta01Reservation(const Arrays& a)noexcept {
    return a.frameHeaders5C.size()==461&&a.nodeIdentifiers58.size()==461&&
        a.nodes60.size()==461&&a.look20.size()==461&&a.up24.size()==461&&
        a.position28.size()==461&&a.scalar50.size()==461;
}
inline void vectorConstructor11C090(std::vector<V3>& a)noexcept {
    // Actual2E8990 element constructor order before publication20/24/28.
    for(auto& v:a){v[2]=0.f;v[1]=0.f;v[0]=0.f;}
}
inline std::uint32_t deriveFrameTa01(const assets::OriginalTaCutFrameBitsCandidate& raw,
    const std::array<std::uint32_t,4>& loadOffset,Arrays& a,std::uint32_t frame)noexcept {
    using namespace original_ta_cut_numeric_candidate_detail;
    // Raw auxiliary writes precede all numeric derivation. Original name tails
    // and all ten original float words are retained without normalization.
    a.frameHeaders5C[frame]=raw.frameWords;
    a.nodeIdentifiers58[frame]=raw.nodeWord;
    a.nodes60[frame].originalNameBytes=raw.nameBytes;
    a.nodes60[frame].originalPayloadWords=raw.payloadWords;
    Math m;V3 look{},up{},position{};
    const std::uint32_t lookPC[3]={0x2831a8,0x2831c0,0x2831d4};
    const std::uint32_t upPC[3]={0x2831e8,0x2831f0,0x2831f8};
    const std::uint32_t positionPC[3]={0x283234,0x283238,0x283240};
    for(unsigned k=0;k<3;++k)look[k]=m.read(value(raw.payloadWords[k]),lookPC[k]);
    for(unsigned k=0;k<3;++k)up[k]=m.read(value(raw.payloadWords[k+3]),upPC[k]);
    for(unsigned k=0;k<3;++k)
        position[k]=m.add(value(raw.payloadWords[k+6]),value(loadOffset[k]),positionPC[k]);
    const float scalar=m.read(value(raw.payloadWords[9]),0x283264);
    a.scalar50[frame]=scalar; //28326C BEFORE corrected-basis arithmetic.
    if(m.badPC)return m.badPC; //Native rejection retains reached raw/scalar writes.
    const auto right0=cross(up,look,m,0x2832b8,0x2832bc);
    const auto correctedUp=cross(look,right0,m,0x2832cc,0x2832d0);
    const auto normalUp=normalize(correctedUp,m,0x2832e8,0x2832ec,0x2832f0,0x2832f4,0x283304);
    const auto normalLook=normalize(look,m,0x283308,0x28330c,0x283310,0x283314,0x283324);
    const auto right=cross(normalUp,normalLook,m,0x283328,0x28332c);
    (void)right;
    if(m.badPC)return m.badPC;
    //283378..2833F8, no update/activation and no whole-row rollback.
    for(unsigned k=0;k<3;++k)a.look20[frame][k]=normalLook[k];
    for(unsigned k=0;k<3;++k)a.up24[frame][k]=normalUp[k];
    for(unsigned k=0;k<3;++k)a.position28[frame][k]=position[k];
    return 0;
}
} // namespace rr::game::original_ta_cut_resource_candidate_detail
