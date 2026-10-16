#pragma once
#include "original_wreck_physical50la.h"
#include "original_wreck_assets50la.h"
#include <cmath>

namespace rr::game {
// The real one-piece aggregate's saved physical authoring. Its primary3C is
//a borrow of piece0.D0, never a second physics body. No nonroot slot exists.
struct OriginalWreckSingleGroupFields50LA {
    std::uint32_t attachedCount30=0,capacity34=0;
    //2CA9DC clears this before the attached-detector loop;2CAA30 increments
    //for each actual reached attached detector. Constructor leaves it unwritten;
    //zero here is the native fresh-storage policy, not a completed frame.
    std::uint32_t detectedAttached38=0;
    OriginalWreckPhysical50LA* primary3C=nullptr;
    OriginalWreckPieceFields50LA* primary40=nullptr;
    float mass4C=0.f;
    SpringVector4_48N center50{};
    BodyMatrix48G inertia60{};
};
namespace original_wreck_tuning_detail50LA {
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float x=a*b;return keep(x);}
    float div(float a,float b) noexcept {keep(a);keep(b);volatile float x=a/b;return keep(x);}
    float neg(float a) noexcept {return keep(-a);}
    float root(float a) noexcept {
        keep(a);volatile float x=static_cast<float>(std::sqrt(static_cast<double>(a)));return keep(x);
    }
    //14E910, same cofactor/order as existing authored_geometry49O. Its inverse
    //has ZERO v15, including nonsingular input. Do not replace with1/diagonal
    //or a rigid-transform inverse: signed-zero cofactor lanes are observable.
    BodyMatrix48G inverse(const BodyMatrix48G& m) noexcept {
        const float a=m.v[0],b=m.v[4],c=m.v[8],d=m.v[1],e=m.v[5],f=m.v[9],g=m.v[2],h=m.v[6],i=m.v[10];
        const float c00=sub(mul(e,i),mul(f,h)),ci=sub(mul(b,i),mul(c,h)),c02=sub(mul(b,f),mul(c,e));
        float det=sub(mul(a,c00),mul(d,ci));det=add(det,mul(g,c02));BodyMatrix48G out{};
        if(det==0.f)return out;
        const float u=div(1.f,det);
        out.v[0]=mul(u,c00);out.v[1]=mul(u,neg(sub(mul(d,i),mul(f,g))));out.v[2]=mul(u,sub(mul(d,h),mul(e,g)));
        out.v[4]=mul(u,neg(ci));out.v[5]=mul(u,sub(mul(a,i),mul(c,g)));out.v[6]=mul(u,neg(sub(mul(a,h),mul(b,g))));
        out.v[8]=mul(u,c02);out.v[9]=mul(u,neg(sub(mul(a,f),mul(c,d))));out.v[10]=mul(u,sub(mul(a,e),mul(b,d)));
        return out;
    }
};
inline void inverseToParameters(const BodyMatrix48G& inverse,SecondaryBodyParameters49M& p) noexcept {
    for(unsigned c=0;c<3;++c)for(unsigned r=0;r<3;++r)
        p.body.inverseInertiaColumns[c].v[r]=inverse.v[c*4+r];
    constexpr unsigned indices[]={3,7,11,12,13,14,15};
    for(unsigned i=0;i<7;++i)p.inverseInertiaExtra[i]=inverse.v[indices[i]];
}
}
// Actual2CA020 root-only field binding. The surrounding group owner still
//performs its genuine constructor/effect work. The eight pointer-array
//allocations have zero capacity for this route; no synthetic child is added.
inline bool bindOriginalOnePieceGroup50LA(OriginalWreckSingleGroupFields50LA& g,
    OriginalWreckPhysical50LA& p,OriginalWreckPieceFields50LA& piece,
    OriginalWreckComponent50LA& pieceComponent) noexcept {
    if(!p.bodyConstructorFieldsApplied||g.primary3C||
       pieceComponent.kind!=OriginalWreckComponentKind50LA::piece2CB4B0)return false;
    g.attachedCount30=0;g.capacity34=0;g.primary3C=&p;g.primary40=&piece;
    piece.wake30=1;pieceComponent.mask20=0;
    g.mass4C=p.parameters.body.mass;g.inertia60=p.inertia100;
    for(unsigned i=0;i<3;++i)g.center50.v[i]=p.parameters.centerOffset.v[i];
    g.center50.v[3]=p.parameters.centerOffsetW;return true;
}

