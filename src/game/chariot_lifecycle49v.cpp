#include "chariot_lifecycle49v.h"
#include "chariot_blend49v.h"
#include "chariot_scene49u.h"
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <utility>
#ifndef RR_CHARIOT_LIFECYCLE49V_STAGE
#define RR_CHARIOT_LIFECYCLE49V_STAGE(pc) ((void)0)
#endif
namespace rr::game {
namespace {
using V = VisualVector49R;
using I = SceneNodeIndex49P;
using M = BodyMatrix48G;
float fp(std::uint32_t word) noexcept { float x; std::memcpy(&x, &word, 4); return x; }
std::uint32_t bits(float x) noexcept { std::uint32_t word; std::memcpy(&word, &x, 4); return word; }
struct Math {
    bool valid = true;
    float read(float x) noexcept { valid = valid && std::isfinite(x); return x; }
    float add(float a, float b) noexcept { read(a); read(b); volatile float x = a + b; return read(x); }
    float sub(float a, float b) noexcept { read(a); read(b); volatile float x = a - b; return read(x); }
    float mul(float a, float b) noexcept { read(a); read(b); volatile float x = a * b; return read(x); }
    float div(float a, float b) noexcept { read(a); read(b); volatile float x = a / b; return read(x); }
    float neg(float a) noexcept { read(a); return fp(bits(a) ^ 0x80000000u); }
    float abs(float a) noexcept { read(a); return fp(bits(a) & 0x7fffffffu); }
    // EE MIN/MAX order the finite binary words, including -0 below +0.
    float maximum(float a, float b) noexcept {
        read(a); read(b); const auto x = static_cast<std::int32_t>(bits(a));
        const auto y = static_cast<std::int32_t>(bits(b));
        return fp(static_cast<std::uint32_t>((x < 0 && y < 0) ? (x < y ? x : y) : (x > y ? x : y)));
    }
    float minimum(float a, float b) noexcept {
        read(a); read(b); const auto x = static_cast<std::int32_t>(bits(a));
        const auto y = static_cast<std::int32_t>(bits(b));
        return fp(static_cast<std::uint32_t>((x < 0 && y < 0) ? (x > y ? x : y) : (x < y ? x : y)));
    }
    float scalarRoot(float a) noexcept {
        read(a);
        // Retained EE reference convention: subnormal input becomes signed0.
        if ((bits(a) & 0x7f800000u) == 0) return fp(bits(a) & 0x80000000u);
        volatile float x = static_cast<float>(std::sqrt(static_cast<double>(a))); return read(x);
    }
    float reciprocalRoot(float a) noexcept {
        read(a);
        // 14F410 uses the scalar EE RSQRT model, not the VU Q model below.
        if ((bits(a) & 0x7f800000u) == 0) return fp(0x7f7fffffu);
        volatile float x = static_cast<float>(1.0 / std::sqrt(static_cast<double>(a)));
        if ((bits(x) & 0x7f800000u) == 0) return 0.0f;
        return read(x);
    }
    float squared(const V& v, bool vu) noexcept {
        const float x = mul(v.v[0], v.v[0]);
        const float y = mul(v.v[1], v.v[1]);
        const float z = mul(v.v[2], v.v[2]);
        return add(add(x, y), vu ? mul(1.0f, z) : z);
    }
    V difference(const V& a, const V& b) noexcept {
        return V{{sub(a.v[0], b.v[0]), sub(a.v[1], b.v[1]), sub(a.v[2], b.v[2]), 1.0f}};
    }
    V vuNormalize(const V& v) noexcept {
        const float norm = squared(v, true); float q;
        //49R excludes positive subnormal squared VU norms; the retained host
        // reference is not a proof of hardware underflow behavior there.
        if (norm != 0.0f && (bits(norm) & 0x7f800000u) == 0) valid = false;
        // VRSQRT(1,0) saturates Q. The original still multiplies all XYZ.
        if (norm == 0.0f) q = fp((bits(norm) & 0x80000000u) | 0x7f7fffffu);
        else { volatile float root = std::sqrt(norm); q = div(1.0f, read(root)); }
        return V{{mul(v.v[0], q), mul(v.v[1], q), mul(v.v[2], q), 1.0f}};
    }
    float dot(const V& a, const V& b) noexcept {
        const float x = mul(a.v[0], b.v[0]);
        const float y = mul(a.v[1], b.v[1]);
        const float z = mul(a.v[2], b.v[2]);
        return add(add(x, y), mul(1.0f, z));
    }
};
void normalize(Quaternion48C& q, Math& f) noexcept {
    const float norm = f.add(f.add(f.mul(q.v[2], q.v[2]),
        f.add(f.mul(q.v[1], q.v[1]), f.mul(q.v[0], q.v[0]))), f.mul(q.v[3], q.v[3]));
    if (norm == 0.0f) q = {}; // Source382B60: zero quaternion, not identity.
    else { const float scale = f.reciprocalRoot(norm); for (float& x : q.v) x = f.mul(x, scale); }
}
void integrate(VisualResponse49R& s, const V& force, const V& torque, float dt, Math& f) noexcept {
    for (unsigned i = 0; i < 3; ++i) s.linearVelocity.v[i] = f.add(s.linearVelocity.v[i], f.mul(dt, force.v[i]));
    for (unsigned i = 0; i < 3; ++i) s.position.v[i] = f.add(s.position.v[i], f.mul(dt, s.linearVelocity.v[i]));
    for (unsigned i = 0; i < 3; ++i) s.angularVelocity.v[i] = f.add(s.angularVelocity.v[i], f.mul(dt, torque.v[i]));
    const auto& q = s.orientation.v; const auto& w = s.angularVelocity.v;
    Quaternion48C derivative{};
    derivative.v[0] = f.mul(-0.5f, f.add(f.add(f.mul(q[1], w[0]), f.mul(q[2], w[1])), f.mul(q[3], w[2])));
    derivative.v[1] = f.mul(0.5f, f.sub(f.add(f.mul(q[0], w[0]), f.mul(q[2], w[2])), f.mul(q[3], w[1])));
    derivative.v[2] = f.mul(0.5f, f.sub(f.add(f.mul(q[0], w[1]), f.mul(q[3], w[0])), f.mul(q[1], w[2])));
    derivative.v[3] = f.mul(0.5f, f.sub(f.add(f.mul(q[0], w[2]), f.mul(q[1], w[1])), f.mul(q[2], w[0])));
    for (float& x : derivative.v) x = f.mul(x, dt);
    for (unsigned i = 0; i < 4; ++i) s.orientation.v[i] = f.add(s.orientation.v[i], derivative.v[i]);
    normalize(s.orientation, f);
}
void addScaled(V& into, float scale, const V& source, Math& f) noexcept {
    float products[3]; for (unsigned i = 0; i < 3; ++i) products[i] = f.mul(scale, source.v[i]);
    for (unsigned i = 0; i < 3; ++i) into.v[i] = f.add(into.v[i], products[i]);
}
bool noiseAdd(float& lane, float coordinate, float amplitude, CoupledNoiseContext49M& noise, Math& f) noexcept {
    float value;
    if (!f.valid || !evaluateGradientNoise49M(noise.noise, noise.sharedRandomState, coordinate, value)) return false;
    lane = f.add(lane, f.mul(amplitude, value)); return f.valid;
}
bool fail(ChariotLifecycleDiagnostic49V& d, ChariotLifecycleError49V e, std::uint32_t pc) noexcept {
    d = {e, pc}; return false;
}
void lazy(VisualLazyWord49R& w, std::uint32_t value) noexcept {
    if (!w.guard) { w.word = value; w.guard = 1; }
}
float endpoints(float x, float first, float last, Math& f) noexcept {
    f.read(x); f.read(first); f.read(last);
    if (last <= first) {
        if (x < last) return last;
        if (!(x <= first)) return first;
    } else {
        if (!(x <= last)) return last;
        if (x < first) return first;
    }
    return x;
}
float interpolate(float x, float lo, float hi, float first, float last, Math& f) noexcept {
    f.read(lo); f.read(hi);
    if (hi == lo) return 0.0f;
    const float dx = f.sub(x, lo);
    const float den = f.sub(hi, lo);
    const float dy = f.sub(last, first);
    return endpoints(f.add(first, f.div(f.mul(dx, dy), den)), first, last, f);
}
V cross(const V& a, const V& b, Math& f) noexcept {
    V out{{0,0,0,0}};
    for (unsigned i=0;i<3;++i) {
        const unsigned j=(i+1)%3,k=(i+2)%3;
        out.v[i]=f.sub(f.mul(a.v[j],b.v[k]),f.mul(b.v[j],a.v[k]));
    }
    return out;
}
SceneNode49P& node(const SceneGraph49P& g,I i) noexcept {
    return g.nodes[static_cast<std::uint32_t>(i)];
}
bool response(const ChariotRootInput49U& in,const ChariotRootParameters49U& p,
    const ChariotRootBindings49U& b,const SceneGraph49P& g,ChariotLifecycleState49V& s,
    const ChariotRootShared49U& shared,const V& zero,float& phase,
    CoupledNoiseContext49M& noise,ChariotLifecycleDiagnostic49V& d) noexcept {
    Math f;
    // The speed square in21A7FC executes even when A54 is zero.
    const float square=f.mul(in.cachedBodySpeed34,in.cachedBodySpeed34);
    float amplitude=interpolate(square,0.0f,p.effectiveSpeedSquaredA54,
        p.minAmplitudeA4C,p.maxAmplitudeA50,f);
    const float rate=interpolate(square,0.0f,p.effectiveSpeedSquaredA54,
        p.minTimeA3C,p.maxTimeA40,f);
    phase=f.add(phase,f.mul(rate,in.frameFactor));
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21a940u);
    V force=zero,torque=zero,forward{{0,0,0,1}},up{{0,0,0,1}},position{{0,0,0,1}};
    // Within finite surviving source VU scratch, direction result W is dead to
    // every represented output. Other scratch self-subtractions are not a
    // claim of arbitrary incoming VU NaN independence.
    detail::SceneVectorContext49U context{0.0f};
    if (!detail::axes49U(g,b.hidden198,b.container194,context,forward,up) ||
        !detail::position49U(g,b.hidden198,b.container194,context,position))
        return fail(d,ChariotLifecycleError49V::graph,0x21a954);
    if (!in.aeroD8) {
        // Left first, then right. Cable nodes are actual animated Chariot PSG
        // indices; both getter refreshes and only the resulting torque Z live.
        const I engines[]={b.left1BC,b.right1B8};
        const I cables[]={b.leftCable1E0,b.rightCable1E4};
        for (unsigned side=0;side<2;++side) {
            V engine{},attachment{},arm{},world{},local{};
            if (!detail::position49U(g,engines[side],b.container194,context,engine) ||
                !detail::position49U(g,cables[side],b.container194,context,attachment))
                return fail(d,ChariotLifecycleError49V::graph,0x21a98c);
            V cable=f.vuNormalize(f.difference(engine,attachment));
            for(unsigned i=0;i<3;++i)cable.v[i]=f.mul(p.cableForceA6C,cable.v[i]);
            if (!detail::position49U(g,cables[side],b.hidden198,context,arm) ||
                !detail::direction49U(g,b.container194,cable,context,world) ||
                !detail::inverseDirection49U(g,b.hidden198,world,context,local))
                return fail(d,ChariotLifecycleError49V::graph,0x21aa4c);
            torque.v[2]=f.add(torque.v[2],cross(arm,local,f).v[2]);
        }
    }
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21abf4u);
    const V velocityDirection=f.vuNormalize(in.bodyVelocity80);
    const float threshold=fp(shared.speed434750.word);
    const float twice=f.mul(2.0f,threshold);
    const float angleLimit=interpolate(in.cachedBodySpeed34,threshold,twice,
        f.neg(fp(shared.yaw434758.word)),p.yawAngleA8C,f);
    const float angle=interpolate(in.bodyAngularVelocityYB4,f.neg(p.yawInputRangeA88),
        p.yawInputRangeA88,f.neg(angleLimit),angleLimit,f);
    V backward{{f.neg(velocityDirection.v[0]),f.neg(velocityDirection.v[1]),
        f.neg(velocityDirection.v[2]),1}},target{{0,0,0,1}};
    if (!f.valid || !detail::rotateVector49U(shared.up434650,backward,angle,target))
        return fail(d,ChariotLifecycleError49V::arithmetic,0x142b80);
    V radial=f.vuNormalize(s.response.position);
    const float error=f.scalarRoot(f.squared(cross(radial,target,f),false));
    V wanted{{f.mul(p.restLength854,target.v[0]),f.mul(p.restLength854,target.v[1]),
        f.mul(p.restLength854,target.v[2]),1}};
    V tangent=f.difference(wanted,s.response.position);
    const float projection=f.dot(radial,tangent);
    for(unsigned i=0;i<3;++i)tangent.v[i]=f.sub(tangent.v[i],f.mul(projection,radial.v[i]));
    tangent=f.vuNormalize(tangent);
    float derivative=0.0f;
    if(in.frameFactor>0.0f)derivative=f.div(f.sub(s.previous85C,error),in.frameFactor);
    s.previous85C=error;
    const float restoring=in.aeroD8?p.slideRestoringA78:p.restoringA74;
    addScaled(force,f.mul(p.returnGainA7C,f.sub(f.mul(restoring,error),derivative)),tangent,f);
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21b07cu);
    V impact{};
    if(!detail::inverseDirection49U(g,b.main190,in.impactBA0,context,impact))
        return fail(d,ChariotLifecycleError49V::graph,0x21b08c);
    if(f.read(in.objectD34_10)>fp(0x3e4ccccdu)&&(in.vehicleD00||in.vehicleCFC))impact.v[0]=0.0f;
    for(unsigned i=0;i<3;++i)impact.v[i]=f.minimum(f.maximum(impact.v[i],f.neg(p.collisionBoundA98)),p.collisionBoundA98);
    V impactForce{{f.mul(p.collisionXYAA0,impact.v[0]),f.mul(p.collisionXYAA0,impact.v[1]),
        f.mul(p.collisionZAA4,impact.v[2]),1}};
    for(unsigned i=0;i<3;++i)force.v[i]=f.add(force.v[i],impactForce.v[i]);
    V ground{};
    if(!detail::inverseDirection49U(g,b.main190,in.probeVector1D0,context,ground))
        return fail(d,ChariotLifecycleError49V::graph,0x21b204);
    float groundForce=f.mul(p.groundGainAA8,f.minimum(f.maximum(ground.v[1],f.neg(p.groundBoundA9C)),p.groundBoundA9C));
    if(s.vehicle840&&!in.probe10C)groundForce=f.mul(groundForce,p.groundTransitionGainAAC);
    force.v[1]=f.add(force.v[1],groundForce);
    addScaled(force,p.linearDampingA80,s.response.linearVelocity,f);
    addScaled(torque,p.angularDampingA84,s.response.angularVelocity,f);
    V direction{{f.neg(position.v[0]),f.neg(position.v[1]),f.neg(position.v[2]),1}};
    if(f.abs(direction.v[1])<0.5f)direction.v[1]=0.0f;
    direction=f.vuNormalize(direction);
    V alignment=cross(direction,forward,f);
    const float alignmentLength=f.scalarRoot(f.squared(alignment,false));
    alignment=f.vuNormalize(alignment);
    addScaled(torque,f.mul(p.orientationGainA94,alignmentLength),alignment,f);
    radial=f.vuNormalize(s.response.position);
    const float length=f.scalarRoot(f.squared(s.response.position,false));
    addScaled(force,f.mul(p.radiusForceA70,f.sub(p.restLength854,length)),radial,f);
    if(!f.valid)return fail(d,ChariotLifecycleError49V::arithmetic,0x21b590);
    integrate(s.response,force,torque,in.frameFactor,f);
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21b740u);
    const float noiseScale=interpolate(in.cachedBodySpeed34,threshold,f.mul(2.0f,threshold),0.0f,1.0f,f);
    amplitude=f.mul(noiseScale,amplitude);
    V visualPosition=s.response.position;
    if(!noiseAdd(visualPosition.v[0],phase,amplitude,noise,f)||
        !noiseAdd(visualPosition.v[1],f.add(13.0f,phase),amplitude,noise,f))
        return fail(d,ChariotLifecycleError49V::noise,0x21b7c4);
    Quaternion48C visual=s.response.orientation;
    amplitude=f.mul(amplitude,p.orientationScaleA60);
    const float offsets[]={5.0f,3.0f,17.0f,1.0f};
    for(unsigned i=0;i<4;++i)
        if(!noiseAdd(visual.v[(i+1)%4],f.add(offsets[i],phase),amplitude,noise,f))
            return fail(d,ChariotLifecycleError49V::noise,0x21b838);
    normalize(visual,f);
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21b8c8u);
    M local{};
    if(!f.valid||!quaternionToMatrix48G(visual,local))return fail(d,ChariotLifecycleError49V::arithmetic,0x14f620);
    //14F620's discarded rotated-position scratch is still finite arithmetic.
    for(unsigned lane=0;lane<3;++lane){
        float x=f.mul(local.v[lane],visualPosition.v[0]);
        x=f.add(x,f.mul(local.v[4+lane],visualPosition.v[1]));
        x=f.add(x,f.mul(local.v[8+lane],visualPosition.v[2]));
        (void)f.add(x,0.0f);local.v[12+lane]=visualPosition.v[lane];
    }
    local.v[15]=1.0f;
    const auto& world=node(g,b.container194).world100;
    M composed{};
    for(unsigned row=0;row<4;++row)for(unsigned lane=0;lane<4;++lane){
        float x=f.mul(world.v[lane],local.v[4*row]);
        x=f.add(x,f.mul(world.v[4+lane],local.v[4*row+1]));
        x=f.add(x,f.mul(world.v[8+lane],local.v[4*row+2]));
        composed.v[4*row+lane]=f.add(x,f.mul(world.v[12+lane],local.v[4*row+3]));
    }
    if(!f.valid)return fail(d,ChariotLifecycleError49V::arithmetic,0x21b998);
    if(!setLocalMatrix49P(g,b.hidden198,composed))return fail(d,ChariotLifecycleError49V::graph,0x21b99c);
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21b9a4u);
    return true;
}
bool overlap(const void* a,std::size_t n,const void* b,std::size_t m) noexcept {
    const auto x=reinterpret_cast<std::uintptr_t>(a),y=reinterpret_cast<std::uintptr_t>(b);
    return n&&m&&(x<=y?y-x<n:x-y<m);
}
bool bindings(const SceneGraph49P& g,const ChariotRootBindings49U& b) noexcept {
    const I list[]={b.main190,b.container194,b.hidden198,b.physical19C,b.model1B4,
        b.left1BC,b.right1B8,b.leftCable1E0,b.rightCable1E4};
    for(unsigned i=0;i<9;++i){
        const auto n=static_cast<std::int32_t>(list[i]);
        if(n<0||static_cast<std::uint32_t>(n)>=g.count)return false;
        for(unsigned j=0;j<i;++j)if(list[i]==list[j])return false;
    }
    for(I root:{b.main190,b.hidden198,b.physical19C,b.model1B4,b.left1BC,b.right1B8})
        if(node(g,root).parent180!=I::none)return false;
    if(node(g,b.container194).parent180!=b.main190)return false;
    for(I cable:{b.leftCable1E0,b.rightCable1E4}){
        I n=node(g,cable).parent180;
        while(n!=I::none&&n!=b.model1B4)n=node(g,n).parent180;
        if(n!=b.model1B4)return false;
    }
    return true;
}
}
bool updateChariotLifecycle49V(const ChariotLifecycleInput49V& input,
    const ChariotLifecycleParameters49V& p,const ChariotLifecycleBindings49V& b,
    const SceneGraph49P& graph,const MainBodyState49K& main,const MainSphereLanes49M& mainLanes,
    SecondaryBodyState49M& secondary,SecondaryActivationSupplement49V& supplement,
    const SecondaryActivationGlobals49V& globals,std::uint32_t& physical,
    ChariotLifecycleState49V& s,ChariotLifecycleShared49V& shared,const V& zero,
    CoupledNoiseContext49M& noise,ChariotLifecycleWorkspace49V& w,
    ChariotLifecycleDiagnostic49V& d) noexcept {
    d={};
    if(input.phase!=ChariotLifecyclePhase49V::afterEngineResponseBeforePhysics||
        main.vehicleState40||input.vehicle58||(!physical&&!s.vehicle83C))
        return fail(d,ChariotLifecycleError49V::context,0x21a660);
    if(std::fegetround()!=FE_TONEAREST)return fail(d,ChariotLifecycleError49V::rounding_mode,0x21a660);
    if(!validateSceneGraph49P(graph)||!bindings(graph,b))
        return fail(d,ChariotLifecycleError49V::graph,0x21a660);
    const void* records[]={graph.nodes,&s,&shared,&secondary,&supplement,&physical,&noise,&w,&main,&mainLanes};
    const std::size_t sizes[]={graph.count*sizeof(SceneNode49P),sizeof s,sizeof shared,sizeof secondary,
        sizeof supplement,sizeof physical,sizeof noise,sizeof w,sizeof main,sizeof mainLanes};
    for(unsigned i=0;i<10;++i)for(unsigned j=0;j<i;++j)
        if(overlap(records[i],sizes[i],records[j],sizes[j]))
            return fail(d,ChariotLifecycleError49V::records,0x21a660);
    for(const auto& item:{std::pair<const void*,std::size_t>{&input,sizeof input},{&p,sizeof p},
        {&b,sizeof b},{&graph,sizeof graph},{&zero,sizeof zero},{&globals,sizeof globals}})
        if(overlap(&w,sizeof w,item.first,item.second))
            return fail(d,ChariotLifecycleError49V::records,0x21a660);
    const auto& physicalNode=node(graph,b.physical19C);
    if(!secondary.scenePresent||std::memcmp(&secondary.root.scene,&physicalNode.world100,sizeof(M))||
        (secondary.sceneFlags190&255u)!=physicalNode.sceneFlags190)
        return fail(d,ChariotLifecycleError49V::body_scene,0x21a660);
    const float effective=shared.speed434750.guard?fp(shared.speed434750.word):fp(0x4312aa7fu);
    if(!std::isfinite(input.frameFactor)||input.frameFactor<0||!std::isfinite(main.cachedSpeed34)||
        !std::isfinite(effective)||!std::isfinite(s.duration848)||!(s.duration848>0))
        return fail(d,ChariotLifecycleError49V::input,0x21a660);
    // This temporary source-shaped input is derived only at the call boundary;
    // it is not another persistent body/cache/clock owner.
    ChariotRootInput49U in{};
    in.phase=input.phase;in.vehicle58=input.vehicle58;in.vehicle40=main.vehicleState40;
    in.aeroD8=input.aeroD8;in.vehicleCFC=input.vehicleCFC;in.vehicleD00=input.vehicleD00;
    in.probe10C=main.components.probe348.field10C;in.frameFactor=input.frameFactor;
    in.cachedBodySpeed34=main.cachedSpeed34;
    in.bodyAngularVelocityYB4=main.components.root.body.angularVelocity.v[1];
    in.objectD34_10=input.objectD34_10;in.impactBA0=input.impactBA0;
    for(unsigned i=0;i<3;++i)in.bodyVelocity80.v[i]=main.components.root.localVelocity80.v[i];
    in.bodyVelocity80.v[3]=1.0f; // Dead in every represented source output.
    for(unsigned i=0;i<4;++i)in.probeVector1D0.v[i]=main.components.probe348.vector1D0[i];
    std::memcpy(w.nodes,graph.nodes,graph.count*sizeof(SceneNode49P));
    w.state=s;w.shared=shared;w.secondary=secondary;w.supplement=supplement;
    w.noise=noise;w.vehicle838=physical;
    const SceneGraph49P g{w.nodes,graph.count};
    lazy(w.shared.speed434750,0x4312aa7fu);lazy(w.shared.yaw434758,0x3dcccccdu);
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21a7ccu);
    if(w.state.vehicle83C){
        if(!response(in,p,b,g,w.state,w.shared,zero,w.secondary.time864,w.noise,d))return false;
    }else RR_CHARIOT_LIFECYCLE49V_STAGE(0x21b9a4u);
    if(w.vehicle838&&w.state.vehicle83C){
        M blended{};
        RR_CHARIOT_LIFECYCLE49V_STAGE(0x21ba08u);
        if(!detail::blendChariotMatrix49V(node(g,b.hidden198).localC0,
            node(g,b.physical19C).world100,w.state.elapsed84C,w.state.duration848,blended))
            return fail(d,ChariotLifecycleError49V::arithmetic,0x21babc);
        if(!setLocalMatrix49P(g,b.model1B4,blended))
            return fail(d,ChariotLifecycleError49V::graph,0x21be84);
        RR_CHARIOT_LIFECYCLE49V_STAGE(0x21be8cu);
        Math f;w.state.elapsed84C=f.add(w.state.elapsed84C,in.frameFactor);
        if(!f.valid)return fail(d,ChariotLifecycleError49V::arithmetic,0x21be98);
        if(!(w.state.elapsed84C<=w.state.duration848))w.state.vehicle83C=0;
        RR_CHARIOT_LIFECYCLE49V_STAGE(0x21beb4u);
    }else if(w.state.vehicle83C){
        if(!setLocalMatrix49P(g,b.model1B4,node(g,b.hidden198).localC0))
            return fail(d,ChariotLifecycleError49V::graph,0x21b9f4);
        RR_CHARIOT_LIFECYCLE49V_STAGE(0x21beb4u);
    }else{
        if(!setLocalMatrix49P(g,b.model1B4,node(g,b.physical19C).localC0))
            return fail(d,ChariotLifecycleError49V::graph,0x21b9c4);
    }
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21beb8u);
    if(in.cachedBodySpeed34<effective){
        if(!w.vehicle838){
            RR_CHARIOT_LIFECYCLE49V_STAGE(0x21bef8u);
            if(!detail::copyLocal49U(g,b.physical19C,node(g,b.model1B4).localC0))
                return fail(d,ChariotLifecycleError49V::graph,0x21bf00);
            RR_CHARIOT_LIFECYCLE49V_STAGE(0x21bf08u);
            resetSecondaryActivation49V(w.secondary,w.supplement,globals);
            RR_CHARIOT_LIFECYCLE49V_STAGE(0x21bf1cu);
            copySecondaryMainVelocities49V(main.components.root,mainLanes.velocity70W,
                mainLanes.angularVelocityB0W,w.secondary);
            w.state.elapsed84C=0.0f;w.vehicle838=1;
        }
    }else if(!w.state.vehicle83C){
        RR_CHARIOT_LIFECYCLE49V_STAGE(0x21bf58u);
        if(!detail::copyLocal49U(g,b.hidden198,node(g,b.model1B4).localC0))
            return fail(d,ChariotLifecycleError49V::graph,0x21bf60);
        detail::SceneVectorContext49U context{0.0f};M relative{};
        if(!detail::position49U(g,b.model1B4,b.container194,context,w.state.response.position)||
            !detail::relativeMatrix49U(g,b.model1B4,b.container194,context,relative)||
            !detail::matrixQuaternion49U(relative,w.state.response.orientation))
            return fail(d,ChariotLifecycleError49V::arithmetic,0x21bf70);
        w.state.response.linearVelocity=zero;w.state.response.angularVelocity=zero;
        w.vehicle838=0;w.state.vehicle83C=1;
    }
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21c0f4u);
    w.state.vehicle840=in.probe10C;
    RR_CHARIOT_LIFECYCLE49V_STAGE(0x21c100u);
    // No getter is inserted here: root.scene remains the cached-world
    // projection, including its stale bits after the reverse local setter.
    w.secondary.root.scene=node(g,b.physical19C).world100;
    w.secondary.sceneFlags190=(w.secondary.sceneFlags190&0xffffff00u)|node(g,b.physical19C).sceneFlags190;
    std::memcpy(graph.nodes,w.nodes,graph.count*sizeof(SceneNode49P));
    s=w.state;shared=w.shared;secondary=w.secondary;supplement=w.supplement;
    physical=w.vehicle838;noise=w.noise;d={};return true;
}
}
