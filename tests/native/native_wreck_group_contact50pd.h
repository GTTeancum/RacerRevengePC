#pragma once
// Stage F5: the original wreck-group contact solver for the native debris sim
// (docs/pod-destruction-20261108.md, work/scratch/wreck-breakoff/PLAN.md s2, s7).
//
// 2CA840 nonflight (2CA9C4..2CAD50) for a group root and its attached members,
// built from the existing source-reviewed numeric leaves:
//  * D8 detector 295630 -> 296490 reset -> 11FF60 prepare (11F680 sphere) ->
//    121920 shape2 world query with the 294BB0/2963E0 static callback, per
//    piece E0 (its m.COL type2 rays) at the piece's model pose
//    (produceIndexedStaticType2Contacts50CC, the 49X/50CC kernel);
//  * 167250 forces (gravity 22C610, quadratic drag dragEC, rate damping F0);
//  * 2964C0 generated-record response for the root DC and then every attached
//    member DC, all bound to the root body (295C80 rebinds member DC.30):
//    296884 depth, 296F10 contact velocity, 297610 impact (applySpringImpact48K)
//    and 297390 friction (applySpringFriction48J); 1660F0 leaves spring force,
//    secondary and point contacts off;
//  * 2CAB08..2CABD8 per-axis largest |1C0| retention into primary C0;
//  * 167C40 integration (updateRootBody48H) and the 124/128 maxima;
//  * 167640 settling (settleOriginalWreckBody50LA) on the root DC.3C.
// DC tuning is 295A40 -> 1660F0 -> 22C310 (tuneOriginalWreckPiece22C310_50LA):
// restitution 0, friction by material [0] = [Wreckage] Friction (others 0),
// position correction 0.005, force torque scale 0, friction torque scale 1.
//
// STAND-INS (not proven from the original, flagged):
//  * Candidate order is the live track candidate storage order, pre-selected by
//    the 126EB0 sphere gap; the original walks its E0 zone SearchLists
//    (121920 passes 1/2). Group-type, trigger (E8 callback) and native trigger
//    owners are never dispatched (the original would recurse into groups and
//    run trigger E8 responses).
//  * Matrix/vector helper arithmetic below (point, direction, inverse direction
//    by transpose) follows the kernels' operation order where known; the
//    28BBB0/28B9B0 graph getters' exact rounding is not reproduced.
//  * Water (2B10F0 buoyancy in 167250) is not modelled for debris.
// Memory: everything is host-side in this file's Workspace and the sim's
// Group/LivePiece records. Every array write is bounds-checked against the
// fixed capacities below first; a query that would not fit is refused and
// logged, never truncated.
#include "../../src/game/indexed_type2_contact50cc.h"
#include "../../src/game/spring_impulse48k.h"
#include "../../src/game/spring_contact48j.h"
#include "../../src/game/body_wrapper48h.h"
#include "../../src/game/query_prepare48o.h"
#include "../../src/game/original_wreck_settling50la.h"
#include "../../src/game/original_native_collision_categories.h"
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <span>

