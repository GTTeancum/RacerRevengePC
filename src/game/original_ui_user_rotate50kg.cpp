#include "original_ui_user_rotate50kg.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using V=std::array<float,3>;
float mul(float a,float b) noexcept { volatile float r=a*b;return r; }
float add(float a,float b) noexcept { volatile float r=a+b;return r; }
float sub(float a,float b) noexcept { volatile float r=a-b;return r; }
bool finite(const V& v) noexcept {return std::isfinite(v[0])&&std::isfinite(v[1])&&std::isfinite(v[2]);}
V cross(const V& a,const V& b) noexcept {
    return {{sub(mul(a[1],b[2]),mul(a[2],b[1])),
        sub(mul(a[2],b[0]),mul(a[0],b[2])),sub(mul(a[0],b[1]),mul(a[1],b[0]))}};
}
bool normalize(V& v) noexcept {
    const float length2=add(add(mul(v[0],v[0]),mul(v[1],v[1])),mul(v[2],v[2]));
    if (!std::isfinite(length2)||length2<=0) return false;
    volatile float inverse=1.0f/std::sqrt(length2);
    for (auto& x:v) x=mul(x,inverse);
    return finite(v);
}
}
bool originalUiUserRotatePose50KG(const V& position,const V& look,const V& up,
    BodyMatrix48G& output) noexcept {
    if (std::fegetround()!=FE_TONEAREST||!finite(position)||!finite(look)||!finite(up)) return false;
    //1D3BA4..1D3BD0: right=up x look, then correctedLook=right x up.
    V corrected=cross(cross(up,look),up),normalUp=up;
    if (!normalize(normalUp)||!normalize(corrected)) return false;
    const auto right=cross(normalUp,corrected);
    if (!finite(right)) return false;
    BodyMatrix48G result{};
    for (unsigned i=0;i<3;++i) {
        result.v[i]=right[i];result.v[4+i]=normalUp[i];
        result.v[8+i]=corrected[i];result.v[12+i]=position[i];
    }
    result.v[15]=1;output=result;return true;
}
bool advanceOriginalUiUserRotate50KG(const OriginalUiUserRotateParameters50KG& parameters,
    bool loaded,bool paused,float dt,const SceneGraph49P& graph,SceneNodeIndex49P root) noexcept {
    if (!loaded||paused) return true;
    const auto index=static_cast<std::int32_t>(root);
    if (!std::isfinite(dt)||dt<0||!validateSceneGraph49P(graph)||index<0||
        static_cast<std::uint32_t>(index)>=graph.count) return false;
    std::array<SceneNode49P,maxSceneNodes49P> nodes;
    std::memcpy(nodes.data(),graph.nodes,graph.count*sizeof(SceneNode49P));
    const SceneGraph49P scratch{nodes.data(),graph.count};
    float twoPi;const std::uint32_t sourceTwoPi=0x40c90fdbu;
    std::memcpy(&twoPi,&sourceTwoPi,sizeof twoPi);
    for (unsigned axis=0;axis<3;++axis) if (parameters.enabled[axis]) {
        if (!std::isfinite(parameters.revolutionsPerSecond[axis])) return false;
        VisualVector49R direction{{0,0,0,1}};direction.v[axis]=1;
        const float angle=mul(twoPi,mul(dt,parameters.revolutionsPerSecond[axis]));
        // Preserve X,Y,Z calls even when their enabled velocity is zero.
        if (!detail::rotateAxisGraph49W(scratch,root,direction,angle)) return false;
    }
    const auto& local=nodes[static_cast<std::size_t>(index)].localC0;
    const V look{{local.v[8],local.v[9],local.v[10]}};
    const V up{{local.v[4],local.v[5],local.v[6]}};
    BodyMatrix48G restored;
    if (!originalUiUserRotatePose50KG(parameters.position,look,up,restored)||
        !detail::copyLocal49U(scratch,root,restored)) return false;
    std::memcpy(graph.nodes,nodes.data(),graph.count*sizeof(SceneNode49P));
    return true;
}
}
