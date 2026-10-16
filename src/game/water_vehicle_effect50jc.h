#pragma once
#include "water_wake50jc.h"
#include "scene_graph49p.h"
#include "effect_control50bc.h"
#include "impact_effects50b.h"

namespace rr::game {
// Native22A620 call projection. This is a vehicle caller, not a replacement
// particle simulation, water owner, frame scheduler or attachment334 binder.
enum class WaterVehicleEffectStatus50JC : std::uint8_t {
    Complete,Owner,Graph,Arithmetic,Storage,Unsupported
};
struct WaterVehicleEffectResult50JC {
    WaterVehicleEffectStatus50JC status=WaterVehicleEffectStatus50JC::Owner;
    std::uint32_t sourcePC=0;
};
struct WaterVehicleModel50JC {
    SceneNodeIndex49P node=SceneNodeIndex49P::none;
    bool bound=false; // bound+none means genuinely absent, never missing metadata.
};
struct WaterVehicleSpray50JC {
    std::uint32_t id=0;
    const EffectControlState50BC* controls=nullptr;
    bool entryBound=false; // A bound null entry is used only where source tests it.
    // Actual19CC50 construction/19CD40 load, vtable3CEAC0:80=15ACE0,
    //84=15AD00. This is NOT one of the four EffectProfile50BE alternatives.
    bool source19CC50Bound=false;
};
struct WaterVehicleEffectInput50JC {
    const std::uint32_t* query348State118=nullptr;
    const std::uint32_t* component7FC=nullptr;
    const std::uint32_t* vehicle58=nullptr;
    const float* query348WaterY164=nullptr;
    const float* body340Speed34=nullptr;
    const float* previousBE0=nullptr; // Four original vector words.
    // Actual434630 copy at22A7A0, used as the step when speed<=100.
    // Startup3C9FDC writes(0,0,0,1); still bind the actual live application owner.
    const float* initialStep434630=nullptr;
    WaterVehicleModel50JC chariot1B4{},model1C4{},model1BC{},model1C0{},model1B8{};
    WaterVehicleSpray50JC sprays81C[2]{};
    bool table81CBound=false;
    // Native identity of the genuine vehicle334 provider. Every water action
    // routes to this same owner. A value alone cannot establish its provenance.
    std::uint32_t waterOwner334=0;
    bool waterAttachment334Bound=false;
};
enum class WaterVehicleActionKind50JC : std::uint8_t {
    Anchor2B1690,Force2B1370,WriteSprayPosition30,Effect50B,WritePreviousBE0,GraphRead49P
};
struct WaterVehicleAction50JC {
    WaterVehicleActionKind50JC kind=WaterVehicleActionKind50JC::WritePreviousBE0;
    std::uint32_t sourcePC=0,target=0;
    float point[4]{}; // Also the entire sourceBE0/position30 copy when applicable.
    WaterWakeForceInput50JC force{};
    ImpactEffectEvent50B effect{};
    std::int32_t anchorBucket=0; // Actual22A798 passes zero.
    std::size_t firstGraphWrite=0,graphWriteCount=0;
};
// distance<=70 boundsCVT(1+.2*distance) to15. Each model contributes at
// most3 spray operations and30 force calls, plus one anchor and oneBE0 write.
// The optional ordered-cache form adds the three actual28B2B0 read sites.
constexpr std::size_t maxWaterVehicleActions50JC=71;
constexpr std::size_t maxWaterVehicleGraphWrites50JC=3u*maxSceneNodes49P;
struct WaterVehicleGraphWrite50JC {
    std::uint32_t node=0;
    SceneNode49P before{},after{};
};
struct WaterVehicleEffectWorkspace50JC {
    SceneNode49P nextNodes[maxSceneNodes49P];
    WaterVehicleAction50JC actions[maxWaterVehicleActions50JC];
    SceneNode49P graphPublishedNodes[maxSceneNodes49P];
    WaterVehicleGraphWrite50JC graphWrites[maxWaterVehicleGraphWrites50JC];
};
struct WaterVehicleEffectProposal50JC {
    std::size_t actionCount=0;
    std::uint32_t graphCount=0;
    bool graphRead=false;
    bool orderedGraphReads=false;
};
namespace water_vehicle_effect_detail50jc {
using Status=WaterVehicleEffectStatus50JC;
using I=SceneNodeIndex49P;
using Math=water_wake_detail50jc::Math;
inline WaterVehicleEffectResult50JC result(Status s,std::uint32_t pc) noexcept {return {s,pc};}
inline bool model(const WaterVehicleModel50JC& m,const SceneGraph49P& g) noexcept {
    return m.bound&&(m.node==I::none||static_cast<std::uint32_t>(m.node)<g.count);
}
inline bool presentSpray(const WaterVehicleSpray50JC& s) noexcept {
    return s.entryBound&&s.id&&s.controls&&s.source19CC50Bound;
}
inline bool position(const SceneGraph49P& g,I i,float (&p)[4]) noexcept {
    if(i==I::none||!refreshSceneWorld49P(g,i))return false;
    std::memcpy(p,g.nodes[static_cast<std::uint32_t>(i)].world100.v+12,12);
    p[3]=1.f;return true; //28B2B0(relative=null), initialized vf0.W.
}
inline bool sameNode(const SceneNode49P& a,const SceneNode49P& b) noexcept {
    return std::memcmp(a.localC0.v,b.localC0.v,sizeof a.localC0.v)==0&&
        std::memcmp(a.world100.v,b.world100.v,sizeof a.world100.v)==0&&
        a.parent180==b.parent180&&a.firstChild184==b.firstChild184&&
        a.nextSibling188==b.nextSibling188&&a.sceneFlags190==b.sceneFlags190;
}
inline void transform(const BodyMatrix48G& matrix,const float (&p)[4],
    float (&out)[4],Math& m,std::uint32_t pc) noexcept {
    float a[3];
    // Each source vector operation covers its XYZ lanes before the next
    // operation. Keep that group order for the first arithmetic boundary.
    for(unsigned c=0;c<3;++c)a[c]=m.mul(matrix.v[c],p[0],pc);
    for(unsigned c=0;c<3;++c)a[c]=m.add(a[c],m.mul(matrix.v[4+c],p[1],pc+4),pc+4);
    for(unsigned c=0;c<3;++c)a[c]=m.add(a[c],m.mul(matrix.v[8+c],p[2],pc+8),pc+8);
    for(unsigned c=0;c<3;++c)out[c]=m.add(a[c],matrix.v[12+c],pc+12);
    out[3]=1.f;
}
inline void inversePoint(const BodyMatrix48G& source,const float (&p)[4],
    float (&out)[4],Math& m) noexcept {
    //28BD50 constructs a basis transpose, NOT a general matrix inverse.
    // Preserve all source ADD-zero basis/fourth-lane operations and separately
    // transform translation before the point. Do not reduce to R*(p-T).
    BodyMatrix48G inverse=source;
    for(unsigned row=0;row<3;++row)for(unsigned c=0;c<4;++c)
        inverse.v[4*row+c]=m.add(source.v[4*row+c],0.f,0x28be3cu+row*4);
    inverse.v[4]=m.add(0.f,source.v[1],0x28be48u);
    inverse.v[8]=m.add(0.f,source.v[2],0x28be4cu);
    inverse.v[9]=m.add(0.f,source.v[6],0x28be50u);
    inverse.v[1]=m.add(0.f,source.v[4],0x28be54u);
    inverse.v[2]=m.add(0.f,source.v[8],0x28be58u);
    inverse.v[6]=m.add(0.f,source.v[9],0x28be5cu);
    for(unsigned c=0;c<4;++c)inverse.v[12+c]=m.add(source.v[12+c],0.f,0x28be68u);
    float a[3];
    for(unsigned c=0;c<3;++c)a[c]=m.mul(inverse.v[c],source.v[12],0x28be6cu);
    for(unsigned c=0;c<3;++c)a[c]=m.add(a[c],m.mul(inverse.v[4+c],source.v[13],0x28be70u),0x28be70u);
    for(unsigned c=0;c<3;++c)a[c]=m.add(a[c],m.mul(inverse.v[8+c],source.v[14],0x28be74u),0x28be74u);
    const auto negativeOne=m.sub(0.f,1.f,0x28be7cu);
    for(unsigned c=0;c<3;++c)inverse.v[12+c]=m.mul(a[c],negativeOne,0x28be80u);
    transform(inverse,p,out,m,0x28beb0u);
}
struct Builder {
    WaterVehicleEffectWorkspace50JC& w;
    std::size_t count=0;
    bool valid=true;
    bool orderedGraphReads=false;
    std::size_t graphWriteCount=0;
    WaterVehicleAction50JC* add(WaterVehicleActionKind50JC kind,
        std::uint32_t pc,std::uint32_t target,const float* point=nullptr) noexcept {
        if(count==maxWaterVehicleActions50JC){valid=false;return nullptr;}
        auto& a=w.actions[count++];a={};a.kind=kind;a.sourcePC=pc;a.target=target;
        if(point)std::memcpy(a.point,point,16);return &a;
    }
    bool graphRead(std::uint32_t pc,const SceneGraph49P& staged) noexcept {
        if(!orderedGraphReads)return true;
        auto* a=add(WaterVehicleActionKind50JC::GraphRead49P,pc,0);
        if(!a)return false;
        a->firstGraphWrite=graphWriteCount;
        for(std::uint32_t i=0;i<staged.count;++i){
            auto& prior=w.graphPublishedNodes[i];const auto& next=staged.nodes[i];
            if(sameNode(prior,next))continue;
            if(graphWriteCount==maxWaterVehicleGraphWrites50JC){valid=false;return false;}
            w.graphWrites[graphWriteCount++]={i,prior,next};prior=next;++a->graphWriteCount;
        }
        return true;
    }
    void effect(ImpactEffectKind50B kind,std::uint32_t pc,
        std::uint32_t id,const float* p=nullptr) noexcept {
        auto* a=add(WaterVehicleActionKind50JC::Effect50B,pc,id,p);if(!a)return;
        a->effect.kind=kind;a->effect.sourcePC=pc;a->effect.target=id;
        if(p)std::memcpy(a->effect.value,p,16);
    }
    void force(std::uint32_t pc,std::uint32_t id,const float (&p)[4],float amplitude) noexcept {
        auto* a=add(WaterVehicleActionKind50JC::Force2B1370,pc,id,p);if(!a)return;
        std::memcpy(a->force.point,p,12);a->force.amplitude=amplitude;a->force.sourceCallBound=true;
    }
};
}
// Atomic proposal only: canonical graph/effects/BE0/water remain untouched.
// On failure out is unchanged; workspace may change. All canonical inputs,
// workspace and out must be disjoint, stable and serially owned. Strict FP,
// FE_TONEAREST, finite normal/zero arithmetic, no contraction. A Complete
// projection is NOT a completed caller until every action is consumed in
// order and the staged graph caches are published by the enclosing owner.
// Consumers cannot mutate model graphs, source bindings or the other spray.
// Legacy/default projection retains only final nextNodes publication and the
// existing non-graph action order. orderedGraphReads=true also emits exact
// per-getter before/after packets at22A668/22A918/22AB6C, interleaved with
// their source actions. Cursor consumers must apply those packets there,
// never bulk-publish later getters early. nextNodes still holds the final
// snapshot in either form. Cache packet capacity is three complete graphs.
inline WaterVehicleEffectResult50JC projectWaterVehicleEffects50JC(
    const SceneGraph49P& graph,const WaterVehicleEffectInput50JC& in,
    WaterVehicleEffectWorkspace50JC& w,WaterVehicleEffectProposal50JC& out,
    bool orderedGraphReads=false) noexcept {
    using namespace water_vehicle_effect_detail50jc;
    if(!in.waterAttachment334Bound||!in.waterOwner334)
        return result(Status::Owner,0x228b40u);
    if(!in.query348State118)return result(Status::Owner,0x22a648u);
    Builder b{w};b.orderedGraphReads=orderedGraphReads;
    WaterVehicleEffectProposal50JC next;next.orderedGraphReads=orderedGraphReads;
    bool eligible=*in.query348State118==1u;
    if(eligible){if(!in.component7FC)return result(Status::Owner,0x22a654u);eligible=*in.component7FC!=0;}
    if(!eligible){
        //22AD7C: table itself is dereferenced, each entry is genuinely optional.
        if(!in.table81CBound)return result(Status::Owner,0x22ad7cu);
        for(unsigned i=0;i<2;++i){const auto& s=in.sprays81C[i];
            if(!s.entryBound)return result(Status::Owner,i?0x22ada0u:0x22ad80u);
            if(!s.id)continue;
            if(!presentSpray(s))return result(Status::Owner,i?0x22adacu:0x22ad8cu);
            if(i&&in.sprays81C[0].id==s.id)return result(Status::Unsupported,0x22ada0u);
            b.effect(ImpactEffectKind50B::Stop84,i?0x22adb4u:0x22ad94u,s.id);
        }
        next.actionCount=b.count;out=next;return result(Status::Complete,0x22adbcu);
    }
    if(graph.count>maxSceneNodes49P||(!graph.nodes&&graph.count))return result(Status::Storage,0x22a668u);
    if(!validateSceneGraph49P(graph))return result(Status::Graph,0x22a668u);
    if(!model(in.chariot1B4,graph)||in.chariot1B4.node==I::none)
        return result(Status::Owner,0x22a660u);
    if(!in.previousBE0)return result(Status::Owner,0x22a680u);
    if(std::fegetround()!=FE_TONEAREST)return result(Status::Arithmetic,0x22a698u);
    std::memcpy(w.nextNodes,graph.nodes,graph.count*sizeof(SceneNode49P));
    if(orderedGraphReads)std::memcpy(w.graphPublishedNodes,graph.nodes,graph.count*sizeof(SceneNode49P));
    SceneGraph49P staged{w.nextNodes,graph.count};next.graphRead=true;next.graphCount=graph.count;
    float current[4];if(!position(staged,in.chariot1B4.node,current))return result(Status::Graph,0x22a668u);
    if(!b.graphRead(0x22a668u,staged))return result(Status::Storage,0x22a668u);
    Math m;float delta[4]={0,0,0,1};
    constexpr std::uint32_t deltaPC[3]{0x22a698u,0x22a69cu,0x22a6a4u};
    for(unsigned c=0;c<3;++c)delta[c]=m.sub(in.previousBE0[c],current[c],deltaPC[c]);
    const auto squareX=m.mul(delta[0],delta[0],0x22a6c8u);
    const auto squareY=m.mul(delta[1],delta[1],0x22a6ccu);
    float squared=m.add(squareX,squareY,0x22a6d0u);
    squared=m.add(squared,m.mul(delta[2],delta[2],0x22a6d4u),0x22a6d4u);
    volatile float root=std::sqrt(squared);const float distance=m.read(root,0x22a6d8u);
    if(m.badPC)return result(Status::Arithmetic,m.badPC);
    if(distance>70.f){
        // Source leaves existing spray controls alone on this displacement branch.
        b.add(WaterVehicleActionKind50JC::WritePreviousBE0,0x22a708u,0,current);
        next.actionCount=b.count;out=next;return result(Status::Complete,0x22adbcu);
    }
    if(!in.vehicle58)return result(Status::Owner,0x22a70cu);
    if(*in.vehicle58==0u){
        if(!in.query348WaterY164)return result(Status::Owner,0x22a788u);
        float anchor[4]={0,0,0,1};
        constexpr std::uint32_t scalePC[3]{0x22a730u,0x22a734u,0x22a73cu};
        constexpr std::uint32_t addPC[3]{0x22a75cu,0x22a764u,0x22a76cu};
        float scaled[3];
        for(unsigned c=0;c<3;++c)scaled[c]=m.mul(6.f,delta[c],scalePC[c]);
        for(unsigned c=0;c<3;++c)anchor[c]=m.add(current[c],scaled[c],addPC[c]);
        anchor[1]=*in.query348WaterY164;
        b.add(WaterVehicleActionKind50JC::Anchor2B1690,0x22a794u,in.waterOwner334,anchor);
    }
    if(!in.initialStep434630)return result(Status::Owner,0x22a7a0u);
    if(!in.body340Speed34)return result(Status::Owner,0x22a7c0u);
    float step[4];std::memcpy(step,in.initialStep434630,16);
    const float speed=m.mul(water_wake_detail50jc::bits(0x3f2e8bc1u),*in.body340Speed34,0x22a7c4u);
    std::int32_t iterations=1;
    if(speed>100.f){
        const float countFloat=m.add(1.f,m.mul(water_wake_detail50jc::bits(0x3e4ccccdu),distance,0x22a7fcu),0x22a804u);
        iterations=m.cvtWord(countFloat,0x22a808u);
        if(m.badPC)return result(Status::Arithmetic,m.badPC);
        if(iterations<1||iterations>15)return result(Status::Unsupported,0x22a80cu);
        const float reciprocal=m.div(1.f,m.integer(iterations,0x22a814u),0x22a818u);
        constexpr std::uint32_t dividePC[3]{0x22a81cu,0x22a820u,0x22a828u};
        constexpr std::uint32_t scalePC[3]{0x22a844u,0x22a84cu,0x22a854u};
        float divided[3];
        for(unsigned c=0;c<3;++c)divided[c]=m.mul(delta[c],reciprocal,dividePC[c]);
        for(unsigned c=0;c<3;++c)step[c]=m.mul(-3.f,divided[c],scalePC[c]);
        step[3]=1.f;
    }
    float amplitude=m.add(0.f,m.div(m.mul(m.sub(speed,0.f,0x22a874u),3.f,0x22a884u),300.f,0x22a894u),0x22a89cu);
    if(amplitude>3.f)amplitude=3.f;else if(amplitude<0.f)amplitude=0.f;
    if(m.badPC)return result(Status::Arithmetic,m.badPC);
    const WaterVehicleModel50JC* preferred[2]={&in.model1C4,&in.model1C0};
    const WaterVehicleModel50JC* fallback[2]={&in.model1BC,&in.model1B8};
    for(unsigned side=0;side<2;++side){
        const auto& first=*preferred[side];const auto& second=*fallback[side];
        if(!model(first,graph))return result(Status::Owner,side?0x22ab20u:0x22a8ccu);
        I node=first.node;
        if(node==I::none){
            if(!model(second,graph))return result(Status::Owner,side?0x22ab34u:0x22a8e0u);
            node=second.node;
        }
        if(node==I::none)continue;
        if(!in.query348WaterY164)return result(Status::Owner,side?0x22ab8cu:0x22a938u);
        float world[4];if(!position(staged,node,world))return result(Status::Graph,side?0x22ab6cu:0x22a918u);
        if(!b.graphRead(side?0x22ab6cu:0x22a918u,staged))
            return result(Status::Storage,side?0x22ab6cu:0x22a918u);
        world[1]=*in.query348WaterY164;
        const auto& matrix=staged.nodes[static_cast<std::uint32_t>(node)].world100;
        float local[4];inversePoint(matrix,world,local,m);
        local[2]=m.sub(local[2],50.f,side?0x22abc0u:0x22a96cu);
        local[0]=side?m.add(local[0],5.f,0x22abccu):m.sub(local[0],5.f,0x22a978u);
        float sprayPoint[4];transform(matrix,local,sprayPoint,m,0x28bb7cu);
        if(m.badPC)return result(Status::Arithmetic,m.badPC);
        const auto& spray=in.sprays81C[side];
        if(!in.table81CBound||!presentSpray(spray))return result(Status::Owner,side?0x22abf8u:0x22a9a4u);
        if(side&&in.sprays81C[0].entryBound&&in.sprays81C[0].id==spray.id)
            return result(Status::Unsupported,0x22abfcu);
        if(speed>50.f){
            if(!(spray.controls->flags1D4&2u))
                b.add(WaterVehicleActionKind50JC::WriteSprayPosition30,side?0x22ac18u:0x22a9c4u,spray.id,sprayPoint);
            b.effect(ImpactEffectKind50B::Start80,side?0x22ac2cu:0x22a9d8u,spray.id);
            b.effect(ImpactEffectKind50B::Position15ACB0,side?0x22ac3cu:0x22a9e8u,spray.id,sprayPoint);
        }else b.effect(ImpactEffectKind50B::Stop84,side?0x22ac5cu:0x22aa08u,spray.id);
        local[0]=side?m.add(local[0],15.f,0x22ac78u):m.sub(local[0],15.f,0x22aa24u);
        float forcePoint[4];transform(matrix,local,forcePoint,m,0x28bb7cu);
        for(std::int32_t iteration=0;iteration<iterations;++iteration){
            constexpr std::uint32_t subtractPC[2][3]{{0x22aa58u,0x22aa64u,0x22aa70u},
                {0x22acacu,0x22acb8u,0x22acc4u}};
            for(unsigned c=0;c<3;++c)forcePoint[c]=m.sub(forcePoint[c],step[c],subtractPC[side][c]);
            forcePoint[1]=*in.query348WaterY164;
            b.force(side?0x22acdcu:0x22aa88u,in.waterOwner334,forcePoint,amplitude);
            float negative[4]={0,0,0,1};
            constexpr std::uint32_t scalePC[2][3]{{0x22aaa8u,0x22aac0u,0x22aab8u},
                {0x22acfcu,0x22ad14u,0x22ad0cu}};
            constexpr std::uint32_t addPC[2][3]{{0x22aae0u,0x22aae4u,0x22aaecu},
                {0x22ad34u,0x22ad38u,0x22ad40u}};
            float scaled[3];
            constexpr unsigned multiplyOrder[3]{0u,2u,1u};
            for(unsigned k=0;k<3;++k){const auto c=multiplyOrder[k];scaled[c]=m.mul(10.f,delta[c],scalePC[side][c]);}
            for(unsigned c=0;c<3;++c)negative[c]=m.add(forcePoint[c],scaled[c],addPC[side][c]);
            b.force(side?0x22ad54u:0x22ab00u,in.waterOwner334,negative,-amplitude);
        }
    }
    if(m.badPC)return result(Status::Arithmetic,m.badPC);
    b.add(WaterVehicleActionKind50JC::WritePreviousBE0,0x22ad78u,0,current);
    if(!b.valid)return result(Status::Storage,0x22ad78u);
    next.actionCount=b.count;out=next;return result(Status::Complete,0x22adbcu);
}
// Exact controls for the authentic19CC50 spray subtype. The enclosing caller
// may use this against its transaction scratch, interleaved with water actions.
// No emitter construction or19CED0 simulation is claimed. A false result is
// non-mutating. Real lifecycle/source method binding remains mandatory.
inline bool applyWaterVehicleSprayAction50JC(const WaterVehicleAction50JC& a,
    std::uint32_t sourceOwnerId,bool source19CC50Bound,EffectControlState50BC& state) noexcept {
    if(!source19CC50Bound||!sourceOwnerId||a.target!=sourceOwnerId)return false;
    std::uint32_t words[4];
    if(a.kind==WaterVehicleActionKind50JC::WriteSprayPosition30){
        std::memcpy(state.position30,a.point,16);return true;
    }
    if(a.kind!=WaterVehicleActionKind50JC::Effect50B||a.effect.target!=sourceOwnerId)return false;
    switch(a.effect.kind){
    case ImpactEffectKind50B::Start80:startEffect50BC(state);return true;
    case ImpactEffectKind50B::Stop84:stopEffect50BC(state);return true;
    case ImpactEffectKind50B::Position15ACB0:
        std::memcpy(words,a.effect.value,16);positionEffect50BC(state,words);return true;
    default:return false;
    }
}
}
