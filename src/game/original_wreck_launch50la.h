#pragma once
#include "body_integration48c.h"
#include "lap_event_random50id.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace rr::game {

enum class OriginalWreckLaunchStatus50LA : std::uint8_t {
    complete,roundingMode,input,arithmetic
};
struct OriginalWreckLaunchResult50LA {
    OriginalWreckLaunchStatus50LA status=OriginalWreckLaunchStatus50LA::input;
    std::uint32_t sourcePC=0;
};
namespace original_wreck_launch_detail50LA {
inline float word(std::uint32_t bits) noexcept {
    float value;std::memcpy(&value,&bits,sizeof(value));return value;
}
inline bool admitted(float value) noexcept {
    return std::isfinite(value)&&(value==0.f||std::isnormal(value));
}
inline float add(float left,float right) noexcept {
    volatile float value=left+right;return value;
}
inline float subtract(float left,float right) noexcept {
    volatile float value=left-right;return value;
}
inline float multiply(float left,float right) noexcept {
    volatile float value=left*right;return value;
}
inline float drawUnit(std::uint32_t& stagedSharedRawRandom) noexcept {
    LapEventRequest50ID request{};request.kind=LapEventCall50ID::Draw308030;
    std::uint32_t value=0;
    // The existing shared308030 provider advances all32 raw bits and returns
    // low31. There is no new RNG algorithm, seed, private stream or modulo.
    applyLapEventRandom50ID(request,stagedSharedRawRandom,value);
    volatile float converted=static_cast<float>(static_cast<std::int32_t>(value));
    return multiply(word(0x30000000u),converted);
}
inline float signedUnit(std::uint32_t& stagedSharedRawRandom) noexcept {
    const float unit=drawUnit(stagedSharedRawRandom);
    return subtract(multiply(2.f,unit),1.f);
}
}

// Exactly ONE reached piece iteration of22CA60, source22CB9C..22CDE4.
// The caller reaches this leaf only after the actual collection activation,
// aggregate detach22CB14, piece activation22CB34, piece20 bit1 store, body90
// enable and bodyAC reset have executed for that SAME ordered piece. This
// numerical leaf neither performs nor certifies those operations. It is not
// an activation API, a replacement debris owner, or a resumable effect receipt.
//
// sourceBody is the genuine a1 body supplied to22CA60; pieceBody is that
// current piece's canonical D0.body. velocity70W/angularVelocityB0W refer to
// the same body's existing full-vector fourth lanes. No body copy is retained.
// actualSharedRawRandom is the one existing session/process C-runtime stream.
// The caller must preserve collection30/38 order and invoke once per reached
// piece, with no unrelated work interleaved inside the source call.
//
// angularScale434870 and velocityScale434878 are the ACTUAL values after the
// earlier lazy initialization22CA74..22CABC. Their initialized raw words are
//41A00000 and4392AA7F. Initialization precedes22C6A0, so this tail does not
// initialize them late, fabricate readiness, or overwrite a live value.
//
// Successful call consumes six308030 draws, in source order: angular X/Y/Z
// signed-unit draws, then linear X signed-unit, Y unit, Z signed-unit. Conversion is
// signed CVT.S.W of low31, separately rounded multiply by2^-31, and (where
// reached) separately rounded multiply2/subtract1. Integer-to-float rounding
// can produce1.0, so there is deliberately no clamp to a half-open interval.
// Angular jitter scales by434870; linear jitter scales by434878. The source
// writes linear70 first, then angularB0, setting both W words to exact1.0.
// Old destination velocity/angular/W and every other body word are unconsumed.
// Source position/orientation/derivatives/loads, source W, and unrelated global
// words are unconsumed and are neither validated nor canonicalized.
//
// Bounded ordinary normal-or-zero binary32 domain, FE_TONEAREST, strict FP,
// no contraction/fast math. Each consumed source/scaling operand is admitted
// only at its reached arithmetic phase. A rejected call leaves both canonical
// bodies, both destination W lanes and shared RNG unchanged; this is a native
// admission failure, not a successful source prefix or permission to continue.
// The source, destination, two W lanes, two scale references and RNG storage
// must be mutually disjoint and live for the serial call. No allocation, model
// operations, collision/query registration, particles or sound occur here.
inline OriginalWreckLaunchResult50LA applyOriginalWreckLaunchTail50LA(
    const BodyState48C& sourceBody,BodyState48C& pieceBody,
    float& velocity70W,float& angularVelocityB0W,
    const float& angularScale434870,const float& velocityScale434878,
    std::uint32_t& actualSharedRawRandom) noexcept {
    using namespace original_wreck_launch_detail50LA;
    using S=OriginalWreckLaunchStatus50LA;
    if(std::fegetround()!=FE_TONEAREST)return {S::roundingMode,0x22cb9cu};
    auto random=actualSharedRawRandom;
    float angular[3];
    angular[0]=signedUnit(random); //22CB9C..22CBDC
    angular[1]=signedUnit(random); //22CBD8..22CC18
    angular[2]=signedUnit(random); //22CC14..22CC58
    if(!admitted(angularScale434870))return {S::input,0x22cc34u};
    for(unsigned i=0;i<3;++i){
        angular[i]=multiply(angular[i],angularScale434870);
        if(!admitted(angular[i]))return {S::arithmetic,0x22cc60u+12u*i};
    }
    float linear[3];
    linear[0]=signedUnit(random); //22CC7C..22CCBC
    linear[1]=drawUnit(random);   //22CCB8..22CCE0; no multiply2/subtract1
    linear[2]=signedUnit(random); //22CCDC..22CD28
    if(!admitted(velocityScale434878))return {S::input,0x22cd00u};
    for(unsigned i=0;i<3;++i){
        linear[i]=multiply(linear[i],velocityScale434878);
        if(!admitted(linear[i]))return {S::arithmetic,0x22cd30u+12u*i};
    }
    //22CD50..22CD7C reads source70;22CD9C..22CDC8 later reads sourceB0.
    // Do not validate or copy the rest of either canonical body.
    for(unsigned i=0;i<3;++i){
        if(!admitted(sourceBody.velocity.v[i]))return {S::input,0x22cd50u+8u*i};
        linear[i]=add(sourceBody.velocity.v[i],linear[i]);
        if(!admitted(linear[i]))return {S::arithmetic,0x22cd68u+(i<2?4u*i:12u)};
    }
    for(unsigned i=0;i<3;++i){
        if(!admitted(sourceBody.angularVelocity.v[i]))return {S::input,0x22cd9cu+8u*i};
        angular[i]=add(sourceBody.angularVelocity.v[i],angular[i]);
        if(!admitted(angular[i]))return {S::arithmetic,0x22cdb4u+(i<2?4u*i:12u)};
    }
    // Commit only the two reached full vectors. No whole-body assignment can
    // overwrite position, derivative history, an unrelated NaN, or signed zero.
    actualSharedRawRandom=random;
    for(unsigned i=0;i<3;++i)pieceBody.velocity.v[i]=linear[i];
    velocity70W=1.f; //22CD98 SQ piece.D0+70
    for(unsigned i=0;i<3;++i)pieceBody.angularVelocity.v[i]=angular[i];
    angularVelocityB0W=1.f; //22CDE4 SQ piece.D0+B0
    return {S::complete,0x22cde4u};
}
}
