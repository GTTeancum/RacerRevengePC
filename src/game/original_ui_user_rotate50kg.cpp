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
bool originalUiRiderPodOrientationPose50KG(const V& position,
    const OriginalUiRiderPodOrientation50KG& orientation,BodyMatrix48G& output) noexcept {
    if (std::fegetround()!=FE_TONEAREST||!finite(position)||
        !finite(orientation.look)||!finite(orientation.up)) return false;
    //289F90 skips289F9C..289FC8 for a3=0. Unlike1D3B60's rotation path,
    // neither captured vector is replaced by a cross/cross correction.
    V normalUp=orientation.up,normalLook=orientation.look;
    if (!normalize(normalUp)||!normalize(normalLook)) return false;
    const auto right=cross(normalUp,normalLook);
    if (!finite(right)) return false;
    BodyMatrix48G result{};
    for (unsigned i=0;i<3;++i) {
        result.v[i]=right[i];result.v[4+i]=normalUp[i];
        result.v[8+i]=normalLook[i];result.v[12+i]=position[i];
    }
    //28A014..28A028 clear basis W and set translation W=1 explicitly.
    result.v[15]=1;output=result;return true;
}
bool captureOriginalUiRiderPodOrientation50KG(const SceneGraph49P& graph,
    SceneNodeIndex49P root,OriginalUiRiderPodOrientation50KG& output) noexcept {
    const auto index=static_cast<std::int32_t>(root);
    if (!validateSceneGraph49P(graph)||index<0||
        static_cast<std::uint32_t>(index)>=graph.count) return false;
    const auto& local=graph.nodes[static_cast<std::size_t>(index)].localC0;
    OriginalUiRiderPodOrientation50KG saved;
    //28A538..28A56C are copies, including signed zeros and opaque FP bits.
    std::memcpy(saved.look.data(),local.v+8,sizeof(float)*3);
    std::memcpy(saved.up.data(),local.v+4,sizeof(float)*3);
    std::memcpy(&output,&saved,sizeof saved);return true;
}
bool restoreOriginalUiRiderPodOrientation50KG(const SceneGraph49P& graph,
    SceneNodeIndex49P root,const OriginalUiRiderPodOrientation50KG& orientation) noexcept {
    const auto index=static_cast<std::int32_t>(root);
    if (!validateSceneGraph49P(graph)||index<0||
        static_cast<std::uint32_t>(index)>=graph.count) return false;
    const auto& local=graph.nodes[static_cast<std::size_t>(index)].localC0;
    V position;std::memcpy(position.data(),local.v+12,sizeof(float)*3);
    BodyMatrix48G restored;
    if (!originalUiRiderPodOrientationPose50KG(position,orientation,restored)) return false;
    // Source28A03C..28A074 exactly matches this existing setter's post-write
    // continuation: mark node/ancestors80, then clear ready01 and descend only
    // if this target was ready. It does not eagerly recompute any world matrix.
    return setLocalMatrix49P(graph,root,restored);
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
