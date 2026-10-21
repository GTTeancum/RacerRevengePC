#pragma once
#include "gb_dynamic_materials_fixture.h"
#include "material_alpha50gx_fixture.h"
#include <array>
#include "../../platform/linux/native/vehicle_window_gl50ds.h"

namespace gb_dynamic_window50GB {
namespace p=rr::platform;
class Owner final {
 std::shared_ptr<const gb_animation_runtime50GB::Owner> world_;
 gb_dynamic_materials50GB::Owner source_;
 p::MaterialTextureGl50EB gpu_;
 std::array<p::VehicleWindowTrackSpan50DS,10> spans_{};
 bool loaded_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool loadSource(std::shared_ptr<const gb_animation_runtime50GB::Owner> world,gb_dynamic_materials50GB::Diagnostic& d){
  if(loaded_||!world||!world->ready()){d.message="GB dynamic window requires its fresh actual four-animation owner";return false;}
  if(!source_.load(*world->sourceOwner(),d))return false;
  world_=std::move(world);loaded_=true;return true;
 }
 bool loaded()const noexcept{return loaded_&&world_&&world_->ready();}
 bool ownsResources()const noexcept{return gpu_.ownsResources();}
 const gb_dynamic_materials50GB::Owner& source()const noexcept{return source_;}
 bool createGpu(const p::MaterialTextureGlRequest50EB& caller,const gb_animation_runtime50GB::Owner& world,
      const rr::assets::OriginalUiMaterialTemplates50KG& templates,
      const rr::assets::OriginalUiMaterialRuntime50KG& runtime,p::MaterialTextureGlDiagnostic50EB& d){
  if(!loaded()||world_.get()!=&world||!source_.bank()||!source_.selections()||source_.sourceOwner()!=world.sourceOwner()||
      source_.bank()->textureCount()!=7||world.mesh().vertices.size()!=1795||world.mesh().indices.size()!=3393||world.spans().size()!=10)return false;
  std::array<p::MaterialTextureSampler50EB,7> samplers{};
  const auto& selections=*source_.selections();
  for(std::size_t i=0;i<selections.size();++i){const auto& selected=selections[i];const auto& span=world.spans()[i];
   if(span.owner!=gb_dynamic_materials50GB::detail::owners[i]||span.material!=gb_dynamic_materials50GB::detail::localMaterials[i]||selected.textureIndex>=samplers.size())return false;
   material_alpha50gx_fixture::Policy alpha{};
   if(!material_alpha50gx_fixture::selectGb(selected,templates,runtime,alpha))return false;
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
 bool bindRequest(p::VehicleWindowRequest50DS& request,const gb_animation_runtime50GB::Owner& world)const noexcept {
  if(!loaded()||world_.get()!=&world||!gpu_.initialized())return false;
  auto& shown=request.dynamicWorld;
  if(shown.mesh||shown.textures||shown.spans||shown.spanCount)return false;
  shown.mesh=&world.mesh();shown.maximumVertexBytes=128u*1024u;shown.maximumIndexBytes=32u*1024u;
  shown.mode=p::VehicleWindowMode50DS::callerMaterialTextures;shown.vertexColorScale=1.f;
  shown.textures=&gpu_;shown.spans=spans_.data();shown.spanCount=spans_.size();
  shown.sourceNormals=world.normals().data();shown.sourceNormalCount=world.normals().size();shown.maximumNormalBytes=64u*1024u;
  return true;
 }
 bool bindFrame(p::VehicleWindowFrame50DS& frame,const gb_animation_runtime50GB::Owner& world)const noexcept {
  if(!loaded()||world_.get()!=&world||!gpu_.initialized())return false;
  frame.dynamicWorld={world.mesh().vertices.data(),static_cast<std::uint32_t>(world.mesh().vertices.size()),
      world.normals().data(),static_cast<std::uint32_t>(world.normals().size())};return true;
 }
 bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
}
