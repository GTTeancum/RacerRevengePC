#pragma once
#include "original_wreck_physical50la.h"
#include "water_height50jc.h"
#include <cmath>
#include <limits>

namespace rr::game {
// Actual282670/282750(1)/282790 one-target owner. It holds a borrowed pointer
//to the piece's canonical body, not another physical state. force40.Y is a
//real mutable source cache written by2827D0; its X/Z/W come from real4431C0.
struct OriginalWreckGravity50LA {
    std::uint32_t count30=0,capacity34=0;
    float acceleration38=0.f;
    SpringVector4_48N force40{};
    OriginalWreckPhysical50LA* target50=nullptr;
};
inline bool initializeOriginalWreckGravity50LA(OriginalWreckGravity50LA& g,
    OriginalWreckPhysical50LA& p,const OriginalWreckComponent50LA& component,
    const SpringVector4_48N& actualVector4431C0) noexcept {
    if(component.kind!=OriginalWreckComponentKind50LA::gravity282670||
       !component.objectKey||!p.bodyConstructorFieldsApplied||g.target50)return false;
    g.count30=0;g.capacity34=1;
    g.acceleration38=original_wreck_physical_detail50LA::word(0x4200cccdu);
    g.force40=actualVector4431C0;g.target50=&p;g.count30=1;return true;
}
namespace original_wreck_force_detail50LA {
using V=Vector3_48C;
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x)&&(x==0.f||std::isnormal(x));return x;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {keep(a);keep(b);volatile float r=a/b;return keep(r);}
    float neg(float a) noexcept {return keep(-a);}
    float square(const V& a) noexcept {
        const float x=mul(a.v[0],a.v[0]),y=mul(a.v[1],a.v[1]),z=mul(a.v[2],a.v[2]);
        return add(add(x,y),mul(1.f,z));
    }
    float scalarLength(const V& a) noexcept {
        const float y=mul(a.v[1],a.v[1]),x=mul(a.v[0],a.v[0]);
        const float xy=add(x,y),z=mul(a.v[2],a.v[2]),sum=add(xy,z);
        volatile float r=static_cast<float>(std::sqrt(static_cast<double>(sum)));return keep(r);
    }
    V normalize(const V& a) noexcept {
        const float squareLength=square(a);float q=std::numeric_limits<float>::max();
        if(squareLength!=0.f){volatile float r=std::sqrt(std::fabs(squareLength));q=div(1.f,keep(r));}
        V out{};for(unsigned i=0;i<3;++i)out.v[i]=mul(a.v[i],q);return out;
    }
    V scale(const V& a,float scalar) noexcept {
        V out{};for(unsigned i=0;i<3;++i)out.v[i]=mul(scalar,a.v[i]);return out;
    }
    void addTo(V& a,const V& b) noexcept {for(unsigned i=0;i<3;++i)a.v[i]=add(a.v[i],b.v[i]);}
};
}
enum class OriginalWreckForceStatus50LA : std::uint8_t {complete,owner,arithmetic,water};
struct OriginalWreckForceResult50LA {
    OriginalWreckForceStatus50LA status=OriginalWreckForceStatus50LA::owner;
    std::uint32_t sourcePC=0;
    bool gravityCommitted=false,airCommitted=false,waterFlagsCommitted=false,waterCommitted=false;
    WaterHeightResult50JC water{};
};
// Complete167250 for a genuine282750(1) D4 owner. waterB0 MUST be the actual
//piece's165C30 owner binding: null closes the source null branch; missing or
//unbound nonnull water remains an error, never a fabricated dry surface.
//A present owner invokes actual2B10F0 viaqueryWaterHeight50JC against canonical
//body60. No guessed height, constant waterline, callback or post-force receipt.
//
//Source f12 is unused. W lanes of force/torque are preserved. Stages commit in
//source order; a later water/arithmetic failure retains earlier gravity/air
//effects. A failed invocation must NOT be replayed from its beginning. The
//caller stops its actual frame and reports the reached dependency. Strict
//FE_TONEAREST, normal-or-zero finite consumed arithmetic, no contraction.
inline OriginalWreckForceResult50LA applyOriginalWreckForces50LA(
    OriginalWreckPhysical50LA& p,OriginalWreckPieceFields50LA& piece,
    OriginalWreckGravity50LA& gravity,const WaterHeightProvider50JC* waterB0,
    const WaterHeightGlobals50JC* waterGlobals) noexcept {
    using namespace original_wreck_force_detail50LA;
    using S=OriginalWreckForceStatus50LA;OriginalWreckForceResult50LA r;
    auto fail=[&](S s,std::uint32_t pc){r.status=s;r.sourcePC=pc;return r;};
    if(!p.bodyConstructorFieldsApplied||gravity.target50!=&p||gravity.count30!=1u||gravity.capacity34!=1u)
        return fail(S::owner,0x167280u);
    if(std::fegetround()!=FE_TONEAREST)return fail(S::arithmetic,0x167250u);
    Math m;auto force=p.body.root.loads.force;
    const float gy=m.mul(m.neg(p.parameters.body.mass),gravity.acceleration38);
    const V gv{{gravity.force40.v[0],gy,gravity.force40.v[2]}};
    m.addTo(force,gv);
    if(!m.valid)return fail(S::arithmetic,0x2827f4u);
    gravity.force40.v[1]=gy;p.body.root.loads.force=force;r.gravityCommitted=true;
    const auto direction=m.normalize(p.body.root.body.velocity);
    const float speed=m.scalarLength(p.body.root.body.velocity);
    const float drag=m.mul(speed,m.mul(m.neg(piece.dragEC),speed));
    m.addTo(force,m.scale(direction,drag));
    auto torque=p.body.root.loads.torque;
    m.addTo(torque,m.scale(p.body.root.body.angularVelocity,m.neg(piece.rateDampingF0)));
    if(!m.valid)return fail(S::arithmetic,0x167288u);
    p.body.cachedSpeed34=speed;p.body.root.loads.force=force;p.body.root.loads.torque=torque;r.airCommitted=true;
    if(!waterB0){piece.water40=0;piece.water3C=0;r.status=S::complete;r.sourcePC=0x167624u;return r;}
    if(!waterGlobals)return fail(S::water,0x167420u);
    float surface[3];std::memcpy(surface,p.body.root.body.position.v,sizeof surface);
    r.water=queryWaterHeight50JC(*waterB0,*waterGlobals,surface,nullptr);
    if(r.water.status!=WaterHeightStatus50JC::Complete)return fail(S::water,r.water.sourcePC);
    m.keep(surface[1]);const float bodyY=m.keep(p.body.root.body.position.v[1]);
    if(!m.valid)return fail(S::arithmetic,0x167434u);
    if(surface[1]<bodyY){piece.water40=0;piece.water3C=0;r.waterCommitted=true;r.status=S::complete;r.sourcePC=0x167618u;return r;}
    //167444..167464 publishes entry/wet state before consuming wet coefficients.
    piece.water3C=piece.water40?0u:1u;piece.water40=1u;r.waterFlagsCommitted=true;
    const auto waterDirection=m.normalize(p.body.root.body.velocity);
    const float waterDrag=m.mul(speed,m.mul(m.neg(piece.waterDragF4),speed));
    m.addTo(force,m.scale(waterDirection,waterDrag));
    m.addTo(torque,m.scale(p.body.root.body.angularVelocity,m.neg(piece.waterRateF8)));
    const float lift=m.mul(gravity.acceleration38,m.mul(piece.buoyancyFC,p.parameters.body.mass));
    //1675E4 also adds explicit+0 to X/Z; signed-zero effects are retained.
    m.addTo(force,V{{0.f,lift,0.f}});
    if(!m.valid)return fail(S::arithmetic,0x167468u);
    p.body.root.loads.force=force;p.body.root.loads.torque=torque;
    r.waterCommitted=true;r.status=S::complete;r.sourcePC=0x16762cu;return r;
}
}
