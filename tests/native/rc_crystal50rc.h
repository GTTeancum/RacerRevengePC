#pragma once
#include "rc_original_world50lm.h"
#include "../../src/game/original_track_environment50rc.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include "../../src/game/camera_view49f.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <thread>
#include <exception>
#include <stdexcept>
// Exact cold source association and lifetime bridge. No canonical model light
// registration, runtime Scene/FIFO receipt, GL name or caller-ready setter.
namespace rc_crystal50RC {
namespace a=rr::assets;namespace g=rr::game;namespace p=rr::platform;
namespace math=g::original_track_environment50RC;
class Owner final {
 std::shared_ptr<const rc_original_world50LM::Owner> world_;
 std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> lighting_;
 const a::World48F* lightWorld_=nullptr;
 std::array<material_alpha50gx_fixture::Policy,44> policies_{};
 std::thread::id thread_=std::this_thread::get_id();bool attempted_=false;
public:
 static constexpr std::size_t uvCount=20596;
 Owner()=default;~Owner(){if(world_&&thread_!=std::this_thread::get_id())std::terminate();}
 Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool ready()const noexcept {
  std::array<float,4> row{};
  return thread_==std::this_thread::get_id()&&world_&&lighting_&&world_->loaded()&&
   lighting_->world()==lightWorld_&&lighting_->currentSourceGlobalAmbient50(row)&&
   row==std::array<float,4>({7.5f,5.f,10.f,64.f});
 }
 const a::Mesh48B* mesh()const noexcept{return ready()?world_->mesh():nullptr;}
 const auto& ranges()const noexcept{return *world_->crystalPlacements();} // ready precondition
 bool prepare(std::shared_ptr<const rc_original_world50LM::Owner> world,
   std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> lighting,
   const a::OriginalUiMaterialTemplates50KG& templates,const a::OriginalUiMaterialRuntime50KG& runtime,
   std::size_t availableCpuPayloadBytes,std::string& error)noexcept {
  if(attempted_||thread_!=std::this_thread::get_id()){error="RC crystal preparation order";return false;}
  attempted_=true;
  try {
   const auto need=[](bool v,const char* what){if(!v)throw std::runtime_error(what);};
   need(availableCpuPayloadBytes>=sizeof(Owner),"RC crystal explicit CPU payload budget");
   need(world&&world->loaded()&&world->mesh()&&world->bank()&&world->spans()&&world->selections()&&
     world->bank()->textureCount()==44&&world->selections()->size()==44&&world->spans()->size()==1767&&
     world->mesh()->vertices.size()==271949&&world->mesh()->indices.size()==475035,
     "RC crystal complete world source extent");
   need(lighting&&lighting->world()&&lighting->world()->parameters.original_bytes.size()==468918&&
     pod_materials50ed_fixture::detail::sha256(lighting->world()->parameters.original_bytes)==
     "1efe553a10cbeb349794c75c6f9e898f2871474a2dd587e0109772ae7cd1e27a",
     "RC crystal actual lighting source differs");
   std::array<float,4> ambient{};need(lighting->currentSourceGlobalAmbient50(ambient)&&
     ambient==std::array<float,4>({7.5f,5.f,10.f,64.f}),"RC crystal authenticated ambient row");
   std::array<material_alpha50gx_fixture::Policy,44> next{};std::size_t marked=0;
   for(std::size_t i=0;i<next.size();++i){need(world->alpha(i,templates,runtime,next[i]),"RC crystal source policy");
     marked+=next[i].rcCrystalPass!=0;need(!world->selections()->at(i).projectedPrelit,"RC cannot share generic projection stream");}
   need(marked==2&&world->crystalSources()&&world->crystalPlacements()&&
     world->crystalSources()->size()==4&&world->crystalPlacements()->size()==79,"RC crystal association counts");
   std::size_t vertices=0;std::uint32_t prior=0;
   for(const auto& range:*world->crystalPlacements()){
    need(range.sourceIndex<4&&range.firstSpan+1<world->spans()->size()&&
      (!vertices||range.firstSpan>prior),"RC crystal source range order");prior=range.firstSpan;
    const auto& src=world->crystalSources()->at(range.sourceIndex);
    need(src.vertices.size()==range.vertexCount&&std::isfinite(src.qPosition)&&src.qPosition>0&&
      std::isfinite(src.qNormal)&&src.qNormal>0,"RC crystal raw extent");
    const auto& base=world->spans()->at(range.firstSpan);const auto& env=world->spans()->at(range.firstSpan+1);
    need(base.sourcePass==1&&env.sourcePass==2&&base.sourcePassCount==2&&env.sourcePassCount==2&&
      base.vertexStart==range.vertexStart&&base.vertexCount==range.vertexCount&&
      base.indexStart==range.indexStart&&base.indexCount==range.indexCount&&
      env.vertexStart==base.vertexStart&&env.vertexCount==base.vertexCount&&
      env.indexStart==base.indexStart&&env.indexCount==base.indexCount&&
      base.materialSelection<44&&env.materialSelection<44&&
      next[base.materialSelection].rcCrystalPass==1&&next[env.materialSelection].rcCrystalPass==2,
      "RC crystal actual adjacent source spans differ");
    for(double n:range.matrix)need(std::isfinite(n)&&std::isfinite(static_cast<float>(n)),"RC crystal placement binary32 boundary");
    vertices+=range.vertexCount;
   }
   need(vertices==uvCount,"RC crystal compact UV extent");
   policies_=next;lightWorld_=lighting->world();world_=std::move(world);lighting_=std::move(lighting);error.clear();return true;
  }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}catch(...){return false;}
 }
 // Root calls after complete ordinary alpha lowering. Only the exact source
 // crystal spans acquire the marker; unrelated spans are verified unmarked.
 bool bindSpan(std::size_t i,p::VehicleWindowTrackSpan50DS& out)const noexcept {
  if(!ready()||i>=world_->spans()->size())return false;const auto& s=world_->spans()->at(i);
  if(s.materialSelection>=44||out.indexStart!=s.indexStart||out.indexCount!=s.indexCount||
    out.vertexStart!=s.vertexStart||out.vertexCount!=s.vertexCount||out.sourcePass!=s.sourcePass||
    out.sourcePassCount!=s.sourcePassCount||out.material.textureIndex!=s.textureIndex||out.rcCrystalPass)return false;
  const auto mark=policies_[s.materialSelection].rcCrystalPass;
  if(mark){out.rcCrystalPass=mark;out.allowPodShadow=false;}
  return true;
 }
 bool validate(const p::VehicleWindowTrack50DS& track)const noexcept {
  if(!ready()||track.mesh!=world_->mesh()||!track.spans||track.spanCount!=1767||!track.textures||
    track.sourceProjectionUvs||track.sourceProjectionCount||track.maximumProjectionBytes||track.vertexColorScale!=1.f)return false;
  bool crowdAa1=false;std::size_t marked=0;
  for(std::size_t i=0;i<track.spanCount;++i){const auto& src=world_->spans()->at(i);const auto& dst=track.spans[i];
   if(src.materialSelection>=44||dst.indexStart!=src.indexStart||dst.indexCount!=src.indexCount||
      dst.vertexStart!=src.vertexStart||dst.vertexCount!=src.vertexCount||dst.sourcePass!=src.sourcePass||
      dst.sourcePassCount!=src.sourcePassCount||dst.material.textureIndex!=src.textureIndex||
      dst.rcCrystalPass!=policies_[src.materialSelection].rcCrystalPass)return false;
   const auto& selected=world_->selections()->at(src.materialSelection);
   if(pod_materials50ed_fixture::detail::normalized(selected.materialName)=="CROWD"){
    if(dst.nativeCoverage!=p::NativeTrackCoverage50DS::fourSampleAa1)return false;crowdAa1=true;}
   if(dst.rcCrystalPass){if(dst.rcCrystalPass==2&&!i)return false;
    ++marked;const std::size_t first=dst.rcCrystalPass==1?i:i-1;
    if(first+1>=track.spanCount||!p::validRcCrystalPair50DS(track.spans[first],track.spans[first+1]))return false;}
  }
  if(!track.textures->matchesSourceBank(*world_->bank(),track.textures->publicationToken())||
     track.textures->textureCount()!=44)return false;
  for(std::uint32_t i=0;i<44;++i){const auto* cpu=world_->bank()->texture(i);const auto* gpu=track.textures->asset(i);
   if(!cpu||!gpu||!gpu->name||cpu->identity!=gpu->identity)return false;}
  return crowdAa1&&marked==158;
 }
 // Renderer-private scratch may hold a rejected prefix, but no output is
 // uploaded/drawn until the whole call succeeds. Ambient commits at the end.
 bool project(const g::CameraViewOutput49F& camera,math::Uv* output,std::size_t capacity,
   std::array<std::uint8_t,4>& ambient)const noexcept {
  if(!ready()||!output||capacity!=uvCount||!math::detail::ready())return false;
  std::array<float,4> actualAmbient{};std::array<std::uint8_t,4> nextAmbient{};
  if(!lighting_->currentSourceGlobalAmbient50(actualAmbient)||!math::projectAmbient(actualAmbient,nextAmbient))return false;
  const math::Vec3 position{{camera.position.x,camera.position.y,camera.position.z}};
  std::size_t at=0;
  for(const auto& range:ranges()){
   const auto& src=world_->crystalSources()->at(range.sourceIndex);std::array<float,16> m{};
   // Exact double placement is the mesh producer's retained authority. This
   // explicit native binary32 projection boundary does not assert equality.
   for(unsigned i=0;i<16;++i){if(!std::isfinite(range.matrix[i]))return false;
    m[i]=static_cast<float>(range.matrix[i]);if(!std::isfinite(m[i]))return false;}
   math::Vec3 delta{},local{};
   for(unsigned a=0;a<3;++a){delta[a]=math::detail::sub(position[a],m[12+a]);if(!std::isfinite(delta[a]))return false;}
   for(unsigned a=0;a<3;++a){const float x=math::detail::mul(delta[0],m[a*4]);
    const float y=math::detail::mul(delta[1],m[a*4+1]);const float z=math::detail::mul(delta[2],m[a*4+2]);
    local[a]=a==2?math::detail::add(math::detail::add(z,x),y):a==1?
      math::detail::add(math::detail::add(y,x),z):math::detail::add(math::detail::add(x,y),z);
    if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)||!std::isfinite(local[a]))return false;}
   const math::Inputs inputs{local,src.qPosition,src.qNormal};
   for(const auto& vertex:src.vertices){if(at>=capacity||!math::projectUv(vertex,inputs,output[at]))return false;++at;}
  }
  if(at!=capacity)return false;ambient=nextAmbient;return true;
 }
};
} // namespace rc_crystal50RC
