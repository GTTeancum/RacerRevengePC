#pragma once
#include "body_integration48c.h"
#include <cfenv>
#include <cmath>
#include <cstdint>

namespace rr::game {
enum class OriginalWreckContactStatus50LA : std::uint8_t {
    complete,bodyEnableRequired,roundingMode,input,arithmetic
};
struct OriginalWreckContactResult50LA {
    OriginalWreckContactStatus50LA status=OriginalWreckContactStatus50LA::input;
    std::uint32_t sourcePC=0;
    bool velocityWritten=false;
};

// Actual2CB420 callback, including the165920 prefix's genuine piece30=0
// no-call branch. The source2CA480 attached-piece route sets that field to0;
// neither this callback nor an arbitrary "already enabled" flag may bypass
// the first165948 call when piece30 is nonzero.
//
// Inputs borrow the callback receiver's ACTUAL E4+9C piece and E4+A0 rigid
// body: piece30, that body's enabled204, canonical body state and70W, plus
// piece.DC+C0. otherBody is the other collision object's actual+A0, including
// genuine null for no body. No copied contact list, identity lookup, synthetic
// collision or callback dispatch is performed here.
//
// A nonzero piece30 returns at165948 BEFORE any write: genuine1653C0(1)
// propagates activation through linked bodies, and the subsequent actual
// piece virtual20 performs its component activation. A caller must implement
// both owners before admitting that route; retrying with piece30 forced0 is
// invalid. This leaf does not claim a completed wake-up or activation receipt.
//
// On the admitted no-wake route, enabled204 or null otherBody returns without
// consuming restitution or velocity. Otherwise source2CB464..2CB498 stores
// (1+DC.C0)*otherBody70 XYZ in receiver70 and writesW=1. It does NOT enable
// the receiver, overwrite angular velocity or alter any query/body histories.
// Separate binary32 operations, FE_TONEAREST, finite normal/signed-zero
// consumed inputs/results. Failure preserves all canonical state. Receiver
// and its70W must be disjoint from input backing; no heap or hidden storage.
inline OriginalWreckContactResult50LA respondOriginalWreckContact50LA(
    std::uint32_t actualPiece30,bool actualBodyEnabled204,
    BodyState48C& receiverBody,float& receiver70W,
    const BodyState48C* otherBody,const float& actualRestitutionDC_C0) noexcept {
    using S=OriginalWreckContactStatus50LA;
    if(actualPiece30)return {S::bodyEnableRequired,0x165948u,false};
    if(actualBodyEnabled204||!otherBody)return {S::complete,0x2cb49cu,false};
    if(std::fegetround()!=FE_TONEAREST)return {S::roundingMode,0x2cb46cu,false};
    const auto admitted=[](float value) noexcept {
        return std::isfinite(value)&&(value==0.f||std::isnormal(value));
    };
    if(!admitted(actualRestitutionDC_C0))return {S::input,0x2cb468u,false};
    volatile float scale=1.f+actualRestitutionDC_C0;
    if(!admitted(scale))return {S::arithmetic,0x2cb46cu,false};
    constexpr std::uint32_t loadPC[3]={0x2cb464u,0x2cb478u,0x2cb484u};
    constexpr std::uint32_t multiplyPC[3]={0x2cb470u,0x2cb47cu,0x2cb488u};
    float velocity[3];
    for(unsigned axis=0;axis<3;++axis){
        if(!admitted(otherBody->velocity.v[axis]))return {S::input,loadPC[axis],false};
        volatile float next=scale*otherBody->velocity.v[axis];
        if(!admitted(next))return {S::arithmetic,multiplyPC[axis],false};
        velocity[axis]=next;
    }
    for(unsigned axis=0;axis<3;++axis)receiverBody.velocity.v[axis]=velocity[axis];
    receiver70W=1.f;
    return {S::complete,0x2cb498u,true};
}
} // namespace rr::game
