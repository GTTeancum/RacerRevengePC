#include "static_contact_response50e.h"
#include <cstring>
#include <limits>
#include <type_traits>

namespace rr::game {
namespace {
struct Span {std::uintptr_t begin=0,end=0;};
bool overlaps(Span a,Span b) noexcept {return a.begin<b.end&&b.begin<a.end;}
template<class T>bool span(const T* p,std::size_t n,Span& s) noexcept {
    s={};if(!n)return true;
    if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto a=reinterpret_cast<std::uintptr_t>(p),z=n*sizeof(T);
    if(z>std::numeric_limits<std::uintptr_t>::max()-a)return false;
    s={a,a+z};return true;
}
struct Guard {
    Span writes[7]{};unsigned count=0;
    template<class T>bool write(T*p,std::size_t n) noexcept {
        Span s;if(count==7||!span(p,n,s))return false;
        for(unsigned i=0;i<count;++i)if(overlaps(s,writes[i]))return false;
        writes[count++]=s;return true;
    }
    template<class T>bool read(const T*p,std::size_t n) const noexcept {
        Span s;if(!span(p,n,s))return false;
        for(unsigned i=0;i<count;++i)if(overlaps(s,writes[i]))return false;
        return true;
    }
};
}
StaticResponseStatus50E respondStaticGenerated50E(
    const SpringGeneratedParameters48K& parameters,const SpringResponseParameters48L& response,
    const SpringRecordState48N& state,const SpringModelBody48K& primary,
    const StaticResponseScene50E& scene,const SpringRecord48N* records,std::size_t recordCount,
    const StaticResponseWorkspace50E& workspace,SpringRecord48N* destination,
    std::size_t destinationCapacity,StaticResponseResult50E& output) noexcept {
    using S=StaticResponseStatus50E;
    static_assert(std::is_trivially_copyable<StaticResponseResult50E>::value,"byte transport");
    if(state.generatedCount84<0||state.generatedCapacity88<0||state.generatedCount84>state.generatedCapacity88||
       static_cast<std::size_t>(state.generatedCapacity88)>recordCount)return S::InvalidInput;
    const auto count=static_cast<std::size_t>(state.generatedCount84);
    if(parameters.pointContacts48)return S::UnsupportedScope;
    if(workspace.projectedCapacity<count||workspace.scratch.bodyCapacity<1||workspace.prepared.bodyCapacity<1||
       workspace.scratch.recordCapacity<count||workspace.prepared.recordCapacity<count||destinationCapacity<recordCount)return S::InsufficientCapacity;
    Guard guard;
    if(!guard.write(workspace.projected,workspace.projectedCapacity)||
       !guard.write(workspace.scratch.bodies,workspace.scratch.bodyCapacity)||
       !guard.write(workspace.scratch.recordFlags50,workspace.scratch.recordCapacity)||
       !guard.write(workspace.prepared.bodies,workspace.prepared.bodyCapacity)||
       !guard.write(workspace.prepared.recordFlags50,workspace.prepared.recordCapacity)||
       !guard.write(destination,destinationCapacity)||!guard.write(&output,1)||
       !guard.read(&parameters,1)||!guard.read(&response,1)||!guard.read(&state,1)||
       !guard.read(&primary,1)||!guard.read(&scene,1)||!guard.read(records,recordCount)||
       !guard.read(&workspace,1))return S::InvalidInput;
    for(std::size_t i=0;i<count;++i){
        const auto& in=records[i];auto& out=workspace.projected[i];
        if(in.other44.value)return S::UnsupportedScope;
        if(in.scene40.value&&in.scene40.value!=scene.identity.value)return S::InvalidInput;
        out={};
        std::memcpy(&out.localPoint10,in.localPoint10.v,sizeof(out.localPoint10));
        std::memcpy(&out.normal20,in.normal20.v,sizeof(out.normal20));
        std::memcpy(&out.surfacePoint30,in.surfacePoint30.v,sizeof(out.surfacePoint30));
        out.hasScene40=in.scene40.value!=0;
        if(out.hasScene40)std::memcpy(&out.scene40,&scene.world,sizeof(out.scene40));
        out.otherBodyIdentity44=0;out.material4C=in.material4C;out.flags50=in.flags50;
    }
    const SpringGeneratedState48K state48{state.pending38,state.contact3C,state.upward40,state.other44,
        state.point74,state.generated78,state.currentCandidate130};
    SpringGeneratedResult48K result{};
    const auto status=updateSpringContacts48L(parameters,response,state48,&primary,1,nullptr,0,
        workspace.projected,count,workspace.scratch,workspace.prepared,result);
    if(status!=SpringGeneratedStatus48K::Complete)return status==SpringGeneratedStatus48K::InsufficientCapacity?
        S::InsufficientCapacity:status==SpringGeneratedStatus48K::UnsupportedScope?S::UnsupportedScope:S::InvalidInput;
    StaticResponseResult50E next{};std::memcpy(&next.records,&state,sizeof state);
    next.records.pending38=result.state.pending38;next.records.contact3C=result.state.contact3C;
    next.records.upward40=result.state.upward40;next.records.other44=result.state.other44;
    next.records.point74=result.state.point74;next.records.generated78=result.state.generated78;
    next.records.currentCandidate130=result.state.field130;
    std::memcpy(&next.primary,workspace.prepared.bodies,sizeof next.primary);
    std::memcpy(&next.worldCenter110,&result.worldCenter110,sizeof next.worldCenter110);
    std::memcpy(&next.contactVelocity120,&result.contactVelocity120,sizeof next.contactVelocity120);
    if(recordCount)std::memcpy(destination,records,recordCount*sizeof(*records));
    for(std::size_t i=0;i<count;++i)destination[i].flags50=workspace.prepared.recordFlags50[i];
    std::memcpy(&output,&next,sizeof next);return S::Complete;
}
}
