#pragma once
#include "original_wreck_queries50la.h"
#include "original_wreck_components50la.h"

namespace rr::game {
enum class OriginalWreckMovementQueryStatus50LA : std::uint8_t {
    complete,owner,staging,registration,explicitList,worldQuery,normalMode
};
struct OriginalWreckMovementQueryResult50LA {
    OriginalWreckMovementQueryStatus50LA status=OriginalWreckMovementQueryStatus50LA::owner;
    std::uint32_t sourcePC=0;
    bool endpointsCommitted=false,registrationCommitted=false,queryCommitted=false;
    OriginalWreckQueryResult50LA prefix{};
    OriginalCameraWorldResult world{};
};
//16688C/166D28: actual E8 one-pair11CA80 ->11FF60/120230 ->121920(0).
//Uses the existing exact SearchList121920 nullable-E4 implementation, whose
//type3/F8bit0/filter/iterator semantics are the same for this actual E8 owner.
//No camera controller or camera-specific callback is constructed. Geometry,
//candidate states/copies, methods, memberships and normal-mode word are the
//real application-wide owners borrowed by queries.worldBinding().
//
//E8 state/type3/ray are the sole persistent owners. The small world-call state
//below is transient transaction staging only; it commits back after the real
//query completes. A native failure after staging/registration retains those
//prefix effects; never retry a whole frame from its beginning. Nonzero actual
//piece148 reaches122660 and remains a source guard until its concrete list is
//joined. No caller-supplied candidate order or fabricated miss is accepted.
inline OriginalWreckMovementQueryResult50LA queryOriginalWreckMovement50LA(
    OriginalWreckQueries50LA& queries,const OriginalWreckComponent50LA& pieceComponent,
    const SpringVector4_48N& start,const SpringVector4_48N& end,
    std::uint32_t actualExplicitCandidateCount148) noexcept {
    using S=OriginalWreckMovementQueryStatus50LA;OriginalWreckMovementQueryResult50LA r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    if(!queries.prepared()||!queries.coherent(2)||
       pieceComponent.kind!=OriginalWreckComponentKind50LA::piece2CB4B0||
       !pieceComponent.objectKey||!pieceComponent.sessionGeneration)return fail(S::owner,0x16688cu);
    const auto& w=queries.worldBinding();auto& q=queries.query(2);
    if(!q.owner9C||&q.owner9C->component()!=&pieceComponent||q.bodyA0||!q.membership.attached||
       !w.normalMode3DF5B0||!w.sentinel3E02C0||!w.copies||!w.geometry||
       w.worldQuery.objects!=w.objects||w.worldQuery.geometry!=w.geometry||
       w.worldQuery.objectCount!=w.objectCount||w.mutableQuery.objects!=w.states||
       w.mutableQuery.copies!=w.copies)return fail(S::owner,0x121920u);
    r.prefix=queries.stageMovement80_90(start,end);
    if(r.prefix.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::staging,r.prefix.sourcePC);
    r.endpointsCommitted=true;
    //11FF60 first calls11FE50. Actual shape3 jump-table388A2C is11FF4C,
    //the no-op return; no model preparation or matrix is consumed forE8.
    r.prefix=queries.update(2);
    if(r.prefix.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::registration,r.prefix.sourcePC);
    r.registrationCommitted=true;
    if(actualExplicitCandidateCount148)return fail(S::explicitList,0x122660u);
    auto in=w.worldQuery;const auto& m=q.membership;
    in.queryObject=queries.ref(2);in.registrationChainPresent=m.attached;
    in.zoneLinks=m.storage.links;in.zoneLinkCount=m.storage.capacity;in.zoneHead=1;
    in.shapeTag=3;in.bounds={};in.bounds.shapeTag=3;
    in.bounds.cachedCenter=queries.state(2).cachedCenter;
    in.bounds.cachedRadius=queries.state(2).cachedRadius;
    in.bounds.pairCount=1;in.bounds.endpoints=q.movementEndpoints();in.bounds.endpointCount=2;
    in.bounds.endpointScenes=q.movementMetadata();in.bounds.endpointSceneCount=2;
    in.sentinel3E02C0=*w.sentinel3E02C0;in.type3NormalMode44=q.normalMode;
    OriginalCameraWorldQueryState stage{};
    stage.flagsF8=queries.state(2).flagsF8;stage.currentCandidateIdentity=q.currentIdentity94;
    stage.previousCenter70=q.previousCenter70;stage.distance84=q.distance84;
    stage.copied40=queries.copies(2).copied40;stage.copied50=queries.copies(2).copied50;
    stage.type3=q.type3;stage.ray=q.ray;
    const OriginalCameraWorldCallbackBinding callback{pieceComponent.objectKey,0u,nullptr};
    const auto status=queryOriginalCameraWorld(in,w.mutableQuery,w.queryWorkspace,stage,callback,r.world);
    if(status!=OriginalCameraWorldStatus::Complete)return fail(S::worldQuery,r.world.sourcePC);
    q.currentIdentity94=stage.currentCandidateIdentity;q.previousCenter70=stage.previousCenter70;
    q.distance84=stage.distance84;q.type3=stage.type3;q.ray=stage.ray;
    //Core already committed flags/cache/copies in their canonical world tables.
    r.queryCommitted=true;
    if(!w.normalMode3DF5B0->acceptCompleted({r.world.normalMode3DF5B0,r.world.normalModeWritten,true}))
        return fail(S::normalMode,0x126fa0u);
    if(r.world.resetGlobalsWritten)w.normalMode3DF5B0->acceptCompletedWorldReset();
    r.status=S::complete;r.sourcePC=0x1668d8u;return r;
}
}
