#include "original_gameplay_binder_candidate.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game::original_gameplay_binder_candidate {
namespace {
float add(float a,float b) noexcept {volatile float v=a+b;return v;}
float sub(float a,float b) noexcept {volatile float v=a-b;return v;}
float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
bool valid(SceneNodeIndex49P i,std::uint32_t n) noexcept {
    return static_cast<std::int32_t>(i)>=0&&static_cast<std::uint32_t>(i)<n;
}
bool descendant(const SceneGraph49P& g,SceneNodeIndex49P i,SceneNodeIndex49P root) noexcept {
    if(!valid(i,g.count)||!valid(root,g.count)||i==root)return false;
    for(std::uint32_t n=0;n<g.count;++n){
        i=g.nodes[static_cast<std::uint32_t>(i)].parent180;
        if(i==root)return true;
        if(i==SceneNodeIndex49P::none)return false;
    }
    return false;
}
}
Status project(const VehicleFrameState49W& s,const VehicleFrameBindings49W& frame,
    const Bindings& b,const std::array<float,3>& camera,
    OriginalUiRiderBinderQuad50KG& output,Workspace& work) noexcept {
    if(s.phase!=VehicleFramePhase49W::Ready&&s.phase!=VehicleFramePhase49W::PostGraphics)
        return Status::phase;
    if(s.bodies.main.vehicleState40!=0||s.fields.vehicle3C!=0)return Status::unsupportedVehicle;
    if(std::fegetround()!=FE_TONEAREST)return Status::arithmetic;
    if(!std::isfinite(b.halfHeight25C)||b.halfHeight25C<=0)return Status::input;
    for(float x:camera)if(!std::isfinite(x))return Status::input;
    if(!validVehicleSceneCount49W(s,frame))return Status::graph;
    std::memcpy(work.nodes.data(),s.nodes,s.sceneNodeCount*sizeof(SceneNode49P));
    const SceneGraph49P graph{work.nodes.data(),s.sceneNodeCount};
    if(!validateSceneGraph49P(graph))return Status::graph;
    auto leftRoot=frame.lifecycle.left1BC,rightRoot=frame.lifecycle.right1B8;
    if(b.pair==Pair::bottomLeftToPrimaryRight)leftRoot=frame.visual.bottomLeft1C4;
    else if(b.pair==Pair::primaryLeftToBottomRight)rightRoot=frame.visual.bottomRight1C0;
    else if(b.pair!=Pair::primary)return Status::binding;
    if(b.pair!=Pair::primary&&(!frame.visual.extra1C0Present||!frame.visual.extra1C4Present))
        return Status::binding;
    if(!descendant(graph,b.left230,leftRoot)||!descendant(graph,b.right238,rightRoot))return Status::binding;
    if(!refreshSceneWorld49P(graph,b.left230)||!refreshSceneWorld49P(graph,b.right238))
        return Status::graph;
    const auto& left=work.nodes[static_cast<std::uint32_t>(b.left230)].world100;
    const auto& right=work.nodes[static_cast<std::uint32_t>(b.right238)].world100;
    std::array<float,3> delta{},width{};
    for(unsigned c=0;c<3;++c)delta[c]=sub(left.v[12+c],right.v[12+c]);
    width={{sub(mul(camera[1],delta[2]),mul(camera[2],delta[1])),
        sub(mul(camera[2],delta[0]),mul(camera[0],delta[2])),
        sub(mul(camera[0],delta[1]),mul(camera[1],delta[0]))}};
    const float n=add(add(mul(width[0],width[0]),mul(width[1],width[1])),mul(width[2],width[2]));
    if(!std::isfinite(n)||n<=0||std::fpclassify(n)==FP_SUBNORMAL)return Status::arithmetic;
    volatile float inverse=1.f/std::sqrt(n);
    OriginalUiRiderBinderQuad50KG next;
    for(unsigned c=0;c<3;++c){
        const float w=mul(b.halfHeight25C,mul(width[c],inverse));
        next.position[0][c]=sub(left.v[12+c],w);
        next.position[1][c]=add(left.v[12+c],w);
        next.position[2][c]=sub(right.v[12+c],w);
        next.position[3][c]=add(right.v[12+c],w);
    }
    for(auto& v:next.position){v[3]=1.f;for(float x:v)if(!std::isfinite(x))return Status::arithmetic;}
    output=next;return Status::complete;
}
Status shade(const OriginalUiRiderBinderQuad50KG& quad,
    const std::array<float,4>& ambient,Mesh& output) noexcept {
    if(std::fegetround()!=FE_TONEAREST)return Status::arithmetic;
    std::array<std::uint8_t,4> color;
    for(unsigned c=0;c<4;++c){
        if(!std::isfinite(ambient[c]))return Status::input;
        float p=add(ambient[c],8388608.f);
        if(!std::isfinite(p)||p<8388608.f)return Status::arithmetic;
        if(p>8388863.f)p=8388863.f;
        std::uint32_t bits=0;std::memcpy(&bits,&p,4);color[c]=static_cast<std::uint8_t>(bits&255u);
    }
    Mesh next;
    for(unsigned i=0;i<4;++i){
        if(quad.position[i][3]!=1.f)return Status::input;
        for(unsigned c=0;c<3;++c){
            if(!std::isfinite(quad.position[i][c]))return Status::input;
            next.vertices[i].position[c]=quad.position[i][c];
        }
        for(unsigned c=0;c<2;++c){
            if(!std::isfinite(quad.uv[i][c]))return Status::input;
            next.vertices[i].uv[c]=quad.uv[i][c];
        }
        for(unsigned c=0;c<4;++c)next.vertices[i].color[c]=color[c];
    }
    output=next;return Status::complete;
}
}
