#include "model_playback50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using U=std::uint32_t;
using I=SceneNodeIndex49P;
bool fail(ModelPlaybackDiagnostic50KG& d,ModelPlaybackError50KG error,U pc) noexcept {
    d.error=error;d.sourceStage=pc;return false;
}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {keep(a);keep(b);volatile float r=a+b;return keep(r);}
    float sub(float a,float b) noexcept {keep(a);keep(b);volatile float r=a-b;return keep(r);}
    float mul(float a,float b) noexcept {keep(a);keep(b);volatile float r=a*b;return keep(r);}
    float div(float a,float b) noexcept {keep(a);keep(b);if(b==0){valid=false;return 0;}volatile float r=a/b;return keep(r);}
};
bool resource(const ModelPlaybackMotion50KG& m) noexcept {
    return m.motion&&m.motion->data&&std::isfinite(m.motion->duration)&&m.motion->duration>0;
}
bool graphAndBindings(const assets::MotionNodeBindings49O& b,const SceneGraph49P& g) noexcept {
    if(!g.nodes||!g.count||g.count>maxSceneNodes49P||!validateSceneGraph49P(g)||!b.count||b.count>64)return false;
    for(U i=0;i<b.count;++i)if(b.rows[i].sceneNode>=g.count)return false;
    for(U i=0;i<g.count;++i)for(U k=0;k<16;++k)
        if(!std::isfinite(g.nodes[i].localC0.v[k])||!std::isfinite(g.nodes[i].world100.v[k]))return false;
    return true;
}
bool overlap(const void* a,std::size_t n,const void* b,std::size_t m) noexcept {
    if(!n||!m)return false;
    auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
    return x<=y?y-x<n:x-y<m;
}
bool index(I i,U count) noexcept {return static_cast<std::int32_t>(i)>=0&&static_cast<U>(i)<count;}
bool restore(const ModelCapturedPose50KG& pose,const ModelPlaybackIk50KG& ik,
    const assets::MotionNodeBindings49O& bindings,const SceneGraph49P& graph) noexcept {
    if(pose.count!=bindings.count)return false;
    for(U i=0;i<ik.restoreCount214;++i){
        const U row=ik.restoreRows218[i];
        if(row>=bindings.count)return false;
        const auto target=static_cast<I>(bindings.rows[row].sceneNode);
        auto local=graph.nodes[bindings.rows[row].sceneNode].localC0;
        BodyMatrix48G basis{};
        if(!quaternionToMatrix48G(pose.rotation[row],basis))return false;
        for(U k=0;k<12;++k)local.v[k]=basis.v[k];
        if(!setLocalMatrix49P(graph,target,local))return false;
        for(U k=0;k<3;++k)local.v[12+k]=pose.position[row].v[k];
        if(!setLocalMatrix49P(graph,target,local))return false;
    }
    return true;
}
bool endpointContribution(const ModelPlaybackIk50KG& ik,const SceneGraph49P& g,
    float weight,Vector3_48C* targets,Math& f) noexcept {
    for(U i=0;i<ik.count208;++i){
        const I node=ik.endpoints20C[i];
        if(!refreshSceneWorld49P(g,node))return false;
        const auto& m=g.nodes[static_cast<U>(node)].world100;
        for(U k=0;k<3;++k)targets[i].v[k]=f.add(targets[i].v[k],f.mul(weight,m.v[12+k]));
    }
    return f.valid;
}
}
bool captureModelPose50KG(const assets::MotionNodeBindings49O& bindings,
    const SceneGraph49P& graph,ModelCapturedPose50KG& output,ModelPlaybackDiagnostic50KG& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST||!graphAndBindings(bindings,graph))return fail(d,ModelPlaybackError50KG::topology,0x155920);
    ModelCapturedPose50KG next{};next.count=bindings.count;
    for(U row=0;row<bindings.count;++row){
        const auto& local=graph.nodes[bindings.rows[row].sceneNode].localC0;
        if(!matrixToQuaternion48G(local,next.rotation[row]))return fail(d,ModelPlaybackError50KG::arithmetic,0x28a890);
        for(U k=0;k<3;++k)next.position[row].v[k]=local.v[12+k];
    }
    output=next;return true;
}
bool captureModelPlaybackMatrices50KG(const assets::MotionNodeBindings49O& bindings,
    const SceneGraph49P& graph,ModelPlayback50KG& state,ModelPlaybackDiagnostic50KG& d) noexcept {
    d={};if(!graphAndBindings(bindings,graph))return fail(d,ModelPlaybackError50KG::topology,0x155730);
    for(U row=0;row<bindings.count;++row)state.savedMatrices1F4[row]=graph.nodes[bindings.rows[row].sceneNode].localC0;
    state.savedMatrixCount1F4=bindings.count;return true;
}
bool startModelPlayback50KG(const ModelPlaybackMotion50KG& motion,ModelPlayback50KG& state,
    ModelPlaybackDiagnostic50KG& d) noexcept {
    d={};if(!resource(motion))return fail(d,ModelPlaybackError50KG::input,0x155800);
    state.stopped10=false;state.current18=&motion;state.blending30=false;state.time34=0;return true;
}
bool transitionModelPlayback50KG(const ModelPlaybackMotion50KG& motion,float duration,
    const assets::MotionNodeBindings49O& bindings,const SceneGraph49P& graph,
    ModelPlayback50KG& state,ModelPlaybackDiagnostic50KG& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST||!resource(motion)||!std::isfinite(duration)||duration<0||
       !std::isfinite(state.time34)||state.time34<0)return fail(d,ModelPlaybackError50KG::input,0x155830);
    if(duration==0)return startModelPlayback50KG(motion,state,d);
    auto next=state;Math f;
    next.blendStart40=state.time34;next.oldTime38=state.time34;
    next.blendEnd3C=f.add(state.time34,duration);next.blendDuration44=duration;
    next.previous1C=state.current18;
    if(!state.current18||state.blending30){
        next.previous1C=nullptr;
        if(!captureModelPose50KG(bindings,graph,next.captured,d))return false;
    }else if(!resource(*state.current18))return fail(d,ModelPlaybackError50KG::input,0x1558bc);
    if(!f.valid)return fail(d,ModelPlaybackError50KG::arithmetic,0x1558b0);
    next.stopped10=false;next.current18=&motion;next.blending30=true;next.time34=0;
    state=next;return true;
}
bool bindModelPlaybackIk50KG(const I* endpoints,U count,
    const assets::MotionNodeBindings49O& bindings,const SceneGraph49P& graph,
    ModelPlaybackIk50KG& output,ModelPlaybackDiagnostic50KG& d) noexcept {
    d={};if(count>64||(count&&!endpoints)||!graphAndBindings(bindings,graph))return fail(d,ModelPlaybackError50KG::input,0x156620);
    ModelPlaybackIk50KG next{};next.enabled204=true;next.count208=count;
    bool rows[64]{};
    for(U i=0;i<count;++i){
        I node=endpoints[i];
        if(!index(node,graph.count))return fail(d,ModelPlaybackError50KG::binding,0x156650);
        next.endpoints20C[i]=node;
        // The endpoint's parent and grandparent must exist for28EB90.
        I parent=graph.nodes[static_cast<U>(node)].parent180;
        if(!index(parent,graph.count)||!index(graph.nodes[static_cast<U>(parent)].parent180,graph.count))
            return fail(d,ModelPlaybackError50KG::binding,0x156658);
        while(graph.nodes[static_cast<U>(node)].parent180!=I::none){
            bool found=false;
            for(U row=0;row<bindings.count;++row)if(bindings.rows[row].sceneNode==static_cast<U>(node)){rows[row]=true;found=true;}
            if(!found)return fail(d,ModelPlaybackError50KG::binding,0x156678);
            node=graph.nodes[static_cast<U>(node)].parent180;
        }
    }
    for(U row=0;row<bindings.count;++row)if(rows[row])next.restoreRows218[next.restoreCount214++]=static_cast<std::uint8_t>(row);
    output=next;return true;
}
bool advanceModelPlayback50KG(float dt,const assets::MotionNodeBindings49O& bindings,
    const SceneGraph49P& graph,const ModelPlaybackIk50KG& ik,ModelPlayback50KG& state,
    ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ikShared,
    ModelPlaybackWorkspace50KG& work,ModelPlaybackDiagnostic50KG& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(dt)||dt<0||!graphAndBindings(bindings,graph))
        return fail(d,ModelPlaybackError50KG::input,0x154480);
    auto stagedShared=shared;
    if(!stagedShared.updateGuard3D5DBC){
        stagedShared.updateValue433468=0xffffffffu;stagedShared.updateGuard3D5DBC=1;
    }
    if(state.stopped10||!state.current18){shared=stagedShared;return true;}
    if(!resource(*state.current18)||!std::isfinite(state.time34)||state.time34<0||
       !std::isfinite(state.oldTime38)||state.oldTime38<0||!std::isfinite(state.blendEnd3C)||
       !std::isfinite(state.blendStart40)||!std::isfinite(state.blendDuration44)||
       (state.blending30&&!(state.blendDuration44>0)))return fail(d,ModelPlaybackError50KG::input,0x1544f8);
    const auto bytes=sizeof(SceneNode49P)*graph.count;
    if(overlap(&work,sizeof work,graph.nodes,bytes)||overlap(&work,sizeof work,&state,sizeof state)||
       overlap(&work,sizeof work,&shared,sizeof shared)||overlap(&work,sizeof work,&ikShared,sizeof ikShared)||
       overlap(&work,sizeof work,&bindings,sizeof bindings)||
       overlap(&work,sizeof work,&ik,sizeof ik)||overlap(&state,sizeof state,graph.nodes,bytes)||
       overlap(&shared,sizeof shared,graph.nodes,bytes))return fail(d,ModelPlaybackError50KG::input,0x154480);
    if(ik.count208>64||ik.restoreCount214>64)return fail(d,ModelPlaybackError50KG::binding,0x154994);
    if(state.blending30&&ik.enabled204){
        if(ik.count208&&!ik.solve)return fail(d,ModelPlaybackError50KG::required_ik,0x28ffd0);
        for(U i=0;i<ik.count208;++i)if(!index(ik.endpoints20C[i],graph.count))return fail(d,ModelPlaybackError50KG::binding,0x156dcc);
        for(U i=0;i<ik.restoreCount214;++i)if(ik.restoreRows218[i]>=bindings.count)return fail(d,ModelPlaybackError50KG::binding,0x156eb4);
    }
    auto next=state;auto stagedIkShared=ikShared;Math f;
    std::memcpy(work.nodes,graph.nodes,bytes);const SceneGraph49P staged{work.nodes,graph.count};
    next.time34=f.add(next.time34,dt);
    bool ended=false;
    if(next.time34>next.current18->motion->duration){
        if(next.current18->looping)next.time34=f.sub(next.time34,next.current18->motion->duration);
        else ended=true;
    }
    if(!f.valid)return fail(d,ModelPlaybackError50KG::arithmetic,0x154500);
    if(!next.blending30){
        if(!sampleModelMotionTime50IG(*next.current18->motion,bindings,next.time34,staged,stagedShared,work.motion,d.sample))
            return fail(d,ModelPlaybackError50KG::sample,0x154580);
    }else{
        next.oldTime38=f.add(next.oldTime38,dt);
        float weight=f.div(f.sub(f.sub(next.oldTime38,next.blendStart40),dt),next.blendDuration44);
        if(weight<0)weight=0;
        else if(!(weight<1))weight=1;
        else {const float u=f.div(f.sub(weight,0),1);weight=f.mul(f.sub(3,f.mul(2,u)),f.mul(u,u));}
        if(!f.valid)return fail(d,ModelPlaybackError50KG::arithmetic,0x1548e0);
        if(next.previous1C&&!resource(*next.previous1C))return fail(d,ModelPlaybackError50KG::input,0x154b60);
        if(ik.enabled204){
            for(U i=0;i<ik.count208;++i)work.targets[i]={{0,0,0}};
            if(!sampleModelMotionTime50IG(*next.current18->motion,bindings,next.time34,staged,stagedShared,work.motion,d.sample))
                return fail(d,ModelPlaybackError50KG::sample,0x156960);
            if(!endpointContribution(ik,staged,weight,work.targets,f))return fail(d,ModelPlaybackError50KG::arithmetic,0x156dcc);
            if(next.previous1C){
                if(!sampleModelMotionTime50IG(*next.previous1C->motion,bindings,next.oldTime38,staged,stagedShared,work.motion,d.sample))
                    return fail(d,ModelPlaybackError50KG::sample,0x156960);
            }else if(!restore(next.captured,ik,bindings,staged))return fail(d,ModelPlaybackError50KG::sample,0x156e90);
            if(!endpointContribution(ik,staged,f.sub(1,weight),work.targets,f))return fail(d,ModelPlaybackError50KG::arithmetic,0x156f4c);
        }
        if(!sampleModelMotionTransition50KG(*next.current18->motion,next.previous1C?next.previous1C->motion:nullptr,
            next.previous1C?nullptr:&next.captured,bindings,next.time34,next.oldTime38,weight,staged,stagedShared,work.motion,d.sample))
            return fail(d,ModelPlaybackError50KG::sample,next.previous1C?0x154a40:0x154fb0);
        if(ik.enabled204)for(U i=0;i<ik.count208;++i)
            if(!ik.solve(ik.context,i,work.targets[i],stagedIkShared,staged))return fail(d,ModelPlaybackError50KG::ik,0x1549bc);
    }
    if(!(next.oldTime38<next.blendEnd3C)){next.blending30=false;next.oldTime38=0;}
    if(ended){next.stopped10=true;next.time34=0;next.oldTime38=0;}
    std::memcpy(graph.nodes,work.nodes,bytes);state=next;shared=stagedShared;ikShared=stagedIkShared;return true;
}
}
