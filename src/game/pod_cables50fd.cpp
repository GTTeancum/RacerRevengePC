#include "pod_cables50fd.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using I = SceneNodeIndex49P;
using V = Vector3_48C;
float fromBits(std::uint32_t b) noexcept { float x; std::memcpy(&x, &b, 4); return x; }
std::uint32_t bits(float x) noexcept { std::uint32_t b; std::memcpy(&b, &x, 4); return b; }
struct Math {
    bool valid = true;
    float keep(float x) noexcept { valid = valid && std::isfinite(x); return x; }
    float add(float a, float b) noexcept { keep(a); keep(b); volatile float x = a + b; return keep(x); }
    float sub(float a, float b) noexcept { keep(a); keep(b); volatile float x = a - b; return keep(x); }
    float mul(float a, float b) noexcept { keep(a); keep(b); volatile float x = a * b; return keep(x); }
    float div(float a, float b) noexcept { keep(a); keep(b); volatile float x = a / b; return keep(x); }
    V normalize(const V& v) noexcept {
        const float n = add(add(mul(v.v[0],v.v[0]),mul(v.v[1],v.v[1])),mul(1.f,mul(v.v[2],v.v[2])));
        if (n != 0.f && (bits(n) & 0x7f800000u) == 0) valid = false;
        float q = fromBits(0x7f7fffffu);
        if (n > 0.f) { volatile float root = std::sqrt(n); q = div(1.f, keep(root)); }
        V out{}; for(unsigned c=0;c<3;++c) out.v[c]=mul(v.v[c],q); return out;
    }
    V cross(const V& a, const V& b) noexcept {
        return {{sub(mul(a.v[1],b.v[2]),mul(a.v[2],b.v[1])),
                 sub(mul(a.v[2],b.v[0]),mul(a.v[0],b.v[2])),
                 sub(mul(a.v[0],b.v[1]),mul(a.v[1],b.v[0]))}};
    }
};
bool reject(PodCableDiagnostic50FD& d, PodCableError50FD e) noexcept { d.error=e;return false; }
bool valid(I i) noexcept { return static_cast<std::int32_t>(i)>=0 && static_cast<std::uint32_t>(i)<vehicleFrameNodeCount49W; }
bool descendant(const SceneGraph49P& g,I child,I root) noexcept {
    if(!valid(child)||!valid(root)||child==root)return false;
    I n=g.nodes[static_cast<std::uint32_t>(child)].parent180;
    for(unsigned k=0;k<vehicleFrameNodeCount49W&&n!=I::none;++k) {
        if(n==root)return true;
        n=g.nodes[static_cast<std::uint32_t>(n)].parent180;
    }
    return false;
}
}
bool buildPodCables50FD(const VehicleFrameState49W& state,
    const VehicleFrameBindings49W& bindings,const PodCableBindings50FD& cable,
    const V& cameraForward,PodCableMesh50FD& output,
    PodCableWorkspace50FD& w,PodCableDiagnostic50FD& d) noexcept {
    d={};
    if(state.phase!=VehicleFramePhase49W::Ready && state.phase!=VehicleFramePhase49W::PostGraphics)
        return reject(d,PodCableError50FD::phase);
    if(state.bodies.main.vehicleState40!=0)return reject(d,PodCableError50FD::unsupportedState);
    if(std::fegetround()!=FE_TONEAREST)return reject(d,PodCableError50FD::roundingMode);
    for(float v:cameraForward.v)if(!std::isfinite(v))return reject(d,PodCableError50FD::input);
    if(cameraForward.v[0]==0.f&&cameraForward.v[1]==0.f&&cameraForward.v[2]==0.f)
        return reject(d,PodCableError50FD::input);
    std::memcpy(w.nodes,state.nodes,sizeof w.nodes);
    const SceneGraph49P graph{w.nodes,vehicleFrameNodeCount49W};
    if(!validateSceneGraph49P(graph))return reject(d,PodCableError50FD::graph);
    const auto& b=bindings.lifecycle;
    const I roots[2]={b.left1BC,b.right1B8};
    if(!valid(b.model1B4)||cable.chariot[0]==cable.chariot[1]||cable.engine[0]==cable.engine[1])
        return reject(d,PodCableError50FD::binding);
    for(unsigned side=0;side<2;++side) {
        d.cable=side;
        if(!descendant(graph,cable.chariot[side],b.model1B4)||!descendant(graph,cable.engine[side],roots[side]))
            return reject(d,PodCableError50FD::binding);
        if(!refreshSceneWorld49P(graph,cable.chariot[side])||!refreshSceneWorld49P(graph,cable.engine[side]))
            return reject(d,PodCableError50FD::graph);
    }
    if(!refreshSceneWorld49P(graph,b.model1B4))return reject(d,PodCableError50FD::graph);
    Math f;
    // 2249BC..224A98 interpolates world Y scaling from1 to0 as cached34
    // runs0..float(0x4392AA7F), then clamps to those endpoints.
    const float speed=f.keep(state.bodies.main.cachedSpeed34);
    float yScale=f.add(1.f,f.div(f.mul(f.sub(speed,0.f),-1.f),fromBits(0x4392aa7fu)));
    if(yScale<0.f)yScale=0.f;else if(yScale>1.f)yScale=1.f;
    const auto& model=w.nodes[static_cast<std::uint32_t>(b.model1B4)].world100;
    V chariotTangent{},engineTangent{};
    for(unsigned c=0;c<3;++c) {
        chariotTangent.v[c]=f.keep(-model.v[8+c]);
        engineTangent.v[c]=f.add(model.v[4+c],chariotTangent.v[c]);
    }
    chariotTangent.v[1]=f.mul(chariotTangent.v[1],yScale);
    engineTangent.v[1]=f.mul(engineTangent.v[1],yScale);
    chariotTangent=f.normalize(chariotTangent);engineTangent=f.normalize(engineTangent);
    for(unsigned c=0;c<3;++c) {
        chariotTangent.v[c]=f.mul(6.5f,chariotTangent.v[c]);
        engineTangent.v[c]=f.mul(6.5f,engineTangent.v[c]);
    }
    if(!f.valid)return reject(d,PodCableError50FD::arithmetic);
    for(unsigned side=0;side<2;++side) {
        d.cable=side;
        const auto& chariot=w.nodes[static_cast<std::uint32_t>(cable.chariot[side])].world100;
        const auto& engine=w.nodes[static_cast<std::uint32_t>(cable.engine[side])].world100;
        for(unsigned sample=0;sample<podCableSampleCount50FD;++sample) {
            d.sample=sample;
            const float t=f.div(static_cast<float>(sample),8.f),t2=f.mul(t,t);
            const float h00=f.add(1.f,f.mul(f.sub(f.mul(2.f,t),3.f),t2));
            const float h01=f.sub(1.f,h00);
            const float h10=f.mul(t,f.add(1.f,f.mul(t,f.sub(t,2.f))));
            const float h11=f.mul(t2,f.sub(t,1.f));
            for(unsigned c=0;c<3;++c) {
                const float p0=f.mul(h00,engine.v[12+c]),p1=f.mul(h01,chariot.v[12+c]);
                const float m0=f.mul(h10,engineTangent.v[c]),m1=f.mul(h11,chariotTangent.v[c]);
                w.samples[side][sample].v[c]=f.add(f.add(f.add(p0,p1),m0),m1);
            }
        }
        V segment{};
        for(unsigned sample=0;sample<podCableSampleCount50FD;++sample) {
            d.sample=sample;
            if(sample+1<podCableSampleCount50FD)
                for(unsigned c=0;c<3;++c)segment.v[c]=f.sub(w.samples[side][sample].v[c],w.samples[side][sample+1].v[c]);
            const V width=f.normalize(f.cross(cameraForward,segment));
            const unsigned start=side*18+sample*2;
            for(unsigned lane=0;lane<2;++lane) {
                auto& v=w.next.vertices[start+lane];
                for(unsigned c=0;c<3;++c) {
                    const float p=w.samples[side][sample].v[c];
                    v.position[c]=lane?f.add(p,f.mul(.25f,width.v[c])):p;
                }
                v.uv[0]=static_cast<float>(lane);v.uv[1]=f.mul(10.f,f.div(static_cast<float>(sample),8.f));
                for(auto& color:v.color)color=255;
            }
            if(sample+1<podCableSampleCount50FD) {
                const unsigned at=side*48+sample*6;
                const std::uint32_t indices[6]={start,start+1,start+2,start+2,start+1,start+3};
                std::memcpy(w.next.indices+at,indices,sizeof indices);
            }
        }
    }
    if(!f.valid)return reject(d,PodCableError50FD::arithmetic);
    for(unsigned c=0;c<3;++c)w.next.bounds.minimum[c]=w.next.bounds.maximum[c]=w.next.vertices[0].position[c];
    for(const auto& v:w.next.vertices)for(unsigned c=0;c<3;++c) {
        if(v.position[c]<w.next.bounds.minimum[c])w.next.bounds.minimum[c]=v.position[c];
        if(v.position[c]>w.next.bounds.maximum[c])w.next.bounds.maximum[c]=v.position[c];
    }
    output=w.next;d={};return true;
}
}
