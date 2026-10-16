#pragma once
#include "original_wreck_owner50la.h"
#include "original_wreck_flight50la.h"
#include "original_wreck_settling50la.h"
#include "original_wreck_contact_world50la.h"
#include "original_wreck_response50la.h"

namespace rr::game {
struct OriginalWreckNonflightGlobals50LA {
    VisualLazyWord49R detectorSentinel443590{},detectorQuerySentinel443598{};
    VisualLazyWord49R pointContactSentinel4435E0{},generatedContactSentinel4435E8{};
    std::uint8_t initializedGp8534=0;
    float movementSeparation433B60=0.f;
};
enum class OriginalWreckNonflightStatus50LA : std::uint8_t {
    complete,owner,binding,terrainPoints,staging,registration,explicitList,worldQuery121920,
    generatedContacts2964C0,pointContacts2964C0,arithmetic,forces,integration,query,graph,visibility,contactResponse
};
struct OriginalWreckDetectorResult50LA {
    OriginalWreckNonflightStatus50LA status=OriginalWreckNonflightStatus50LA::owner;
    std::uint32_t sourcePC=0;
    bool resetCommitted=false,preparationCommitted=false,registrationCommitted=false;
    bool nativeReturnKnown=false;std::uint32_t nativeV0=0;
    OriginalWreckQueryResult50LA query{};
    OriginalWreckContactWorldResult50LA world{};
};
namespace original_wreck_nonflight_detail50LA {
inline void sentinel(VisualLazyWord49R& v)noexcept {if(!v.guard){v.word=0xffffffffu;v.guard=1;}}
using S=OriginalWreckNonflightStatus50LA;
}
//Actual D8/295630 prefix. Fresh one-piece pairs60=null and genuine absent
//terrain34/pointContacts48 take295944, not a fabricated point/query miss.
//The actual registered shape2 world call owns its result90 and dispatches
//direct static contacts; a reached unsupported route is not nativeReturnKnown.
inline OriginalWreckDetectorResult50LA prepareOriginalWreckDetector295630_50LA(
    OriginalOnePieceWreckOwner50LA& o,OriginalWreckNonflightGlobals50LA& g)noexcept {
    using namespace original_wreck_nonflight_detail50LA;OriginalWreckDetectorResult50LA r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    auto& d=o.detector;
    if(!o.prepared()||!d.constructed||d.contact30!=&o.physical)return fail(S::owner,0x295644u);
    sentinel(g.detectorSentinel443590);
    if(d.pairs60||(o.physical.generated.pointContacts48&&d.terrain34))return fail(S::terrainPoints,0x295698u);
    sentinel(g.detectorQuerySentinel443598);
    if(d.reset6C){resetSpringRecords48N(o.physical.spring);r.resetCommitted=true;}
    if(!d.query38||!d.queryEnabled68){r.status=S::complete;r.sourcePC=0x295a34u;
        r.nativeReturnKnown=true;r.nativeV0=1;return r;}
    if(d.query38!=&o.queries.query(0))return fail(S::owner,0x295978u);
    r.query=o.queries.prepareModelQuery(0);
    original_wreck_flight_detail50LA::projection(o.physical,o.assets.graph());
    if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::staging,r.query.sourcePC);
    r.preparationCommitted=true;
    r.query=o.queries.update(0);
    if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::registration,r.query.sourcePC);
    r.registrationCommitted=true;
    if(d.objects74)return fail(S::explicitList,0x295a08u);
    r.world=queryOriginalWreckContactWorld50LA(o,o.queries.query(0).membership.revision);
    if(r.world.status!=OriginalWreckContactWorldStatus50LA::complete||!r.world.queryCommitted)
        return fail(S::worldQuery121920,r.world.sourcePC);
    r.status=S::complete;r.sourcePC=0x295a34u;r.nativeReturnKnown=true;r.nativeV0=1;return r;
}
struct OriginalWreckNonflightMovementResult50LA {
    OriginalWreckNonflightStatus50LA status=OriginalWreckNonflightStatus50LA::owner;
    std::uint32_t sourcePC=0;
    bool current90Committed=false,previous80Committed=false,normalA0Committed=false,positionCommitted=false;
    OriginalWreckMovementQueryResult50LA query{};
    OriginalWreckVisibilityResult50LA visibility{};
};
//Complete166C60 movement correction, including real registered E8 query.
//It is ONE query, unlike166790's repeated flying correction loop. current90
//is the source XYZ-only getter destination and its existing W is retained.
inline OriginalWreckNonflightMovementResult50LA moveOriginalWreck166C60_50LA(
    OriginalOnePieceWreckOwner50LA& o,OriginalWreckNonflightGlobals50LA& g,
    const SpringVector4_48N& actualZero433B10,OriginalWreckFlightGlobals50LA* flightGlobals)noexcept {
    using namespace original_wreck_nonflight_detail50LA;OriginalWreckNonflightMovementResult50LA r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    if(!o.prepared()||o.physical.model288!=o.assets.model14C())return fail(S::owner,0x166c60u);
    if(!g.initializedGp8534){g.movementSeparation433B60=1.f;g.initializedGp8534=1;}
    const auto graph=o.assets.graph();const auto model=o.physical.model288;
    if(!original_wreck_flight_detail50LA::position(graph,model,o.piece.current90))return fail(S::graph,0x166ca4u);
    original_wreck_flight_detail50LA::projection(o.physical,graph);r.current90Committed=true;
    const auto& zero=actualZero433B10;auto& p=o.piece;
    //166CB0..166D18 compares XYZ with ordinary source equality, lazily.
    if(p.previous80.v[0]==zero.v[0]&&p.previous80.v[1]==zero.v[1]&&p.previous80.v[2]==zero.v[2]){
        p.previous80=p.current90;r.previous80Committed=true;
    }
    r.query=queryOriginalWreckMovement50LA(o.queries,o.pieceComponent(),p.previous80,p.current90,p.explicitCandidateCount148);
    if(r.query.status!=OriginalWreckMovementQueryStatus50LA::complete)return fail(S::query,r.query.sourcePC);
    if(o.queries.state(2).flagsF8&0x20u){
        const auto& hit=o.queries.query(2).type3;p.normalA0=hit.selectedNormal;r.normalA0Committed=true;
        if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x166d9cu);
        original_wreck_force_detail50LA::Math m;SpringVector4_48N corrected{{0.f,0.f,0.f,1.f}};
        for(unsigned i=0;i<3;++i)
            corrected.v[i]=m.add(hit.selectedPoint.v[i],m.mul(g.movementSeparation433B60,p.normalA0.v[i]));
        if(!m.valid)return fail(S::arithmetic,0x166dacu);
        if(!flightGlobals)return fail(S::binding,0x166e04u);
        sentinel(flightGlobals->positionSentinel4433E8);
        auto local=graph.nodes[static_cast<std::int32_t>(model)].localC0;std::memcpy(local.v+12,corrected.v,12);
        if(!setLocalMatrix49P(graph,model,local))return fail(S::graph,0x166e04u);
        original_wreck_flight_detail50LA::projection(o.physical,graph);r.positionCommitted=true;
        r.visibility=o.visibility.update();
        original_wreck_flight_detail50LA::projection(o.physical,graph);
        if(r.visibility.status!=OriginalWreckVisibilityStatus50LA::complete)return fail(S::visibility,r.visibility.sourcePC);
        p.previous80=corrected;
    }else p.previous80=p.current90;
    r.previous80Committed=true;r.status=S::complete;r.sourcePC=0x166e28u;return r;
}
struct OriginalWreckNonflightResult50LA {
    OriginalWreckNonflightStatus50LA status=OriginalWreckNonflightStatus50LA::owner;
    std::uint32_t sourcePC=0;std::size_t substepsCommitted=0;
    bool impulseCleared=false,timerCommitted=false;
    OriginalWreckDetectorResult50LA detector{};
    OriginalWreckForceResult50LA forces{};
    OriginalWreckResponseResult50LA response{};
    OriginalWreckIntegrationResult50LA integration{};
    OriginalWreckNonflightMovementResult50LA movement{};
    OriginalWreckQueryResult50LA query{};
};
//Actual reached166F8C..167214 piece nonflight body. Caller initializes its
//433B68 global and clears124/128 before entering. This function directly
//executes the real detector prefix, forces, response, integration, movement
//and E0/E4 updates. It never accepts a supplied query/response success flag.
//Shape2 world121920 consumes the actual E0 registration and ordered static
//world owners. Reached groups/dynamic/E8 retain explicit query boundaries.
//Positive static generated records execute the genuine2964C0 response before
//impulse retention/integration; unsupported response owners stop that suffix.
inline OriginalWreckNonflightResult50LA advanceOriginalWreckPieceNonflight166F8C_50LA(
    OriginalOnePieceWreckOwner50LA& o,OriginalWreckNonflightGlobals50LA& g,
    const float& actualMaximumStep433B68,const SpringVector4_48N* actualZero433B10,
    OriginalWreckFlightGlobals50LA* flightGlobals,float dt,GraphBodyWorkspace49W* workspace)noexcept {
    using namespace original_wreck_nonflight_detail50LA;OriginalWreckNonflightResult50LA r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    r.detector=prepareOriginalWreckDetector295630_50LA(o,g);
    if(r.detector.status!=S::complete)return fail(r.detector.status,r.detector.sourcePC);
    using namespace original_wreck_settling_detail50LA;
    if(std::fegetround()!=FE_TONEAREST||!admitted(dt)||!admitted(actualMaximumStep433B68))return fail(S::arithmetic,0x166fb0u);
    float elapsed=0.f,step=detail::scalarMin48O(dt,actualMaximumStep433B68);
    if(!actualZero433B10)return fail(S::binding,0x166fc4u);
    o.piece.impulseC0=*actualZero433B10;r.impulseCleared=true;
    while(elapsed<dt){
        if(!(step>0.f))return fail(S::arithmetic,0x1671a0u); //native nonprogress admission, never loop truncation
        r.forces=applyOriginalWreckForces50LA(o.physical,o.piece,o.gravity,o.actualWaterB0,o.actualWaterGlobals);
        if(r.forces.status!=OriginalWreckForceStatus50LA::complete)return fail(S::forces,r.forces.sourcePC);
        r.response=advanceOriginalWreckContact2964C0_50LA(o,g.pointContactSentinel4435E0,g.generatedContactSentinel4435E8);
        if(r.response.status!=OriginalWreckResponseStatus50LA::complete)return fail(S::contactResponse,r.response.sourcePC);
        for(unsigned i=0;i<3;++i){
            const float actual=o.physical.body.root.accumulator1c0.v[i],prior=o.piece.impulseC0.v[i];
            if(!admitted(actual)||!admitted(prior))return fail(S::arithmetic,0x166ff8u+0x48u*i);
            if(!(std::fabs(actual)<=std::fabs(prior)))o.piece.impulseC0.v[i]=actual;
        }
        if(!workspace)return fail(S::binding,0x1670d4u);
        r.integration=integrateOriginalWreckBody50LA(o.physical,o.bodyComponent(),o.assets,o.visibility,step,*workspace);
        if(r.integration.status!=OriginalWreckIntegrationStatus50LA::complete)return fail(S::integration,r.integration.sourcePC);
        float magnitude=0.f;
        if(!angularLength(o.physical.body.root.body.linearAcceleration,magnitude))return fail(S::arithmetic,0x1670e4u);
        o.piece.measuredLinearAcceleration124=detail::scalarMax48O(o.piece.measuredLinearAcceleration124,magnitude);
        if(!angularLength(o.physical.body.root.body.angularAcceleration,magnitude))return fail(S::arithmetic,0x167130u);
        o.piece.measuredAngularAcceleration128=detail::scalarMax48O(o.piece.measuredAngularAcceleration128,magnitude);
        const float next=add(elapsed,step);
        if(!admitted(next)||!(next>elapsed))return fail(S::arithmetic,0x167170u);
        elapsed=next;
        const float left=sub(dt,elapsed);if(!admitted(left))return fail(S::arithmetic,0x16717cu);
        step=detail::scalarMin48O(actualMaximumStep433B68,left);++r.substepsCommitted;
    }
    r.movement=moveOriginalWreck166C60_50LA(o,g,*actualZero433B10,flightGlobals);
    if(r.movement.status!=S::complete)return fail(r.movement.status,r.movement.sourcePC);
    for(unsigned i=0;i<2;++i){
        r.query=o.queries.prepareModelQuery(i);original_wreck_flight_detail50LA::projection(o.physical,o.assets.graph());
        if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::staging,r.query.sourcePC);
        r.query=o.queries.update(i);if(r.query.status!=OriginalWreckQueryStatus50LA::complete)return fail(S::registration,r.query.sourcePC);
    }
    if(o.piece.flight34){
        if(o.physical.spring.contact3C)o.piece.airborneTime11C=0.f;
        else{
            if(!admitted(o.piece.airborneTime11C))return fail(S::arithmetic,0x1671ecu);
            const float next=add(o.piece.airborneTime11C,dt);if(!admitted(next))return fail(S::arithmetic,0x1671f0u);
            o.piece.airborneTime11C=next;
        }
        r.timerCommitted=true;
        if(!admitted(o.piece.airborneTime11C)||!admitted(o.piece.flightDelay120))return fail(S::arithmetic,0x167200u);
        if(!(o.piece.airborneTime11C<=o.piece.flightDelay120))o.piece.flight38=1;
    }
    r.status=S::complete;r.sourcePC=0x167218u;return r;
}
} // namespace rr::game