namespace native_wreck_debris50PD::f5 {
namespace g=rr::game;
using V3=g::Vector3_48C;

// 295C20 allocates 50 generated records per DC (generatedCapacity88).
inline constexpr std::size_t recordCapacity88=50;
// Native workspace bounds (checked before any write; larger requests refused).
inline constexpr std::size_t maxRays=1024,maxCandidates=512,stackCapacity=4096,maxDcs=16;

// Shared E0 query parameters for this race, bound once at race activation
// from the live pod query (same collision/group type tags and the 3E0728
// skip, 3E0718 dynamic and 3E0720 material masks as every static query).
struct World {
    bool bound=false;
    std::uint8_t collisionTypeTag=0,groupTypeTag=0;
    std::uint32_t skipMask=0,dynamicMask=0,materialMask=0;
    // E0 filter (11B190/166050/22C214): category PHYSICSBODY, accept all,
    // category mode 1 excluding WATER, flags31 bits 0/1, F8 bit1 (11B570).
    g::CollisionFilter48J filter{};
};
inline World& world() noexcept { static World owner; return owner; }
// Allocates the filter's one-entry category list: call at race activation only.
inline void bindWorld(std::uint8_t collisionTypeTag,std::uint8_t groupTypeTag,std::uint32_t skipMask,
    std::uint32_t dynamicMask,std::uint32_t materialMask) {
    using C=g::OriginalNativeCollisionCategories;
    auto& w=world();w.collisionTypeTag=collisionTypeTag;w.groupTypeTag=groupTypeTag;
    w.skipMask=skipMask;w.dynamicMask=dynamicMask;w.materialMask=materialMask;
    w.filter=g::CollisionFilter48J{};w.filter.identity=0xf5d00000u;w.filter.categoryA4=C::physicsBody;
    w.filter.acceptMaskB8=0xffffffffu;w.filter.categoryModeD4=1;w.filter.categoryMasks.assign(1,C::water);
    w.filter.flagsF8=2u;w.filter.flags31=3u;w.bound=true;
}

// One piece's DC (spring collision model) for one group frame.
struct Dc {
    g::SpringRecordState48N state{};
    std::array<g::SpringRecord48N,recordCapacity88> records{};
    std::size_t positive=0;   // records with positive depth in the last response
};
// The group's root body (piece 0's D0 carrying the aggregate).
struct Body {
    g::RootBodyState48H root{};          // scene = root model world; B0 local
    g::BodyParameters48C parameters{};   // mass/inverse mass/inverse inertia (2CA620 -> 78/7C)
    g::BodyMatrix48G inertia100{};       // current aggregate inertia (diagonal sums)
    V3 center1E0{};                      // root-local centre (unweighted mean, 2CA620)
    float cachedSpeed34=0.f;             // 167250 speed
    bool enabled204=true;
    g::SpringImpactParameters48K impact{};
    std::array<float,10> frictionByMaterial{};
    float frictionTorqueScaleC4=1.f;
    // Primary piece fields (piece 1659C0 + 22C310 overrides).
    float dragEC=0.f,rateDampingF0=0.f,baseDrag100=0.f,baseRateDamping104=0.f;
    float finalRateDamping108=200.f,finalDrag10C=50.f,changeWindow130=2.f;
    float stopSpeed134=10.f,stopAngularSpeed138=.5f,stopLinearAcceleration13C=10.f,stopAngularAcceleration140=1.f;
    float contactTime12C=0.f,measured124=0.f,measured128=0.f;
    V3 impulseC0{};                      // primary piece C0 (per-axis peak 1C0)
    std::array<float,4> previous80{{0.f,0.f,0.f,1.f}};
    bool previousSet=false;
};

struct Stats { std::uint64_t queries=0,refused=0,candidates=0,skippedGroups=0,records=0,saturated=0,positive=0; };
inline Stats& stats() noexcept { static Stats owner; return owner; }

struct Workspace {
    std::array<g::SpringCumulativePair48N,maxRays> pairsIn{},pairsScratch{},pairsOut{};
    std::array<g::Type2PairMetadata49X,maxRays> metaIn{},metaScratch{},metaOut{};
    std::array<g::SpringRecord48N,recordCapacity88> recordsScratch{},recordsOut{};
    std::array<g::TerrainCandidateEffects48J,maxCandidates> effectsScratch{},effectsOut{};
    std::array<g::Type2WorkItem49X,stackCapacity> stack{};
    std::array<std::size_t,maxCandidates> indices{};
    std::array<Dc,maxDcs> dc{};          // root = 0, attached member i = i + 1
};
inline Workspace& workspace() noexcept { static Workspace owner; return owner; }

namespace detail {
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {volatile float r=a/b;return keep(r);}
    V3 point(const g::BodyMatrix48G& m,const V3& p) noexcept {
        V3 o{};for(unsigned i=0;i<3;++i)o.v[i]=add(add(add(mul(m.v[i],p.v[0]),mul(m.v[4+i],p.v[1])),mul(m.v[8+i],p.v[2])),m.v[12+i]);return o;
    }
    V3 direction(const g::BodyMatrix48G& m,const V3& p) noexcept {
        V3 o{};for(unsigned i=0;i<3;++i)o.v[i]=add(add(mul(m.v[i],p.v[0]),mul(m.v[4+i],p.v[1])),mul(m.v[8+i],p.v[2]));return o;
    }
    V3 inverseDirection(const g::BodyMatrix48G& m,const V3& p) noexcept { // rotation transpose
        V3 o{};for(unsigned i=0;i<3;++i)o.v[i]=add(add(mul(m.v[4*i],p.v[0]),mul(m.v[4*i+1],p.v[1])),mul(m.v[4*i+2],p.v[2]));return o;
    }
    V3 cross(const V3& a,const V3& b) noexcept {
        V3 o{};for(unsigned i=0;i<3;++i)o.v[i]=sub(mul(a.v[(i+1)%3],b.v[(i+2)%3]),mul(a.v[(i+2)%3],b.v[(i+1)%3]));return o;
    }
    float dot(const V3& a,const V3& b) noexcept {return add(add(mul(a.v[0],b.v[0]),mul(a.v[1],b.v[1])),mul(1.f,mul(a.v[2],b.v[2])));}
    float length(const V3& a) noexcept { // 167250 scalar length order (Y*Y, X*X, +Z*Z)
        const float y=mul(a.v[1],a.v[1]),x=mul(a.v[0],a.v[0]),xy=add(x,y),z=mul(a.v[2],a.v[2]),s=add(xy,z);
        volatile float r=static_cast<float>(std::sqrt(static_cast<double>(s)));return keep(r);
    }
    // 14E910 cofactor inverse (same order as original_wreck_tuning50la.h);
    // v15 stays zero, as the source writes it.
    g::BodyMatrix48G inverse(const g::BodyMatrix48G& m) noexcept {
        const float a=m.v[0],b=m.v[4],c=m.v[8],d=m.v[1],e=m.v[5],f=m.v[9],gg=m.v[2],h=m.v[6],i=m.v[10];
        const float c00=sub(mul(e,i),mul(f,h)),ci=sub(mul(b,i),mul(c,h)),c02=sub(mul(b,f),mul(c,e));
        float det=sub(mul(a,c00),mul(d,ci));det=add(det,mul(gg,c02));g::BodyMatrix48G out{};
        if(det==0.f)return out;
        const float u=div(1.f,det);
        out.v[0]=mul(u,c00);out.v[1]=mul(u,-(sub(mul(d,i),mul(f,gg))));out.v[2]=mul(u,sub(mul(d,h),mul(e,gg)));
        out.v[4]=mul(u,-ci);out.v[5]=mul(u,sub(mul(a,i),mul(c,gg)));out.v[6]=mul(u,-(sub(mul(a,h),mul(b,gg))));
        out.v[8]=mul(u,c02);out.v[9]=mul(u,-(sub(mul(a,f),mul(c,d))));out.v[10]=mul(u,sub(mul(a,e),mul(b,d)));
        return out;
    }
};
inline V3 xyz(const g::SpringVector4_48N& a) noexcept {V3 v;std::memcpy(v.v,a.v,12);return v;}
}

