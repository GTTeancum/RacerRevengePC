#include "original_ui_rider_model_fog50kg.h"
#include <cfenv>
#include <cmath>
namespace rr::game {
namespace {
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
}
bool prepareOriginalUiRiderModelFog50KG(const assets::PsgHierarchyView49O& psg,
    const SceneGraph49P& graph,const std::array<float,3>& camera,float fogStart,
    OriginalUiRiderModelFog50KG& output) noexcept {
    if(std::fegetround()!=FE_TONEAREST||!std::isfinite(fogStart)||
        psg.nodeCount<2||psg.nodeCount>assets::maxModelNodes49O||
        graph.count!=psg.nodeCount+1||graph.count>maxSceneNodes49P||
        !validateSceneGraph49P(graph)||graph.nodes[0].parent180!=SceneNodeIndex49P::none)
        return false;
    for(float c:camera)if(!std::isfinite(c))return false;
    std::array<SceneNode49P,maxSceneNodes49P> snapshot{};
    for(std::uint32_t n=0;n<graph.count;++n)snapshot[n]=graph.nodes[n];
    SceneGraph49P working{snapshot.data(),graph.count};
    if(!refreshSceneWorld49P(working,static_cast<SceneNodeIndex49P>(0)))return false;
    ModelBoundsContext49O context{};
    context.rootWorld100=snapshot[0].world100;
    context.rootSceneFlags190=snapshot[0].sceneFlags190;
    // The additional model owner has no authored box. Source PSG nodes carry
    // every actual box below it; reject a different owner layout rather than
    // inventing root bounds or silently dropping them.
    if(context.rootSceneFlags190&0x14u)return false;
    std::array<ModelBoundsNode49O,assets::maxModelNodes49O> nodes{};
    for(std::uint32_t n=0;n<psg.nodeCount;++n){
        assets::PsgNode49O source{};
        if(!assets::readPsgNode49O(psg,n,source))return false;
        const auto expected=source.parent<0?0:source.parent+1;
        if(static_cast<std::int32_t>(snapshot[n+1].parent180)!=expected)return false;
        auto& row=nodes[n];row.local=snapshot[n+1].localC0;
        row.parent=source.parent;row.sceneFlags190=snapshot[n+1].sceneFlags190;
        for(unsigned axis=0;axis<3;++axis){
            row.center140.v[axis]=source.bounds[axis];
            row.halfExtent150.v[axis]=source.bounds[axis+3];
        }
    }
    OriginalUiRiderModelFog50KG next{};ModelBoundsWorkspace49O workspace{};
    if(!computeModelBounds49O(nodes.data(),psg.nodeCount,context,workspace,next.bounds)||
        !next.bounds.seeded)return false;
    float displacement[3];
    for(unsigned axis=0;axis<3;++axis)
        displacement[axis]=add(sub(next.bounds.center160[axis],camera[axis]),
            context.rootWorld100.v[12+axis]);
    //26F160..26F170: X*X + Y*Y then Z*Z, followed by source SQRT.
    const float xy=add(mul(displacement[0],displacement[0]),mul(displacement[1],displacement[1]));
    const float xyz=add(xy,mul(displacement[2],displacement[2]));
    next.distance1BC=static_cast<float>(std::sqrt(static_cast<double>(xyz)));
    const float farEdge=add(next.distance1BC,next.bounds.radius198);
    if(!std::isfinite(next.distance1BC)||!std::isfinite(farEdge))return false;
    next.computeRadialFog1B2=!(farEdge<fogStart);
    output=next;return true;
}
}
