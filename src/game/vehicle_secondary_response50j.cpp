#include "vehicle_secondary_contact50j.h"
#include <cstring>
#include <limits>
namespace rr::game {
namespace {
struct Span {std::uintptr_t a=0,z=0;};
bool overlaps(Span a,Span b) noexcept{return a.a<b.z&&b.a<a.z;}
template<class T>bool span(const T*p,std::size_t n,Span&s) noexcept {
    s={};if(!n)return true;if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;s={a,a+bytes};return true;
}
struct Guard {
    Span writes[11]{};unsigned n=0;
    template<class T>bool write(T*p,std::size_t c) noexcept {Span s;if(n==11||!span(p,c,s))return false;for(unsigned i=0;i<n;++i)if(overlaps(s,writes[i]))return false;writes[n++]=s;return true;}
    template<class T>bool read(const T*p,std::size_t c) const noexcept {Span s;if(!span(p,c,s))return false;for(unsigned i=0;i<n;++i)if(overlaps(s,writes[i]))return false;return true;}
};
}
VehicleStaticResponseStatus50G respondSecondaryStaticVehicle50J(
    VehicleFrameState49W& s,const VehicleFrameBindings49W& bindings,const VehicleFrameParameters49W& parameters,
    const SpringGeneratedParameters48K& settings,const SpringResponseParameters48L& response,
    SpringSceneId48N sceneIdentity,std::uint32_t primaryIdentity,
    StaticSpringState50G& spring,SpringRecord48N* records,std::size_t recordCount,
    const VehicleStaticResponseWorkspace50G&w,VehicleFrameDiagnostic49W& diagnostic) noexcept {
    using S=VehicleStaticResponseStatus50G;
    Guard guard;const auto&r=w.response;
    if(!guard.write(&s,1)||!guard.write(&spring,1)||!guard.write(records,recordCount)||
       !guard.write(w.preparedRecords,w.preparedRecordCapacity)||!guard.write(w.frame,1)||!guard.write(&diagnostic,1)||
       !guard.write(r.projected,r.projectedCapacity)||!guard.write(r.scratch.bodies,r.scratch.bodyCapacity)||
       !guard.write(r.scratch.recordFlags50,r.scratch.recordCapacity)||!guard.write(r.prepared.bodies,r.prepared.bodyCapacity)||
       !guard.write(r.prepared.recordFlags50,r.prepared.recordCapacity)||
       !guard.read(&bindings,1)||!guard.read(&parameters,1)||!guard.read(&settings,1)||
       !guard.read(&response,1)||!guard.read(&w,1))return S::InvalidInput;
    if(s.phase!=VehicleFramePhase49W::Post368)return S::WrongPhase;
    if(!primaryIdentity)return S::InvalidInput;
    if(w.preparedRecordCapacity<recordCount)return S::InsufficientCapacity;
    SpringModelBody48K body{};body.identity=primaryIdentity;
    std::memcpy(&body.body.state,&s.bodies.secondary.root,sizeof body.body.state);
    std::memcpy(&body.body.parameters,&parameters.bodies.secondary.body,sizeof body.body.parameters);
    std::memcpy(&body.body.localCenter,&parameters.bodies.secondary.centerOffset,sizeof body.body.localCenter);
    SpringGeneratedParameters48K effective;std::memcpy(&effective,&settings,sizeof effective);
    std::memcpy(&effective.impact.worldCenter110,&spring.worldCenter110,sizeof spring.worldCenter110);
    std::memcpy(&effective.impact.contactVelocity120,&spring.contactVelocity120,sizeof spring.contactVelocity120);
    StaticResponseScene50E scene{sceneIdentity,body.body.state.scene};StaticResponseResult50E result{};
    const auto status=respondStaticGenerated50E(effective,response,spring.records,body,scene,records,recordCount,
        r,w.preparedRecords,w.preparedRecordCapacity,result);
    if(status!=StaticResponseStatus50E::Complete){
        if(status==StaticResponseStatus50E::UnsupportedScope)return S::UnsupportedScope;
        if(status==StaticResponseStatus50E::InsufficientCapacity)return S::InsufficientCapacity;
        return S::InvalidInput;
    }
    Post354_49W post{};post.effects=contactEffects49W(s);auto&main=post.effects.secondary;const auto&next=result.primary.body.state;
    main.velocity70=next.body.velocity;main.angularVelocityB0=next.body.angularVelocity;
    main.loads=next.loads;main.positionCorrection1A0=next.positionCorrection1a0;
    main.rotationCorrection1B0=next.rotationCorrection1b0;main.accumulator1C0=next.accumulator1c0;main.accumulator1D0=next.accumulator1d0;
    if(!respondVehicle368_49W(s,bindings,parameters,post.effects,*w.frame,diagnostic))return S::FrameRejected;
    // No operation after the frame commit can fail. All storage was prevalidated.
    if(recordCount)std::memcpy(records,w.preparedRecords,recordCount*sizeof(*records));
    std::memcpy(&spring.records,&result.records,sizeof spring.records);
    std::memcpy(&spring.worldCenter110,&result.worldCenter110,sizeof spring.worldCenter110);
    std::memcpy(&spring.contactVelocity120,&result.contactVelocity120,sizeof spring.contactVelocity120);
    return S::Complete;
}
}