// 295A40 -> 1660F0 -> 22C310 DC response settings for a wreck piece.
inline void tuneDc(Body& b,float friction) noexcept {
    auto& a=b.impact;a=g::SpringImpactParameters48K{};
    a.normalForceScale98=1.f;a.angularImpulseScaleB0=1.f;a.forceTorqueScaleB4=0.f;  // 22C574 overrides 1660F0's 1
    a.restitutionB8=a.restitutionBC=a.restitutionC0=0.f;                             // 22C4D8
    a.positionCorrectionScaleCC=std::bit_cast<float>(0x3ba3d70au);a.angularCorrectionScaleD0=0.f; // 1660F0
    a.disableLeverArms5C=false;a.correctPenetration64=true;a.yawOnly6C=false;a.customCenter70=false;
    b.frictionByMaterial.fill(0.f);b.frictionByMaterial[0]=friction;b.frictionTorqueScaleC4=1.f;
}
inline void freshDc(Dc& dc) noexcept {
    dc.state=g::SpringRecordState48N{};dc.state.generatedCapacity88=static_cast<std::int32_t>(recordCapacity88);dc.positive=0;
}
// 2CA620 -> root virtual 78 (mass) and 7C (inertia, 14E910 inverse).
inline bool applyMassInertia(Body& b,float mass,const g::BodyMatrix48G& inertia,const V3& center) noexcept {
    detail::Math f;
    const auto inverse=f.inverse(inertia);
    const float inverseMass=mass>0.f?f.div(1.f,mass):b.parameters.inverseMass;
    if(!f.valid)return false;
    if(mass>0.f){b.parameters.mass=mass;b.parameters.inverseMass=inverseMass;}
    for(unsigned c=0;c<3;++c)for(unsigned r=0;r<3;++r)b.parameters.inverseInertiaColumns[c].v[r]=inverse.v[c*4+r];
    b.inertia100=inertia;b.center1E0=center;return true;
}

