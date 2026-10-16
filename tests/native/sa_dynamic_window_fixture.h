#pragma once
#include "sa_animated_world_fixture.h"
#include "sa_dynamic_materials_fixture.h"
#include "material_alpha50gx_fixture.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <array>

// Native presentation binding, separate from the original motion owner.
// All actual SA animated owners use UseLighting=T and DiffuseCompressed.
// Retain the existing native normal-based host illumination and linear repeat
// sampling. Coverage/blend/depth use the actual authenticated DefaultMT owner
// and source PSM fields, exactly as ordinary static SA materials do. This does
// not recover original world-light selection or PS2 shader/raster parity.
namespace sa_dynamic_window {
namespace p=rr::platform;
class Owner final {
 sa_dynamic_materials::Owner source_;
 p::MaterialTextureGl50EB gpu_;
 std::array<p::VehicleWindowTrackSpan50DS,sa_animated_world::surfaceCount> spans_{};
 bool loaded_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool loadSource(const std::filesystem::path& archive,const sa_animated_world::Owner& world,sa_dynamic_materials::Diagnostic& d){
  if(loaded_){d.message="SA dynamic window source is already loaded";return false;}
  if(!world.ready()){d.message="SA dynamic window world is not ready";return false;}
  loaded_=source_.load(archive,world,d);return loaded_;
 }
 bool loaded()const noexcept{return loaded_;}
 bool ownsResources()const noexcept{return gpu_.ownsResources();}
 bool createGpu(const p::MaterialTextureGlRequest50EB& caller,const sa_animated_world::Owner& world,
     const rr::assets::OriginalUiMaterialTemplates50KG& templates,
     const rr::assets::OriginalUiMaterialRuntime50KG& runtime,
     p::MaterialTextureGlDiagnostic50EB& d){
  if(!loaded_||!world.ready()||!source_.bank()||!source_.selections())return false;
  const auto& selections=*source_.selections();
  std::array<p::MaterialTextureSampler50EB,sa_dynamic_materials::textureCount> samplers{};
  if(source_.bank()->textureCount()!=samplers.size()||selections.size()!=world.spans().size())return false;
  for(auto& sampler:samplers)sampler={p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,p::MaterialTextureWrap50EB::repeat,p::MaterialTextureWrap50EB::repeat};
  for(std::size_t i=0;i<selections.size();++i){const auto& entry=selections[i];const auto& span=world.spans()[i];
   if(entry.track!=track_source_fixture::Track::sa||entry.textureIndex>=samplers.size()||span.material!=i||span.owner>=sa_animated_world::ownerCount||
      entry.assetIdentity!=std::uint64_t(span.owner)+1||entry.materialIndex!=span.sourceMaterial||entry.program!="DiffuseCompressed"||
      entry.pass!=1||entry.slot!=1||entry.passCount!=1||entry.clampU||entry.clampV||!world.source(span.owner).useLighting)return false;
   auto& binding=spans_[i];binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;
   binding.material.mode=p::VehicleWindowMaterialMode50DS::requiredTexture;binding.material.textureIndex=entry.textureIndex;
   // These authenticated PSMs omit alpha state. Omission inherits DefaultMT;
   // it cannot disable coverage or manufacture a literal reference value.
   material_alpha50gx_fixture::Policy alpha{};
   if(!material_alpha50gx_fixture::selectMaSa(entry,templates,runtime,alpha))return false;
   using B=material_alpha50gx_fixture::Blend;
   binding.material.blend=alpha.blend==B::additive?p::VehicleWindowBlend50DS::additive:
       alpha.blend==B::sourceAlpha?p::VehicleWindowBlend50DS::sourceAlpha:p::VehicleWindowBlend50DS::opaque;
   binding.material.depthWrite=alpha.depthWrite;binding.material.depthEqual=alpha.depthEqual;
   binding.material.alphaScale=alpha.alphaScale;binding.material.unlit=alpha.unlit;
   binding.material.sourcePrelit=alpha.sourcePrelit;binding.discardZeroAlpha=alpha.discardZeroAlpha;
   binding.sourceAlphaTest=alpha.sourceAlphaTest;binding.sourceAlphaReference=alpha.sourceAlphaReference;
   binding.sourceFogKnown=alpha.sourceFogKnown;binding.sourceFogEnabled=alpha.sourceFogEnabled;
   binding.sourcePass=binding.sourcePassCount=1;
  }
  auto request=caller;request.samplers=samplers.data();request.samplerCount=samplers.size();request.maximumGpuBytes=256u*1024u;
  return gpu_.create(request,*source_.bank(),d);
 }
 bool bindRequest(p::VehicleWindowRequest50DS& request,const sa_animated_world::Owner& world)const noexcept {
  if(!loaded_||!gpu_.initialized()||!world.ready())return false;
  auto& shown=request.dynamicWorld;shown={};shown.mesh=&world.mesh();
  shown.maximumVertexBytes=sa_animated_world::vertexCount*sizeof(rr::assets::Vertex48B);
  shown.maximumIndexBytes=sa_animated_world::indexCapacity*sizeof(std::uint32_t);
  shown.mode=p::VehicleWindowMode50DS::callerMaterialTextures;shown.vertexColorScale=1.f;
  shown.textures=&gpu_;shown.spans=spans_.data();shown.spanCount=spans_.size();
  shown.sourceNormals=world.normals().data();shown.sourceNormalCount=world.normals().size();
  shown.maximumNormalBytes=sa_animated_world::vertexCount*sizeof(rr::assets::WorldNormal50HK);return true;
 }
 bool bindFrame(p::VehicleWindowFrame50DS& frame,const sa_animated_world::Owner& world)const noexcept {
  if(!loaded_||!gpu_.initialized()||!world.ready())return false;
  frame.dynamicWorld={world.mesh().vertices.data(),static_cast<std::uint32_t>(world.mesh().vertices.size()),world.normals().data(),static_cast<std::uint32_t>(world.normals().size())};return true;
 }
 bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
} // namespace sa_dynamic_window
