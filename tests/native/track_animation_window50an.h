#pragma once
// Common track-animation WINDOW (piece 4, ANIM.md): the course's one dynamic world mesh,
// published through the existing renderer path VehicleWindowDynamicWorldFrame50DS. Replaces the
// seven per-course *_dynamic_window fixtures. The renderer header is only included, never edited.
// Material alpha/fog/depth: the original 2476A0 rule (DefaultMT GS fields inherited, then the
// pass's own non-empty values), the same rule the MA/SA world route uses (selectMaSa), here
// without a course gate.
#include "track_animation_materials50an.h"
#include "../../src/game/original_material_uv_animation50uv.h"
#include "material_alpha50gx_fixture.h"
#include "../../src/assets/original_ui_material50kg.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"

namespace material_alpha50gx_fixture {
// Defined in material_alpha50gx_fixture.cpp next to selectMaSa (same rule, no course gate).
// fields = the pass's authored fields; program = its microprogram; textureResolved = the span has
// a texture. Transactional: false leaves output unchanged.
bool selectDefaultMtFields(const rr::assets::MaterialFields48E& fields,const std::string& program,bool textureResolved,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,const rr::assets::OriginalUiMaterialRuntime50KG& runtime,
    Policy& output);
}

namespace track_animation_window50AN {
namespace p=rr::platform;namespace a=rr::assets;namespace rt=track_animation_runtime50AN;
namespace src=track_animation_source50AN;namespace mat=track_animation_materials50AN;
// The source limits ARE the renderer's dynamic-world caps (one place defines them; checked here).
static_assert(src::Limits{}.vertices==p::maxVehicleWindowDynamicVertices50DS&&
  src::Limits{}.indices==p::maxVehicleWindowDynamicIndices50DS&&src::Limits{}.spans==p::maxVehicleWindowDynamicSpans50DS,
  "track animation limits must equal the renderer dynamic-world caps");
class Owner final {
 std::shared_ptr<const rt::Owner> runtime_;
 mat::Owner materials_;
 p::MaterialTextureGl50EB gpu_;
 std::vector<p::VehicleWindowTrackSpan50DS> spans_;
 // PORT EXTENSION (N64 data only): per-span opacity of the current frame (runtime row alpha,
 // extension (d)); handed to the draw only when some row is faded, so stock frames are unchanged.
 mutable std::vector<float> spanAlphas_;
 bool loaded_=false;
 std::map<std::string,rr::game::OriginalMaterialUv50UV> uvControllers_;
 std::vector<rr::game::OriginalMaterialUv50UV*> spanUv_;
 std::uint32_t uvTimestamp_=0;
 bool uvPaused_=false;
 unsigned uvSamples_=0;
 bool serviceUv(std::uint32_t span,float& u,float& v) noexcept {
  if(span>=spanUv_.size())return false;
  u=v=0.f;
  auto* state=spanUv_[span];
  if(!state||!runtime_->state(runtime_->spans()[span].row).enabled29)return true;
  if(!rr::game::serviceOriginalMaterialUv50UV(*state,uvTimestamp_,uvPaused_))return false;
  u=state->matrix.rows[2][0];v=state->matrix.rows[2][1];
  if(std::getenv("RR_TEST_N64_EFFECT_CAPTURE")&&uvSamples_<3&&u!=0.f){
   std::fprintf(stderr,"track_animation_uv span=%u timestamp=%u u=%g v=%g\n",span,uvTimestamp_,u,v);
   ++uvSamples_;
  }
  return true;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 bool loadSource(std::shared_ptr<const rt::Owner> runtime,const src::Extract& extract,mat::Diagnostic& d){
  if(loaded_||!runtime||!runtime->ready()){d={};d.message="track animation window requires a fresh window and a ready runtime";return false;}
  if(!runtime->spans().empty()&&!materials_.load(*runtime,extract,d))return false;
  runtime_=std::move(runtime);loaded_=true;return true;
 }
 bool loaded()const noexcept{return loaded_&&runtime_&&runtime_->ready();}
 bool collisionOnly()const noexcept{return loaded()&&runtime_->spans().empty();}
 bool ownsResources()const noexcept{return gpu_.ownsResources();}
 // Teardown: after close() released the GPU side, the window drops its reference to the
 // track's animation runtime (track.roles.animation) so the Track can release it.
 // Refused while GPU resources remain.
 bool releaseSource()noexcept{if(gpu_.ownsResources())return false;runtime_.reset();loaded_=false;return true;}
 const mat::Owner& materials()const noexcept{return materials_;}
 bool createGpu(const p::MaterialTextureGlRequest50EB& caller,const a::OriginalUiMaterialTemplates50KG& templates,
      const a::OriginalUiMaterialRuntime50KG& uiRuntime,p::MaterialTextureGlDiagnostic50EB& d){
  if(collisionOnly())return true;
  if(!loaded()||!materials_.bank())return false;
  const auto& spans=runtime_->spans();const auto textures=materials_.textureCount();
  std::vector<p::MaterialTextureSampler50EB> samplers(textures);std::vector<std::uint8_t> samplerSet(textures,0);
  uvControllers_.clear();spanUv_.assign(spans.size(),nullptr);
  spans_.assign(spans.size(),{});spanAlphas_.assign(spans.size(),1.f);
  for(std::size_t i=0;i<spans.size();++i){const auto& span=spans[i];const auto& m=materials_.spanMaterial(i);
   if(m.textureIndex>=textures)return false;
   if(src::detail::iequal(m.program,"DiffuseTexTransCompressed")){
    std::vector<rr::game::OriginalMaterialUvField50UV> fields;
    for(const auto& f:m.fields.authored)fields.push_back({f.name,f.value});
    rr::game::OriginalMaterialUvConfig50UV config;rr::game::OriginalMaterialUvDiagnostic50UV diagnostic;
    if(!rr::game::readOriginalMaterialUv50UV(fields,config,diagnostic))return false;
    if(diagnostic.status==rr::game::OriginalMaterialUvStatus50UV::ok)
     spanUv_[i]=&uvControllers_.try_emplace(m.name,rr::game::makeOriginalMaterialUv50UV(config)).first->second;
   }
   material_alpha50gx_fixture::Policy alpha{};
   if(!material_alpha50gx_fixture::selectDefaultMtFields(m.fields,m.program,true,templates,uiRuntime,alpha)){
    std::cerr<<"track_animation_window course="<<runtime_->label()<<" span="<<i<<" material="<<m.name<<" program="<<m.program<<" alpha policy rejected\n";return false;}
   auto& binding=spans_[i];binding={};binding.indexStart=span.indexStart;binding.indexCount=span.indexCount;
   binding.vertexStart=span.vertexStart;binding.vertexCount=span.vertexCount;binding.sourcePass=binding.sourcePassCount=1;
   binding.material.mode=p::VehicleWindowMaterialMode50DS::requiredTexture;binding.material.textureIndex=m.textureIndex;
   using B=material_alpha50gx_fixture::Blend;
   binding.material.blend=alpha.blend==B::additive?p::VehicleWindowBlend50DS::additive:
     alpha.blend==B::sourceAlpha?p::VehicleWindowBlend50DS::sourceAlpha:p::VehicleWindowBlend50DS::opaque;
   binding.material.depthWrite=alpha.depthWrite;binding.material.depthEqual=alpha.depthEqual;
   binding.material.alphaScale=alpha.alphaScale;binding.material.fixedBlendAlpha=alpha.fixedBlendAlpha;
   binding.material.unlit=alpha.unlit;binding.material.sourcePrelit=alpha.sourcePrelit;
   binding.discardZeroAlpha=alpha.discardZeroAlpha;binding.sourceAlphaTest=alpha.sourceAlphaTest;
   binding.sourceAlphaReference=alpha.sourceAlphaReference;binding.sourceFogKnown=alpha.sourceFogKnown;binding.sourceFogEnabled=alpha.sourceFogEnabled;
   const p::MaterialTextureSampler50EB sampler{p::MaterialTextureFilter50EB::linear,p::MaterialTextureFilter50EB::linear,
     m.clampU?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat,
     m.clampV?p::MaterialTextureWrap50EB::clampToEdge:p::MaterialTextureWrap50EB::repeat};
   if(samplerSet[m.textureIndex]&&(samplers[m.textureIndex].s!=sampler.s||samplers[m.textureIndex].t!=sampler.t))return false;
   samplers[m.textureIndex]=sampler;samplerSet[m.textureIndex]=1;
  }
  auto request=caller;request.samplers=samplers.data();request.samplerCount=static_cast<std::uint32_t>(samplers.size());
  request.maximumGpuBytes=materials_.bank()->rgbaByteCount();
  return gpu_.create(request,*materials_.bank(),d);
 }
 bool bindRequest(p::VehicleWindowRequest50DS& request)const noexcept{
  if(collisionOnly())return true;
  if(!loaded()||!gpu_.initialized())return false;auto& shown=request.dynamicWorld;
  if(shown.mesh||shown.textures||shown.spans||shown.spanCount)return false;
  const src::Limits limits{};
  shown.mesh=&runtime_->mesh();shown.maximumVertexBytes=std::size_t(limits.vertices)*sizeof(a::Vertex48B);
  shown.maximumIndexBytes=std::size_t(limits.indices)*sizeof(std::uint32_t);
  shown.mode=p::VehicleWindowMode50DS::callerMaterialTextures;shown.vertexColorScale=1.f;
  shown.textures=&gpu_;shown.spans=spans_.data();shown.spanCount=static_cast<std::uint32_t>(spans_.size());
  shown.sourceNormals=runtime_->normals().data();shown.sourceNormalCount=static_cast<std::uint32_t>(runtime_->normals().size());
  shown.maximumNormalBytes=std::size_t(limits.vertices)*sizeof(a::WorldNormal50HK);
  return true;
 }
 bool bindFrame(p::VehicleWindowFrame50DS& frame,std::uint32_t timestamp,bool paused) noexcept{
  if(collisionOnly())return true;
  if(!loaded()||!gpu_.initialized())return false;const auto& mesh=runtime_->mesh();
  frame.dynamicWorld={mesh.vertices.data(),static_cast<std::uint32_t>(mesh.vertices.size()),
    runtime_->normals().data(),static_cast<std::uint32_t>(runtime_->normals().size())};
  const auto& spans=runtime_->spans();bool faded=false;
  if(spanAlphas_.size()!=spans.size())return false;
  for(std::size_t i=0;i<spans.size();++i){
   if(spans[i].row>=runtime_->rowCount())return false;
   const float alpha=runtime_->state(spans[i].row).alpha;spanAlphas_[i]=alpha;faded=faded||alpha<1.f;}
  if(!uvControllers_.empty()){
   uvTimestamp_=timestamp;uvPaused_=paused;
   frame.dynamicWorld.uvOwner=this;
   frame.dynamicWorld.uvOffset=[](void* owner,std::uint32_t span,float& u,float& v) noexcept{
    return static_cast<Owner*>(owner)->serviceUv(span,u,v);
   };
  }
  if(faded){frame.dynamicWorld.spanAlphas=spanAlphas_.data();frame.dynamicWorld.spanAlphaCount=static_cast<std::uint32_t>(spanAlphas_.size());}
  return true;
 }
 bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{return gpu_.close(d);}
};
} // namespace track_animation_window50AN
