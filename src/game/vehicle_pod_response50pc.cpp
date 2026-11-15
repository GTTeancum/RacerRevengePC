#include "vehicle_pod_response50pc.h"
#include <cstring>
#include <limits>
#include <type_traits>
namespace rr::game {
namespace {
struct Span {std::uintptr_t a=0,z=0;};
bool overlaps(Span a,Span b) noexcept{return a.a<b.z&&b.a<a.z;}
template<class T>bool span(const T*p,std::size_t n,Span&s) noexcept {
    s={};if(!n)return true;if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p),bytes=n*sizeof(T);if(bytes>std::numeric_limits<std::uintptr_t>::max()-a)return false;s={a,a+bytes};return true;
}
struct Guard {
    Span writes[12]{};unsigned n=0;
    template<class T>bool write(T*p,std::size_t c) noexcept {Span s;if(n==12||!span(p,c,s))return false;for(unsigned i=0;i<n;++i)if(overlaps(s,writes[i]))return false;writes[n++]=s;return true;}
    template<class T>bool read(const T*p,std::size_t c) const noexcept {Span s;if(!span(p,c,s))return false;for(unsigned i=0;i<n;++i)if(overlaps(s,writes[i]))return false;return true;}
};
}
StaticResponseStatus50E respondGeneratedWithPods50PC(
    const SpringGeneratedParameters48K& parameters,const SpringResponseParameters48L& response,
    const SpringRecordState48N& state,const SpringModelBody48K* bodies,std::size_t bodyCount,
    const StaticResponseScene50E& scene,const SpringRecord48N* records,std::size_t recordCount,
    const PodResponseWorkspace50PC& workspace,SpringRecord48N* destination,
    std::size_t destinationCapacity,PodResponseResult50PC& output) noexcept {
    using S=StaticResponseStatus50E;
    static_assert(std::is_trivially_copyable<PodResponseResult50PC>::value,"byte transport");
    if(!bodies||bodyCount<1||bodyCount>podResponseBodyCapacity50PC)return S::InvalidInput;
    for(std::size_t i=0;i<bodyCount;++i){
        if(!bodies[i].identity)return S::InvalidInput;
        for(std::size_t j=0;j<i;++j)if(bodies[i].identity==bodies[j].identity)return S::InvalidInput;
    }
    if(state.generatedCount84<0||state.generatedCapacity88<0||state.generatedCount84>state.generatedCapacity88||
       static_cast<std::size_t>(state.generatedCapacity88)>recordCount)return S::InvalidInput;
    const auto count=static_cast<std::size_t>(state.generatedCount84);
    if(parameters.pointContacts48)return S::UnsupportedScope;
    if(workspace.projectedCapacity<count||workspace.scratch.bodyCapacity<bodyCount||workspace.prepared.bodyCapacity<bodyCount||
       workspace.scratch.recordCapacity<count||workspace.prepared.recordCapacity<count||destinationCapacity<recordCount)return S::InsufficientCapacity;
    Guard guard;
    if(!guard.write(workspace.projected,workspace.projectedCapacity)||
       !guard.write(workspace.scratch.bodies,workspace.scratch.bodyCapacity)||
       !guard.write(workspace.scratch.recordFlags50,workspace.scratch.recordCapacity)||
       !guard.write(workspace.prepared.bodies,workspace.prepared.bodyCapacity)||
       !guard.write(workspace.prepared.recordFlags50,workspace.prepared.recordCapacity)||
       !guard.write(destination,destinationCapacity)||!guard.write(&output,1)||
       !guard.read(&parameters,1)||!guard.read(&response,1)||!guard.read(&state,1)||
       !guard.read(bodies,bodyCount)||!guard.read(&scene,1)||!guard.read(records,recordCount)||
       !guard.read(&workspace,1))return S::InvalidInput;
    for(std::size_t i=0;i<count;++i){
        const auto& in=records[i];auto& out=workspace.projected[i];
        if(in.other44.value){
            //Every named other body must be supplied; never the primary itself.
            bool known=false;
            for(std::size_t b=1;b<bodyCount;++b)if(bodies[b].identity==in.other44.value)known=true;
            if(!known)return S::InvalidInput;
        }
        if(in.scene40.value&&in.scene40.value!=scene.identity.value)return S::InvalidInput;
        out={};
        std::memcpy(&out.localPoint10,in.localPoint10.v,sizeof(out.localPoint10));
        std::memcpy(&out.normal20,in.normal20.v,sizeof(out.normal20));
        std::memcpy(&out.surfacePoint30,in.surfacePoint30.v,sizeof(out.surfacePoint30));
        out.hasScene40=in.scene40.value!=0;
        if(out.hasScene40)std::memcpy(&out.scene40,&scene.world,sizeof(out.scene40));
        out.otherBodyIdentity44=in.other44.value;out.material4C=in.material4C;out.flags50=in.flags50;
    }
    const SpringGeneratedState48K state48{state.pending38,state.contact3C,state.upward40,state.other44,
        state.point74,state.generated78,state.currentCandidate130};
    SpringGeneratedResult48K result{};
    const auto status=updateSpringContacts48L(parameters,response,state48,bodies,bodyCount,nullptr,0,
        workspace.projected,count,workspace.scratch,workspace.prepared,result);
    if(status!=SpringGeneratedStatus48K::Complete)return status==SpringGeneratedStatus48K::InsufficientCapacity?
        S::InsufficientCapacity:status==SpringGeneratedStatus48K::UnsupportedScope?S::UnsupportedScope:S::InvalidInput;
    PodResponseResult50PC next{};std::memcpy(&next.records,&state,sizeof state);
    next.records.pending38=result.state.pending38;next.records.contact3C=result.state.contact3C;
    next.records.upward40=result.state.upward40;next.records.other44=result.state.other44;
    next.records.point74=result.state.point74;next.records.generated78=result.state.generated78;
    next.records.currentCandidate130=result.state.field130;
    std::memcpy(next.bodies,workspace.prepared.bodies,bodyCount*sizeof(SpringModelBody48K));next.bodyCount=bodyCount;
    std::memcpy(&next.worldCenter110,&result.worldCenter110,sizeof next.worldCenter110);
    std::memcpy(&next.contactVelocity120,&result.contactVelocity120,sizeof next.contactVelocity120);
    if(recordCount)std::memcpy(destination,records,recordCount*sizeof(*records));
    for(std::size_t i=0;i<count;++i)destination[i].flags50=workspace.prepared.recordFlags50[i];
    std::memcpy(&output,&next,sizeof next);return S::Complete;
}
VehicleStaticResponseStatus50G respondMainVehicleWithPods50PC(
    VehicleFrameState49W& s,const VehicleFrameBindings49W& bindings,const VehicleFrameParameters49W& parameters,
    const SpringGeneratedParameters48K& settings,const SpringResponseParameters48L& response,
    const SecondaryInputs49M& secondary,SpringSceneId48N sceneIdentity,std::uint32_t primaryIdentity,
    const SpringModelBody48K* others,std::size_t otherCount,SpringModelBody48K* othersOut,
    StaticSpringState50G& spring,SpringRecord48N* records,std::size_t recordCount,
    const VehiclePodResponseWorkspace50PC&w,VehicleFrameDiagnostic49W& diagnostic) noexcept {
    using S=VehicleStaticResponseStatus50G;
    Guard guard;const auto&r=w.response;
    if(otherCount>=podResponseBodyCapacity50PC||(otherCount&&(!others||!othersOut)))return S::InvalidInput;
    if(!guard.write(&s,1)||!guard.write(&spring,1)||!guard.write(records,recordCount)||
       !guard.write(w.preparedRecords,w.preparedRecordCapacity)||!guard.write(w.frame,1)||!guard.write(&diagnostic,1)||
       !guard.write(r.projected,r.projectedCapacity)||!guard.write(r.scratch.bodies,r.scratch.bodyCapacity)||
       !guard.write(r.scratch.recordFlags50,r.scratch.recordCapacity)||!guard.write(r.prepared.bodies,r.prepared.bodyCapacity)||
       !guard.write(r.prepared.recordFlags50,r.prepared.recordCapacity)||!guard.write(othersOut,otherCount)||
       !guard.read(&bindings,1)||!guard.read(&parameters,1)||!guard.read(&settings,1)||
       !guard.read(&response,1)||!guard.read(&secondary,1)||!guard.read(&w,1)||!guard.read(others,otherCount))return S::InvalidInput;
    if(s.phase!=VehicleFramePhase49W::Post354)return S::WrongPhase;
    if(!primaryIdentity)return S::InvalidInput;
    if(w.preparedRecordCapacity<recordCount)return S::InsufficientCapacity;
    SpringModelBody48K bodies[podResponseBodyCapacity50PC]{};
    bodies[0].identity=primaryIdentity;
    std::memcpy(&bodies[0].body.state,&s.bodies.main.components.root,sizeof bodies[0].body.state);
    std::memcpy(&bodies[0].body.parameters,&parameters.bodies.main.body,sizeof bodies[0].body.parameters);
    std::memcpy(&bodies[0].body.localCenter,&parameters.bodies.main.centerOffset,sizeof bodies[0].body.localCenter);
    if(otherCount)std::memcpy(bodies+1,others,otherCount*sizeof(*others));
    SpringGeneratedParameters48K effective;std::memcpy(&effective,&settings,sizeof effective);
    std::memcpy(&effective.impact.worldCenter110,&spring.worldCenter110,sizeof spring.worldCenter110);
    std::memcpy(&effective.impact.contactVelocity120,&spring.contactVelocity120,sizeof spring.contactVelocity120);
    StaticResponseScene50E scene{sceneIdentity,bodies[0].body.state.scene};PodResponseResult50PC result{};
    const auto status=respondGeneratedWithPods50PC(effective,response,spring.records,bodies,1+otherCount,scene,records,recordCount,
        r,w.preparedRecords,w.preparedRecordCapacity,result);
    if(status!=StaticResponseStatus50E::Complete){
        if(status==StaticResponseStatus50E::UnsupportedScope)return S::UnsupportedScope;
        if(status==StaticResponseStatus50E::InsufficientCapacity)return S::InsufficientCapacity;
        return S::InvalidInput;
    }
    Post354_49W post{};post.effects=contactEffects49W(s);auto&main=post.effects.main;const auto&next=result.bodies[0].body.state;
    main.velocity70=next.body.velocity;main.angularVelocityB0=next.body.angularVelocity;
    main.loads=next.loads;main.positionCorrection1A0=next.positionCorrection1a0;
    main.rotationCorrection1B0=next.rotationCorrection1b0;main.accumulator1C0=next.accumulator1c0;main.accumulator1D0=next.accumulator1d0;
    post.response3C=result.records.contact3C;post.response44=result.records.other44;
    post.vehicle838=s.bodies.vehicle838; //2964C0 does not write the owning vehicle.
    if(!respondVehicle354_49W(s,bindings,parameters,post,secondary,*w.frame,diagnostic))return S::FrameRejected;
    // No operation after the frame commit can fail. All storage was prevalidated.
    if(recordCount)std::memcpy(records,w.preparedRecords,recordCount*sizeof(*records));
    std::memcpy(&spring.records,&result.records,sizeof spring.records);
    std::memcpy(&spring.worldCenter110,&result.worldCenter110,sizeof spring.worldCenter110);
    std::memcpy(&spring.contactVelocity120,&result.contactVelocity120,sizeof spring.contactVelocity120);
    if(otherCount)std::memcpy(othersOut,result.bodies+1,otherCount*sizeof(*othersOut));
    return S::Complete;
}
}
