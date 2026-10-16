#pragma once
#include "original_wreck_nonflight50la.h"

namespace rr::game {
enum class OriginalWreckGroupNonflightStatus50LA : std::uint8_t {
    complete,owner,detector,attachedPieces,binding,arithmetic,forces,response,integration,movement
};
struct OriginalWreckGroupNonflightResult50LA {
    OriginalWreckGroupNonflightStatus50LA status=OriginalWreckGroupNonflightStatus50LA::owner;
    std::uint32_t sourcePC=0;
    std::size_t substepsCommitted=0;
    bool attachedDetectorCountCleared=false,impulseCleared=false,timerCommitted=false;
    OriginalWreckDetectorResult50LA detector{};
    OriginalWreckForceResult50LA forces{};
    OriginalWreckResponseResult50LA response{};
    OriginalWreckIntegrationResult50LA integration{};
    OriginalWreckNonflightMovementResult50LA movement{};
};

// Actual2CA9C4..2CAD50 nonflight continuation for the one-piece group. Its
// primary3C/40 borrow the same D0/piece as the existing owner; no second body,
// scene, record list or query owner is created. The real detector executes
// once, then each source substep performs forces, DC2964C0, impulse retention
// and body integration. Capacity0 genuinely skips the attached-piece loops.
// Nonzero capacity stops at the reached loop after the real detector prefix.
//
// This cannot call the piece166F8C continuation: primary impulseC0 comes from
// actual4441F0, group38 is cleared, and there are NO post-movement E0/E4 updates
// here. The caller must continue at2CAD50 with its genuine root E4 update,
// settling, axes and group-child dispatch. Flight/paused/disabled routes do
// not enter this function. No query/response success is accepted as input.
//
// Strict source binary32 operation order, FE_TONEAREST, the existing native
// kernels' admission domains and serial nonaliasing owner/global/workspace
// contract. Failure preserves completed prefixes; do not replay the frame.
// No allocation, arbitrary substep cap, host driving, synthetic contact or
// suppression of a reached unsupported source route.
inline OriginalWreckGroupNonflightResult50LA advanceOriginalWreckGroupNonflight2CA9C4_50LA(
    OriginalOnePieceWreckOwner50LA& o,OriginalWreckNonflightGlobals50LA& globals,
    const float& actualMaximumStep444230,const SpringVector4_48N* actualZero433B10,
    OriginalWreckFlightGlobals50LA* flightGlobals,float dt,GraphBodyWorkspace49W* workspace)noexcept {
    using S=OriginalWreckGroupNonflightStatus50LA;OriginalWreckGroupNonflightResult50LA r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    if(!o.prepared()||o.group.primary3C!=&o.physical||o.group.primary40!=&o.piece)
        return fail(S::owner,0x2ca9c4u);
    r.detector=prepareOriginalWreckDetector295630_50LA(o,globals);
    if(r.detector.status!=OriginalWreckNonflightStatus50LA::complete)return fail(S::detector,r.detector.sourcePC);
    o.group.detectedAttached38=0;r.attachedDetectorCountCleared=true;
    if(o.group.capacity34)return fail(S::attachedPieces,0x2caa40u);
    using namespace original_wreck_settling_detail50LA;
    if(std::fegetround()!=FE_TONEAREST||!admitted(dt)||!admitted(actualMaximumStep444230))
        return fail(S::arithmetic,0x2caa60u);
    float elapsed=0.f,step=detail::scalarMin48O(dt,actualMaximumStep444230);
    const auto* initialImpulse=o.groupVector4441F0();
    if(!initialImpulse)return fail(S::binding,0x2caa78u);
    std::memcpy(&o.piece.impulseC0,initialImpulse,sizeof o.piece.impulseC0);r.impulseCleared=true;
    while(elapsed<dt){
        if(!(step>0.f))return fail(S::arithmetic,0x2cace0u); //native nonprogress guard, never truncation
        r.forces=applyOriginalWreckForces50LA(o.physical,o.piece,o.gravity,o.actualWaterB0,o.actualWaterGlobals);
        if(r.forces.status!=OriginalWreckForceStatus50LA::complete)return fail(S::forces,r.forces.sourcePC);
        r.response=advanceOriginalWreckContact2964C0_50LA(o,globals.pointContactSentinel4435E0,globals.generatedContactSentinel4435E8);
        if(r.response.status!=OriginalWreckResponseStatus50LA::complete)return fail(S::response,r.response.sourcePC);
        //The same actual capacity34=0 takes2CAB08 without any attached DC call.
        constexpr std::uint32_t impulsePC[3]={0x2cab38u,0x2cab88u,0x2cabd8u};
        for(unsigned axis=0;axis<3;++axis){
            const float actual=o.physical.body.root.accumulator1c0.v[axis],prior=o.piece.impulseC0.v[axis];
            if(!admitted(actual)||!admitted(prior))return fail(S::arithmetic,impulsePC[axis]);
            if(!(std::fabs(actual)<=std::fabs(prior)))o.piece.impulseC0.v[axis]=actual;
        }
        if(!workspace)return fail(S::binding,0x2cac04u);
        r.integration=integrateOriginalWreckBody50LA(o.physical,o.bodyComponent(),o.assets,o.visibility,step,*workspace);
        if(r.integration.status!=OriginalWreckIntegrationStatus50LA::complete)return fail(S::integration,r.integration.sourcePC);
        float magnitude=0.f;
        if(!angularLength(o.physical.body.root.body.linearAcceleration,magnitude)||
           !admitted(o.piece.measuredLinearAcceleration124))return fail(S::arithmetic,0x2cac14u);
        o.piece.measuredLinearAcceleration124=detail::scalarMax48O(o.piece.measuredLinearAcceleration124,magnitude);
        if(!angularLength(o.physical.body.root.body.angularAcceleration,magnitude)||
           !admitted(o.piece.measuredAngularAcceleration128))return fail(S::arithmetic,0x2cac68u);
        const float angularMaximum=detail::scalarMax48O(o.piece.measuredAngularAcceleration128,magnitude);
        //2CACA8 advances elapsed before2CACB8 subtraction and2CACC0's128
        //store. Keep this ordering, including the earlier committed124 value.
        const float next=add(elapsed,step);
        if(!admitted(next)||!(next>elapsed))return fail(S::arithmetic,0x2caca8u);
        elapsed=next;
        const float remaining=sub(dt,elapsed);
        if(!admitted(remaining))return fail(S::arithmetic,0x2cacb8u);
        o.piece.measuredAngularAcceleration128=angularMaximum;
        step=detail::scalarMin48O(actualMaximumStep444230,remaining);++r.substepsCommitted;
    }
    if(!actualZero433B10)return fail(S::binding,0x2cacf0u);
    r.movement=moveOriginalWreck166C60_50LA(o,globals,*actualZero433B10,flightGlobals);
    if(r.movement.status!=OriginalWreckNonflightStatus50LA::complete)return fail(S::movement,r.movement.sourcePC);
    if(o.piece.flight34){
        if(o.physical.spring.contact3C)o.piece.airborneTime11C=0.f;
        else{
            if(!admitted(o.piece.airborneTime11C))return fail(S::arithmetic,0x2cad20u);
            const float next=add(o.piece.airborneTime11C,dt);
            if(!admitted(next))return fail(S::arithmetic,0x2cad24u);
            o.piece.airborneTime11C=next;
        }
        r.timerCommitted=true;
        if(!admitted(o.piece.airborneTime11C)||!admitted(o.piece.flightDelay120))return fail(S::arithmetic,0x2cad38u);
        if(!(o.piece.airborneTime11C<=o.piece.flightDelay120))o.piece.flight38=1;
    }
    r.status=S::complete;r.sourcePC=0x2cad50u;return r;
}
} // namespace rr::game
