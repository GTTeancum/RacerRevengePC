#include "pod_model_geometry49o.h"
#include <cstring>

namespace rr::game {
namespace {
bool nullcg(const char* s) noexcept {
    constexpr char target[]="NULLCG";
    for(unsigned i=0;i<6;++i){char c=s[i];if(c>='a'&&c<='z')c=char(c-'a'+'A');if(c!=target[i])return false;}
    return s[6]==0;
}
ModelBoundsContext49O rootContext() noexcept {
    ModelBoundsContext49O c{};
    c.rootWorld100.v[0]=c.rootWorld100.v[5]=c.rootWorld100.v[10]=c.rootWorld100.v[15]=1;
    c.rootSceneFlags190=0x80;
    return c;
}
}
bool buildFreshPodGeometry49O(const FreshPodGeometrySources49O& sources,
    FreshPodGeometryWorkspace49O& work,FreshPodGeometryResult49O& output,
    FreshPodGeometryDiagnostic49O& diagnostic) noexcept {
    diagnostic={FreshPodGeometryError49O::none,{ModelMotionError49O::none,0}};
    const auto fail=[&](FreshPodGeometryError49O e){diagnostic.error=e;return false;};
    const auto count=sources.chariot.nodeCount;
    if(!count||count>64||!sources.rightEngine.nodeCount||sources.rightEngine.nodeCount>64||sources.chariotBindings.count!=count)
        return fail(FreshPodGeometryError49O::asset);
    assets::PsgNode49O node{};AuthoredGeometryInputs49O input{};
    if(!assets::readPsgNode49O(sources.chariot,0,node))return fail(FreshPodGeometryError49O::asset);
    if(node.parent!=-1||!nullcg(node.name))return fail(FreshPodGeometryError49O::nullcg);
    for(unsigned j=0;j<3;++j)input.preAnimationChariotNullCgLocal.v[j]=node.local[12+j];
    if(!assets::readPsgNode49O(sources.rightEngine,0,node))return fail(FreshPodGeometryError49O::asset);
    if(node.parent!=-1||!nullcg(node.name))return fail(FreshPodGeometryError49O::nullcg);
    // Execute the original local*identity parent world arithmetic even for the
    // root node; copying its local Z would silently skip floating-point order.
    work.nodes[0]={};work.nodes[0].parent=-1;
    std::memcpy(work.nodes[0].local.v,node.local,sizeof node.local);
    const auto root=rootContext();ModelBoundsResult49O ignored{};
    if(!computeModelBounds49O(work.nodes,1,root,work.bounds,ignored))return fail(FreshPodGeometryError49O::bounds);
    input.preAnimationRightEngineNullCgWorldZ=work.bounds.world[0].v[14];
    for(std::uint32_t i=0;i<count;++i){
        if(!assets::readPsgNode49O(sources.chariot,i,node))return fail(FreshPodGeometryError49O::asset);
        if(i&&node.parent<0)return fail(FreshPodGeometryError49O::asset);
        std::memcpy(work.local[i].m,node.local,sizeof node.local);
        auto& bound=work.nodes[i];bound={};bound.parent=node.parent;
        //2893A0 sets80;28CB90..28CBF0 sets20 and04 on every source record.
        bound.sceneFlags190=0xa4;
        for(unsigned j=0;j<3;++j){bound.center140.v[j]=node.bounds[j];bound.halfExtent150.v[j]=node.bounds[3+j];}
    }
    if(!sampleModelMotionBlend49O(sources.forward,sources.back,sources.chariotBindings,.5f,0,work.local,count,diagnostic.motion))
        return fail(FreshPodGeometryError49O::motion);
    for(std::uint32_t i=0;i<count;++i)std::memcpy(work.nodes[i].local.v,work.local[i].m,sizeof work.local[i].m);
    FreshPodGeometryResult49O result{};
    if(!computeModelBounds49O(work.nodes,count,root,work.bounds,result.postMotionBounds))return fail(FreshPodGeometryError49O::bounds);
    input.postMotionRadius198=result.postMotionBounds.radius198;
    result.sourceInputs=input;
    if(!bindAuthoredGeometry49O(input,result.parameters))return fail(FreshPodGeometryError49O::parameters);
    output=result;return true;
}
}
