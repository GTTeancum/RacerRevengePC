#pragma once
#include "gb_particle_runtime50gb.h"
#include "gb_particle_visual50gb.h"
#include "gb_track_fog50gb.h"
#include "original_camera_session50kx.h"
#include "../../src/game/original_gc_particle_render50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <limits>
#include <memory>
#include <thread>

namespace rr::platform {class VehicleWindowGl50DS;}
namespace gb_particle_render50GB {
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
// Inherited native finite instruction-step policy. No universal PS2 Q,
// subnormal or exception parity is claimed. Every arithmetic operation rounds
// separately as binary32 under FE_TONEAREST, without fused contraction.
inline bool finite(const V& v)noexcept {for(float x:v)if(!std::isfinite(x))return false;return true;}
inline bool normalize(V& a) noexcept {
    const float square=dot(a,a);if(!std::isfinite(square))return false;
    float reciprocal=std::numeric_limits<float>::max();
    if(square!=0){volatile float root=std::sqrt(std::fabs(square));reciprocal=div(1,root);}
    if(!std::isfinite(reciprocal))return false;
    for(float& v:a){v=mul(v,reciprocal);if(!std::isfinite(v))return false;}return true;
}
inline bool lighting(const CameraViewOutput49F& camera,
    const original_gc_gameplay_lighting_candidate::GbParticleLightInputs& light,
    std::array<std::array<std::uint8_t,3>,4>& out) noexcept {
    V look{{camera.sceneAfter.look.x,camera.sceneAfter.look.y,camera.sceneAfter.look.z}};
    V up{{camera.sceneAfter.up.x,camera.sceneAfter.up.y,camera.sceneAfter.up.z}};
    V origin{{camera.position.x,camera.position.y,camera.position.z}};
    V position{{light.positionA0[0],light.positionA0[1],light.positionA0[2]}};
    if(!finite(look)||!finite(up)||!finite(origin)||!finite(position))return false;
    for(unsigned c=0;c<3;++c){
        if(!std::isfinite(light.globalAmbient50[c])||!std::isfinite(light.registeredDirectional3C[c]))return false;
        origin[c]=add(origin[c],mul(100.f,look[c]));
    }
    //162B2C..162CB4: origin=position+100*look; rawRight=cross(up,-look),
    // rawUp=cross(-look,rawRight), normalize up/back, then recompute right.
    V back{{-look[0],-look[1],-look[2]}};V right=cross(up,back);up=cross(back,right);
    if(!finite(origin)||!finite(right)||!finite(up)||!normalize(up)||!normalize(back))return false;
    right=cross(up,back);if(!finite(right))return false; // No normalization here.
    V axes[3]{right,up,back};
    for(auto& axis:axes)for(float& v:axis)v=add(v,0.f);
    V direction;
    //149350..149448: inverse rigid basis, then Light+A0 transform plus its
    // translation. Never simplify to dot(axis,Light-origin): rounding differs.
    for(unsigned c=0;c<3;++c){
        const float translation=mul(dot(axes[c],origin),-1.f);
        direction[c]=add(dot(axes[c],position),translation);
        if(!std::isfinite(translation)||!std::isfinite(direction[c]))return false;
    }
    if(!normalize(direction))return false;
    constexpr float corners[4][2]={{-1,1},{-1,-1},{1,1},{1,-1}};
    for(unsigned n=0;n<4;++n){
        V local{{corners[n][0],corners[n][1],.25f}};if(!normalize(local))return false;
        const float contribution=-dot(local,direction);if(!std::isfinite(contribution))return false;
        for(unsigned c=0;c<3;++c){
            float scaled;
            //149588 ambient-only branch intentionally has no min(1) clamp.
            if(contribution<=0)scaled=div(light.globalAmbient50[c],128.f);
            else {const float product=mul(contribution,light.registeredDirectional3C[c]);
                const float sum=add(light.globalAmbient50[c],product);
                if(!std::isfinite(product)||!std::isfinite(sum))return false;
                scaled=std::min(div(sum,128.f),1.f);}
            const float channel=mul(255.f,scaled);
            if(!std::isfinite(scaled)||!std::isfinite(channel)||channel<0||channel>255)return false;
            //149530..14956C /149588..1495CC: EE CVT.W.S truncates.
            out[n][c]=static_cast<std::uint8_t>(static_cast<std::int32_t>(channel));
        }
    }
    return true;
}
} // namespace detail
class Owner final {
 friend class rr::platform::VehicleWindowGl50DS;
 friend class ::gb_trigger_actions50GB::Owner;
 struct Storage {
  std::array<rr::assets::Vertex48B,originalGcParticleCapacity50KG*4> vertices{};
  std::array<float,originalGcParticleCapacity50KG*4> fogFactors{};
  std::array<OriginalGcParticleRun50KG,originalGcParticleCapacity50KG> runs{};
  std::array<std::int32_t,originalGcParticleDepthBuckets50KG> heads{};
  std::array<std::int32_t,originalGcParticleCapacity50KG> next{};
  std::array<float,originalGcParticleCapacity50KG> depths{};
  OriginalGcParticleDraw50KG draw{};std::array<float,3> cameraPosition{};std::uint64_t serial=0;bool ready=false;
 };
 std::shared_ptr<gb_particle_runtime50GB::Owner> runtime_;
 std::shared_ptr<original_gc_gameplay_lighting_candidate::Owner> lighting_;
 std::shared_ptr<gb_particle_visual50GB::Window> window_;
 std::shared_ptr<gb_track_fog50GB::Owner> fog_;
 std::unique_ptr<Storage> storage_;
 const gb_trigger_actions50GB::Owner* queryOwner_=nullptr;
 bool worldService_=false;
 const gb_particle_visual50GB::MaterialBorrow* materialBorrow_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,borrowing_=false,closed_=false,terminal_=false;
 bool lightingCurrent()const noexcept {
  std::array<float,4> ambient;return lighting_&&lighting_->world()&&lighting_->currentSourceGlobalAmbient50(ambient);
 }
 bool current()const noexcept{return thread_==std::this_thread::get_id()&&!closed_&&storage_&&runtime_&&
  runtime_->sourceStaged()&&runtime_->renderBinding_==this&&lightingCurrent()&&window_&&
  window_->renderOwner_==this&&window_->generation_==runtime_->generation_&&window_->ready()&&fog_&&fog_->ready()&&
  window_->attachments_.get()==runtime_->graphs_.get()&&window_->material_->particles.get()==runtime_->cold_.get()&&
  fog_->sourceOwner()==runtime_->source_->sourceOwner();}
 bool beginActionQuery(const gb_trigger_actions50GB::Owner& owner)noexcept {
  if(!ready()||!idle()||runtime_->actions_!=&owner)return false;queryOwner_=&owner;worldService_=false;return true;
 }
 void endActionQuery(const gb_trigger_actions50GB::Owner& owner)noexcept {
  if(thread_!=std::this_thread::get_id()||queryOwner_!=&owner||worldService_)std::terminate();queryOwner_=nullptr;
 }
 bool beginWorldService(const gb_trigger_actions50GB::Owner& owner)noexcept {
  if(!beginActionQuery(owner))return false;worldService_=true;return true;
 }
 void endWorldService(const gb_trigger_actions50GB::Owner& owner)noexcept {
  if(thread_!=std::this_thread::get_id()||queryOwner_!=&owner||!worldService_)std::terminate();queryOwner_=nullptr;worldService_=false;
 }
 bool cameraCurrent(const original_camera_session50KX::Session& session,
  original_camera_session50KX::Session::FogPreparation& observation)const noexcept {
  return current()&&(&session.actual_.visibility==lighting_.get())&&
   session.actual_.actualTrackParticles4A0==runtime_->cold_.get()&&
   session.readFogPreparation(observation)&&observation.owner==&fog_->fields()&&
   observation.generation==session.viewport().sessionGeneration&&
   session.viewport().sceneKey&&session.viewport().sceneKey==session.spatial_.sceneKey;
 }
 // Only the actual renderer may inspect this synchronous owner-backed packet.
 // The shared frame Owner pins storage; withDraw pins the exact Window borrow.
 const OriginalGcParticleDraw50KG* rendererDraw()const noexcept {
  return current()&&borrowing_&&runtime_->rendering_&&storage_->ready&&materialBorrow_?&storage_->draw:nullptr;
 }
 const gb_particle_visual50GB::MaterialBorrow* rendererMaterial()const noexcept {
  return rendererDraw()?materialBorrow_:nullptr;
 }
 Status build(const gb_particle_visual50GB::MaterialBorrow& material,const CameraViewOutput49F& core,
  const OriginalCameraViewportFields& viewport,const OriginalRaceFogFields& fog,
  const original_gc_gameplay_lighting_candidate::GbParticleLightInputs& light,Diagnostic& diagnostic)noexcept {
  using namespace detail;using S=Status;auto& s=*storage_;s.ready=false;s.draw={};
  const auto* reference=runtime_->cold_->material150();
  if(!reference||!reference->retained()||material.coldSource()!=runtime_->source_->sourceOwner()||
     material.trackParticles()!=runtime_->cold_.get()||material.table382B38()!=reference->table382B38()||
     material.materialRow()!=reference->material00()||!material.texture()||!material.width()||!material.height()||
     double(float(material.width()))!=double(material.width())||double(float(material.height()))!=double(material.height()))return S::material;
    if(std::fegetround()!=FE_TONEAREST||s.serial==std::numeric_limits<std::uint64_t>::max())return S::arithmetic;
    const auto& manager=*runtime_->manager_;const auto count=manager.particleCount();
    if(!manager.prepared()||count>originalGcParticleCapacity50KG||
       manager.emitterRegistryReservation().registeredEmitters!=gb_particle_authoring50GB::attachmentCount)return S::manager;
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
    if(fog.color30!=0xff1f1c37u||!std::isfinite(fog.start34)||!std::isfinite(fog.end38)||
       fog.end38<=fog.start34||core.sceneAfter.nearPlane!=1||core.sceneAfter.farPlane!=std::min(fog.end38,100000.f))return S::fog;
    OriginalUiMainParticleCamera50KG camera;
    camera.sourceScreen140=viewport.screen140;camera.combinedSourceScreen250=viewport.combinedScreen250;
    camera.focalPixels1D8=viewport.focalPixels1D8;
    OriginalUiMainParticleRenderConstants50KG constants;
    constants.combinedClip100=core.combined;constants.sourceCanvasOffset={{1728,1824}};
    constants.fogStart33Z=fog.end38;constants.fogScale33W=div(255,sub(fog.start34,fog.end38));
    if(!lighting(core,light,constants.sourceLighting))return S::lighting;
    s.heads.fill(-1);s.next.fill(-1);
    constexpr float maximumDepth=16777000.f;
    const float scale=div(float(originalGcParticleDepthBuckets50KG),maximumDepth),inverseScale=div(1,scale);
    for(std::size_t i=0;i<count;++i){
        const auto* p=manager.particleAt(i);
        if(!p||!p->ownerId||p->ownerId>gb_particle_authoring50GB::attachmentCount||!manager.frame(p->frame))return S::frame;
        const auto& association=runtime_->source_->attachment(p->ownerId-1u);
        const auto& descriptor=runtime_->source_->descriptor(association.descriptor);
        const auto& definition=runtime_->source_->definition(descriptor.definitionIndex);
        const auto* emitter=manager.emitter(p->ownerId);const auto* group=manager.group(definition.particleGroup1B8);
        if(!emitter||emitter->particleGroup!=definition.particleGroup1B8||!group||p->frame<group->first||p->frame>group->last||
           (p->flags&~0xc000u)!=definition.particleFlags198)return S::frame;
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
        //1624E8 selects ordinary64 or this actual immutable GB GlowScalar128
        // before the persistent birth/proximity branch. No GC override.
        float brightness=(p.flags&0x2000u)?runtime_->source_->atlas().glowScalar148:64.f;
        // This source contains no Fog12/proximity emitter. Unknown shared-pool
        // producers must receive their own concrete owner join, never a GC fog substitute.
        if(p.flags&0x80u)return S::frame;
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
        if(projectOriginalParticleQuad50KG(packet,state,camera,constants,130,128,quad)!=OriginalUiMainParticleGeometryStatus50KG::complete)
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
    draw.fogColor={{31.f/255.f,28.f/255.f,55.f/255.f}};
    draw.cameraSceneKey=viewport.sceneKey;draw.cameraSessionGeneration=viewport.sessionGeneration;draw.generation=++s.serial;
    s.cameraPosition={{core.position.x,core.position.y,core.position.z}};s.draw=draw;s.ready=true;return S::complete;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id()||borrowing_||queryOwner_||
   (runtime_&&runtime_->renderBinding_==this)||(window_&&window_->renderOwner_==this))std::terminate();}
 static constexpr std::size_t retainedPayloadBytes()noexcept{return sizeof(Owner)+sizeof(Storage);}
 bool prepare(std::shared_ptr<gb_particle_runtime50GB::Owner> actualRuntime,
  std::shared_ptr<original_gc_gameplay_lighting_candidate::Owner> actualLighting,
  std::shared_ptr<gb_particle_visual50GB::Window> actualWindow,
  std::shared_ptr<gb_track_fog50GB::Owner> actualFog,std::size_t availableBytes,Diagnostic& d)noexcept {
  d={};if(thread_!=std::this_thread::get_id()||attempted_||closed_){d.status=Status::alreadyPrepared;return false;}attempted_=true;
  if(!actualRuntime||!actualRuntime->sourceStaged()||actualRuntime->visiting_||actualRuntime->rendering_||actualRuntime->renderBinding_||
     !actualLighting||!actualLighting->world()||!actualWindow||!actualWindow->ready()||actualWindow->renderOwner_||
     actualWindow->generation_!=actualRuntime->generation_||actualWindow->attachments_.get()!=actualRuntime->graphs_.get()||
     actualWindow->material_->particles.get()!=actualRuntime->cold_.get()||!actualFog||!actualFog->ready()||
     actualFog->sourceOwner()!=actualRuntime->source_->sourceOwner()||
     actualLighting->world()->parameters.original_bytes!=actualRuntime->source_->sourceOwner()->world().parameters.original_bytes||
     actualRuntime->source_->atlas().glowScalar148!=128.f||availableBytes<retainedPayloadBytes()){
   d.status=Status::manager;return false;}
  original_gc_gameplay_lighting_candidate::GbParticleLightLease admission;
  original_gc_gameplay_lighting_candidate::Diagnostic lightDiagnostic;
  if(!actualLighting->beginGbParticleLightBorrow(actualLighting,admission,lightDiagnostic)){
   d.status=Status::lighting;return false;}
  try{auto next=std::make_unique<Storage>();runtime_=std::move(actualRuntime);lighting_=std::move(actualLighting);
   window_=std::move(actualWindow);fog_=std::move(actualFog);storage_=std::move(next);
   runtime_->renderBinding_=this;window_->renderOwner_=this;d.status=Status::complete;return true;
  }catch(...){d.status=Status::allocation;return false;}
 }
 // Structural real-destination observation, independent of held action query.
 // First completed visibility is not required; source emitters see Stale until
 // the renderer actually completes every span of the respective placement.
 bool ready()const noexcept{return current()&&!terminal_&&window_->rendererCurrent();}
 bool idle()const noexcept{return thread_==std::this_thread::get_id()&&!borrowing_&&!queryOwner_&&
  (!runtime_||(!runtime_->visiting_&&!runtime_->rendering_))&&(!window_||!window_->material_||!window_->material_->active);}
 bool closed()const noexcept{return thread_==std::this_thread::get_id()&&closed_&&!runtime_&&!window_&&!lighting_&&!fog_&&!storage_;}
 const gb_particle_runtime50GB::Owner* runtimeOwner()const noexcept{return current()?runtime_.get():nullptr;}
 const gb_particle_visual50GB::Window* windowOwner()const noexcept{return current()?window_.get():nullptr;}
 bool advance(float dt,gb_particle_runtime50GB::Diagnostic& d)noexcept {
  if(!ready()||!idle()){d={};d.source.status=gb_particle_runtime50GB::Status::unprepared;return false;}
  using Completion=gb_particle_runtime50GB::Owner::Completion;
  std::array<Completion,gb_particle_attachments50GB::placementCount> completed;
  for(std::size_t i=0;i<completed.size();++i)completed[i]=window_->completed_[i]?Completion::Current:Completion::Stale;
  return runtime_->advance(dt,completed,d);
 }
 // Retain the actual Camera Session for the entire material/numeric/GL borrow.
 // Its private source FogPreparation is checked both before and after submit.
 template<class Submit>
 bool withDraw(const gb_particle_visual50GB::MaterialBorrow& material,
  std::shared_ptr<const original_camera_session50KX::Session> camera,Diagnostic& d,Submit&& submit)noexcept {
  d={};if(!ready()||borrowing_||queryOwner_||runtime_->visiting_||runtime_->rendering_||materialBorrow_||!camera||
   !window_->acceptsMaterial(material)||(runtime_->group_&&runtime_->group_->queryActive)){d.status=Status::unprepared;return false;}
  original_camera_session50KX::Session::FogPreparation before;
  if(!cameraCurrent(*camera,before)){d.status=Status::camera;return false;}
  original_gc_gameplay_lighting_candidate::GbParticleLightLease light;
  original_gc_gameplay_lighting_candidate::Diagnostic lightDiagnostic;
  if(!lighting_->beginGbParticleLightBorrow(lighting_,light,lightDiagnostic)){d.status=Status::lighting;return false;}
  original_gc_gameplay_lighting_candidate::GbParticleLightInputs inputs;
  if(!light.read(inputs)){d.status=Status::lighting;return false;}
  borrowing_=true;runtime_->rendering_=true;materialBorrow_=&material;
  struct End {bool& pool;bool& borrow;bool& ready;const gb_particle_visual50GB::MaterialBorrow*& material;
   ~End(){material=nullptr;pool=false;borrow=false;ready=false;}} end{runtime_->rendering_,borrowing_,storage_->ready,materialBorrow_};
  d.status=build(material,camera->view(),camera->viewport(),fog_->fields(),inputs,d);
  if(d.status!=Status::complete){terminal_=true;return false;}
  d.particles=storage_->draw.sourceParticleCount;d.quads=storage_->draw.quadCount;d.runs=storage_->draw.runCount;
  bool accepted=false;try{accepted=submit(storage_->draw);}catch(...){accepted=false;}
  original_camera_session50KX::Session::FogPreparation after;
  original_gc_gameplay_lighting_candidate::GbParticleLightInputs currentLight;
  if(!accepted||!cameraCurrent(*camera,after)||after.revision!=before.revision||
     !light.read(currentLight)||currentLight.timedServiceRevision!=inputs.timedServiceRevision){
   d.status=Status::submission;terminal_=true;
   if(window_->attemptActive_&&!window_->failRenderAttempt(window_->renderer_,window_->lastAttempt_))std::terminate();
   return false;
  }
  return true;
 }
 bool close()noexcept {
  if(thread_!=std::this_thread::get_id()||!idle())return false;
  if(window_&&window_->renderer_){
   if(window_->attemptActive_&&!window_->failRenderAttempt(window_->renderer_,window_->lastAttempt_))return false;
   if(!window_->invalidateRenderPublication(window_->renderer_))return false;
  }
  if(runtime_){if(runtime_->renderBinding_!=this)return false;runtime_->renderBinding_=nullptr;}
  if(window_){if(window_->renderOwner_!=this)return false;window_->renderOwner_=nullptr;}
  terminal_=true;storage_.reset();runtime_.reset();lighting_.reset();window_.reset();fog_.reset();closed_=true;return true;
 }
};
} // namespace gb_particle_render50GB
