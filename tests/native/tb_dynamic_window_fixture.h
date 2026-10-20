#pragma once
#include "tb_dynamic_materials_fixture.h"
#include "material_alpha50gx_fixture.h"
#include <array>
#include "../../platform/linux/native/vehicle_window_gl50ds.h"

namespace tb_dynamic_window50TB {
namespace p=rr::platform;
class Owner final {
 std::shared_ptr<const tb_door_runtime50TB::Owner> world_;
 tb_dynamic_materials50TB::Owner source_;
 p::MaterialTextureGl50EB gpu_;
 std::array<p::VehicleWindowTrackSpan50DS,1> spans_{};
 bool loaded_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool loadSource(std::shared_ptr<const tb_door_runtime50TB::Owner> world,tb_dynamic_materials50TB::Diagnostic& d){
  if(loaded_||!world||!world->ready()){d.message="TB dynamic window requires its fresh actual door owner";return false;}
  if(!source_.load(world->source(),d))return false;
  world_=std::move(world);loaded_=true;return true;
 }
 bool loaded()const noexcept{return loaded_&&world_&&world_->ready();}
 bool ownsResources()const noexcept{return gpu_.ownsResources();}
 const tb_dynamic_materials50TB::Owner& source()const noexcept{return source_;}
 bool createGpu(const p::MaterialTextureGlRequest50EB& caller,const tb_door_runtime50TB::Owner& world,
      const rr::assets::OriginalUiMaterialTemplates50KG& templates,
      const rr::assets::OriginalUiMaterialRuntime50KG& runtime,p::MaterialTextureGlDiagnostic50EB& d){
  if(!loaded()||world_.get()!=&world||!source_.bank()||source_.bank()->textureCount()!=1||
      world.mesh().vertices.size()!=314||world.mesh().indices.size()!=510||world.spans().size()!=1)return false;
  const auto& selected=*source_.selection();const auto& span=world.spans()[0];
  if(span.material!=0||span.vertexStart||span.indexStart||span.vertexCount!=314||span.indexCount!=510||selected.textureIndex)return false;
  material_alpha50gx_fixture::Policy alpha{};
  if(!material_alpha50gx_fixture::selectTb(selected,templates,runtime,alpha))return false;
  auto& binding=spans_[0];binding={};binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
  binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;
  binding.sourcePass=binding.sourcePassCount=1;
  binding.material.mode=p::VehicleWindowMaterialMode50DS::requiredTexture;binding.material.textureIndex=0;
  using B=material_alpha50gx_fixture::Blend;
  binding.material.blend=alpha.blend==B::additive?p::VehicleWindowBlend50DS::additive:
      alpha.blend==B::sourceAlpha?p::VehicleWindowBlend50DS::sourceAlpha:p::VehicleWindowBlend50DS::opaque;
  binding.material.depthWrite=alpha.depthWrite;binding.material.depthEqual=alpha.depthEqual;
  binding.material.alphaScale=alpha.alphaScale;binding.material.fixedBlendAlpha=alpha.fixedBlendAlpha;
  binding.material.unlit=alpha.unlit;binding.material.sourcePrelit=alpha.sourcePrelit;
  binding.discardZeroAlpha=alpha.discardZeroAlpha;binding.sourceAlphaTest=alpha.sourceAlphaTest;
  binding.sourceAlphaReference=alpha.sourceAlphaReference;binding.sourceFogKnown=alpha.sourceFogKnown;
  binding.sourceFogEnabled=alpha.sourceFogEnabled;
  const p::MaterialTextureSampler50EB sampler{p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
      selected.clampU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat,
      selected.clampV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat};
  auto request=caller;request.samplers=&sampler;request.samplerCount=1;request.maximumGpuBytes=256u*1024u;
  return gpu_.create(request,*source_.bank(),d);
 }
 bool bindRequest(p::VehicleWindowRequest50DS& request,const tb_door_runtime50TB::Owner& world)const noexcept {
  if(!loaded()||world_.get()!=&world||!gpu_.initialized())return false;
  auto& shown=request.dynamicWorld;
  if(shown.mesh||shown.textures||shown.spans||shown.spanCount)return false;
  shown.mesh=&world.mesh();shown.maximumVertexBytes=64u*1024u;shown.maximumIndexBytes=16u*1024u;
  shown.mode=p::VehicleWindowMode50DS::callerMaterialTextures;shown.vertexColorScale=1.f;
  shown.textures=&gpu_;shown.spans=spans_.data();shown.spanCount=1;
  shown.sourceNormals=world.normals().data();shown.sourceNormalCount=world.normals().size();shown.maximumNormalBytes=64u*1024u;
  return true;
 }
 bool bindFrame(p::VehicleWindowFrame50DS& frame,const tb_door_runtime50TB::Owner& world)const noexcept {
  if(!loaded()||world_.get()!=&world||!gpu_.initialized())return false;
  frame.dynamicWorld={world.mesh().vertices.data(),static_cast<std::uint32_t>(world.mesh().vertices.size()),
      world.normals().data(),static_cast<std::uint32_t>(world.normals().size())};return true;
 }
 bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
}
