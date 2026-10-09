#include "impact_effects50b.h"
#include <cfenv>
#include <cstring>
#include <initializer_list>
namespace rr::game {
namespace {
using I=SceneNodeIndex49P;
struct Math {
    bool valid=true;
    float read(float f) noexcept {std::uint32_t b;std::memcpy(&b,&f,4);const auto e=b&0x7f800000u;if(e==0x7f800000u||(!e&&(b&0x7fffffu)))valid=false;return valid?f:0.0f;}
    float sub(float a,float b) noexcept{volatile float r=read(a)-read(b);return read(r);}
    float mul(float a,float b) noexcept{volatile float r=read(a)*read(b);return read(r);}
    float add(float a,float b) noexcept{volatile float r=read(a)+read(b);return read(r);}
};
I find(const SceneGraph49P& g,const std::uint8_t* names,I root) noexcept {
    //28C590: self, next-sibling subtree, first-child subtree. A coherent forest
    // gives each reached node one visit; bounded stack avoids recursive hot calls.
    I pending[maxSceneNodes49P];std::size_t count=0;pending[count++]=root;
    while(count){const auto index=pending[--count];const auto n=static_cast<std::uint32_t>(index);
        if(names[n])return index;
        const auto& node=g.nodes[n];
        if(node.firstChild184!=I::none)pending[count++]=node.firstChild184;
        if(node.nextSibling188!=I::none)pending[count++]=node.nextSibling188;
    }return I::none;
}
struct Builder {
    ImpactEffectsWorkspace50B& w;std::size_t used=0;
    void emit(ImpactEffectKind50B kind,std::uint32_t pc,std::uint32_t target,
        std::uint32_t argument=0,const float* value=nullptr) noexcept {
        auto& e=w.events[used++];e={};e.kind=kind;e.sourcePC=pc;e.target=target;e.argument=argument;
        if(value)std::memcpy(e.value,value,16);
    }
};
bool position(const SceneGraph49P& g,const std::uint8_t* names,I model,float* value) noexcept {
    // First28C590 finds actualdamage00. Helper repeats lookup starting at that
    // result, so self is immediately found. Null outer lookup is unsupported.
    const auto anchor=find(g,names,model);if(anchor==I::none)return false;
    if(!refreshSceneWorld49P(g,anchor))return false;
    std::memcpy(value,&g.nodes[static_cast<std::uint32_t>(anchor)].world100.v[12],12);
    value[3]=1.0f;return true; //28B1A0 writesXYZ into initialvf0{0,0,0,1}.
}
}
ImpactEffectsStatus50B updateImpactEffects50B(const SceneGraph49P& graph,
    const ImpactEffectsInput50B& in,std::uint32_t& sharedRandomState,
    ImpactEffectsWorkspace50B& workspace,ImpactEffectEvent50B* events,
    std::size_t capacity,std::size_t& count) noexcept {
    if(graph.count>maxSceneNodes49P||(!graph.nodes&&graph.count)||(!events&&capacity))return ImpactEffectsStatus50B::storage;
    if(!validateSceneGraph49P(graph))return ImpactEffectsStatus50B::graph;
    const bool namesBound=in.nameCount>=graph.count&&(in.damage00Matches||!graph.count);
    std::uint32_t ids[13]{};std::size_t idCount=0;
    for(const auto& model:in.models){
        if(model.node!=I::none&&static_cast<std::uint32_t>(model.node)>=graph.count)return ImpactEffectsStatus50B::binding;
        for(const auto e:{model.effect800,model.effect804,model.effect80C})if(e.id){
            for(std::size_t j=0;j<idCount;++j)if(ids[j]==e.id)return ImpactEffectsStatus50B::binding;ids[idCount++]=e.id;
        }
    }
    if(in.effectD40)for(std::size_t j=0;j<idCount;++j)if(ids[j]==in.effectD40)return ImpactEffectsStatus50B::binding;
    if(std::fegetround()!=FE_TONEAREST)return ImpactEffectsStatus50B::arithmetic;
    if(graph.count)std::memcpy(workspace.nextNodes,graph.nodes,graph.count*sizeof(SceneNode49P));
    SceneGraph49P next{workspace.nextNodes,graph.count};Builder b{workspace};Math m;auto rng=sharedRandomState;
    const unsigned damageIndex[]={1,0,3,2};
    for(unsigned n=0;n<4;++n){
        const auto& model=in.models[n];if(model.node==I::none)continue;
        const std::uint32_t offset=n*0x2ec;
        auto stop=[&](ImpactEffectBinding50B e,std::uint32_t pc){if(e.id&&(e.flags1D4&2))b.emit(ImpactEffectKind50B::Stop84,pc+offset,e.id);};
        if(model.flags03&4){stop(model.effect800,0x2237cc);stop(model.effect804,0x223800);stop(model.effect80C,0x223834);continue;}
        const auto damage=m.read(in.damage7D0[damageIndex[n]]);
        const auto delta=m.sub(1.0f,damage);const auto chance=delta<0.4f?delta:0.4f;
        bool smoke=false;
        if(model.effect800.id){
            if(damage>0.7f)smoke=true;
            else if(damage>0.4f){rng=rng*0x41c64e6du+0x3039u;volatile float draw=static_cast<float>(rng&0x7fffffffu);
                smoke=m.mul(0x1p-31f,draw)>chance;}
            if(smoke){
                if(!m.valid)return ImpactEffectsStatus50B::arithmetic;
                if(!namesBound)return ImpactEffectsStatus50B::binding;
                float p[4];if(!position(next,in.damage00Matches,model.node,p))return ImpactEffectsStatus50B::binding;
                if(!(model.effect800.flags1D4&2)){
                    b.emit(ImpactEffectKind50B::Start80,0x2241ec,model.effect800.id);
                    b.emit(ImpactEffectKind50B::Position15ACB0,0x2241f8,model.effect800.id,0,p);
                }
                b.emit(ImpactEffectKind50B::Position15ACB0,0x224204,model.effect800.id,0,p);
                const float intensity[4]={m.add(1.0f,m.mul(6.0f,m.sub(damage,0.34f))),0,0,0};
                b.emit(ImpactEffectKind50B::Intensity19A810,0x22422c,model.effect800.id,0,intensity);
            }else stop(model.effect800,0x223670);
        }
        if(model.effect80C.id){
            if(damage>0.7f){
                if(!m.valid)return ImpactEffectsStatus50B::arithmetic;
                if(!namesBound)return ImpactEffectsStatus50B::binding;
                //224250 starts before its second scene getter. Original outer
                // lookup precedes this event, and null lookup cannot be admitted.
                if(find(next,in.damage00Matches,model.node)==I::none)return ImpactEffectsStatus50B::binding;
                b.emit(ImpactEffectKind50B::Start80,0x224270,model.effect80C.id);
                float p[4];if(!position(next,in.damage00Matches,model.node,p))return ImpactEffectsStatus50B::binding;
                b.emit(ImpactEffectKind50B::Position15ACB0,0x2242b0,model.effect80C.id,0,p);
            }else stop(model.effect80C,0x223704);
        }
        if(model.effect804.id){
            if(m.read(in.timers820[n])>0.0f){
                if(!m.valid)return ImpactEffectsStatus50B::arithmetic;
                if(!namesBound)return ImpactEffectsStatus50B::binding;
                if(find(next,in.damage00Matches,model.node)==I::none)return ImpactEffectsStatus50B::binding;
                b.emit(ImpactEffectKind50B::Start80,0x2242f8,model.effect804.id);
                float p[4];if(!position(next,in.damage00Matches,model.node,p))return ImpactEffectsStatus50B::binding;
                b.emit(ImpactEffectKind50B::Velocity15FBF0,0x22433c,model.effect804.id,0,in.bodyVelocity70);
                b.emit(ImpactEffectKind50B::Position15ACB0,0x224348,model.effect804.id,0,p);
            }else stop(model.effect804,0x223790);
        }
    }
    if(in.effectD40){const auto active=m.read(in.timers820[4])>0.0f;b.emit(ImpactEffectKind50B::ChannelA8,active?0x224130:0x224148,in.effectD40,active?2u:3u);}
    if(!m.valid)return ImpactEffectsStatus50B::arithmetic;
    if(b.used>capacity||(!events&&b.used))return ImpactEffectsStatus50B::capacity;
    if(graph.count)std::memcpy(graph.nodes,workspace.nextNodes,graph.count*sizeof(SceneNode49P));
    if(b.used)std::memcpy(events,workspace.events,b.used*sizeof(*events));sharedRandomState=rng;count=b.used;
    return ImpactEffectsStatus50B::applied;
}
}
