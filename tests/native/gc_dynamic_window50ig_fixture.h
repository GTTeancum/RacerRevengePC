#pragma once
#include "gc_animated_world50ig_fixture.h"
#include "gc_dynamic_materials_fixture.h"
#include "material_alpha50gx_fixture.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <array>

namespace gc_dynamic_window50IG {
namespace p=rr::platform;
class Owner final {
 gc_dynamic_materials_fixture::Owner source_;
 p::MaterialTextureGl50EB gpu_;
 std::array<p::VehicleWindowTrackSpan50DS,gc_animated_world50IG::surfaceCount> spans_{};
 bool loaded_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool loadSource(const std::filesystem::path& archive,const gc_animated_world50IG::Owner& world,
     gc_dynamic_materials_fixture::Diagnostic& d){
  loaded_=world.ready()&&source_.load(archive,world.source(0).scene,world.source(1).scene,d);return loaded_;
 }
 bool loaded()const noexcept{return loaded_;}
 bool ownsResources()const noexcept{return gpu_.ownsResources();}
 bool createGpu(const p::MaterialTextureGlRequest50EB& caller,const gc_animated_world50IG::Owner& world,
     p::MaterialTextureGlDiagnostic50EB& d){
  if(!loaded_||!source_.bank()||!source_.selections())return false;
  const auto& selections=*source_.selections();
  std::array<p::MaterialTextureSampler50EB,6> samplers{};
  if(source_.bank()->textureCount()!=samplers.size())return false;
  for(std::size_t i=0;i<selections.size();++i){const auto& entry=selections[i];if(entry.textureIndex>=samplers.size())return false;
   samplers[entry.textureIndex]={p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
       entry.clampU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat,
       entry.clampV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat};
   const auto& span=world.spans()[i];auto& binding=spans_[i];
   binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
   binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;
   binding.material.mode=p::VehicleWindowMaterialMode50DS::requiredTexture;binding.material.textureIndex=entry.textureIndex;
   material_alpha50gx_fixture::Policy alpha{};if(!material_alpha50gx_fixture::select(entry,alpha))return false;
   using B=material_alpha50gx_fixture::Blend;
   binding.material.blend=alpha.blend==B::additive?p::VehicleWindowBlend50DS::additive:
       alpha.blend==B::sourceAlpha?p::VehicleWindowBlend50DS::sourceAlpha:p::VehicleWindowBlend50DS::opaque;
   binding.material.depthWrite=alpha.depthWrite;binding.material.depthEqual=alpha.depthEqual;
   binding.material.alphaScale=alpha.alphaScale;binding.material.unlit=alpha.unlit;
   binding.discardZeroAlpha=alpha.discardZeroAlpha;
  }
  auto request=caller;request.samplers=samplers.data();request.samplerCount=samplers.size();request.maximumGpuBytes=256u*1024u;
  return gpu_.create(request,*source_.bank(),d);
 }
 bool bindRequest(p::VehicleWindowRequest50DS& request,const gc_animated_world50IG::Owner& world)const noexcept {
  if(!loaded_||!gpu_.initialized()||!world.ready())return false;
  auto& shown=request.dynamicWorld;shown.mesh=&world.mesh();
  shown.maximumVertexBytes=64u*1024u;shown.maximumIndexBytes=16u*1024u;
  shown.mode=p::VehicleWindowMode50DS::callerMaterialTextures;shown.vertexColorScale=1.f;
  shown.textures=&gpu_;shown.spans=spans_.data();shown.spanCount=spans_.size();
  shown.sourceNormals=world.normals().data();shown.sourceNormalCount=world.normals().size();shown.maximumNormalBytes=64u*1024u;
  return true;
 }
 bool bindFrame(p::VehicleWindowFrame50DS& frame,const gc_animated_world50IG::Owner& world)const noexcept {
  if(!loaded_||!gpu_.initialized()||!world.ready())return false;
  frame.dynamicWorld={world.mesh().vertices.data(),static_cast<std::uint32_t>(world.mesh().vertices.size()),
      world.normals().data(),static_cast<std::uint32_t>(world.normals().size())};return true;
 }
 bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
}
