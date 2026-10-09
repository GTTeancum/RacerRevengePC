#include "vehicle_animation49t.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
#include <initializer_list>

#ifndef RR_VEHICLE_ANIMATION49T_STAGE_TRACE
#define RR_VEHICLE_ANIMATION49T_STAGE_TRACE(pc, graph) ((void)0)
#endif

namespace rr::game {
namespace {
using Index = SceneNodeIndex49P;
bool reject(VehicleAnimationDiagnostic49T& d, VehicleAnimationError49T e,
            std::uint32_t pc) noexcept { d.error=e;d.sourceStage=pc;return false; }
float value(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return keep(x);}
    float div(float a,float b) noexcept {volatile float x=a/b;return keep(x);}
};
float clamp(float x) noexcept {return detail::scalarMin48O(detail::scalarMax48O(x,0.f),1.f);}
bool filter(AnimationFilter49Q& s,float target,float dt,float& mirror) noexcept {
    if(!std::isfinite(s.value0)||!std::isfinite(s.time4)||!(s.time4>0)||!std::isfinite(target))return false;
    Math f;const float step=detail::scalarMin48O(dt,s.time4);
    s.alpha8=f.div(step,s.time4);
    const float difference=f.sub(target,s.value0);
    s.value0=f.add(s.value0,f.mul(s.alpha8,difference));mirror=s.value0;return f.valid;
}
bool overlap(const void* a,std::size_t an,const void* b,std::size_t bn) noexcept {
    if(!an||!bn)return false;
    const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
    return x<=y?y-x<an:x-y<bn;
}
bool root(const SceneGraph49P& g,Index i) noexcept {
    return i!=Index::none&&static_cast<std::uint32_t>(i)<g.count&&g.nodes[static_cast<std::uint32_t>(i)].parent180==Index::none;
}
bool subtree(const SceneGraph49P& g,Index node,Index r) noexcept {
    if(node==r||static_cast<std::uint32_t>(node)>=g.count)return false;
    while(node!=Index::none&&node!=r)node=g.nodes[static_cast<std::uint32_t>(node)].parent180;
    return node==r;
}
bool rows(const SceneGraph49P& g,const assets::MotionNodeBindings49O* b,Index r) noexcept {
    if(!b||!b->count||b->count>64)return false;
    for(std::uint32_t i=0;i<b->count;++i)if(!subtree(g,static_cast<Index>(b->rows[i].sceneNode),r))return false;
    return true;
}
bool spans(const VehicleAnimationInput49T& in,const VehicleAnimationBindings49T& b,
           const SceneGraph49P& g,const VehicleAnimationState49T& s,
           const VehicleAnimationShared49T& h,const VisualShared49R& v,
           const VehicleAnimationWorkspace49T& w) noexcept {
    const void* p[]={g.nodes,&s,&h,&v,&w};
    const std::size_t n[]={g.count*sizeof(SceneNode49P),sizeof s,sizeof h,sizeof v,sizeof w};
    for(unsigned i=0;i<5;++i)for(unsigned j=i+1;j<5;++j)if(overlap(p[i],n[i],p[j],n[j]))return false;
    if(overlap(&w,sizeof w,&in,sizeof in)||overlap(&w,sizeof w,&b,sizeof b)||overlap(&w,sizeof w,&g,sizeof g))return false;
    const assets::MotionView49O* views[]={b.brakeLeft28C,b.brakeRight290,b.forward280,b.back284,b.left2A0,b.right29C};
    for(const auto* view:views)if(view&&(overlap(&w,sizeof w,view,sizeof *view)||overlap(&w,sizeof w,view->data,view->bytes)))return false;
    const assets::MotionNodeBindings49O* maps[]={b.chariotRows,b.leftRows,b.rightRows};
    for(const auto* map:maps)if(map&&overlap(&w,sizeof w,map,sizeof *map))return false;
    return !b.handRecords||!overlap(&w,sizeof w,b.handRecords,sizeof *b.handRecords);
}
// Exact28BBB0 reached inverse scratch. Translation products still execute;
// direction W is unconsumed and final output W is literal1.
VisualVector49R inverseDirection(const BodyMatrix48G& m,const VisualVector49R& v,Math& f) noexcept {
    BodyMatrix48G inverse=m;
    for(unsigned c=0;c<3;++c)for(unsigned r=0;r<4;++r)
        inverse.v[c*4+r]=(r==3||r==c)?f.add(m.v[c*4+r],0.f):f.add(0.f,m.v[r*4+c]);
    for(unsigned r=0;r<3;++r){float x=f.mul(inverse.v[r],m.v[12]);x=f.add(x,f.mul(inverse.v[4+r],m.v[13]));x=f.add(x,f.mul(inverse.v[8+r],m.v[14]));inverse.v[12+r]=f.mul(x,-1.f);}
    inverse.v[15]=f.add(m.v[15],0.f);
    VisualVector49R out{{0,0,0,1}};
    for(unsigned r=0;r<3;++r){float x=f.mul(inverse.v[r],v.v[0]);x=f.add(x,f.mul(inverse.v[4+r],v.v[1]));x=f.add(x,f.mul(inverse.v[8+r],v.v[2]));out.v[r]=f.add(x,0.f);}
    return out;
}
}
bool initializeVehicleAnimationFilters49T(float left,float right,VehicleAnimationState49T& s) noexcept {
    if(!std::isfinite(left)||!std::isfinite(right))return false;
    s.leftB00={left,value(0x3e4ccccd),1};s.rightB08={right,value(0x3e4ccccd),1};
    s.poseB18={left,value(0x3dcccccd),1};s.blendB10={right,value(0x3dcccccd),1};
    s.leftB04=left;s.rightB0C=right;return true;
}
bool advanceVehicleAnimation49T(const VehicleAnimationInput49T& in,
    const VehicleAnimationBindings49T& b,const SceneGraph49P& graph,
    VehicleAnimationState49T& state,VehicleAnimationShared49T& shared,VisualShared49R& visual,
    VehicleAnimationWorkspace49T& w,VehicleAnimationDiagnostic49T& d) noexcept {
    d={};
    if(in.phase!=VehicleAnimationPhase49T::beforeAnimation21A000||!in.midRecordsAbsent)
        return reject(d,VehicleAnimationError49T::context,0x21a000);
    if(std::fegetround()!=FE_TONEAREST)return reject(d,VehicleAnimationError49T::rounding_mode,0x21a000);
    if(!graph.count||!validateSceneGraph49P(graph))return reject(d,VehicleAnimationError49T::graph,0x21a000);
    if(!spans(in,b,graph,state,shared,visual,w))return reject(d,VehicleAnimationError49T::records,0x21a000);
    for(std::uint32_t n=0;n<graph.count;++n)for(unsigned i=0;i<16;++i)
        if(!std::isfinite(graph.nodes[n].localC0.v[i])||!std::isfinite(graph.nodes[n].world100.v[i]))
            return reject(d,VehicleAnimationError49T::input,0x21a000);
    w.state=state;w.shared=shared;w.visual=visual;
    if(!w.shared.enableGuard3D60F4){w.shared.enable434748=1;w.shared.enableGuard3D60F4=1;}
    if(!w.shared.enable434748){shared=w.shared;RR_VEHICLE_ANIMATION49T_STAGE_TRACE(0x21a654,graph);return true;}
    if(!std::isfinite(in.animationDelta23C)||in.animationDelta23C<0)
        return reject(d,VehicleAnimationError49T::input,0x21a040);
    if(!filter(w.state.leftB00,in.aeroE4,in.animationDelta23C,w.state.leftB04)||
       !filter(w.state.rightB08,in.aeroE8,in.animationDelta23C,w.state.rightB0C))
        return reject(d,VehicleAnimationError49T::arithmetic,0x21a040);
    const Index roots[]={b.chariot1B4,b.left1BC,b.right1B8};
    for(unsigned i=0;i<3;++i)if(roots[i]!=Index::none){
        if(!root(graph,roots[i]))return reject(d,VehicleAnimationError49T::bindings,0x21a0d8);
        for(unsigned j=0;j<i;++j)if(roots[i]==roots[j])return reject(d,VehicleAnimationError49T::bindings,0x21a0d8);
    }
    const bool chariot=b.chariot1B4!=Index::none&&(in.chariot191&1);
    const bool brake=chariot&&b.brakeLeft28C&&b.brakeRight290;
    const bool left=b.left2A0&&b.left1BC!=Index::none&&(in.left191&1);
    const bool right=b.right29C&&b.right1B8!=Index::none&&(in.right191&1);
    const bool suffix=chariot&&!in.chariot1B0;
    const bool pair=suffix&&b.forward280&&b.back284&&in.body340Present;
    if(((brake||pair)&&!rows(graph,b.chariotRows,b.chariot1B4))||
       (left&&!rows(graph,b.leftRows,b.left1BC))||(right&&!rows(graph,b.rightRows,b.right1B8)))
        return reject(d,VehicleAnimationError49T::bindings,0x21a184);
    if(suffix){
        for(const auto* hand:{&b.hands.left,&b.hands.right})if(hand->target!=Index::none)
            if(!b.handRecords||!subtree(graph,hand->target,b.chariot1B4)||
               !subtree(graph,hand->shoulder,b.chariot1B4)||!subtree(graph,hand->elbow,b.chariot1B4)||!subtree(graph,hand->wrist,b.chariot1B4))
                return reject(d,VehicleAnimationError49T::bindings,0x21a620);
    }
    std::memcpy(w.nodes,graph.nodes,graph.count*sizeof(SceneNode49P));
    const SceneGraph49P staged{w.nodes,graph.count};
    auto single=[&](const assets::MotionView49O* motion,const assets::MotionNodeBindings49O* map,
                    float fraction,ModelMotionState49T& model,std::uint32_t pc) noexcept {
        RR_VEHICLE_ANIMATION49T_STAGE_TRACE(pc,staged);
        if(!sampleModelMotion49T(*motion,*map,fraction,staged,model,w.shared.motion,w.motion,d.motion))
            return reject(d,VehicleAnimationError49T::motion,pc);
        return true;
    };
    if(brake){w.state.leftB04=clamp(w.state.leftB04);w.state.rightB0C=clamp(w.state.rightB0C);
        if(!single(b.brakeLeft28C,b.chariotRows,w.state.leftB04,w.state.chariotModel,0x21a190)||
           !single(b.brakeRight290,b.chariotRows,w.state.rightB0C,w.state.chariotModel,0x21a1a8))return false;}
    if(left){w.state.leftB04=clamp(w.state.leftB04);if(!single(b.left2A0,b.leftRows,w.state.leftB04,w.state.leftModel,0x21a298))return false;}
    if(right){w.state.rightB0C=clamp(w.state.rightB0C);if(!single(b.right29C,b.rightRows,w.state.rightB0C,w.state.rightModel,0x21a318))return false;}
    if(pair){
        if(static_cast<std::uint32_t>(b.bodyScene288)>=graph.count)return reject(d,VehicleAnimationError49T::bindings,0x21a460);
        for(unsigned i=0;i<3;++i)if(!std::isfinite(in.bodyAccelerationC0.v[i]))return reject(d,VehicleAnimationError49T::input,0x21a460);
        if(!refreshSceneWorld49P(staged,b.bodyScene288))return reject(d,VehicleAnimationError49T::graph,0x21a460);
        Math f;const auto local=inverseDirection(w.nodes[static_cast<std::uint32_t>(b.bodyScene288)].world100,in.bodyAccelerationC0,f);
        const float tx=detail::scalarMin48O(detail::scalarMax48O(0.f,f.add(.5f,f.mul(.5f,f.mul(value(0x3b23d70a),-local.v[0])))),1.f);
        const float tz=detail::scalarMin48O(detail::scalarMax48O(0.f,f.add(.5f,f.mul(.5f,f.mul(value(0x3b03126f),local.v[2])))),1.f);
        const float blendTarget=f.sub(1.f,tz);
        if(!f.valid||!filter(w.state.poseB18,tx,in.animationDelta23C,w.state.poseB1C)||
           !filter(w.state.blendB10,blendTarget,in.animationDelta23C,w.state.blendB14))
            return reject(d,VehicleAnimationError49T::arithmetic,0x21a490);
        w.state.poseB1C=clamp(w.state.poseB1C);w.state.blendB14=clamp(w.state.blendB14);
        RR_VEHICLE_ANIMATION49T_STAGE_TRACE(0x21a618,staged);
        if(!sampleModelMotionBlend49T(*b.forward280,*b.back284,*b.chariotRows,w.state.poseB1C,w.state.blendB14,
            staged,w.state.chariotModel,w.shared.motion,w.motion,d.motion))return reject(d,VehicleAnimationError49T::motion,0x21a618);
    }
    if(suffix)RR_VEHICLE_ANIMATION49T_STAGE_TRACE(0x21a620,staged);
    if(suffix&&(b.hands.left.target!=Index::none||b.hands.right.target!=Index::none))
        if(!solveChariotHands49T(b.hands,*b.handRecords,staged,w.shared.libm,w.shared.ik,w.visual,w.ik,d.ik))
            return reject(d,VehicleAnimationError49T::ik,0x21a62c);
    RR_VEHICLE_ANIMATION49T_STAGE_TRACE(0x21a654,staged);
    std::memcpy(graph.nodes,w.nodes,graph.count*sizeof(SceneNode49P));
    state=w.state;shared=w.shared;visual=w.visual;return true;
}
bool vehicleAnimationInputFromBindings49T(const ControlModelBindings48W& source,
    const VehicleAnimationInput49T& context,VehicleAnimationInput49T& out) noexcept {
    if(!std::isfinite(source.aeroDrive[0])||!std::isfinite(source.aeroDrive[1]))return false;
    auto result=context;result.aeroE4=source.aeroDrive[0];result.aeroE8=source.aeroDrive[1];out=result;return true;
}
}
