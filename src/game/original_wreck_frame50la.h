#pragma once
#include "original_wreck_owner50la.h"
#include "original_wreck_flight50la.h"
#include "original_wreck_settling50la.h"
#include "original_wreck_nonflight50la.h"
#include "original_wreck_group_nonflight50la.h"
#include "original_camera_start_native.h"

namespace rr::game {
// ONE application-owned projection of the actual lazy globals. These are
// separate from166790's flight globals, and are not recreated per owner/frame.
struct OriginalWreckFrameGlobals50LA {
    std::uint8_t initializedGp8538=0,initializedGp853C=0,initializedGp8E18=0;
    float maximumStep433B68=0.f,maximumGroupStep444230=0.f;
    std::uint32_t sentinel433B70=0;
};
// Borrows of the actual Session/application owners. A pointer is read only
// when its source branch reaches it. In particular, a paused call consumes
// only application11C; disabled bodies do not consume flight/workspace/dt.
struct OriginalWreckFrameBinding50LA {
    const std::uint32_t* application11C=nullptr;
    OriginalWreckFrameGlobals50LA* globals=nullptr;
    OriginalWreckFlightGlobals50LA* flightGlobals=nullptr;
    GraphBodyWorkspace49W* integrationWorkspace=nullptr;
    const OriginalRendererDispatchFields* renderer442F70=nullptr;
    platform::VehicleWindowGl50DS* actualRenderer=nullptr;
    platform::VehicleWindowDiagnostic50DS* rendererDiagnostic=nullptr;
    OriginalWreckNonflightGlobals50LA* nonflightGlobals=nullptr;
    const SpringVector4_48N* actualZero433B10=nullptr;
};
enum class OriginalWreckFrameStatus50LA : std::uint8_t {
    complete,binding,owner,flight,arithmetic,settling,query,graph,
    componentTopology,renderer,childMethod,bodyCache,
    detector295630,attachedGroupPieces,nonflight
};
struct OriginalWreckChildFrameResult50LA {
    OriginalWreckFrameStatus50LA status=OriginalWreckFrameStatus50LA::owner;
    std::uint32_t sourcePC=0;
    std::size_t rendererCalls=0,bodyCacheCalls=0;
    OriginalCameraStartResult renderer{OriginalCameraStartStatus::complete,0};
    OriginalWreckBodyControlStatus50LA body=OriginalWreckBodyControlStatus50LA::owner;
};
struct OriginalWreckFrameResult50LA {
    OriginalWreckFrameStatus50LA status=OriginalWreckFrameStatus50LA::owner;
    std::uint32_t sourcePC=0;
    bool paused=false,globalsCommitted=false,measurementsCleared=false;
    bool flightCommitted=false,linearMeasurementCommitted=false,angularMeasurementCommitted=false;
    bool rootE4Prepared=false,rootE4Updated=false,settlingCommitted=false,groupAxesRead=false;
    bool nativeReturnKnown=false;
    std::int32_t nativeV0=0;
    // Ephemeral copies of the actual group stack40/50, not persistent poses.
    SpringVector4_48N groupLook{{0.f,0.f,0.f,1.f}},groupUp{{0.f,0.f,0.f,1.f}};
    OriginalWreckFlightResult50LA flight{};
    OriginalWreckNonflightResult50LA nonflight{};
    OriginalWreckGroupNonflightResult50LA groupNonflight{};
    OriginalWreckSettlingResult50LA settling{};
    OriginalWreckQueryResult50LA query{};
    OriginalWreckChildFrameResult50LA children{};
};

namespace original_wreck_frame_detail50LA {
using S=OriginalWreckFrameStatus50LA;
inline OriginalWreckSettlingBinding50LA settlingBinding(OriginalOnePieceWreckOwner50LA& o)noexcept {
    auto& p=o.piece;auto& b=o.physical;
    return {p.flight34,p.flight38,b.spring,b.body.root.body,b.body.cachedSpeed34,b.body.enabled,
        p.contactTime12C,p.dragEC,p.rateDampingF0,p.baseDrag100,p.baseRateDamping104,
        p.finalRateDamping108,p.finalDrag10C,p.changeWindow130,p.stopSpeed134,p.stopAngularSpeed138,
        p.stopLinearAcceleration13C,p.stopAngularAcceleration140,
        p.measuredLinearAcceleration124,p.measuredAngularAcceleration128};
}
// Exact141ED0 root24 gate and ordered child18/next14 traversal. The only
// implemented reached child slot38 is this owner's real167B80 body component.
// No renderer prelude runs for skipped children. next14 is read AFTER a
// completed real child call, not prefetched into a synthetic work list.
inline OriginalWreckChildFrameResult50LA children(OriginalOnePieceWreckOwner50LA& o,
    OriginalWreckComponent50LA& root,const OriginalWreckFrameBinding50LA& binding)noexcept {
    using namespace original_wreck_component_detail50LA;
    OriginalWreckChildFrameResult50LA r;
    auto fail=[&](S status,std::uint32_t pc){r.status=status;r.sourcePC=pc;return r;};
    if(!domain(o.domain)||!contains(o.domain,&root))return fail(S::owner,0x141ed0u);
    if(!(root.descendants24&2u)){r.status=S::complete;r.sourcePC=0x141f84u;return r;}
    auto* child=root.firstChild18;OriginalWreckComponent50LA* previous=nullptr;
    std::size_t remaining=o.domain.count;
    while(child){
        if(!remaining--||!contains(o.domain,child)||child->parent1C!=&root||child->previous10!=previous)
            return fail(S::componentTopology,0x141efcu);
        if(originalWreckChildEligible50LA(*child)){
            if(!binding.renderer442F70||!binding.actualRenderer||!binding.rendererDiagnostic)
                return fail(S::binding,0x141f3cu);
            std::int32_t rendererV0=0;
            r.renderer=serviceOriginalRenderer244D60(*binding.renderer442F70,false,
                *binding.actualRenderer,*binding.rendererDiagnostic,rendererV0);
            ++r.rendererCalls;
            if(r.renderer.status!=OriginalCameraStartStatus::complete)
                return fail(S::renderer,r.renderer.sourcePC);
            if(&root!=&o.pieceComponent()||child!=&o.bodyComponent()||
               child->kind!=OriginalWreckComponentKind50LA::body167920)
                return fail(S::childMethod,0x141f60u);
            r.body=refreshOriginalWreckBodyFrame50LA(o.physical,*child);
            if(r.body!=OriginalWreckBodyControlStatus50LA::complete)
                return fail(S::bodyCache,0x167b80u);
            ++r.bodyCacheCalls;
        }
        previous=child;child=child->next14;
    }
    r.status=S::complete;r.sourcePC=0x141f84u;return r;
}
inline bool flight(OriginalOnePieceWreckOwner50LA& o,const OriginalWreckFrameBinding50LA& b,
    float dt,OriginalWreckFrameResult50LA& r,std::uint32_t sourcePC)noexcept {
    if(!b.flightGlobals||!b.integrationWorkspace){r.status=S::binding;r.sourcePC=sourcePC;return false;}
    r.flight=advanceOriginalWreckFlight50LA(o.physical,o.piece,o.pieceComponent(),o.bodyComponent(),
        o.assets,o.visibility,o.queries,o.gravity,o.actualWaterB0,o.actualWaterGlobals,
        *b.flightGlobals,dt,*b.integrationWorkspace);
    if(r.flight.status!=OriginalWreckFlightStatus50LA::complete){
        r.status=S::flight;r.sourcePC=r.flight.sourcePC;return false;
    }
    r.flightCommitted=true;return true;
}
// Source scalar C0/D0 lengths: Y*Y, X*X, ADDA X²,Y², MADD Z,Z, SQRT.S.
// This is the same arithmetic as16786C, NOT cached velocity or a VU norm.
// The first store survives a later angular-length boundary.
inline bool measurements(OriginalOnePieceWreckOwner50LA& o,OriginalWreckFrameResult50LA& r,
    std::uint32_t linearPC,std::uint32_t angularPC)noexcept {
    if(std::fegetround()!=FE_TONEAREST){r.status=S::arithmetic;r.sourcePC=linearPC;return false;}
    float value=0.f;
    if(!original_wreck_settling_detail50LA::angularLength(o.physical.body.root.body.linearAcceleration,value)){
        r.status=S::arithmetic;r.sourcePC=linearPC;return false;
    }
    o.piece.measuredLinearAcceleration124=value;r.linearMeasurementCommitted=true;
    if(!original_wreck_settling_detail50LA::angularLength(o.physical.body.root.body.angularAcceleration,value)){
        r.status=S::arithmetic;r.sourcePC=angularPC;return false;
    }
    o.piece.measuredAngularAcceleration128=value;r.angularMeasurementCommitted=true;return true;
}
inline bool settle(OriginalOnePieceWreckOwner50LA& o,float dt,OriginalWreckFrameResult50LA& r)noexcept {
    r.settling=settleOriginalWreckBody50LA(settlingBinding(o),dt);
    if(r.settling.status!=OriginalWreckSettlingStatus50LA::complete){
        r.status=S::settling;r.sourcePC=r.settling.sourcePC;return false;
    }
    r.settlingCommitted=true;return true;
}
inline OriginalWreckFrameResult50LA finishChildren(OriginalOnePieceWreckOwner50LA& o,
    OriginalWreckComponent50LA& root,const OriginalWreckFrameBinding50LA& b,
    OriginalWreckFrameResult50LA r,std::uint32_t returnPC)noexcept {
    r.children=children(o,root,b);
    if(r.children.status!=S::complete){r.status=r.children.status;r.sourcePC=r.children.sourcePC;return r;}
    r.status=S::complete;r.sourcePC=returnPC;r.nativeReturnKnown=true;r.nativeV0=1;return r;
}
}

// Actual BASE PhysicsObject166E40, called from the genuine2CB8F0 derived
// method. Complete means this base call returned1, including a paused call;
// it does NOT mean the derived effect suffix or whole piece frame completed.
// Caller must enter at this source call exactly once, after any outer141ED0
// prelude. Activity29/mask20 selection belongs to that outer dispatcher.
//
// Real flight and initially-disabled branches are closed. The enabled
// nonflight branch executes the genuine295630 prefix and native substeps
//within its declared owner domain, including positive static2964C0 response.
//Unsupported query/response owners remain explicit boundaries.
// Prefixes survive failures. Do not replay a partially completed source call.
inline OriginalWreckFrameResult50LA advanceOriginalWreckPiece166E40_50LA(
    OriginalOnePieceWreckOwner50LA& o,const OriginalWreckFrameBinding50LA& b,float dt)noexcept {
    using namespace original_wreck_frame_detail50LA;OriginalWreckFrameResult50LA r;
    auto fail=[&](S status,std::uint32_t pc){r.status=status;r.sourcePC=pc;return r;};
    if(!b.application11C)return fail(S::binding,0x166e64u);
    if(*b.application11C){r.status=S::complete;r.sourcePC=0x166e74u;r.paused=true;
        r.nativeReturnKnown=true;r.nativeV0=1;return r;}
    if(!o.prepared())return fail(S::owner,0x166e40u);
    if(!b.globals)return fail(S::binding,0x166e78u);
    auto& g=*b.globals;
    if(!g.initializedGp8538){g.maximumStep433B68=original_wreck_physical_detail50LA::word(0x3c888889u);g.initializedGp8538=1;}
    if(!g.initializedGp853C){g.sentinel433B70=0xffffffffu;g.initializedGp853C=1;}
    r.globalsCommitted=true;
    o.piece.measuredLinearAcceleration124=0.f;o.piece.measuredAngularAcceleration128=0.f;
    r.measurementsCleared=true;
    if(o.physical.body.enabled){
        if(o.piece.flight34&&o.piece.flight38){
            if(!flight(o,b,dt,r,0x166f00u)||!measurements(o,r,0x166f10u,0x166f44u))return r;
            //166F6C jumps directly to167218. No E0/E4 update on this branch.
        }else{
            if(!b.nonflightGlobals)
                return fail(S::binding,0x166f98u);
            r.nonflight=advanceOriginalWreckPieceNonflight166F8C_50LA(o,*b.nonflightGlobals,
                g.maximumStep433B68,b.actualZero433B10,b.flightGlobals,dt,b.integrationWorkspace);
            if(r.nonflight.status!=OriginalWreckNonflightStatus50LA::complete)
                return fail(S::nonflight,r.nonflight.sourcePC);
        }
        if(!settle(o,dt,r))return r;
    }
    return finishChildren(o,o.pieceComponent(),b,r,0x167230u);
}

// Actual BASE PhysicsObjectGroup2CA840 from derived2CC1B0, bounded to the
// authentic one-piece owner (capacity34=0). It shares the real primary body
// and piece; there is no extra aggregate physics state. Complete covers this
// base call only;2CC1B0's effect/debug suffix remains the caller's obligation.
//
// Unlike166E40 flight, enabled group flight ALWAYS prepares/updates rootE4
// through11FF60 before settling and28A700. Capacity0 skips attached-piece
// collision updates, fracture RNG, transforms and effects without faking any
// such operation. The final141ED0 is the actual group component's child list,
// not the primary piece's list: fresh group has no body-cache child call.
// Nonflight performs its real primary detector/static response/substeps and
// movement before joining that same2CAD50 suffix. It does not borrow the
// piece continuation's distinct post-movement E0/E4 updates or433B68/433B10
// impulse initialization in place of the actual444230/4441F0 group globals.
inline OriginalWreckFrameResult50LA advanceOriginalWreckGroup2CA840_50LA(
    OriginalOnePieceWreckOwner50LA& o,const OriginalWreckFrameBinding50LA& b,float dt)noexcept {
    using namespace original_wreck_frame_detail50LA;OriginalWreckFrameResult50LA r;
    auto fail=[&](S status,std::uint32_t pc){r.status=status;r.sourcePC=pc;return r;};
    if(!b.application11C)return fail(S::binding,0x2ca868u);
    if(*b.application11C){r.status=S::complete;r.sourcePC=0x2ca878u;r.paused=true;
        r.nativeReturnKnown=true;r.nativeV0=1;return r;}
    if(!o.prepared()||o.group.primary3C!=&o.physical||o.group.primary40!=&o.piece)
        return fail(S::owner,0x2ca840u);
    if(!b.globals)return fail(S::binding,0x2ca87cu);
    auto& g=*b.globals;
    if(!g.initializedGp8E18){g.maximumGroupStep444230=original_wreck_physical_detail50LA::word(0x3c888889u);g.initializedGp8E18=1;}
    r.globalsCommitted=true;
    o.piece.measuredLinearAcceleration124=0.f;o.piece.measuredAngularAcceleration128=0.f;
    r.measurementsCleared=true;
    if(o.physical.body.enabled){
        if(o.piece.flight34&&o.piece.flight38){
            if(!flight(o,b,dt,r,0x2ca92cu)||!measurements(o,r,0x2ca93cu,0x2ca974u))return r;
        }else{
            if(!b.nonflightGlobals)return fail(S::binding,0x2ca9d4u);
            r.groupNonflight=advanceOriginalWreckGroupNonflight2CA9C4_50LA(o,*b.nonflightGlobals,
                g.maximumGroupStep444230,b.actualZero433B10,b.flightGlobals,dt,b.integrationWorkspace);
            if(r.groupNonflight.status!=OriginalWreckGroupNonflightStatus50LA::complete)
                return fail(S::nonflight,r.groupNonflight.sourcePC);
        }
        r.query=o.queries.prepareModelQuery(1);
        //11FE50's reached scene getter can publish world cache before a
        //later preparation boundary, so preserve the canonical body alias.
        original_wreck_flight_detail50LA::projection(o.physical,o.assets.graph());
        if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::query,r.query.sourcePC);
        r.rootE4Prepared=true;
        r.query=o.queries.update(1);
        if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::query,r.query.sourcePC);
        r.rootE4Updated=true;
        if(o.group.capacity34)return fail(S::attachedGroupPieces,0x2cada0u);
        if(!settle(o,dt,r))return r;
        const auto graph=o.assets.graph();const auto model=o.assets.model14C();
        if(!refreshSceneWorld49P(graph,model))return fail(S::graph,0x2cadccu);
        const auto& world=graph.nodes[static_cast<std::int32_t>(model)].world100;
        //28A700(null,stack40,stack50): XYZ-only copies preserve both W=1.
        std::memcpy(r.groupLook.v,world.v+8,12);std::memcpy(r.groupUp.v,world.v+4,12);
        original_wreck_flight_detail50LA::projection(o.physical,graph);r.groupAxesRead=true;
        if(o.group.capacity34)return fail(S::attachedGroupPieces,0x2cb3d8u);
    }
    return finishChildren(o,o.groupComponent(),b,r,0x2cb3f4u);
}
} // namespace rr::game
