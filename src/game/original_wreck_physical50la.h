#pragma once
#include "original_wreck_components50la.h"
#include "graph_body49w.h"
#include "spring_model48k.h"
#include "spring_records48n.h"
#include "original_wreck_record_array50la.h"
#include "vehicle_aero48e.h"
#include <array>
#include <cfenv>
#include <cstring>

namespace rr::game {
class OriginalWreckSceneDomain50LA;

// The one genuine D0 body.49M supplies its canonical root, caches, full70/B0
// and load lanes, enabled204 and body20C. Its vehicle-only time864/phase are
// not wreck fields and are never consulted by this owner. No second mutable
// body/inverse-inertia/center authority is stored for the aggregate.
struct OriginalWreckPhysical50LA final {
    SecondaryBodyState49M body{};
    SecondaryBodyParameters49M parameters{};
    SecondaryActivationSupplement49V supplement{};
    GraphBodyExtraLanes49W extra{};
    BodyMatrix48G inertia100{};
    float cachedAnglesE0W=0.f;
    std::uint32_t body320=0,awake208=0,linkedCount268=0;
    SceneNodeIndex49P model288=SceneNodeIndex49P::none;
    // Native identity of the actual1652E0 scene owner; graph-local model288
    // alone cannot distinguish distinct retained PartN asset occurrences.
    const SceneNode49P* modelGraphNodes=nullptr;
    std::uint32_t modelGraphCount=0;
    // Native provenance of actual E0.shape44 publication. Compare-only borrow;
    // response also requires a live supplied domain and its unrevoked entry.
    const OriginalWreckSceneDomain50LA* modelSceneDomain=nullptr;
    SpringRecordState48N spring{};
    //295C20 allocates two separate50-record arrays. Their unused payload is
    //not source-initialized; only reached records written by real producers
    //may be consumed. C++ initial values are not a source validity receipt.
    OriginalWreckRecordArray50LA records;       //DC8C generated storage
    OriginalWreckRecordArray50LA pointRecords;  //DC80 point storage
    SpringGeneratedParameters48K generated{};
    // Full source DC110/120 caches; the existing48K parameters hold XYZ.
    //2964C0 and296F10 write W=1 independently of every D0 fourth lane.
    float customLocalCenter100W=0.f,worldCenter110W=0.f,contactVelocity120W=0.f;
    SpringResponseParameters48L response{};
    float normalVelocityScaleAC=0.f;
    SecondaryBodyState49M* contactBody30=nullptr;
    std::uint32_t contactMode34=0;
    bool bodyConstructorFieldsApplied=false,contactConstructorFieldsApplied=false;
    OriginalWreckPhysical50LA() noexcept=default;
    OriginalWreckPhysical50LA(const OriginalWreckPhysical50LA&)=delete;
    OriginalWreckPhysical50LA& operator=(const OriginalWreckPhysical50LA&)=delete;
};

// Exact represented PhysicsObject1659C0 scalar/vector fields. These remain
//piece-owned, including endpoint history80/90 and flight/contact timers. No
//query result, model position or force is inferred from these constructors.
struct OriginalWreckPieceFields50LA {
    std::uint32_t wake30=0,flight34=0,flight38=0,water3C=0,water40=0;
    SpringVector4_48N authored50{},look60{},up70{},previous80{},current90{},normalA0{},impulseC0{};
    float dragEC=0.f,rateDampingF0=0.f,waterDragF4=0.f,waterRateF8=0.f,buoyancyFC=0.f;
    float baseDrag100=0.f,baseRateDamping104=0.f,finalRateDamping108=0.f,finalDrag10C=0.f;
    float restitution110=0.f,frictionTorqueScale114=0.f,inverseExtent118=0.f;
    float airborneTime11C=0.f,flightDelay120=0.f;
    float measuredLinearAcceleration124=0.f,measuredAngularAcceleration128=0.f;
    float contactTime12C=0.f,changeWindow130=0.f,stopSpeed134=0.f,stopAngularSpeed138=0.f;
    float stopLinearAcceleration13C=0.f,stopAngularAcceleration140=0.f;
    std::uint32_t explicitCandidateCount148=0;
};
struct OriginalWreckConstructorGlobals50LA {
    std::uint32_t bodyVector433AD0[4],pieceVector433B10[4];
    SecondaryActivationGlobals49V reset;
};
// Borrowed immutable source lanes, never a duplicate Application authority.
struct OriginalWreckConstructorGlobalViews50LA {
    const void* bodyVector433AD0=nullptr;
    const void* pieceVector433B10=nullptr;
    struct Reset {const void* vector433B80=nullptr;const void* quaternion382B60=nullptr;} reset;
    bool current()const noexcept {
        return bodyVector433AD0&&pieceVector433B10&&reset.vector433B80&&reset.quaternion382B60;
    }
};
// Source static initializers3C9190/3C9210/3C9290 and file382B60. Writes a
//caller-owned application-global projection; construction consumes that same
//actual projection rather than inventing per-piece mutable globals.
inline void initializedOriginalWreckConstructorGlobals50LA(
    OriginalWreckConstructorGlobals50LA& g) noexcept {
    constexpr std::uint32_t vector[4]={0,0,0,0x3f800000u};
    std::memcpy(g.bodyVector433AD0,vector,sizeof vector);
    std::memcpy(g.pieceVector433B10,vector,sizeof vector);
    initializedSecondaryActivationGlobals49V(g.reset);
}
namespace original_wreck_physical_detail50LA {
inline float word(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
inline void copy(Vector3_48C& xyz,float& w,const void* v) noexcept {
    std::memcpy(xyz.v,v,12);std::memcpy(&w,static_cast<const char*>(v)+12,4);
}
inline void copy(SpringVector4_48N& to,const void* v) noexcept {
    std::memcpy(to.v,v,16);
}
inline BodyMatrix48G identity() noexcept {
    BodyMatrix48G m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1.f;return m;
}
}

// Actual164ED0->167920 constructor represented body writes, after the real
//component141300 subobject was initialized. Does not attach the model, tune
//mass/inertia, allocate a query, set body320=1, or claim live activation.
//Copy-only globals retain their exact words in all rounding modes.
template<class Globals>
inline bool initializeOriginalWreckBodyFromGlobals50LA(OriginalWreckPhysical50LA& p,
    const OriginalWreckComponent50LA& bodyComponent,
    const Globals& g) noexcept {
    using namespace original_wreck_physical_detail50LA;
    if(bodyComponent.kind!=OriginalWreckComponentKind50LA::body167920||
       !bodyComponent.objectKey||!bodyComponent.sessionGeneration||p.bodyConstructorFieldsApplied)return false;
    auto& b=p.body;auto& r=b.root;auto& q=p.parameters;
    b.enabled=true;p.awake208=0;
    copy(r.body.position,p.extra.position60W,g.bodyVector433AD0);
    copy(r.body.velocity,b.velocity70W,g.bodyVector433AD0);
    copy(r.localVelocity80,p.supplement.localVelocity80W,g.bodyVector433AD0);
    copy(r.worldAngularVelocityA0,p.extra.worldAngularVelocityA0W,g.bodyVector433AD0);
    copy(r.body.angularVelocity,b.angularVelocityB0W,g.bodyVector433AD0);
    copy(r.body.linearAcceleration,p.supplement.linearAccelerationC0W,g.bodyVector433AD0);
    copy(r.body.angularAcceleration,p.supplement.angularAccelerationD0W,g.bodyVector433AD0);
    copy(r.positionCorrection1a0,p.extra.positionCorrection1A0W,g.bodyVector433AD0);
    copy(r.rotationCorrection1b0,p.extra.rotationCorrection1B0W,g.bodyVector433AD0);
    copy(r.accumulator1c0,p.extra.accumulator1C0W,g.bodyVector433AD0);
    copy(r.accumulator1d0,p.extra.accumulator1D0W,g.bodyVector433AD0);
    std::memcpy(r.body.orientation.v,g.reset.quaternion382B60,16);
    copy(b.cachedAnglesE0,p.cachedAnglesE0W,g.bodyVector433AD0);
    q.body.linearAccelerationLimitSquared=word(0x4963d240u);
    q.body.angularAccelerationLimitSquared=word(0x48742400u);
    q.body.linearVelocityLimitSquared=word(0x468e6200u);
    q.body.angularVelocityLimitSquared=word(0x441c4000u);
    q.body.mass=0.f;q.body.inverseMass=0.f;b.cachedSpeed34=0.f;
    p.inertia100=identity();
    for(unsigned c=0;c<3;++c)for(unsigned k=0;k<3;++k)
        q.body.inverseInertiaColumns[c].v[k]=c==k?1.f:0.f;
    constexpr float extraInverse[7]={0.f,0.f,0.f,0.f,0.f,0.f,1.f};
    std::memcpy(q.inverseInertiaExtra,extraInverse,sizeof extraInverse);
    copy(q.centerOffset,q.centerOffsetW,g.bodyVector433AD0);
    p.linkedCount268=0;p.model288=SceneNodeIndex49P::none;b.scenePresent=false;
    p.modelGraphNodes=nullptr;p.modelGraphCount=0;p.modelSceneDomain=nullptr;
    b.body20C=0;r.callbackState238=0;
    for(unsigned i=0;i<3;++i){q.body.translationEnabled[i]=true;q.body.rotationEnabled[i]=true;}
    //167920 calls its genuine slotAC167A50, including167AD0 histories.
    // Transient copy-only reset argument, never retained global authority.
    SecondaryActivationGlobals49V reset;
    std::memcpy(reset.vector433B80,g.reset.vector433B80,16);
    std::memcpy(reset.quaternion382B60,g.reset.quaternion382B60,16);
    resetSecondaryActivation49V(b,p.supplement,reset);
    p.body320=0;p.bodyConstructorFieldsApplied=true;return true;
}

//165E90 reached1652E0 binding, then body320=1, then1653C0(0). The scene
//projection is copied from the actual graph cache without forcing a refresh;
//graph49P stays the sole transform authority. This is not model loading,
//registration, visibility, activation or the subsequent141960 attachment.
inline bool bindOriginalWreckBodyModel50LA(OriginalWreckPhysical50LA& p,
    const SceneGraph49P& graph,SceneNodeIndex49P model) noexcept {
    const auto index=static_cast<std::int32_t>(model);
    if(!p.bodyConstructorFieldsApplied||index<0||!graph.nodes||
       static_cast<std::uint32_t>(index)>=graph.count||!validateSceneGraph49P(graph))return false;
    p.model288=model;p.modelGraphNodes=graph.nodes;p.modelGraphCount=graph.count;
    p.body.scenePresent=true;
    p.body.root.scene=graph.nodes[index].world100;
    p.body.sceneFlags190=graph.nodes[index].sceneFlags190;
    p.body320=1;p.body.enabled=false;return true;
}

template<class Globals>
inline void initializeOriginalWreckPieceFieldsFromGlobals50LA(OriginalWreckPieceFields50LA& p,
    const Globals& g) noexcept {
    using namespace original_wreck_physical_detail50LA;
    p.wake30=1;p.flight34=0;p.flight38=0;p.water3C=0;p.water40=0;
    copy(p.authored50,g.pieceVector433B10);copy(p.look60,g.pieceVector433B10);
    copy(p.up70,g.pieceVector433B10);copy(p.normalA0,g.pieceVector433B10);
    copy(p.previous80,g.pieceVector433B10);copy(p.current90,g.pieceVector433B10);
    //C0 only has the vector default constructor SQC2 vf0 at165A3C.
    p.impulseC0={{0.f,0.f,0.f,1.f}};
    p.dragEC=0.f;p.rateDampingF0=0.f;p.waterDragF4=0.f;p.waterRateF8=0.f;p.buoyancyFC=0.f;
    p.baseDrag100=0.f;p.baseRateDamping104=0.f;p.finalRateDamping108=200.f;p.finalDrag10C=50.f;
    p.restitution110=0.f;p.frictionTorqueScale114=1.f;p.inverseExtent118=1.f;
    p.airborneTime11C=0.f;p.flightDelay120=.5f;p.measuredLinearAcceleration124=0.f;
    p.measuredAngularAcceleration128=0.f;p.contactTime12C=0.f;p.changeWindow130=0.f;
    p.stopSpeed134=10.f;p.stopAngularSpeed138=.5f;
    p.stopLinearAcceleration13C=10.f;p.stopAngularAcceleration140=1.f;
    p.explicitCandidateCount148=0;
}

//295A40 represented response defaults only. Array allocation295C20, body
//binding and165E90 overrides are later phases; no record payload is valid.
//DC30 must be bound to this same body by the owner; detector/query/scene links
//are separate concrete owners. Authored166400/22C310 tuning follows later.
// Fresh source constructor only: ordered stores on the SAME numeric owner.
// Source80/8C pointer aliases are published by the enclosing Storage owner.
inline void initializeFreshWreckContact295A40_50LA(OriginalWreckPhysical50LA& p,
    const std::array<std::uint32_t,4>& actual4435A0,
    const SpringRecord48N*& source80,const SpringRecord48N*& source8C) noexcept {
    auto& g=p.generated;auto& a=g.impact;auto& s=p.spring;auto& r=p.response;
    a.customLocalCenter100={{0,0,0}};p.customLocalCenter100W=1.f; //295A64
    a.worldCenter110={{0,0,0}};p.worldCenter110W=1.f; //295A6C
    a.contactVelocity120={{0,0,0}};p.contactVelocity120W=1.f; //295A74
    s.pointCount7C=0;s.generatedCount84=0;s.generatedCapacity88=0;
    p.contactBody30=nullptr; //295A84
    source80=nullptr;source8C=nullptr; //295A88/8C
    s.currentCandidate130=0;p.contactMode34=0;s.contact3C=0;s.pending38=0;
    a.customCenter70=false;g.pointContacts48=true;g.springForce4C=true;g.impact54=true;g.friction50=true;
    a.disableLeverArms5C=false;g.otherSpring58=false;r.secondaryAtLimit60=false;
    a.correctPenetration64=false;g.secondary68=false;a.yawOnly6C=false;
    s.point74=0;s.generated78=0;s.upward40=0;s.other44=0;
    r.maxPenetration90=0;r.maxSpeed94=original_wreck_physical_detail50LA::word(0x461c4000u);
    r.stiffness9C=0;r.dampingA0=0;r.impactStiffnessA4=0;r.impactDampingA8=0;
    p.normalVelocityScaleAC=1;a.angularImpulseScaleB0=1;a.forceTorqueScaleB4=1;g.frictionTorqueScaleC4=1;
    a.positionCorrectionScaleCC=original_wreck_physical_detail50LA::word(0x3d4ccccdu);
    a.angularCorrectionScaleD0=0;a.normalForceScale98=1;
    std::memcpy(a.customLocalCenter100.v,actual4435A0.data(),12);
    std::memcpy(&p.customLocalCenter100W,actual4435A0.data()+3,4); //295B34
    std::memcpy(a.contactVelocity120.v,actual4435A0.data(),12);
    std::memcpy(&p.contactVelocity120W,actual4435A0.data()+3,4); //295B44
    for(auto& f:g.frictionByMaterial)f=0; //295B54 ten actual stores
    s.lastMaterialD4=0;a.restitutionB8=0;a.restitutionC0=0;a.restitutionBC=0;
    p.contactConstructorFieldsApplied=true;
}

inline bool initializeOriginalWreckContactConstructor295A40_50LA(OriginalWreckPhysical50LA& p,
    const OriginalWreckComponent50LA& contactComponent,
    const SpringVector4_48N& actualVector4435A0) noexcept {
    using namespace original_wreck_physical_detail50LA;
    if(contactComponent.kind!=OriginalWreckComponentKind50LA::contact295A40||
       !contactComponent.objectKey||!contactComponent.sessionGeneration||p.contactConstructorFieldsApplied)return false;
    p.spring={};
    p.contactBody30=nullptr;p.contactMode34=0;
    auto& g=p.generated;auto& a=g.impact;
    a.worldCenter110={{0.f,0.f,0.f}};
    p.worldCenter110W=1.f; //295A6C vector default constructor
    std::memcpy(a.customLocalCenter100.v,actualVector4435A0.v,12);
    std::memcpy(a.contactVelocity120.v,actualVector4435A0.v,12);
    std::memcpy(&p.contactVelocity120W,actualVector4435A0.v+3,4); //295B44
    a.normalForceScale98=1.f;a.angularImpulseScaleB0=1.f;a.forceTorqueScaleB4=1.f;
    a.restitutionB8=0.f;a.restitutionBC=0.f;a.restitutionC0=0.f;
    a.positionCorrectionScaleCC=word(0x3d4ccccdu);a.angularCorrectionScaleD0=0.f;
    a.disableLeverArms5C=false;a.correctPenetration64=false;a.yawOnly6C=false;a.customCenter70=false;
    g.frictionTorqueScaleC4=1.f;g.frictionByMaterial.fill(0.f);
    g.pointContacts48=true;g.springForce4C=true;g.friction50=true;g.impact54=true;
    g.otherSpring58=false;g.secondary68=false;
    p.response.maxPenetration90=0.f;p.response.maxSpeed94=word(0x461c4000u);
    p.response.stiffness9C=0.f;p.response.dampingA0=0.f;
    p.response.impactStiffnessA4=0.f;p.response.impactDampingA8=0.f;
    p.response.secondaryAtLimit60=false;p.normalVelocityScaleAC=1.f;
    p.contactConstructorFieldsApplied=true;return true;
}

// Actual1660F0..16615C overrides, AFTER POINT generation296190.
inline void applyOriginalWreckContact1660F0_50LA(OriginalWreckPhysical50LA& p)noexcept {
    p.contactMode34=1;p.generated.impact54=true;p.generated.springForce4C=false;
    p.generated.impact.correctPenetration64=true;p.generated.friction50=true;
    p.normalVelocityScaleAC=1.f;p.generated.impact.angularImpulseScaleB0=1.f;
    p.generated.impact.forceTorqueScaleB4=1.f;p.generated.frictionTorqueScaleC4=1.f;
    p.generated.impact.positionCorrectionScaleCC=original_wreck_physical_detail50LA::word(0x3ba3d70au);
    p.generated.impact.angularCorrectionScaleD0=0.f;p.generated.impact.normalForceScale98=1.f;
}
// Preserve the existing legacy owner's combined admission. The full owner
// invokes the genuine phases separately, with actual array ownership between.
inline bool initializeOriginalWreckContact50LA(OriginalWreckPhysical50LA& p,
    const OriginalWreckComponent50LA& component,const SpringVector4_48N& vector4435A0)noexcept {
    if(!initializeOriginalWreckContactConstructor295A40_50LA(p,component,vector4435A0))return false;
    p.spring.generatedCapacity88=50;p.contactBody30=&p.body;
    applyOriginalWreckContact1660F0_50LA(p);return true;
}

enum class OriginalWreckBodyControlStatus50LA : std::uint8_t {
    complete,owner,linkedBodies,integration,children,arithmetic
};
// Exact1653C0. A false argument always writes204=false. A true argument can
//complete when208 is already nonzero, or when the REAL268 count is zero.
//Nonzero linked-body traversal is a source guard, not fabricated wake-up.
//Native admission failure leaves all body fields unchanged.
inline OriginalWreckBodyControlStatus50LA setOriginalWreckBodyEnabled50LA(
    OriginalWreckPhysical50LA& p,bool enable) noexcept {
    using S=OriginalWreckBodyControlStatus50LA;
    if(!p.bodyConstructorFieldsApplied)return S::owner;
    if(!enable){p.body.enabled=false;return S::complete;}
    if(!p.awake208&&p.linkedCount268)return S::linkedBodies;
    p.body.enabled=true;if(!p.awake208)p.awake208=1;return S::complete;
}
// Complete167B80 body320!=0 route with its actual empty141ED0 child route.
//Called by the real parent dispatcher AFTER442F70 slot20(0). It refreshes
//caches even when body204 is disabled. Reuses14FC00 and167BC0 numeric kernels
//against the canonical body; does not copy a SecondaryBody persistent owner.
//The source vector default at14FC20 establishes the fourth angle lane as1.
inline OriginalWreckBodyControlStatus50LA refreshOriginalWreckBodyFrame50LA(
    OriginalWreckPhysical50LA& p,const OriginalWreckComponent50LA& component) noexcept {
    using S=OriginalWreckBodyControlStatus50LA;
    if(!p.bodyConstructorFieldsApplied||component.kind!=OriginalWreckComponentKind50LA::body167920)
        return S::owner;
    if(!p.body320)return S::integration;
    if((component.descendants24&2u)&&component.firstChild18)return S::children;
    if(std::fegetround()!=FE_TONEAREST)return S::arithmetic;
    Vector3_48C angles{};float speed=0.f;
    if(!quaternionToAngles48G(p.body.root.body.orientation,angles)||
       !refreshCachedSpeed48E(p.body.root.body.velocity,speed))return S::arithmetic;
    p.body.cachedAnglesE0=angles;p.cachedAnglesE0W=1.f;p.body.cachedSpeed34=speed;
    return S::complete;
}
inline bool initializeOriginalWreckBody50LA(OriginalWreckPhysical50LA& p,
    const OriginalWreckComponent50LA& body,const OriginalWreckConstructorGlobals50LA& globals)noexcept {
    return initializeOriginalWreckBodyFromGlobals50LA(p,body,globals);
}
inline bool initializeOriginalWreckBody50LA(OriginalWreckPhysical50LA& p,
    const OriginalWreckComponent50LA& body,const OriginalWreckConstructorGlobalViews50LA& globals)noexcept {
    return globals.current()&&initializeOriginalWreckBodyFromGlobals50LA(p,body,globals);
}
inline void initializeOriginalWreckPieceFields50LA(OriginalWreckPieceFields50LA& p,
    const OriginalWreckConstructorGlobals50LA& globals)noexcept {
    initializeOriginalWreckPieceFieldsFromGlobals50LA(p,globals);
}
inline void initializeOriginalWreckPieceFields50LA(OriginalWreckPieceFields50LA& p,
    const OriginalWreckConstructorGlobalViews50LA& globals)noexcept {
    if(globals.current())initializeOriginalWreckPieceFieldsFromGlobals50LA(p,globals);
}

}