// D8/295630 for one piece: reset its DC records (296490), prepare its E0 at
// the piece's model world (11F680 sphere) and run the shape2 world query with
// the static 294BB0 callback. Returns false (logged) on any refused request.
inline bool detect(const g::Type2ResourceView49X& e0,const g::BodyMatrix48G& model,std::uint32_t identity,
    Dc& dc,std::span<g::Type2StaticCandidate49X> track) noexcept {
    auto& w=world();auto& ws=workspace();auto& st=stats();
    if(!w.bound||!e0.rayCount||e0.rayCount>maxRays){++st.refused;return false;}
    g::QuerySpherePrepareInput48O in{};in.preparedScene=model;in.shapeType=2;
    if(e0.root&g::type2Leaf49X){
        const auto ray=static_cast<std::size_t>(e0.root&~g::type2Leaf49X);if(ray>=e0.rayCount){++st.refused;return false;}
        in.first=e0.rays[ray].first;in.second=e0.rays[ray].second;in.pairCount=1;in.hasPairTree=false;
    }else{
        if(e0.root>=e0.nodeCount){++st.refused;return false;}
        in.hasPairTree=true;const auto& root=e0.nodes[e0.root];
        for(unsigned a=0;a<3;++a){in.treeCenter.v[a]=root.center[a];in.treeHalf.v[a]=root.half[a];}
    }
    g::QuerySpherePrepareOutput48O prepared{};
    if(g::prepareQuerySphere48O(in,prepared)!=g::QuerySpherePrepareStatus48O::Ready){++st.refused;return false;}
    w.filter.identity=identity;
    g::Type2StaticQuery49X q{};q.resource=&e0;q.worldMatrix=prepared.world;q.filter=&w.filter;
    for(unsigned a=0;a<3;++a)q.sphere.center.v[a]=prepared.center.v[a];q.sphere.radius=prepared.radius;
    q.copied40={0.f,0.f,0.f,1.f};q.copied50={0.f,0.f,0.f,1.f};q.queryScene={identity};
    q.skipMask=w.skipMask;q.dynamicMask=w.dynamicMask;q.materialMask=w.materialMask;
    q.collisionTypeTag=w.collisionTypeTag;q.groupTypeTag=w.groupTypeTag;q.callbackEnabled=true;q.resetRecordsBefore=true;
    // Candidate pre-selection (STAND-IN for the zone SearchList walk): the
    // kernel repeats the exact 126EB0 gap test, this one is a superset.
    std::size_t count=0;
    for(std::size_t i=0;i<track.size();++i){
        const auto& t=track[i];const auto& c=t.candidate.collision;
        if(c.typeTag!=w.collisionTypeTag){if(c.typeTag==w.groupTypeTag)++st.skippedGroups;continue;}
        if(c.hasResponseCallbackE8||t.gate||t.nativeTriggers||t.nativeTriggersTC||t.nativeTriggersTB||
           t.nativeTriggersMC||t.nativeTriggersGB)continue;
        if(t.shapeType==0){if(!c.geometry.mesh)continue;}else if(t.shapeType!=1||!t.compound)continue;
        const float dx=q.sphere.center.v[0]-c.sphere.center.v[0],dy=q.sphere.center.v[1]-c.sphere.center.v[1],
            dz=q.sphere.center.v[2]-c.sphere.center.v[2],reach=q.sphere.radius+c.sphere.radius+1.f;
        if(dx*dx+dy*dy+dz*dz>reach*reach)continue;
        if(count>=maxCandidates){++st.refused;return false;}
        ws.indices[count++]=i;
    }
    ++st.queries;st.candidates+=count;
    g::Type2StaticInitial49X initial{};initial.records=dc.state;
    initial.pairs=ws.pairsIn.data();initial.metadata=ws.metaIn.data();initial.pairSlots=e0.rayCount;
    initial.recordSlots=dc.records.data();initial.recordCount=recordCapacity88;
    const g::Type2StaticWorkspace49X work{ws.pairsScratch.data(),maxRays,ws.metaScratch.data(),maxRays,
        ws.recordsScratch.data(),recordCapacity88,ws.effectsScratch.data(),maxCandidates,ws.stack.data(),stackCapacity,nullptr,0};
    const g::Type2StaticDestination49X destination{ws.pairsOut.data(),maxRays,ws.metaOut.data(),maxRays,
        ws.recordsOut.data(),recordCapacity88,ws.effectsOut.data(),maxCandidates};
    g::Type2StaticResult49X result{};
    const auto status=g::produceIndexedStaticType2Contacts50CC(q,track.data(),track.size(),ws.indices.data(),count,
        initial,work,destination,result);
    if(status!=g::Type2StaticStatus49X::Complete){++st.refused;return false;}
    if(result.records.generatedCount84<0||result.records.generatedCount84>static_cast<std::int32_t>(recordCapacity88)){
        ++st.refused;return false;}
    dc.state=result.records;dc.records=ws.recordsOut;st.records+=static_cast<std::size_t>(dc.state.generatedCount84);
    st.saturated+=result.saturated;
    return true;
}

