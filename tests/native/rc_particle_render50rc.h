#pragma once
#include "rc_particle_runtime50rc.h"
#include "rc_particle_material_borrow50rc.h"
#include "../../src/game/original_gc_particle_render50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <limits>
#include <memory>
#include <thread>

namespace rr::platform {class VehicleWindowGl50DS;}
namespace rc_particle_render50RC {
using namespace rr::game;
enum class Status : std::uint8_t {complete,unprepared,alreadyPrepared,manager,camera,fog,lighting,arithmetic,
 frame,unsupportedBrightness,storage,allocation,material,submission};
struct Diagnostic {Status status=Status::unprepared;std::uint32_t sourcePC=0,particles=0,quads=0,runs=0;};
namespace detail {
using S=Status;
using V=std::array<float,3>;
inline float add(float a,float b) noexcept {volatile float r=a+b;return r;}
inline float sub(float a,float b) noexcept {volatile float r=a-b;return r;}
inline float mul(float a,float b) noexcept {volatile float r=a*b;return r;}
inline float div(float a,float b) noexcept {volatile float r=a/b;return r;}
inline float dot(const V& a,const V& b) noexcept {return add(add(mul(a[0],b[0]),mul(a[1],b[1])),mul(a[2],b[2]));}
inline V cross(const V& a,const V& b) noexcept {
    return {{sub(mul(a[1],b[2]),mul(a[2],b[1])),sub(mul(a[2],b[0]),mul(a[0],b[2])),sub(mul(a[0],b[1]),mul(a[1],b[0]))}};
}
inline bool normalize(V& a) noexcept {
    const float square=dot(a,a);if(!std::isfinite(square)||square<=0)return false;
    const float reciprocal=div(1,std::sqrt(square));
    for(float& v:a){v=mul(v,reciprocal);if(!std::isfinite(v))return false;}return true;
}
inline bool lighting(const CameraViewOutput49F& camera,
    const original_gc_gameplay_lighting_candidate::Owner& owner,
    const NativeGcSelectedLightsCandidate& selected,
    std::array<std::array<std::uint8_t,3>,4>& out) noexcept {
    std::array<float,4> ambient;
    const auto* lights=selected.component(0);
    if(selected.ownerIdentity()!=reinterpret_cast<std::uintptr_t>(&owner)||
       !owner.currentSourceGlobalAmbient50(ambient)||!lights||ambient!=std::array<float,4>{{7.5f,5,10,64}})
        return false;
    //1493EC..1493F4 consumes the registered directional RGB+3C, not zone44.
    //For this authenticated RC source,146CC4..146CF4 initially writes the
    //same RGB to+3C and+4C. prepareMaSa retains that row in authoredSunSpecular;
    //selectAuthoredZones copies it to native specular[0] without zone overrides
    //or portal interpolation. That native field transports the required RGB.
    //diffuse[0] instead contains the actual racer-specific zone override/blend.
    const auto& registeredDirectional=lights->specular[0];
    if(registeredDirectional!=std::array<float,4>{{79,79,98.5f,0}})return false;
    for(unsigned i=1;i<3;++i)for(unsigned c=0;c<3;++c)
        if(lights->diffuse[i][c]!=0)return false;
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
            const float channel=mul(255,std::min(1.f,div(add(ambient[c],mul(contribution,registeredDirectional[c])),128)));
            if(!std::isfinite(channel)||channel<0||channel>255)return false;
            out[n][c]=static_cast<std::uint8_t>(channel);
        }
    }
    return true;
}
} // namespace detail
class Owner final {
 friend class rr::platform::VehicleWindowGl50DS;
 struct Storage {
  std::array<rr::assets::Vertex48B,originalGcParticleCapacity50KG*4> vertices{};
  std::array<float,originalGcParticleCapacity50KG*4> fogFactors{};
  std::array<OriginalGcParticleRun50KG,originalGcParticleCapacity50KG> runs{};
  std::array<std::int32_t,originalGcParticleDepthBuckets50KG> heads{};
  std::array<std::int32_t,originalGcParticleCapacity50KG> next{};
  std::array<float,originalGcParticleCapacity50KG> depths{};
  OriginalGcParticleDraw50KG draw{};std::uint64_t serial=0;bool ready=false;
 };
 std::shared_ptr<rc_particle_runtime50RC::Owner> runtime_;
 std::shared_ptr<original_gc_gameplay_lighting_candidate::Owner> lighting_;
 std::unique_ptr<Storage> storage_;
 const rc_particle_visual50RC::MaterialBorrow* materialBorrow_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,borrowing_=false,closed_=false,terminal_=false;
 bool current()const noexcept{return thread_==std::this_thread::get_id()&&!closed_&&storage_&&runtime_&&runtime_->current()&&lighting_&&lighting_->world();}
 // Only the actual renderer may inspect this synchronous owner-backed packet.
 // The shared frame Owner pins storage; withDraw pins the exact Window borrow.
 const OriginalGcParticleDraw50KG* rendererDraw()const noexcept {
  return current()&&borrowing_&&runtime_->rendering_&&storage_->ready&&materialBorrow_?&storage_->draw:nullptr;
 }
 const rc_particle_visual50RC::MaterialBorrow* rendererMaterial()const noexcept {
  return rendererDraw()?materialBorrow_:nullptr;
 }
 Status build(const rc_particle_visual50RC::MaterialBorrow& material,const CameraViewOutput49F& core,
  const OriginalCameraViewportFields& viewport,const OriginalRaceFogFields& fog,
  const NativeGcSelectedLightsCandidate& selected,Diagnostic& diagnostic)noexcept {
  using namespace detail;using S=Status;auto& s=*storage_;s.ready=false;s.draw={};
  const auto* reference=runtime_->cold_->material150();
  if(!reference||!reference->retained()||material.coldSource()!=runtime_->source_->sourceOwner()||
     material.trackParticles()!=runtime_->cold_.get()||material.table382B38()!=reference->table382B38()||
     material.materialRow()!=reference->material00()||!material.texture()||!material.width()||!material.height()||
     double(float(material.width()))!=double(material.width())||double(float(material.height()))!=double(material.height()))return S::material;
    if(std::fegetround()!=FE_TONEAREST||s.serial==std::numeric_limits<std::uint64_t>::max())return S::arithmetic;
    const auto& manager=*runtime_->manager_;const auto count=manager.particleCount();
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
    if(fog.color30!=0xff000000u||!std::isfinite(fog.start34)||!std::isfinite(fog.end38)||
       fog.end38<=fog.start34||core.sceneAfter.nearPlane!=1||core.sceneAfter.farPlane!=fog.end38)return S::fog;
    OriginalUiMainParticleCamera50KG camera;
    camera.sourceScreen140=viewport.screen140;camera.combinedSourceScreen250=viewport.combinedScreen250;
    camera.focalPixels1D8=viewport.focalPixels1D8;
    OriginalUiMainParticleRenderConstants50KG constants;
    constants.combinedClip100=core.combined;constants.sourceCanvasOffset={{1728,1824}};
    constants.fogStart33Z=fog.end38;constants.fogScale33W=div(255,sub(fog.start34,fog.end38));
    if(!lighting(core,*lighting_,selected,constants.sourceLighting))return S::lighting;
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
        //1624E8 selects ordinary64 or this actual immutable RC GlowScalar128
        // before the persistent birth/proximity branch. No GC override.
        float brightness=(p.flags&0x2000u)?runtime_->source_->atlas().glowScalar148:64.f;
        if(p.flags&0x80u){
            bool actualFog=false;
            for(const auto& child:runtime_->children_)if(child.managerOwnerId==p.ownerId&&child.kind==rc_particle_authoring50RC::Kind::Fog12)actualFog=true;
            if(!actualFog)return S::frame;
            if(!rr::game::original_rc_fog_numeric_detail50RC::brightness(runtime_->source_->definition(rc_particle_authoring50RC::Kind::Fog12),
                p,{{core.position.x,core.position.y,core.position.z}},brightness,brightness,diagnostic.sourcePC))return S::arithmetic;
        }
        else if(p.flags&2u)brightness=mul(brightness,sub(1,mul(p.ageMilliseconds,p.inverseLife)));
        else if(p.flags&0x1000u){
            const float initialCube=mul(p.initialSize,mul(p.initialSize,p.initialSize));
            const float sizeCube=mul(p.size,mul(p.size,p.size));
            if(sizeCube==0)return S::unsupportedBrightness;
            brightness=mul(brightness,mul(mul(p.ageMilliseconds,p.inverseLife),div(initialCube,sizeCube)));
        }
        //No arbitrary clamp or alpha wrap substitute on unsupported source data.
        if(!std::isfinite(brightness)||brightness<0||brightness>128)return S::unsupportedBrightness;
        packet.sourceBrightness=brightness;packet.normalizedUv=manager.frame(p.frame)->sourceUv;
        for(auto& uv:packet.normalizedUv){uv[0]=div(uv[0],float(material.width()));uv[1]=div(uv[1],float(material.height()));}
        if(p.flags&0x4000u){const auto old=packet.normalizedUv;
            packet.normalizedUv[0][0]=old[1][0];packet.normalizedUv[1][0]=old[0][0];
            packet.normalizedUv[2][0]=old[3][0];packet.normalizedUv[3][0]=old[2][0];
            if(p.flags&0x8000u){packet.normalizedUv[0][1]=old[2][1];packet.normalizedUv[1][1]=old[3][1];
                packet.normalizedUv[2][1]=old[0][1];packet.normalizedUv[3][1]=old[1][1];}}
        OriginalUiMainParticleQuad50KG quad;
        if(projectOriginalParticleQuad50KG(packet,state,camera,constants,105,128,quad)!=OriginalUiMainParticleGeometryStatus50KG::complete)
            return S::arithmetic;
        if(!quad.visible){runState=2;continue;}
        // Preserve the admitted native source-alpha framebuffer contract.
        // Additive glow is submitted through the actual GPU additive branch.
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
    draw.fogColor={{0,0,0}};
    draw.cameraSceneKey=viewport.sceneKey;draw.cameraSessionGeneration=viewport.sessionGeneration;draw.generation=++s.serial;
    s.draw=draw;s.ready=true;return S::complete;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id()||borrowing_)std::terminate();}
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(Owner)+sizeof(Storage);}
 bool prepare(std::shared_ptr<rc_particle_runtime50RC::Owner> actualRuntime,
  std::shared_ptr<original_gc_gameplay_lighting_candidate::Owner> actualLighting,
  std::size_t availableBytes,Diagnostic& d)noexcept {
  d={};if(thread_!=std::this_thread::get_id()||attempted_||closed_){d.status=Status::alreadyPrepared;return false;}attempted_=true;
  if(!actualRuntime||!actualRuntime->current()||!actualLighting||!actualLighting->world()||
     actualLighting->world()->parameters.original_bytes!=actualRuntime->source_->sourceOwner()->world().parameters.original_bytes||
     actualRuntime->source_->atlas().glowScalar148!=128.f||availableBytes<retainedPayloadBytes()){
   d.status=Status::manager;return false;}
  try{auto next=std::make_unique<Storage>();runtime_=std::move(actualRuntime);lighting_=std::move(actualLighting);storage_=std::move(next);
   d.status=Status::complete;return true;}catch(...){d.status=Status::allocation;return false;}
 }
 // The actual Window's private material borrow remains live throughout this
 // synchronous numerical build and actual submit callback. No draw pointer is
 // published outside that borrow; shared-pool advance is excluded until return.
 template<class Submit>
 bool withDraw(const rc_particle_visual50RC::MaterialBorrow& material,const CameraViewOutput49F& core,
  const OriginalCameraViewportFields& viewport,const OriginalRaceFogFields& fog,
  const NativeGcSelectedLightsCandidate& selected,Diagnostic& d,Submit&& submit)noexcept {
  d={};if(!current()||terminal_||!runtime_->active()||runtime_->visiting_||runtime_->rendering_||borrowing_||materialBorrow_){d.status=Status::unprepared;return false;}
  borrowing_=true;runtime_->rendering_=true;materialBorrow_=&material;
  struct End {bool& pool;bool& borrow;bool& ready;const rc_particle_visual50RC::MaterialBorrow*& material;
   ~End(){material=nullptr;pool=false;borrow=false;ready=false;}} end{runtime_->rendering_,borrowing_,storage_->ready,materialBorrow_};
  d.status=build(material,core,viewport,fog,selected,d);if(d.status!=Status::complete){terminal_=true;return false;}
  d.particles=storage_->draw.sourceParticleCount;d.quads=storage_->draw.quadCount;d.runs=storage_->draw.runCount;
  try{if(!submit(storage_->draw)){d.status=Status::submission;terminal_=true;return false;}}catch(...){d.status=Status::submission;terminal_=true;return false;}
  return true;
 }
 bool close()noexcept {if(thread_!=std::this_thread::get_id()||borrowing_)return false;storage_.reset();runtime_.reset();lighting_.reset();closed_=true;return true;}
};
} // namespace rc_particle_render50RC
