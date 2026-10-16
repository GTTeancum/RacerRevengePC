#pragma once
#include "native_water_runtime50jc_fixture.h"
#include "../../src/game/water_vehicle_effect50jc.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

// Concrete ordered22A620 consumer. This is a per-racer continuation; the water
// runtime and genuine external19CC50 control owners are borrowed each visit.
// It does not construct sprays or report missing19CED0 simulation complete.
namespace native_water_vehicle50JC {
using namespace rr::game;
struct Spray {
    std::uint32_t id=0;
    EffectControlState50BC* controls=nullptr;
    bool entryBound=false,source19CC50Bound=false;
};
struct Bindings {
    native_water_runtime50JC::Owner* water=nullptr;
    std::uint32_t waterOwner334=0; // Verified identity of this exact shared owner.
    float* previousBE0=nullptr; // Four canonical source words, never a receipt.
    Spray sprays81C[2]{};
    bool table81CBound=false;
};
enum class Status : std::uint8_t {Complete,Duplicate,Owner,Storage,Projection,Water,Spray,Graph};
struct Result {
    Status status=Status::Owner;
    std::uint32_t sourcePC=0;
    std::size_t action=0;
    WaterVehicleEffectResult50JC projection{};
    native_water_runtime50JC::Result water{};
};
struct SourceSnapshot {
    // Optional query118/component7FC/vehicle58/water164/speed34 and zero434630.
    std::uint32_t words[9]{};
    std::uint16_t present=0;
    WaterVehicleModel50JC models[5]{};
};
enum class Stage : std::uint8_t {Idle,Actions,Complete};
struct State {
    // Entirely pointer-free, fixed-size racer-bank state. Scratch may change
    // on failed projection. The action cursor advances only after real writes.
    WaterVehicleEffectWorkspace50JC workspace{};
    WaterVehicleEffectProposal50JC proposal{};
    SourceSnapshot source{};
    EffectControlState50BC expectedControls[2]{};
    std::uint32_t expectedBE0[4]{},sprayId[2]{};
    std::uint32_t racer=0,waterOwner334=0,graphCount=0;
    std::uint64_t sequence=0;
    std::size_t nextAction=0;
    Stage stage=Stage::Idle;
    bool hasSequence=false,previousBound=false,tableBound=false;
    bool sprayEntryBound[2]{},spraySourceBound[2]{};
};
namespace detail {
inline Result fail(Status s,std::uint32_t pc,std::size_t action=0) noexcept {return {s,pc,action};}
struct Span {std::uintptr_t first=0,last=0;};
template<class T> inline bool span(const T* p,std::size_t n,Span& out) noexcept {
    out={};if(!n)return true;const auto address=reinterpret_cast<std::uintptr_t>(p);
    if(!p||address%alignof(T)||n>std::numeric_limits<std::size_t>::max()/sizeof(T))return false;
    const auto bytes=n*sizeof(T);
    if(bytes>std::numeric_limits<std::uintptr_t>::max()-address)return false;
    out={address,address+bytes};return true;
}
inline bool overlaps(Span a,Span b) noexcept {return a.first<b.last&&b.first<a.last;}
struct Guard {
    Span writes[5]{};unsigned count=0;
    template<class T> bool write(T* p,std::size_t n=1) noexcept {
        Span s;if(count==5||!span(p,n,s))return false;
        for(unsigned i=0;i<count;++i)if(overlaps(s,writes[i]))return false;
        writes[count++]=s;return true;
    }
    template<class T> bool read(const T* p,std::size_t n=1) const noexcept {
        Span s;if(!span(p,n,s))return false;
        for(unsigned i=0;i<count;++i)if(overlaps(s,writes[i]))return false;
        return true;
    }
};
inline bool storage(const SceneGraph49P& graph,const WaterVehicleEffectInput50JC& in,
    const Bindings& b,State& state) noexcept {
    if(graph.count>maxSceneNodes49P)return false;
    Guard g;
    if(!g.write(&state)||!g.write(graph.nodes,graph.count)||
       !g.write(b.previousBE0,b.previousBE0?4u:0u))return false;
    for(const auto& spray:b.sprays81C)
        if(!g.write(spray.controls,spray.controls?1u:0u))return false;
    if(!g.read(&graph)||!g.read(&in)||!g.read(&b)||!g.read(b.water,b.water?1u:0u)||
       !g.read(in.query348State118,in.query348State118?1u:0u)||
       !g.read(in.component7FC,in.component7FC?1u:0u)||
       !g.read(in.vehicle58,in.vehicle58?1u:0u)||
       !g.read(in.query348WaterY164,in.query348WaterY164?1u:0u)||
       !g.read(in.body340Speed34,in.body340Speed34?1u:0u)||
       !g.read(in.initialStep434630,in.initialStep434630?4u:0u))return false;
    // The read-only spray and BE0 views deliberately alias precisely their
    // writable owners; ownerBindings() verifies those equalities separately.
    return true;
}
inline bool ownerBindings(const WaterVehicleEffectInput50JC& in,const Bindings& b) noexcept {
    if(!b.water||!b.water->initialized()||!b.water->provider()||!b.water->globals()||
       !b.waterOwner334||!in.waterAttachment334Bound||
       in.waterOwner334!=b.waterOwner334||in.previousBE0!=b.previousBE0||
       in.table81CBound!=b.table81CBound)return false;
    for(unsigned i=0;i<2;++i){const auto& input=in.sprays81C[i];const auto& actual=b.sprays81C[i];
        if(input.id!=actual.id||input.controls!=actual.controls||input.entryBound!=actual.entryBound||
           input.source19CC50Bound!=actual.source19CC50Bound)return false;
        if(actual.id&&(!actual.entryBound||!actual.controls||!actual.source19CC50Bound))return false;
        if(!actual.id&&(actual.controls||actual.source19CC50Bound))return false;
    }
    return !b.sprays81C[0].id||b.sprays81C[0].id!=b.sprays81C[1].id;
}
inline SourceSnapshot capture(const WaterVehicleEffectInput50JC& in) noexcept {
    SourceSnapshot out{};
    const auto scalar=[&out](unsigned index,const auto* p) noexcept {
        if(p){std::memcpy(out.words+index,p,4);out.present|=std::uint16_t(1u<<index);}
    };
    scalar(0,in.query348State118);scalar(1,in.component7FC);scalar(2,in.vehicle58);
    scalar(3,in.query348WaterY164);scalar(4,in.body340Speed34);
    if(in.initialStep434630)for(unsigned i=0;i<4;++i)scalar(5u+i,in.initialStep434630+i);
    out.models[0]=in.chariot1B4;out.models[1]=in.model1C4;out.models[2]=in.model1BC;
    out.models[3]=in.model1C0;out.models[4]=in.model1B8;return out;
}
inline bool sameSource(const SourceSnapshot& a,const SourceSnapshot& b) noexcept {
    if(a.present!=b.present||std::memcmp(a.words,b.words,sizeof a.words))return false;
    for(unsigned i=0;i<5;++i)if(a.models[i].bound!=b.models[i].bound||a.models[i].node!=b.models[i].node)return false;
    return true;
}
inline bool sameControl(const EffectControlState50BC& a,const EffectControlState50BC& b) noexcept {
    return !std::memcmp(a.position30,b.position30,sizeof a.position30)&&
        !std::memcmp(a.previous40,b.previous40,sizeof a.previous40)&&
        !std::memcmp(a.velocity60,b.velocity60,sizeof a.velocity60)&&
        a.word19C==b.word19C&&a.intensity204==b.intensity204&&a.flags1D4==b.flags1D4;
}
inline bool sameOwners(const SceneGraph49P& graph,const Bindings& b,const State& s) noexcept {
    if(graph.count!=s.graphCount||b.waterOwner334!=s.waterOwner334||b.table81CBound!=s.tableBound||
       bool(b.previousBE0)!=s.previousBound)return false;
    if(s.previousBound&&std::memcmp(b.previousBE0,s.expectedBE0,sizeof s.expectedBE0))return false;
    if(s.proposal.graphRead)for(std::uint32_t i=0;i<graph.count;++i)
        if(!water_vehicle_effect_detail50jc::sameNode(graph.nodes[i],s.workspace.graphPublishedNodes[i]))return false;
    for(unsigned i=0;i<2;++i){const auto& actual=b.sprays81C[i];
        if(actual.id!=s.sprayId[i]||actual.entryBound!=s.sprayEntryBound[i]||
           actual.source19CC50Bound!=s.spraySourceBound[i]||
           (actual.controls&&!sameControl(*actual.controls,s.expectedControls[i])))return false;
    }
    return true;
}
}

// Exactly one sequence per real22A620 call, retained on retry in the selected
// racer bank. Keep the same canonical identities, scalar inputs and graph
// through this continuation. No other racer/root-water update may interleave
// an unfinished call. Bindings must be genuine owners, not effect stubs.
// actualRacerIdentity is a verified nonzero native identity, NOT a bank slot;
// roster slot0 supplies its own nonzero identity like every other racer.
// The enclosing49W caller enters only at its source attached-water boundary;
// it may commit PostPlainEffects ONLY after Complete (or the same-sequence
// Duplicate). Never also run the plain/no-water tail for that call. This API
// deliberately changes no49W phase and gives no19CED0 lifecycle receipt.
// All backing storage is serial, stable and disjoint from State. Full shared
// runtime resources remain owned outside the racer bank. No allocation.
inline Result resume(std::uint64_t sequence,std::uint32_t actualRacerIdentity,
    const SceneGraph49P& graph,const WaterVehicleEffectInput50JC& in,
    const Bindings& bindings,State& state) noexcept {
    using namespace detail;
    if(!storage(graph,in,bindings,state))return fail(Status::Storage,0x22a620u,state.nextAction);
    if(!actualRacerIdentity||!ownerBindings(in,bindings))return fail(Status::Owner,0x228b40u,state.nextAction);
    if(state.hasSequence&&sequence<state.sequence)return fail(Status::Owner,0x22a620u,state.nextAction);
    if(state.hasSequence&&sequence==state.sequence){
        if(actualRacerIdentity!=state.racer||!sameOwners(graph,bindings,state)||!sameSource(capture(in),state.source))
            return fail(Status::Owner,0x22a620u,state.nextAction);
        if(state.stage==Stage::Complete)return {Status::Duplicate,0x22adbcu,state.nextAction};
        if(state.stage!=Stage::Actions)return fail(Status::Owner,0x22a620u,state.nextAction);
    }else{
        if(state.stage==Stage::Actions)return fail(Status::Owner,0x22a620u,state.nextAction);
        WaterVehicleEffectProposal50JC proposed{};
        const auto projection=projectWaterVehicleEffects50JC(graph,in,state.workspace,proposed,true);
        if(projection.status!=WaterVehicleEffectStatus50JC::Complete){
            auto r=fail(Status::Projection,projection.sourcePC);r.projection=projection;return r;
        }
        state.proposal=proposed;state.source=capture(in);state.graphCount=graph.count;
        state.tableBound=bindings.table81CBound;
        if(proposed.graphRead)std::memcpy(state.workspace.graphPublishedNodes,graph.nodes,graph.count*sizeof(SceneNode49P));
        state.previousBound=bindings.previousBE0!=nullptr;
        if(state.previousBound)std::memcpy(state.expectedBE0,bindings.previousBE0,sizeof state.expectedBE0);
        for(unsigned i=0;i<2;++i){const auto& spray=bindings.sprays81C[i];
            state.sprayId[i]=spray.id;state.sprayEntryBound[i]=spray.entryBound;
            state.spraySourceBound[i]=spray.source19CC50Bound;
            if(spray.controls)state.expectedControls[i]=*spray.controls;
        }
        state.racer=actualRacerIdentity;state.waterOwner334=bindings.waterOwner334;state.sequence=sequence;
        state.hasSequence=true;state.nextAction=0;state.stage=Stage::Actions;
    }
    if(!state.proposal.orderedGraphReads||state.proposal.actionCount>maxWaterVehicleActions50JC||
       state.nextAction>state.proposal.actionCount)return fail(Status::Owner,0x22a620u,state.nextAction);
    while(state.nextAction<state.proposal.actionCount){
        const auto& action=state.workspace.actions[state.nextAction];
        switch(action.kind){
        case WaterVehicleActionKind50JC::GraphRead49P: {
            if(!state.proposal.graphRead||action.firstGraphWrite>maxWaterVehicleGraphWrites50JC||
               action.graphWriteCount>maxWaterVehicleGraphWrites50JC-action.firstGraphWrite)
                return fail(Status::Graph,action.sourcePC,state.nextAction);
            // Preflight this source getter's complete cache delta before any
            // node publication. Later getters remain unpublished on failure.
            for(std::size_t i=0;i<action.graphWriteCount;++i){
                const auto& write=state.workspace.graphWrites[action.firstGraphWrite+i];
                if(write.node>=graph.count||
                   !water_vehicle_effect_detail50jc::sameNode(graph.nodes[write.node],write.before))
                    return fail(Status::Graph,action.sourcePC,state.nextAction);
            }
            for(std::size_t i=0;i<action.graphWriteCount;++i){
                const auto& write=state.workspace.graphWrites[action.firstGraphWrite+i];
                graph.nodes[write.node]=write.after;state.workspace.graphPublishedNodes[write.node]=write.after;
            }
            break;
        }
        case WaterVehicleActionKind50JC::Anchor2B1690:
        case WaterVehicleActionKind50JC::Force2B1370: {
            if(action.target!=bindings.waterOwner334)return fail(Status::Owner,action.sourcePC,state.nextAction);
            native_water_runtime50JC::Result applied;
            if(action.kind==WaterVehicleActionKind50JC::Anchor2B1690){
                const float point[3]{action.point[0],action.point[1],action.point[2]};
                applied=bindings.water->sourceAnchor(action.anchorBucket,point);
            }else applied=bindings.water->force(action.force);
            if(applied.status!=native_water_runtime50JC::Status::Complete){
                auto r=fail(Status::Water,applied.sourcePC,state.nextAction);r.water=applied;return r;
            }
            break;
        }
        case WaterVehicleActionKind50JC::WriteSprayPosition30:
        case WaterVehicleActionKind50JC::Effect50B: {
            unsigned side=0;while(side<2&&bindings.sprays81C[side].id!=action.target)++side;
            if(side==2||!bindings.sprays81C[side].controls)return fail(Status::Spray,action.sourcePC,state.nextAction);
            const auto& spray=bindings.sprays81C[side];
            if(!applyWaterVehicleSprayAction50JC(action,spray.id,spray.source19CC50Bound,*spray.controls))
                return fail(Status::Spray,action.sourcePC,state.nextAction);
            state.expectedControls[side]=*spray.controls;break;
        }
        case WaterVehicleActionKind50JC::WritePreviousBE0:
            if(!bindings.previousBE0)return fail(Status::Owner,action.sourcePC,state.nextAction);
            std::memcpy(bindings.previousBE0,action.point,16);std::memcpy(state.expectedBE0,action.point,16);break;
        default:return fail(Status::Owner,action.sourcePC,state.nextAction);
        }
        ++state.nextAction;
    }
    state.stage=Stage::Complete;return {Status::Complete,0x22adbcu,state.nextAction};
}
} // namespace native_water_vehicle50JC
