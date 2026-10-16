#pragma once
#include "original_wreck_forces50la.h"
#include "original_wreck_integration50la.h"
#include "original_wreck_movement_query50la.h"
#include "scalar_minmax48o.h"
#include "vehicle_visual49r.h"

namespace rr::game {
// Application-owned shared lazy globals, never one copy per piece.
struct OriginalWreckFlightGlobals50LA {
    std::uint8_t initializedGp852C=0,initializedGp8530=0;
    float minimumSeparation433B50=0.f,stopNormalSpeed433B58=0.f;
    //Actual28ADD0 lazy gp8C34/4433E8. All other application callers of
    //that setter must share this same projection; no per-piece replacement.
    VisualLazyWord49R positionSentinel4433E8{};
};
enum class OriginalWreckFlightStatus50LA : std::uint8_t {
    complete,owner,graph,forces,integration,query,arithmetic,angularQueryScratch,visibility
};
struct OriginalWreckFlightResult50LA {
    OriginalWreckFlightStatus50LA status=OriginalWreckFlightStatus50LA::owner;
    std::uint32_t sourcePC=0;
    bool globalsCommitted=false,forcesCommitted=false,integrationCommitted=false;
    bool contactCleared=false,hitPrefixCommitted=false,positionCommitted=false;
    OriginalWreckForceResult50LA forces{};
    OriginalWreckIntegrationResult50LA integration{};
    OriginalWreckMovementQueryResult50LA query{};
    OriginalWreckVisibilityResult50LA visibility{};
};
namespace original_wreck_flight_detail50LA {
using M=original_wreck_force_detail50LA::Math;
inline float dot(M& m,const Vector3_48C& a,const Vector3_48C& b) noexcept {
    const float x=m.mul(a.v[0],b.v[0]),y=m.mul(a.v[1],b.v[1]),z=m.mul(a.v[2],b.v[2]);
    //106E70: (x+y)+(1*z), matching the existing48K/50EO VU reduction.
    return m.add(m.add(x,y),m.mul(1.f,z));
}
inline Vector3_48C xyz(const SpringVector4_48N& a) noexcept {return {{a.v[0],a.v[1],a.v[2]}};}
inline bool position(const SceneGraph49P& graph,SceneNodeIndex49P model,SpringVector4_48N& out) noexcept {
    if(!refreshSceneWorld49P(graph,model))return false;
    const auto& world=graph.nodes[static_cast<std::uint32_t>(model)].world100;
    //28B1A0(null) writes only XYZ; source localC0/D0 began asVF0 withW1.
    std::memcpy(out.v,world.v+12,12);return true;
}
inline void projection(OriginalWreckPhysical50LA& p,const SceneGraph49P& graph) noexcept {
    const auto& n=graph.nodes[static_cast<std::uint32_t>(p.model288)];
    std::memcpy(&p.body.root.scene,&n.world100,sizeof p.body.root.scene);
    p.body.sceneFlags190=(p.body.sceneFlags190&0xffffff00u)|n.sceneFlags190;
}
}
// Actual166790 flying branch: real167250 forces,167C40 body/observer, then
//the original repeated E8 movement-query/correction loop. It does not invent
//one query per frame, discard later hits, copy piece80/90 as stack endpoints,
//or cap the source loop. Every iteration retains the SAME pre-integration
//start and obtains the current actual model translation. E0/E4 update and
//settling/effects belong to the actual callers and are deliberately not here.
//
//Admitted query closure is genuine121920(0), including real filter and native
//geometry. Explicit122660 lists and reached unsupported candidate methods
//remain reported by the E8 adapter. A hit that keeps flight38 nonzero uses
//the genuinely produced final query VF9.W for28BBB0's persistent B0.W, then
//performs the original angular scale and repeated correction. Unknown query
//scratch still guards the exact reached transform; it is never supplied by
//the caller, inferred from selectedNormal.W or treated as a successful step.
//
//A failure can retain earlier source stages. Do not replay the whole frame.
//Strict normal-or-zero finite consumed arithmetic, FE_TONEAREST, nonaliasing
//workspace and shared serial owners; these are arithmetic admission limits,
//not a substitute physics/query/effect provider.
inline OriginalWreckFlightResult50LA advanceOriginalWreckFlight50LA(
    OriginalWreckPhysical50LA& p,OriginalWreckPieceFields50LA& piece,
    const OriginalWreckComponent50LA& pieceComponent,const OriginalWreckComponent50LA& bodyComponent,
    OriginalWreckAssets50LA& assets,OriginalWreckVisibility50LA& visibility,
    OriginalWreckQueries50LA& queries,OriginalWreckGravity50LA& gravity,
    const WaterHeightProvider50JC* actualWaterB0,const WaterHeightGlobals50JC* actualWaterGlobals,
    OriginalWreckFlightGlobals50LA& globals,float dt,GraphBodyWorkspace49W& workspace) noexcept {
    using namespace original_wreck_flight_detail50LA;
    using S=OriginalWreckFlightStatus50LA;OriginalWreckFlightResult50LA r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    if(!assets.prepared()||!visibility.boundTo(assets)||p.model288!=assets.model14C()||
       pieceComponent.kind!=OriginalWreckComponentKind50LA::piece2CB4B0||
       bodyComponent.parent1C!=&pieceComponent||gravity.target50!=&p||
       p.contactBody30!=&p.body)return fail(S::owner,0x166790u);
    const auto graph=assets.graph();const auto index=static_cast<std::int32_t>(p.model288);
    if(!validateSceneGraph49P(graph)||index<0||static_cast<std::uint32_t>(index)>=graph.count||
       graph.nodes[index].parent180!=SceneNodeIndex49P::none)return fail(S::graph,0x166838u);
    if(!globals.initializedGp852C){globals.minimumSeparation433B50=original_wreck_physical_detail50LA::word(0x3ccccccdu);globals.initializedGp852C=1;}
    if(!globals.initializedGp8530){globals.stopNormalSpeed433B58=50.f;globals.initializedGp8530=1;}
    r.globalsCommitted=true;
    SpringVector4_48N before{{0.f,0.f,0.f,1.f}},current{{0.f,0.f,0.f,1.f}};
    if(!position(graph,p.model288,before))return fail(S::graph,0x166840u);
    projection(p,graph);
    r.forces=applyOriginalWreckForces50LA(p,piece,gravity,actualWaterB0,actualWaterGlobals);
    if(r.forces.status!=OriginalWreckForceStatus50LA::complete)return fail(S::forces,r.forces.sourcePC);
    r.forcesCommitted=true;
    r.integration=integrateOriginalWreckBody50LA(p,bodyComponent,assets,visibility,dt,workspace);
    if(r.integration.status!=OriginalWreckIntegrationStatus50LA::complete)return fail(S::integration,r.integration.sourcePC);
    r.integrationCommitted=true;
    p.spring.contact3C=0;r.contactCleared=true;
    for(;;){
        if(!position(graph,p.model288,current))return fail(S::graph,0x166884u);
        projection(p,graph);
        r.query=queryOriginalWreckMovement50LA(queries,pieceComponent,before,current,piece.explicitCandidateCount148);
        if(r.query.status!=OriginalWreckMovementQueryStatus50LA::complete)return fail(S::query,r.query.sourcePC);
        if(!(queries.state(2).flagsF8&0x20u)){r.status=S::complete;r.sourcePC=0x166c50u;return r;}
        const auto& hit=queries.query(2).type3;
        p.spring.contact3C=1;piece.normalA0=hit.selectedNormal;r.hitPrefixCommitted=true;
        M m;const auto normal=xyz(piece.normalA0);
        const float normalSpeed=std::fabs(dot(m,normal,p.body.root.body.velocity));m.keep(globals.stopNormalSpeed433B58);
        if(!m.valid)return fail(S::arithmetic,0x16694cu);
        if(normalSpeed<globals.stopNormalSpeed433B58)piece.flight38=0;
        if(piece.flight38){
            const float speed=dot(m,p.body.root.body.velocity,normal);
            Vector3_48C projected{};for(unsigned i=0;i<3;++i)projected.v[i]=m.mul(speed,normal.v[i]);
            const float rebound=m.neg(m.add(1.f,piece.restitution110));
            auto impulse=m.scale(projected,rebound);auto velocity=p.body.root.body.velocity;
            m.addTo(velocity,impulse);Vector3_48C tangent{};
            for(unsigned i=0;i<3;++i)tangent.v[i]=m.sub(velocity.v[i],projected.v[i]);
            Vector3_48C cross{};
            cross.v[0]=m.sub(m.mul(normal.v[1],tangent.v[2]),m.mul(normal.v[2],tangent.v[1]));
            cross.v[1]=m.sub(m.mul(normal.v[2],tangent.v[0]),m.mul(normal.v[0],tangent.v[2]));
            cross.v[2]=m.sub(m.mul(normal.v[0],tangent.v[1]),m.mul(normal.v[1],tangent.v[0]));
            const auto normalized=m.normalize(cross);
            if(!m.valid)return fail(S::arithmetic,0x16696cu);
            std::memcpy(piece.impulseC0.v,impulse.v,12);piece.impulseC0.v[3]=1.f;
            p.body.root.body.velocity=velocity;p.body.root.body.angularVelocity=normalized;
            //166A7C VSUB.w VF6,VF6,VF6 ->+0;166AAC preserves thisW.
            p.body.angularVelocityB0W=0.f;
            if(!r.query.world.type3VectorScratchKnown)return fail(S::angularQueryScratch,0x166ac8u);
            detail::SceneVectorContext49U context{r.query.world.type3VectorScratchVF9W};
            VisualVector49R localAngular{};
            const VisualVector49R normalizedInput{{normalized.v[0],normalized.v[1],normalized.v[2],0.f}};
            if(!detail::inverseDirection49U(graph,p.model288,normalizedInput,context,localAngular))
                return fail(S::arithmetic,0x166ac8u);
            std::memcpy(p.body.root.body.angularVelocity.v,localAngular.v,12);
            p.body.angularVelocityB0W=localAngular.v[3];projection(p,graph);
            //166AF0 multiplies114*cached34 first;166AF4 then118*that.
            const float scale=m.mul(piece.inverseExtent118,m.mul(piece.frictionTorqueScale114,p.body.cachedSpeed34));
            Vector3_48C scaled{};for(unsigned i=0;i<3;++i)scaled.v[i]=m.mul(localAngular.v[i],scale);
            if(!m.valid)return fail(S::arithmetic,0x166af0u);
            p.body.root.body.angularVelocity=scaled; //XYZ only; preserve transformW.
        }
        Vector3_48C delta{};for(unsigned i=0;i<3;++i)delta.v[i]=m.sub(hit.selectedPoint.v[i],current.v[i]);
        const float signedDistance=dot(m,delta,normal);m.keep(globals.minimumSeparation433B50);
        const float distance=detail::scalarMax48O(signedDistance,globals.minimumSeparation433B50);
        const float twice=m.mul(2.f,distance);SpringVector4_48N corrected{{0.f,0.f,0.f,1.f}};
        for(unsigned i=0;i<3;++i)corrected.v[i]=m.add(current.v[i],m.mul(twice,normal.v[i]));
        if(!m.valid)return fail(S::arithmetic,0x166b8cu);
        //28ADDC..28ADF8 initializes the actual shared setter sentinel only
        //when this hit reaches28ADD0. No-hit calls leave both words untouched.
        if(!globals.positionSentinel4433E8.guard){
            globals.positionSentinel4433E8.word=0xffffffffu;
            globals.positionSentinel4433E8.guard=1;
        }
        //28ADD0(null), parentless: writes only localF0/F4/F8, then the same
        //28C4F0 + ready-gated289A20 sequence as setLocalMatrix49P.
        auto local=graph.nodes[index].localC0;std::memcpy(local.v+12,corrected.v,12);
        if(!setLocalMatrix49P(graph,p.model288,local))return fail(S::graph,0x166c10u);
        r.positionCommitted=true;projection(p,graph);
        r.visibility=visibility.update();
        if(r.visibility.status!=OriginalWreckVisibilityStatus50LA::complete)return fail(S::visibility,0x166c18u);
        projection(p,graph);
    }
}
}
