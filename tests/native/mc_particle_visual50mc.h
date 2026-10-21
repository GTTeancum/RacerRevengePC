#pragma once
#include "mc_particle_material_borrow50mc.h"
#include "mc_particle_attachments50mc.h"
#include "mc_original_world50lm.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../platform/linux/native/vehicle_window_gl50ds.h"
#include "../../platform/linux/native/material_texture_gl50eb.h"
#include <array>
#include <limits>

namespace mc_particle_render50MC {class Owner;}
namespace mc_particle_visual50MC {
namespace a=rr::assets;namespace p=rr::platform;
struct Diagnostic {std::string message;a::DecodeError48C decode{};a::OriginalStandardMaterialDiagnostic50LA material{};a::MaterialTextureAssetDiagnostic50EA bank{};};
class Window final {
 friend class rr::platform::VehicleWindowGl50DS;
 friend class ::mc_particle_render50MC::Owner;
 std::shared_ptr<const mc_original_world50LM::Owner> world_;
 std::shared_ptr<mc_particle_attachments50MC::Owner> attachments_;
 std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain_;
 std::shared_ptr<MaterialBorrow::State> material_;
 const a::OriginalStandardMaterialObject50LA* canonicalMaterial_=nullptr;
 a::MaterialTextureAssets50EA bank_;
 p::MaterialTextureGl50EB gpu_;
 std::array<std::size_t,mc_particle_attachments50MC::placementCount> representative_{};
 std::array<bool,mc_particle_attachments50MC::placementCount> completed_{};
 const ::mc_particle_render50MC::Owner* renderOwner_=nullptr;
 const p::VehicleWindowGl50DS* renderer_=nullptr;
 std::uint64_t generation_=0,lastAttempt_=0,lastCompleted_=0;
 void* context_=nullptr;bool(*isCurrent_)(void*)noexcept=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false,closed_=false,attemptActive_=false;
 static constexpr std::uint64_t imageIdentity=0x4d4344454d4f5052ull,materialIdentity=0x4d43504152544943ull;
 static void need(bool x,const char* m){if(!x)throw std::runtime_error(m);}
 bool thread()const noexcept{return thread_==std::this_thread::get_id();}
 static bool particleContext(const a::OriginalStandardMaterialStateContext50LA& context,unsigned state)noexcept {
  using F=a::OriginalUiGsField50KG;
  const auto eq=[&](F f,std::int32_t value){return context.fields[static_cast<std::size_t>(f)].value==value;};
  if(context.runtime435670||state>1)return false;
  if(!eq(F::textureColorComponent,1)||!eq(F::textureFunction,0)||!eq(F::colorClampingMethod,1)||
    !eq(F::alphaCorrectionValue,0)||!eq(F::alphaBlendingControlInUnitsOfPixels,0)||!eq(F::gouraudShadingOnOff,1)||
    !eq(F::textureMappingOnOff,1)||!eq(F::foggingOnOff,1)||!eq(F::alphaBlendingOnOff,1)||
    !eq(F::onePassAntiAliasingOnOff,0)||!eq(F::textureCoordinatesSTQorUV,0)||!eq(F::fragmentValueControlFixedOrNot,0)||
    !eq(F::alphaTestOnOff,1)||!eq(F::alphaTestMethod,6)||!eq(F::alphaReferenceValue,0)||
    !eq(F::processingMethodWhenFailedInAlphaTest,0)||!eq(F::destinationAlphaTestOnOff,0)||
    !eq(F::depthTestMethod,2)||!eq(F::zWriteDisabled,1)||!eq(F::inputColorA,0)||
    !eq(F::inputColorB,state?2:1)||!eq(F::inputAlphaC,0)||!eq(F::inputColorD,1)||
    !eq(F::wrapModeHorizontal,0)||!eq(F::wrapModeVertical,0)||!eq(F::environmentContext0or1,static_cast<std::int32_t>(state)))return false;
  constexpr std::int32_t sampler[6]={0,0,1,5,0,-75};
  for(unsigned i=0;i<6;++i)if(context.sampler[i].value!=sampler[i])return false;
  return true;
 }
 bool map(a::OriginalStandardMaterialMapBindingView50LA& out,Diagnostic& d)const noexcept{
  if(!domain_||!material_||!canonicalMaterial_||domain_->materials382B38()!=material_->table)return false;
  for(unsigned state=0;state<2;++state){
   a::OriginalStandardMaterialMapBindingView50LA view;
   if(!domain_->materials382B38()->prepareColdRepresentativeMap249610(domain_->coldCollector14D440(),material_->row,0,state,view,d.material)||
      view.preparation!=a::OriginalStandardMaterialMapPreparation50LA::ready||view.texture!=material_->texture||
      view.origin.material!=canonicalMaterial_||view.representative.material!=canonicalMaterial_||
      !particleContext(view.contextAfter249610,state))return false;
   if(!state)out=view;
  }
  return true;
 }
 // Renderer-only authority for the actual scoped numeric submission.
 bool acceptsMaterial(const MaterialBorrow& borrow)const noexcept {
  if(!ready()||!renderer_||!material_||!material_->active||
     borrow.state_.get()!=material_.get()||borrow.issuer_!=this||borrow.generation_!=generation_||!borrow.current()||
     borrow.coldSource()!=world_->sourceOwner().get()||borrow.trackParticles()!=material_->particles.get()||
     borrow.table382B38()!=material_->table.get()||borrow.materialRow()!=material_->row||borrow.texture()!=material_->texture||
     !gpu_.validBorrow(context_,isCurrent_,gpu_.publicationToken())||!gpu_.matchesSourceBank(bank_,gpu_.publicationToken()))return false;
  a::OriginalStandardMaterialMapBindingView50LA selected;Diagnostic diagnostic;
  return map(selected,diagnostic);
 }
 const a::MaterialTextureAssets50EA* materialBank(const MaterialBorrow& borrow)const noexcept {
  return acceptsMaterial(borrow)?&bank_:nullptr;
 }
 const p::MaterialTextureGl50EB* materialGpu(const MaterialBorrow& borrow)const noexcept {
  return acceptsMaterial(borrow)?&gpu_:nullptr;
 }
 // Called only by the actual renderer after its own cold request validation.
 bool validateTrack(const p::VehicleWindowTrack50DS& track)const noexcept {
  if(!ready()||track.mesh!=world_->mesh()||!track.spans||track.spanCount!=world_->spans()->size()||
     !track.textures||!world_->bank()||
     !track.textures->matchesSourceBank(*world_->bank(),track.textures->publicationToken()))return false;
  for(std::size_t i=0;i<track.spanCount;++i){const auto& x=world_->spans()->at(i);const auto& y=track.spans[i];
   if(x.indexStart!=y.indexStart||x.indexCount!=y.indexCount||x.vertexStart!=y.vertexStart||x.vertexCount!=y.vertexCount||
      x.sourcePass!=y.sourcePass||x.sourcePassCount!=y.sourcePassCount||x.textureIndex!=y.material.textureIndex)return false;}
  return true;
 }
 bool attachRenderer(const p::VehicleWindowGl50DS* renderer)noexcept{
  if(!ready()||!renderer||renderer_)return false;renderer_=renderer;lastCompleted_=0;completed_.fill(false);return true;
 }
 bool invalidateRenderPublication(const p::VehicleWindowGl50DS* renderer)noexcept {
  if(!thread()||!renderer||renderer_!=renderer||attemptActive_||!attachments_)return false;
  completed_.fill(false);lastCompleted_=0;return true;
 }
 void retireRenderer(const p::VehicleWindowGl50DS* renderer)noexcept{
  if(!thread()||renderer_!=renderer||attemptActive_||(material_&&material_->active))std::terminate();
  if(!invalidateRenderPublication(renderer))std::terminate();renderer_=nullptr;
 }
 bool beginRenderAttempt(const p::VehicleWindowGl50DS* renderer,std::uint64_t attempt)noexcept {
  if(!ready()||renderer_!=renderer||attemptActive_||!attempt||attempt<=lastAttempt_)return false;
  completed_.fill(false);lastAttempt_=attempt;attemptActive_=true;return true;
 }
 bool recordCompleted(const p::VehicleWindowGl50DS* renderer,std::uint64_t attempt,std::uint64_t completed,
    const std::array<bool,mc_particle_attachments50MC::placementCount>& submitted)noexcept {
  if(!ready()||renderer_!=renderer||!attemptActive_||!attempt||attempt!=lastAttempt_||
     !completed||completed<=lastCompleted_||(material_&&material_->active)||
     !gpu_.validBorrow(context_,isCurrent_,gpu_.publicationToken()))return false;
  completed_=submitted;lastCompleted_=completed;attemptActive_=false;return true;
 }
 bool failRenderAttempt(const p::VehicleWindowGl50DS* renderer,std::uint64_t attempt)noexcept {
  if(!thread()||renderer_!=renderer||!attemptActive_||attempt!=lastAttempt_)return false;
  completed_.fill(false);attemptActive_=false;return true;
 }
 bool rendererCurrent()const noexcept {
  return ready()&&renderer_&&material_&&gpu_.validBorrow(context_,isCurrent_,gpu_.publicationToken())&&
   gpu_.matchesSourceBank(bank_,gpu_.publicationToken());
 }

public:
 // Conservative peak during cold image/source decode, bank transfer and hashes.
 // Retained bank/source descriptors are a subset; do not charge both peak and
 // retained observations permanently. Shared source/domain/pool are charged by root.
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept {
  return sizeof(Window)+sizeof(MaterialBorrow::State)+8u*132124u+4u*128u*1024u*4u+128u;
 }
 // This conservative reservation remains charged until actual destruction. The existing
 // bank hides descriptor/vector capacities, so the logical observation below
 // is not permission to reclaim the difference as measured heap bytes.
 std::size_t retainedReservationBytes()const noexcept {
  return thread()&&attempted_?maximumAdditionalPayloadBytes():0;
 }
 std::size_t observedImageAndSpanPayloadBytes()const noexcept {
  if(!thread())return 0;
  return bank_.rgbaByteCount()+sizeof(representative_)+sizeof(completed_);
 }
 Window()=default;Window(const Window&)=delete;Window& operator=(const Window&)=delete;
 ~Window(){if(!thread()||renderer_||renderOwner_||attemptActive_||gpu_.ownsResources()||(material_&&material_->active))std::terminate();}
 bool ready()const noexcept{return thread()&&ready_&&!closed_&&world_&&world_->loaded()&&attachments_&&attachments_->sourceGraphReady();}
 bool prepare(std::shared_ptr<const mc_original_world50LM::Owner> world,
    std::shared_ptr<mc_particle_attachments50MC::Owner> attachments,
    std::shared_ptr<rr::game::OriginalTrackParticleCold50LA> particles,
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain,std::uint64_t generation,
    std::size_t availablePayloadBytes,Diagnostic& d)noexcept {
  d={};if(!thread()||attempted_||closed_)return false;attempted_=true;
  try{
   need(world&&world->loaded()&&attachments&&attachments->sourceGraphReady()&&particles&&particles->prepared()&&domain&&domain->prepared()&&generation,
        "MC actual visual owners unavailable");
   need(world->sourceOwner().get()==attachments->coldSource(),"MC actual graph/world source differs");
   const auto* pool=particles->particles();const auto& atlas=attachments->authoringOwner()->atlas();
   need(pool&&pool->atlasTextureMapName()&&*pool->atlasTextureMapName()==atlas.textureMap&&
        !pool->frame(152)&&!pool->group(10),"MC actual track atlas extent differs");
   for(std::size_t i=0;i<152;++i){const auto* f=pool->frame(static_cast<std::int32_t>(i));
    need(f&&f->sourceUv==atlas.frames[i].sourceUv,"MC actual track atlas frame differs");}
   for(std::size_t i=0;i<10;++i){const auto* g=pool->group(static_cast<std::int32_t>(i));const auto& x=atlas.groups[i];
    need(g&&g->type==x.type&&g->first==x.first&&g->last==x.last,"MC actual track atlas group differs");}
   const auto* reference=particles->material150();
   need(reference&&reference->retained()&&reference->table382B38()==domain->materials382B38().get(),"MC actual canonical particle reference differs");
   // Conservative cold payload reservation for exact indexed atlas decode,
   // serialized bank copy, SHA scratch, descriptors and source-span lists.
   need(availablePayloadBytes>=maximumAdditionalPayloadBytes(),
        "MC particle visual remaining payload unavailable");
   world_=std::move(world);attachments_=std::move(attachments);domain_=std::move(domain);generation_=generation;
   material_=std::make_shared<MaterialBorrow::State>();material_->issuer=this;material_->generation=generation;
   material_->source=world_->sourceOwner();material_->particles=std::move(particles);material_->table=domain_->materials382B38();material_->row=reference->material00();
   a::OriginalStandardMaterialMapBindingView50LA selected;
   need(domain_->materials382B38()->prepareColdRepresentativeMap249610(domain_->coldCollector14D440(),material_->row,0,0,selected,d.material)&&
      selected.preparation==a::OriginalStandardMaterialMapPreparation50LA::ready&&selected.texture,"MC actual final particle map unavailable");
   canonicalMaterial_=selected.representative.material;
   need(canonicalMaterial_&&selected.origin.material==canonicalMaterial_&&canonicalMaterial_->constructed29B550()&&
      canonicalMaterial_->passCount48()&&*canonicalMaterial_->passCount48()==1&&canonicalMaterial_->source(),
      "MC actual particle final material differs");
   std::vector<std::uint8_t> psm;
   need(material_->source->extractMember("particle\\demoprt.psm",psm,d.decode,5022)&&psm.size()==5022&&
      pod_materials50ed_fixture::detail::sha256(psm)=="ed7c0a0799dc749f0c406503f0c4825b481038bb790cae2f50fc12897bf9ebe1"&&
      canonicalMaterial_->source()->originalBytes()&&*canonicalMaterial_->source()->originalBytes()==psm&&
      canonicalMaterial_->source()->member()&&pod_materials50ed_fixture::detail::normalized(*canonicalMaterial_->source()->member())=="PARTICLE/DEMOPRT.PSM",
      "MC actual particle PSM identity differs");
   a::OriginalStandardMaterialPassView50LA pass;
   need(canonicalMaterial_->pass4C(0,pass)&&pass.mapCount20&&*pass.mapCount20==2&&pass.state30&&
      pass.state30->contextCount()==2,"MC actual particle two-context construction differs");
   material_->texture=selected.texture;const auto& resource=*selected.texture;
   need(map(selected,d),"MC actual particle GPU context policy differs");
   std::vector<std::uint8_t> pst;
   need(material_->source->extractMember("particle\\demoprt.pst",pst,d.decode,132124)&&pst.size()==132124&&
      pod_materials50ed_fixture::detail::sha256(pst)=="027296fc1e953a6a936482bf1f51a6ad91e653bf5c322225e73396a259ed2361"&&
      resource.originalBytes()&&*resource.originalBytes()==pst&&resource.sourceMember()&&
      pod_materials50ed_fixture::detail::normalized(*resource.sourceMember())=="PARTICLE/DEMOPRT.PST"&&
      resource.sourceKind()&&*resource.sourceKind()==1&&resource.levelCountB0()&&*resource.levelCountB0()==1,
      "MC actual canonical atlas identity differs");
   a::Texture48E decoded;need(a::decodeTexture48E(pst,decoded,d.decode)&&decoded.kind==1&&decoded.mips.size()==1,
      "MC actual indexed atlas decode rejected");
   a::OriginalStandardTextureLevelView50LA level;
   need(resource.level100(0,level)&&level.width104==128&&level.height110==1024&&level.sourceMipOrdinal==0&&
      resource.palette208()&&sizeof(decoded.original_palette)==1024&&
      !std::memcmp(resource.palette208(),decoded.original_palette.data(),1024)&&level.linearSourceIndices&&
      level.linearSourceBytes==128u*1024u&&level.sourceHeaderOffset==decoded.mips[0].source_header_offset&&
      level.sourceIndicesOffset==decoded.mips[0].source_indices_offset&&level.sourceIndicesOffset<=pst.size()&&
      level.linearSourceBytes<=pst.size()-level.sourceIndicesOffset&&
      !std::memcmp(level.linearSourceIndices,pst.data()+level.sourceIndicesOffset,level.linearSourceBytes),
      "MC canonical atlas level/palette differs");
   material_->width=static_cast<std::uint32_t>(level.width104);material_->height=static_cast<std::uint32_t>(level.height110);
   const a::MaterialTextureAssetInput50EA image{imageIdentity,&decoded.base_rrtex};
   const a::MaterialTextureSlotInput50EA slot{{materialIdentity,0,1,1},imageIdentity,a::MaterialTextureRequirement50EA::required};
   const a::MaterialTextureAssetsInput50EA input{&image,1,&slot,1,{1,1,128u*1024u*4u+16u,128u*1024u*4u,1024}};
   need(bank_.load(input,d.bank),"MC canonical atlas GPU-source bank rejected");
   representative_.fill(mc_particle_attachments50MC::associationCount);
   std::size_t spanReferences=0;
   for(std::size_t i=0;i<mc_particle_attachments50MC::associationCount;++i){
    const auto k=attachments_->placementIndex(i);need(k<representative_.size(),"MC source placement index differs");
    const auto count=attachments_->spanCount(i);const auto* spans=attachments_->spanIndices(i);
    need(count&&spans&&attachments_->placement(i),"MC complete actual placement spans absent");
    if(representative_[k]==mc_particle_attachments50MC::associationCount){representative_[k]=i;spanReferences+=count;}
    else {const auto previous=representative_[k];
     need(attachments_->placement(previous)==attachments_->placement(i)&&attachments_->spanCount(previous)==count,
       "MC repeated association does not identify the same complete placement");
     for(std::size_t j=0;j<count;++j)need(attachments_->spanIndices(previous)[j]==spans[j],"MC repeated association span differs");}
    for(std::size_t j=0;j<count;++j)need(spans[j]<world_->spans()->size(),"MC placement span exceeds actual static world");
   }
   for(const auto i:representative_)need(i<mc_particle_attachments50MC::associationCount,"MC 2-placement coverage incomplete");
   need(spanReferences==mc_particle_attachments50MC::sourceSurfaceReferences&&
    attachments_->spanReferenceCount()==spanReferences,"MC 8-span coverage differs");
   completed_.fill(false);ready_=true;return true;
  }catch(const std::exception& e){try{d.message=e.what();}catch(...){}return false;}catch(...){return false;}
 }
 bool createGpu(const p::MaterialTextureGlRequest50EB& request,p::MaterialTextureGlDiagnostic50EB& d)noexcept{
  if(!ready()||material_->active||renderer_)return false;
  // Native one-level lowering of the checked canonical two-context sampler.
  if(request.samplerCount!=1||!request.samplers)return false;
  const auto& sampler=request.samplers[0];
  if(sampler.minification!=p::MaterialTextureFilter50EB::linear||sampler.magnification!=p::MaterialTextureFilter50EB::linear||
     sampler.s!=p::MaterialTextureWrap50EB::repeat||sampler.t!=p::MaterialTextureWrap50EB::repeat)return false;
  auto bounded=request;bounded.maximumGpuBytes=128u*1024u*4u;
  if(!gpu_.create(bounded,bank_,d))return false;context_=request.context;isCurrent_=request.isCurrent;return true;
 }
 template<class F> bool withMaterial(F&& submit,Diagnostic& d){
  if(!ready()||!renderer_||material_->active||!gpu_.validBorrow(context_,isCurrent_,gpu_.publicationToken())||
     !gpu_.matchesSourceBank(bank_,gpu_.publicationToken()))return false;
  a::OriginalStandardMaterialMapBindingView50LA selected;if(!map(selected,d))return false;
  material_->active=true;struct End{MaterialBorrow::State& s;~End(){s.active=false;}} end{*material_};
  MaterialBorrow borrow(material_,*this,generation_);return submit(borrow,gpu_);
 }
 bool close(p::MaterialTextureGlDiagnostic50EB& d)noexcept{
  if(!thread()||renderer_||renderOwner_||attemptActive_||(material_&&material_->active))return false;
  ready_=false;if(!gpu_.close(d))return false;
  if(material_){material_->active=false;material_->issuer=nullptr;material_->generation=0;}
  canonicalMaterial_=nullptr;material_.reset();domain_.reset();attachments_.reset();world_.reset();closed_=true;return true;
 }
};
} // namespace mc_particle_visual50MC