// Complete166400 numeric tuning reached by22C310 for the genuine authored
//single piece. Uses the SAME cold model's28D670 aggregate half extents already
//computed by assets. Call before any activation or animation changes that
//graph; cached bounds are not a live geometry provider. Source does not use
//the returned bounds center as body1E0. Exact source scaling/operation order,
//mass-setter nonpositive preserve path, full inertia inverse,295C90 contact
//coefficients, material0-only friction and22C310 overrides are retained.
//Native admission is FE_TONEAREST, finite consumed arithmetic, serial/disjoint
//owners. Failure leaves physical/piece/group untouched. No activation receipt.
inline bool tuneOriginalOnePieceWreck50LA(const OriginalWreckAssets50LA& assets,
    OriginalWreckPhysical50LA& p,OriginalWreckPieceFields50LA& piece,
    OriginalWreckSingleGroupFields50LA& group) noexcept {
    using namespace original_wreck_tuning_detail50LA;
    if(std::fegetround()!=FE_TONEAREST||!assets.prepared()||
       assets.selected().pieces.size()!=1||!p.bodyConstructorFieldsApplied||
       !p.contactConstructorFieldsApplied||p.contactBody30!=&p.body||
       group.primary3C!=&p||group.primary40!=&piece||group.capacity34||group.attachedCount30)return false;
    const auto& t=assets.selected().tuning;const auto& authored=assets.selected().pieces[0];
    const auto& bounds=assets.modelBounds();Math f;
    const float x=f.mul(2.f,bounds.halfExtent170[0]);
    const float y=f.mul(2.f,bounds.halfExtent170[1]);
    const float z=f.mul(2.f,bounds.halfExtent170[2]);
    const float xx=f.mul(x,x),yy=f.mul(y,y),zz=f.mul(z,z);
    const float extent=f.root(f.add(f.add(xx,yy),zz));
    float inverseExtent=piece.inverseExtent118;
    if(extent>0.f)inverseExtent=f.div(1.f,extent);
    auto parameters=p.parameters;
    const float gravityUnit=original_wreck_physical_detail50LA::word(0x4200cccdu);
    const float requestedMass=f.div(authored.weight,gravityUnit);
    if(requestedMass>0.f){parameters.body.mass=requestedMass;parameters.body.inverseMass=f.div(1.f,requestedMass);}
    const float mass=parameters.body.mass;
    //1665A4..1665B8 independently computes Y²+Z², X²+Z², X²+Y².
    const float yz=f.add(f.mul(y,y),f.mul(z,z));
    const float xz=f.add(f.mul(x,x),f.mul(z,z));
    const float xy=f.add(f.mul(x,x),f.mul(y,y));
    auto inertia=original_wreck_physical_detail50LA::identity();
    inertia.v[0]=f.mul(f.mul(authored.inertia,f.div(mass,12.f)),yz);
    inertia.v[5]=f.mul(f.mul(authored.inertia,f.div(mass,12.f)),xz);
    inertia.v[10]=f.mul(f.mul(authored.inertia,f.div(mass,12.f)),xy);
    const auto inverse=f.inverse(inertia);inverseToParameters(inverse,parameters);
    auto response=p.response;auto generated=p.generated;
    response.maxPenetration90=f.keep(t.maxCompression);
    response.stiffness9C=std::fabs(f.mul(gravityUnit,f.mul(t.spring,mass)));
    response.dampingA0=(mass>0.f&&response.stiffness9C>0.f)?
        f.mul(f.mul(2.f,t.damping),f.root(f.mul(mass,response.stiffness9C))):0.f;
    response.impactStiffnessA4=std::fabs(f.mul(gravityUnit,f.mul(t.impulseSpring,mass)));
    response.impactDampingA8=(mass>0.f&&response.impactStiffnessA4>0.f)?
        f.mul(f.mul(2.f,t.impulseDamping),f.root(f.mul(mass,response.impactStiffnessA4))):0.f;
    //22C4D8 supplies ZERO to166400's stack restitution argument; the authored
    //Restitution is stored on piece110 later, not in these DC response fields.
    generated.impact.restitutionB8=0.f;generated.impact.restitutionBC=0.f;generated.impact.restitutionC0=0.f;
    generated.frictionByMaterial[0]=f.keep(t.friction);
    generated.impact.forceTorqueScaleB4=0.f; //22C574
    f.keep(t.drag);f.keep(t.rateDamping);f.keep(t.restitution);f.keep(t.frictionTorqueScale);
    if(!f.valid)return false;
    p.parameters=parameters;p.inertia100=inertia;p.response=response;p.generated=generated;
    piece.inverseExtent118=inverseExtent;piece.dragEC=t.drag;piece.rateDampingF0=t.rateDamping;
    piece.waterDragF4=1.f;piece.waterRateF8=200.f;piece.buoyancyFC=.5f;
    piece.baseDrag100=t.drag;piece.baseRateDamping104=t.rateDamping;
    piece.restitution110=t.restitution;piece.frictionTorqueScale114=t.frictionTorqueScale;
    //2CA190's primary40 equality returns without consuming breakRate/Scale.
    //22C5B0 then saves the tuned root mass and full inertia matrix.
    group.mass4C=p.parameters.body.mass;group.inertia60=p.inertia100;return true;
}
// Exact2CA620 zero-capacity/zero-attached route. It restores saved center,
//multiplies each XYZ by1/(0+1), writes W1, and reruns165300/165340. Do not skip
//the inverse recomputation or borrow an engine group's saved state.
inline bool recomputeOriginalOnePieceGroup50LA(OriginalWreckSingleGroupFields50LA& g) noexcept {
    using namespace original_wreck_tuning_detail50LA;
    if(std::fegetround()!=FE_TONEAREST||!g.primary3C||!g.primary40||g.capacity34||g.attachedCount30)return false;
    auto& p=*g.primary3C;auto parameters=p.parameters;Math f;
    const float factor=f.div(1.f,1.f);
    for(unsigned i=0;i<3;++i)parameters.centerOffset.v[i]=f.mul(g.center50.v[i],factor);
    parameters.centerOffsetW=1.f;
    if(f.keep(g.mass4C)>0.f){parameters.body.mass=g.mass4C;parameters.body.inverseMass=f.div(1.f,g.mass4C);}
    const auto inverse=f.inverse(g.inertia60);inverseToParameters(inverse,parameters);
    if(!f.valid)return false;
    p.parameters=parameters;p.inertia100=g.inertia60;return true;
}
}
