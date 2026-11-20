#include "original_gc_particle_render50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <limits>

namespace rr::game {
namespace {
using S=OriginalGcParticleRenderStatus50KG;
using V=std::array<float,3>;
float add(float a,float b) noexcept {volatile float r=a+b;return r;}
float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
float div(float a,float b) noexcept {volatile float r=a/b;return r;}
float dot(const V& a,const V& b) noexcept {return add(add(mul(a[0],b[0]),mul(a[1],b[1])),mul(a[2],b[2]));}
V cross(const V& a,const V& b) noexcept {
    return {{sub(mul(a[1],b[2]),mul(a[2],b[1])),sub(mul(a[2],b[0]),mul(a[0],b[2])),sub(mul(a[0],b[1]),mul(a[1],b[0]))}};
}
bool normalize(V& a) noexcept {
    const float square=dot(a,a);if(!std::isfinite(square)||square<=0)return false;
    const float reciprocal=div(1,std::sqrt(square));
    for(float& v:a){v=mul(v,reciprocal);if(!std::isfinite(v))return false;}return true;
}
bool lighting(const CameraViewOutput49F& camera,
    const original_gc_gameplay_lighting_candidate::Owner& owner,
    const NativeGcSelectedLightsCandidate& selected,
    std::array<std::array<std::uint8_t,3>,4>& out) noexcept {
    std::array<float,4> ambient;
    const auto* lights=selected.component(0);
    if(selected.ownerIdentity()!=reinterpret_cast<std::uintptr_t>(&owner)||
       !owner.currentSourceGlobalAmbient50(ambient)||!lights)
        return false;
    // Selected zone ambient is NOT used: 149310 consumes the track's global+50
    // ambient and the actual last directional registration, which the native
    // selected publication carries in slot 0 (the one sun slot of every track).
    // Common rewrite (area E): the values are the track's own authored lights on
    // every course (the GC 40,40,45 / 85,85,80 value pins are retired).
    for(unsigned c=0;c<4;++c)if(!std::isfinite(ambient[c])||!std::isfinite(lights->diffuse[0][c]))return false;
    V toward{{lights->directions[0][0],lights->directions[1][0],lights->directions[2][0]}};
    V back{{-camera.sceneAfter.look.x,-camera.sceneAfter.look.y,-camera.sceneAfter.look.z}};
    V up{{camera.sceneAfter.up.x,camera.sceneAfter.up.y,camera.sceneAfter.up.z}};
    V right=cross(up,back);up=cross(back,right);
    if(!normalize(back)||!normalize(up)||!normalize(right)||!normalize(toward))return false;
    constexpr float corners[4][2]={{-1,1},{-1,-1},{1,1},{1,-1}};
    for(unsigned n=0;n<4;++n){
        V local{{corners[n][0],corners[n][1],.25f}};
        if(!normalize(local))return false;
        V world;
        for(unsigned c=0;c<3;++c)
            world[c]=add(add(mul(local[0],right[c]),mul(local[1],up[c])),mul(local[2],back[c]));
        const float contribution=std::max(0.f,dot(world,toward));
        for(unsigned c=0;c<3;++c){
            const float channel=mul(255,std::min(1.f,div(add(ambient[c],mul(contribution,lights->diffuse[0][c])),128)));
            if(!std::isfinite(channel)||channel<0||channel>255)return false;
            out[n][c]=static_cast<std::uint8_t>(channel);
        }
    }
    return true;
}
}
struct OriginalGcParticleRenderOwner50KG::Storage {
    const OriginalGcParticleManager50KG* manager=nullptr;
    std::array<assets::Vertex48B,originalGcParticleCapacity50KG*4> vertices{};
    std::array<float,originalGcParticleCapacity50KG*4> fogFactors{};
    std::array<OriginalGcParticleRun50KG,originalGcParticleCapacity50KG> runs{};
    std::array<std::int32_t,originalGcParticleDepthBuckets50KG> heads{};
    std::array<std::int32_t,originalGcParticleCapacity50KG> next{};
    std::array<float,originalGcParticleCapacity50KG> depths{};
    OriginalGcParticleDraw50KG draw;
    std::uint64_t serial=0;
    std::uint16_t frameCount=0;   // actual atlas frames (prepare)
    bool ready=false;
};
OriginalGcParticleRenderOwner50KG::OriginalGcParticleRenderOwner50KG() noexcept=default;
OriginalGcParticleRenderOwner50KG::~OriginalGcParticleRenderOwner50KG()=default;
S OriginalGcParticleRenderOwner50KG::prepare(const OriginalGcParticleManager50KG& manager) noexcept {
    if(storage_)return S::alreadyPrepared;
    // The actual atlas frame count of this track's PRT (1617A0 NumFrames), counted
    // from the prepared manager on every course (the GC 129-frame pin is retired).
    // 4096 = the manager's own native atlas storage bound (prepareTrackAtlas160E00).
    if(!manager.prepared())return S::manager;
    std::uint16_t frames=0;
    while(frames<4096u&&manager.frame(frames))++frames;
    if(!frames||frames>=4096u)return S::manager;
    try{auto next=std::make_unique<Storage>();next->manager=&manager;next->frameCount=frames;storage_=std::move(next);return S::complete;}
    catch(...){return S::allocation;}
}
S OriginalGcParticleRenderOwner50KG::build(const CameraViewOutput49F& core,
    const OriginalCameraViewportFields& viewport,const OriginalRaceFogFields& fog,
    const original_gc_gameplay_lighting_candidate::Owner& lightOwner,
    const NativeGcSelectedLightsCandidate& selected) noexcept {
    if(!storage_)return S::unprepared;
    auto& s=*storage_;s.ready=false;s.draw={};
    if(std::fegetround()!=FE_TONEAREST||s.serial==std::numeric_limits<std::uint64_t>::max())return S::arithmetic;
    const auto& manager=*s.manager;const auto count=manager.particleCount();
    if(!manager.prepared()||count>originalGcParticleCapacity50KG)return S::manager;
    if(!viewport.prepared||!viewport.matricesReady||!viewport.sessionGeneration||!viewport.sceneKey||
       viewport.fullWidth204!=640||viewport.fullHeight208!=448||viewport.x1E0||viewport.y1E4||
       viewport.width1E8!=640||viewport.height1EC!=448||viewport.zoom328!=1||viewport.zoom32C!=1||
       viewport.depth310!=1||viewport.depth314!=8388591.f||
       !std::isfinite(viewport.focalPixels1D8)||viewport.focalPixels1D8<=0)return S::camera;
    if(!std::isfinite(core.position.x)||!std::isfinite(core.position.y)||!std::isfinite(core.position.z))return S::camera;
    for(float v:core.combined)if(!std::isfinite(v))return S::camera;
    for(float v:viewport.combinedScreen250)if(!std::isfinite(v))return S::camera;
    constexpr std::array<float,16> sourceScreen{{320,0,0,0,0,-224,0,0,
        0,0,-8388590.f,0,2048,2048,8388591.f,1}};
    if(viewport.screen140!=sourceScreen)return S::camera;
    //A viewport from an older publication cannot accompany the current core.
    //Preserve236E50 binary32 multiplication order; no normalization or repair.
    for(unsigned row=0;row<4;++row)for(unsigned col=0;col<4;++col){
        float v=mul(sourceScreen[col],core.combined[row*4]);
        v=add(v,mul(sourceScreen[4+col],core.combined[row*4+1]));
        v=add(v,mul(sourceScreen[8+col],core.combined[row*4+2]));
        v=add(v,mul(sourceScreen[12+col],core.combined[row*4+3]));
        if(!std::isfinite(v)||v!=viewport.combinedScreen250[row*4+col])return S::camera;
    }
    // The track's own [Fog] colour and range (273C30) on every course.
    if(!std::isfinite(fog.start34)||!std::isfinite(fog.end38)||
       fog.end38<=fog.start34||core.sceneAfter.nearPlane!=1||core.sceneAfter.farPlane!=fog.end38)return S::fog;
    OriginalUiMainParticleCamera50KG camera;
    camera.sourceScreen140=viewport.screen140;camera.combinedSourceScreen250=viewport.combinedScreen250;
    camera.focalPixels1D8=viewport.focalPixels1D8;
    OriginalUiMainParticleRenderConstants50KG constants;
    constants.combinedClip100=core.combined;constants.sourceCanvasOffset={{1728,1824}};
    constants.fogStart33Z=fog.end38;constants.fogScale33W=div(255,sub(fog.start34,fog.end38));
    if(!lighting(core,lightOwner,selected,constants.sourceLighting))return S::lighting;
    s.heads.fill(-1);s.next.fill(-1);
    constexpr float maximumDepth=16777000.f;
    const float scale=div(float(originalGcParticleDepthBuckets50KG),maximumDepth),inverseScale=div(1,scale);
    for(std::size_t i=0;i<count;++i){
        const auto* p=manager.particleAt(i);if(!p||!manager.emitter(p->ownerId)||!manager.frame(p->frame))return S::frame;
        const auto& m=viewport.combinedScreen250;
        const float depth=add(add(add(mul(p->position[0],m[2]),mul(p->position[1],m[6])),mul(p->position[2],m[10])),m[14]);
        if(!std::isfinite(depth))return S::arithmetic;
        if(depth<=0||depth>=maximumDepth)continue;
        const auto bucket=static_cast<std::int32_t>(sub(float(originalGcParticleDepthBuckets50KG),mul(float(static_cast<std::int32_t>(depth)),scale)));
        if(bucket<0||bucket>=static_cast<std::int32_t>(originalGcParticleDepthBuckets50KG))continue;
        s.depths[i]=mul(float(static_cast<std::int32_t>(mul(depth,scale))),inverseScale);
        s.next[i]=s.heads[std::size_t(bucket)];s.heads[std::size_t(bucket)]=static_cast<std::int32_t>(i);
    }
    OriginalGcParticleDraw50KG draw;
    std::uint32_t packetOrdinal=0,runPacketCount=0;std::uint8_t runState=2;
    for(std::size_t n=s.heads.size();n>0;--n)for(auto index=s.heads[n-1];index>=0;index=s.next[std::size_t(index)]){
        const auto& p=*manager.particleAt(std::size_t(index));
        const std::uint8_t state=(p.flags&1u)?1u:0u;
        //162230 batches12 packets; a state transition splits the signed run.
        //Invisible center-clipped packets still consume their source batch slot.
        const bool newRun=(packetOrdinal%12u)==0u||state!=runState||runPacketCount==12u;
        if(newRun){runState=state;runPacketCount=0;}++packetOrdinal;++runPacketCount;
        OriginalUiMainParticlePacket50KG packet;
        packet.position=p.position;packet.sizeTimesCamera1D8=mul(p.size,camera.focalPixels1D8);
        packet.quantizedSourceDepth=s.depths[std::size_t(index)];
        packet.sourceParticleIndex=static_cast<std::uint16_t>(index);packet.sourceFrame=static_cast<std::uint16_t>(p.frame);
        packet.sourceDepthBucket=static_cast<std::uint16_t>(n-1);
        //160BB0/160C5C constructor writes manager+148=255; ordinary admitted
        //GC has no separate brightness mutator. Non-glowing source starts64.
        float brightness=(p.flags&0x2000u)?255.f:64.f;
        if(p.flags&0x80u)brightness=manager.fogBrightness(p,{{core.position.x,core.position.y,core.position.z}});
        else if(p.flags&2u)brightness=mul(brightness,sub(1,mul(p.ageMilliseconds,p.inverseLife)));
        else if(p.flags&0x1000u){
            const float initialCube=mul(p.initialSize,mul(p.initialSize,p.initialSize));
            const float sizeCube=mul(p.size,mul(p.size,p.size));
            if(sizeCube==0)return S::unsupportedBrightness;
            brightness=mul(brightness,mul(mul(p.ageMilliseconds,p.inverseLife),div(initialCube,sizeCube)));
        }
        //No arbitrary clamp or alpha wrap substitute on unsupported source data.
        if(!std::isfinite(brightness)||brightness<0||brightness>255)return S::unsupportedBrightness;
        packet.sourceBrightness=brightness;packet.normalizedUv=manager.frame(p.frame)->sourceUv;
        // PORT STAND-IN (flagged): pixel UVs are normalised by the 128x1024 extent of
        // the stock atlas texture (demoprt.tga, every stock course); the texture's own
        // extent is not carried to this owner yet. A frame outside it is refused.
        for(auto& uv:packet.normalizedUv){
            if(!(uv[0]>=0&&uv[0]<=128&&uv[1]>=0&&uv[1]<=1024))return S::frame;
            uv[0]=div(uv[0],128);uv[1]=div(uv[1],1024);}
        if(p.flags&0x4000u){const auto old=packet.normalizedUv;
            packet.normalizedUv[0][0]=old[1][0];packet.normalizedUv[1][0]=old[0][0];
            packet.normalizedUv[2][0]=old[3][0];packet.normalizedUv[3][0]=old[2][0];
            if(p.flags&0x8000u){packet.normalizedUv[0][1]=old[2][1];packet.normalizedUv[1][1]=old[3][1];
                packet.normalizedUv[2][1]=old[0][1];packet.normalizedUv[3][1]=old[1][1];}}
        OriginalUiMainParticleQuad50KG quad;
        if(projectOriginalParticleQuad50KG(packet,state,camera,constants,s.frameCount,255,quad)!=OriginalUiMainParticleGeometryStatus50KG::complete)
            return S::arithmetic;
        if(!quad.visible){runState=2;continue;}
        //A source-alpha factor above1 needs a different framebuffer contract.
        //Authentic fog/spray/dust fit this actual full-atlas255 bound; additive
        //glow is handled by the native GPU premultiplied additive branch.
        if(!state&&std::uint32_t(quad.vertices[0].color[3])*255u>128u*128u)return S::unsupportedBrightness;
        if(newRun||!draw.runCount||s.runs[draw.runCount-1].stateIndex!=state||
           s.runs[draw.runCount-1].quadCount>=12){
            s.runs[draw.runCount++]={draw.quadCount,0,state};
        }
        ++s.runs[draw.runCount-1].quadCount;
        for(unsigned c=0;c<4;++c){s.vertices[draw.quadCount*4+c]=quad.vertices[c];s.fogFactors[draw.quadCount*4+c]=quad.fogFactors[c];}
        ++draw.quadCount;
    }
    draw.vertices=s.vertices.data();draw.fogFactors=s.fogFactors.data();draw.runs=s.runs.data();
    draw.sourceParticleCount=static_cast<std::uint32_t>(count);draw.sourceCombined=core.combined;
    draw.fogColor={{div(float((fog.color30>>16)&0xffu),255),div(float((fog.color30>>8)&0xffu),255),div(float(fog.color30&0xffu),255)}};
    draw.cameraSceneKey=viewport.sceneKey;draw.cameraSessionGeneration=viewport.sessionGeneration;draw.generation=++s.serial;
    s.draw=draw;s.ready=true;return S::complete;
}
const OriginalGcParticleDraw50KG* OriginalGcParticleRenderOwner50KG::draw() const noexcept {
    return storage_&&storage_->ready?&storage_->draw:nullptr;
}
}
