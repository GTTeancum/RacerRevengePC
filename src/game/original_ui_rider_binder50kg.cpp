#include "original_ui_rider_binder50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>
namespace rr::game {
namespace {
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
}
bool projectOriginalUiRiderBinder50KG(const SceneGraph49P& graph,
    SceneNodeIndex49P first,SceneNodeIndex49P second,const std::array<float,3>& camera,
    OriginalUiRiderBinderQuad50KG& output) noexcept {
    const auto a=static_cast<std::int32_t>(first),b=static_cast<std::int32_t>(second);
    if (std::fegetround()!=FE_TONEAREST||!validateSceneGraph49P(graph)||a<0||b<0||
        static_cast<std::uint32_t>(a)>=graph.count||static_cast<std::uint32_t>(b)>=graph.count)
        return false;
    for (float c:camera) if (!std::isfinite(c)) return false;
    std::array<SceneNode49P,maxSceneNodes49P> nodes;
    std::memcpy(nodes.data(),graph.nodes,graph.count*sizeof(SceneNode49P));
    const SceneGraph49P scratch{nodes.data(),graph.count};
    //28B1A0 with relative=null retrieves actual world positions in source order.
    if (!refreshSceneWorld49P(scratch,first)||!refreshSceneWorld49P(scratch,second)) return false;
    const auto& left=nodes[static_cast<std::size_t>(a)].world100;
    const auto& right=nodes[static_cast<std::size_t>(b)].world100;
    std::array<float,3> delta{},width{};
    for (unsigned i=0;i<3;++i) delta[i]=sub(left.v[12+i],right.v[12+i]);
    width={{sub(mul(camera[1],delta[2]),mul(camera[2],delta[1])),
        sub(mul(camera[2],delta[0]),mul(camera[0],delta[2])),
        sub(mul(camera[0],delta[1]),mul(camera[1],delta[0]))}};
    const float length2=add(add(mul(width[0],width[0]),mul(width[1],width[1])),mul(width[2],width[2]));
    if (!std::isfinite(length2)||length2<=0) return false;
    volatile float inverse=1.0f/std::sqrt(length2);
    OriginalUiRiderBinderQuad50KG result;
    for (unsigned i=0;i<3;++i) {
        // Source normalize first, then multiply constructor+88=2.25.
        const float offset=mul(2.25f,mul(width[i],inverse));
        result.position[0][i]=sub(left.v[12+i],offset);
        result.position[1][i]=add(left.v[12+i],offset);
        result.position[2][i]=sub(right.v[12+i],offset);
        result.position[3][i]=add(right.v[12+i],offset);
    }
    for (auto& vertex:result.position) {
        vertex[3]=1;
        for (float c:vertex) if (!std::isfinite(c)) return false;
    }
    std::memcpy(graph.nodes,nodes.data(),graph.count*sizeof(SceneNode49P));
    output=result;return true;
}
}
