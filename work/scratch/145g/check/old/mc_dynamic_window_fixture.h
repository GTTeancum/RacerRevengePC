#pragma once
#include "mc_dynamic_materials_fixture.h"
#include "mc_dynamic_alpha50mc.h"
#include <array>
#include "../../platform/linux/native/vehicle_window_gl50ds.h"

namespace mc_dynamic_window50MC {
namespace p=rr::platform;
class Owner final {
 std::shared_ptr<const mc_animation_runtime50MC::Owner> world_;
 mc_dynamic_materials50MC::Owner source_;
 p::MaterialTextureGl50EB gpu_;
 std::array<p::VehicleWindowTrackSpan50DS,14> spans_{};
 bool loaded_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool loadSource(std::shared_ptr<const mc_animation_runtime50MC::Owner> world,mc_dynamic_materials50MC::Diagnostic& d){
  if(loaded_||!world||!world->ready()){d.message="MC dynamic window requires its fresh actual fourteen-animation owner";return false;}
  if(!source_.load(*world->sourceOwner(),d))return false;
  world_=std::move(world);loaded_=true;return true;
 }
 bool loaded()const noexcept{return loaded_&&world_&&world_->ready();}
 bool ownsResources()const noexcept{return gpu_.ownsResources();}
 const mc_dynamic_materials50MC::Owner& source()const noexcept{return source_;}
 bool createGpu(const p::MaterialTextureGlRequest50EB& caller,const mc_animation_runtime50MC::Owner& world,
      const rr::assets::OriginalUiMaterialTemplates50KG& templates,
      const rr::assets::OriginalUiMaterialRuntime50KG& runtime,p::MaterialTextureGlDiagnostic50EB& d){
  if(!loaded()||world_.get()!=&world||!source_.pixelsPinned()||!source_.bank()||!source_.selections()||source_.sourceOwner()!=world.sourceOwner()||
      source_.bank()->textureCount()!=2||world.mesh().vertices.size()!=5806||world.mesh().indices.size()!=13182||world.spans().size()!=14)return false;
  std::array<p::MaterialTextureSampler50EB,2> samplers{};
  const auto& selections=*source_.selections();
  for(std::size_t i=0;i<selections.size();++i){const auto& selected=selections[i];const auto& span=world.spans()[i];
   if(span.owner!=mc_dynamic_materials50MC::detail::owners[i]||span.material!=mc_dynamic_materials50MC::detail::localMaterials[i]||selected.textureIndex>=samplers.size())return false;
   material_alpha50gx_fixture::Policy alpha{};
   if(!material_alpha50gx_fixture::selectMcDynamic(selected,templates,runtime,alpha))return false;
   auto& binding=spans_[i];binding={};binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
   binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;binding.sourcePass=binding.sourcePassCount=1;
   binding.material.mode=p::VehicleWindowMaterialMode50DS::requiredTexture;binding.material.textureIndex=selected.textureIndex;
   using B=material_alpha50gx_fixture::Blend;
   binding.material.blend=alpha.blend==B::additive?p::VehicleWindowBlend50DS::additive:
       alpha.blend==B::sourceAlpha?p::VehicleWindowBlend50DS::sourceAlpha:p::VehicleWindowBlend50DS::opaque;
   binding.material.depthWrite=alpha.depthWrite;binding.material.depthEqual=alpha.depthEqual;
   binding.material.alphaScale=alpha.alphaScale;binding.material.fixedBlendAlpha=alpha.fixedBlendAlpha;
   binding.material.unlit=alpha.unlit;binding.material.sourcePrelit=alpha.sourcePrelit;
   binding.discardZeroAlpha=alpha.discardZeroAlpha;binding.sourceAlphaTest=alpha.sourceAlphaTest;
   binding.sourceAlphaReference=alpha.sourceAlphaReference;binding.sourceFogKnown=alpha.sourceFogKnown;binding.sourceFogEnabled=alpha.sourceFogEnabled;
   samplers[selected.textureIndex]={p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
       selected.clampU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat,
       selected.clampV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat};
  }
  auto request=caller;request.samplers=samplers.data();request.samplerCount=samplers.size();request.maximumGpuBytes=512u*1024u;
  return gpu_.create(request,*source_.bank(),d);
 }
 bool bindRequest(p::VehicleWindowRequest50DS& request,const mc_animation_runtime50MC::Owner& world)const noexcept {
  if(!loaded()||world_.get()!=&world||!gpu_.initialized())return false;
  auto& shown=request.dynamicWorld;
  if(shown.mesh||shown.textures||shown.spans||shown.spanCount)return false;
  shown.mesh=&world.mesh();shown.maximumVertexBytes=512u*1024u;shown.maximumIndexBytes=64u*1024u;
  shown.mode=p::VehicleWindowMode50DS::callerMaterialTextures;shown.vertexColorScale=1.f;
  shown.textures=&gpu_;shown.spans=spans_.data();shown.spanCount=spans_.size();
  shown.sourceNormals=world.normals().data();shown.sourceNormalCount=world.normals().size();shown.maximumNormalBytes=128u*1024u;
  return true;
 }
 bool bindFrame(p::VehicleWindowFrame50DS& frame,const mc_animation_runtime50MC::Owner& world)const noexcept {
  if(!loaded()||world_.get()!=&world||!gpu_.initialized())return false;
  frame.dynamicWorld={world.mesh().vertices.data(),static_cast<std::uint32_t>(world.mesh().vertices.size()),
      world.normals().data(),static_cast<std::uint32_t>(world.normals().size())};return true;
 }
 bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
}
