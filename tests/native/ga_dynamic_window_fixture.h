#pragma once
#include "ga_animated_world_fixture.h"
#include "ga_dynamic_materials_fixture.h"
#include "material_alpha50gx_fixture.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include <array>

namespace ga_dynamic_window {
namespace p=rr::platform;
class Owner final {
 ga_dynamic_materials::Owner source_;
 p::MaterialTextureGl50EB gpu_;
 std::array<p::VehicleWindowTrackSpan50DS,ga_animated_world::surfaceCount> spans_{};
 bool loaded_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool loadSource(const std::filesystem::path& archive,const ga_animated_world::Owner& world,
     ga_dynamic_materials::Diagnostic& d){
  return loadSource(archive,world,source_texture50fc_fixture::decodeSourceTextureGa,d);
 }
 // Direct-disc callers select their pinned decoder explicitly. Motion, GL,
 // source-time ownership and material/alpha policy are unchanged.
 bool loadSource(const std::filesystem::path& archive,const ga_animated_world::Owner& world,
     ga_dynamic_materials::SourceTextureDecoder sourceDecoder,ga_dynamic_materials::Diagnostic& d){
  if(loaded_){d.message="GA dynamic window source is already loaded";return false;}
  if(!world.ready()){d.message="GA dynamic window world is not ready";return false;}
  loaded_=source_.load(archive,world.source(0).scene,world.source(1).scene,sourceDecoder,d);return loaded_;
 }
 bool loaded()const noexcept{return loaded_;}
 bool ownsResources()const noexcept{return gpu_.ownsResources();}
 bool createGpu(const p::MaterialTextureGlRequest50EB& caller,const ga_animated_world::Owner& world,
     const rr::assets::OriginalUiMaterialTemplates50KG& templates,
     const rr::assets::OriginalUiMaterialRuntime50KG& runtime,
     p::MaterialTextureGlDiagnostic50EB& d){
  if(!loaded_||!source_.bank()||!source_.selections())return false;
  const auto& selections=*source_.selections();
  std::array<p::MaterialTextureSampler50EB,5> samplers{};
  if(source_.bank()->textureCount()!=samplers.size())return false;
  for(std::size_t i=0;i<selections.size();++i){const auto& entry=selections[i];if(entry.textureIndex>=samplers.size())return false;
   samplers[entry.textureIndex]={p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
       entry.clampU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat,
       entry.clampV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat};
   const auto& span=world.spans()[i];auto& binding=spans_[i];
   binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
   binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;
   binding.material.mode=p::VehicleWindowMaterialMode50DS::requiredTexture;binding.material.textureIndex=entry.textureIndex;
   // Moving GA models inherit the same authenticated DefaultMT alpha fields
   // as static GA selections. An omitted field is not an alpha-test disable.
   material_alpha50gx_fixture::Policy alpha{};
   if(!material_alpha50gx_fixture::selectGa(entry,templates,runtime,alpha))return false;
   using B=material_alpha50gx_fixture::Blend;
   binding.material.blend=alpha.blend==B::additive?p::VehicleWindowBlend50DS::additive:
       alpha.blend==B::sourceAlpha?p::VehicleWindowBlend50DS::sourceAlpha:p::VehicleWindowBlend50DS::opaque;
   binding.material.depthWrite=alpha.depthWrite;binding.material.depthEqual=alpha.depthEqual;
   binding.material.alphaScale=alpha.alphaScale;binding.material.unlit=alpha.unlit;
   binding.material.sourcePrelit=alpha.sourcePrelit;
   binding.discardZeroAlpha=alpha.discardZeroAlpha;
   binding.sourceAlphaTest=alpha.sourceAlphaTest;
   binding.sourceAlphaReference=alpha.sourceAlphaReference;
   binding.sourceFogKnown=alpha.sourceFogKnown;binding.sourceFogEnabled=alpha.sourceFogEnabled;
  }
  auto request=caller;request.samplers=samplers.data();request.samplerCount=samplers.size();request.maximumGpuBytes=256u*1024u;
  return gpu_.create(request,*source_.bank(),d);
 }
 bool bindRequest(p::VehicleWindowRequest50DS& request,const ga_animated_world::Owner& world)const noexcept {
  if(!loaded_||!gpu_.initialized()||!world.ready())return false;
  auto& shown=request.dynamicWorld;shown.mesh=&world.mesh();
  shown.maximumVertexBytes=64u*1024u;shown.maximumIndexBytes=16u*1024u;
  shown.mode=p::VehicleWindowMode50DS::callerMaterialTextures;shown.vertexColorScale=1.f;
  shown.textures=&gpu_;shown.spans=spans_.data();shown.spanCount=spans_.size();
  shown.sourceNormals=world.normals().data();shown.sourceNormalCount=world.normals().size();shown.maximumNormalBytes=64u*1024u;
  return true;
 }
 bool bindFrame(p::VehicleWindowFrame50DS& frame,const ga_animated_world::Owner& world)const noexcept {
  if(!loaded_||!gpu_.initialized()||!world.ready())return false;
  frame.dynamicWorld={world.mesh().vertices.data(),static_cast<std::uint32_t>(world.mesh().vertices.size()),
      world.normals().data(),static_cast<std::uint32_t>(world.normals().size())};return true;
 }
 bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
}