// 167250 on the root body (no water owner for debris).
inline bool forces(Body& b,float mass,float gravityAcceleration) noexcept {
    detail::Math m;auto& r=b.root;auto force=r.loads.force;auto torque=r.loads.torque;
    const float gy=m.mul(-mass,gravityAcceleration);
    force.v[1]=m.add(force.v[1],gy);force.v[0]=m.add(force.v[0],0.f);force.v[2]=m.add(force.v[2],0.f); // 4431C0 XZ = 0
    const auto& v=r.body.velocity.v;
    const float sq=m.add(m.add(m.mul(v[0],v[0]),m.mul(v[1],v[1])),m.mul(1.f,m.mul(v[2],v[2])));
    float q=3.40282347e38f;if(sq!=0.f){volatile float root=std::sqrt(std::fabs(sq));q=m.div(1.f,root);}
    const float speed=m.length(r.body.velocity);
    const float drag=m.mul(speed,m.mul(-b.dragEC,speed));
    for(unsigned i=0;i<3;++i)force.v[i]=m.add(force.v[i],m.mul(drag,m.mul(v[i],q)));
    for(unsigned i=0;i<3;++i)torque.v[i]=m.add(torque.v[i],m.mul(-b.rateDampingF0,r.body.angularVelocity.v[i]));
    if(!m.valid)return false;
    b.cachedSpeed34=speed;r.loads.force=force;r.loads.torque=torque;return true;
}

// 2964C0 generated-record route of one DC bound to the root body. recordScene
// is the model world of the piece whose E0 produced the records (the cached
// matrix, as 296884/2968CC read it). Positive records run serially.
inline bool respond(Body& b,Dc& dc,const g::BodyMatrix48G& recordScene) noexcept {
    auto& s=dc.state;dc.positive=0;
    if(s.generatedCount84<=0){s.pending38=0;return true;}
    if(s.generatedCapacity88<s.generatedCount84||static_cast<std::size_t>(s.generatedCapacity88)>dc.records.size())return false;
    detail::Math m;
    const V3 center=m.point(b.root.scene,b.center1E0);
    if(!m.valid)return false;
    b.impact.worldCenter110=center;
    for(std::int32_t i=0;i<s.generatedCount84;++i){
        auto& record=dc.records[static_cast<std::size_t>(i)];
        const V3 world=m.point(recordScene,detail::xyz(record.localPoint10));
        const V3 normal=detail::xyz(record.normal20),surface=detail::xyz(record.surfacePoint30);
        V3 separation{};for(unsigned a=0;a<3;++a)separation.v[a]=m.sub(surface.v[a],world.v[a]);
        const float depth=m.dot(separation,normal);
        if(!m.valid)return false;
        if(depth<=0.f){record.flags50=static_cast<std::uint8_t>(record.flags50&~2u);continue;}
        if(record.other44.value)return false;                    // dynamic body: never produced by the static callback
        s.contact3C=1;s.generated78=1;if(normal.v[1]>.8f)s.upward40=1;
        record.flags50=static_cast<std::uint8_t>(record.flags50|2u);++dc.positive;
        V3 point{};for(unsigned a=0;a<3;++a)point.v[a]=m.add(world.v[a],m.mul(depth,normal.v[a]));
        V3 arm{};for(unsigned a=0;a<3;++a)arm.v[a]=m.sub(point.v[a],center.v[a]);
        const V3 local=m.inverseDirection(b.root.scene,arm);
        const V3 spin=m.cross(b.root.body.angularVelocity,local);
        const V3 rotational=m.direction(b.root.scene,spin);
        V3 velocity{};for(unsigned a=0;a<3;++a)velocity.v[a]=m.sub(m.add(rotational.v[a],b.root.body.velocity.v[a]),0.f);
        if(!m.valid)return false;
        b.impact.contactVelocity120=velocity;
        {   // 297610 impact (impact54)
            g::SpringImpactBody48K body{};body.state=b.root;body.parameters=b.parameters;body.localCenter=b.center1E0;
            g::SpringImpactResult48K next{};
            if(!g::applySpringImpact48K(normal,point,depth,b.impact,body,nullptr,next))return false;
            auto& r=b.root;const auto& n=next.primary;
            r.accumulator1c0=n.accumulator1c0;r.body.velocity=n.body.velocity;
            r.accumulator1d0=n.accumulator1d0;r.body.angularVelocity=n.body.angularVelocity;
            r.loads=n.loads;r.positionCorrection1a0=n.positionCorrection1a0;r.rotationCorrection1b0=n.rotationCorrection1b0;
        }
        {   // 297390 friction (friction50), same cached pre-impact velocity
            if(record.material4C>=b.frictionByMaterial.size())return false;
            const g::SpringPointForceParameters48J force{b.center1E0,b.impact.customLocalCenter100,b.frictionTorqueScaleC4,false,false};
            const g::SpringFrictionBody48J body{b.root.scene,b.root.loads,b.parameters.mass};
            g::SpringFrictionResult48J next{};
            if(!g::applySpringFriction48J(normal,point,velocity,b.frictionByMaterial[record.material4C],force,body,nullptr,next))return false;
            b.root.loads=next.primary;
        }
    }
    s.pending38=0;stats().positive+=dc.positive;return true;
}

// 2CAB08..2CABD8: keep the larger |1C0| per axis in the primary's C0.
inline void retain(Body& b) noexcept {
    for(unsigned a=0;a<3;++a){const float actual=b.root.accumulator1c0.v[a];
        if(!(std::fabs(actual)<=std::fabs(b.impulseC0.v[a])))b.impulseC0.v[a]=actual;}
}
// 167C40 then the 124/128 maxima (2CAC14..2CACC0).
inline bool integrate(Body& b,float step) noexcept {
    g::RootBodyState48H next{};
    if(!g::updateRootBody48H(b.root,b.parameters,b.center1E0,true,b.enabled204,step,next))return false;
    b.root=next;float magnitude=0.f;
    using namespace g::original_wreck_settling_detail50LA;
    if(!angularLength(b.root.body.linearAcceleration,magnitude))return false;
    b.measured124=g::detail::scalarMax48O(b.measured124,magnitude);
    if(!angularLength(b.root.body.angularAcceleration,magnitude))return false;
    b.measured128=g::detail::scalarMax48O(b.measured128,magnitude);
    return true;
}
// 167640 on the primary (flight34/38 are 0 for a group root).
inline bool settle(Body& b,const g::SpringRecordState48N& rootDc,float dt) noexcept {
    const std::uint32_t zero=0;
    const g::OriginalWreckSettlingBinding50LA binding{zero,zero,rootDc,b.root.body,b.cachedSpeed34,b.enabled204,
        b.contactTime12C,b.dragEC,b.rateDampingF0,b.baseDrag100,b.baseRateDamping104,b.finalRateDamping108,b.finalDrag10C,
        b.changeWindow130,b.stopSpeed134,b.stopAngularSpeed138,b.stopLinearAcceleration13C,b.stopAngularAcceleration140,
        b.measured124,b.measured128};
    return g::settleOriginalWreckBody50LA(binding,dt).status==g::OriginalWreckSettlingStatus50LA::complete;
}

} // namespace native_wreck_debris50PD::f5
