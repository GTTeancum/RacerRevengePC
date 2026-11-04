#include "original_ui_controller_popup_resources50kg.h"
#include "original_ui_shared_dialog_material_history50kg.h"
#include "original_ui_shared_dialogs50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "original_ui_options_draw_resources50kg.h"
#include "original_ui_main_draw50kg.h"
#include "original_ui_rider_draw_resources50kg.h"
#include "../../../src/game/original_ui_controller_popup_geometry50kg.h"
#include <limits>
#include <utility>
namespace rr::platform {
namespace {
using E=OriginalUiControllerPopupResourceError50KG;
using D=OriginalUiControllerPopupResourceDiagnostic50KG;
bool fail(D& d,E e,std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
constexpr const char* names[]={"ui/textures/dimpopup","ui/textures/overlay1popup","font/venus14popup","ui/textures/icon_x_popup","ui/textures/icon_tri_popup"};
bool sameRuntime(const assets::OriginalUiMaterialRuntime50KG& a,const assets::OriginalUiMaterialRuntime50KG& b) noexcept {
 return a.overrideKnown==b.overrideKnown&&a.override435670==b.override435670&&
 a.callbackOverrideKnown==b.callbackOverrideKnown&&a.callbackOverride435678==b.callbackOverride435678;
}
bool sameVideo(const OriginalUiOptionsVideoOwner50KG& a,const OriginalUiOptionsVideoOwner50KG& b) noexcept {
 return a.lifetime&&a.lifetime.get()==b.lifetime.get()&&!a.lifetime.owner_before(b.lifetime)&&!b.lifetime.owner_before(a.lifetime)&&
 a.width==b.width&&a.height==b.height&&a.colorBits==b.colorBits&&a.packetZMaximum==b.packetZMaximum&&a.zBufferFormat==b.zBufferFormat;
}
}
struct OriginalUiControllerPopupResources50KG::Storage {
 std::shared_ptr<const assets::OriginalUiArchive50KG> appStart,popup,fonts;
 std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;
 OriginalUiOptionsVideoOwner50KG video;
 assets::OriginalUiMaterialRuntime50KG runtime;
 std::array<assets::OriginalUiMaterialEquivalenceKey50KG,5> keys;
 std::array<assets::OriginalUiMaterialBindingState50KG,5> history;
 // Null preserves the existing five-role owner. Once transferred, those five
 // history records are empty and the legacy acquire/copy/commit path is shut.
 std::shared_ptr<OriginalUiSharedDialogMaterialHistory50KG> sharedHistory;
 game::OriginalUiControllerPopupGeometry50KG geometry;
 const OriginalUiSourceOwner50KM* lease=nullptr;
 std::uint64_t revision=0;
 bool poisoned=false,retiring=false;
 const OriginalUiSharedDialogs50KG* sharedOwner=nullptr;
 std::size_t retiredSharedParts=0;
};
OriginalUiControllerPopupResources50KG::OriginalUiControllerPopupResources50KG() noexcept=default;
OriginalUiControllerPopupResources50KG::~OriginalUiControllerPopupResources50KG(){
 if(storage_&&storage_->sharedOwner)std::terminate();
}
bool OriginalUiControllerPopupResources50KG::sharedDialogOwnerCurrent(const OriginalUiSharedDialogs50KG& owner)const noexcept {
 const auto& in=owner.inputs_;return storage_&&!storage_->poisoned&&!storage_->retiring&&
  (!storage_->sharedOwner||storage_->sharedOwner==&owner)&&in.video&&in.popup&&
  storage_->popup==in.popup&&storage_->video.lifetime.get()==in.video.get()&&
  !storage_->video.lifetime.owner_before(in.video)&&!in.video.owner_before(storage_->video.lifetime)&&
  storage_->video.width==in.video->width&&storage_->video.height==in.video->height&&
  storage_->video.colorBits==in.video->colorBits&&storage_->video.packetZMaximum==in.video->packetZMaximum&&
  storage_->video.zBufferFormat==in.video->zBufferFormat;
}
bool OriginalUiControllerPopupResources50KG::bindSharedDialogOwner2055B0(OriginalUiSharedDialogs50KG& owner)noexcept {
 if(!sharedDialogOwnerCurrent(owner))return false;storage_->sharedOwner=&owner;return true;
}
bool OriginalUiControllerPopupResources50KG::sharedDialogBoundCurrent(const OriginalUiSharedDialogs50KG& owner)const noexcept {
 return sharedDialogOwnerCurrent(owner)&&storage_->sharedOwner==&owner;
}
bool OriginalUiControllerPopupResources50KG::preflightSharedRetirement20DF80(const OriginalUiSharedDialogs50KG& owner)const noexcept {
 return storage_&&storage_->sharedOwner==&owner&&!storage_->lease&&
  (storage_->sharedHistory?storage_->sharedHistory->preflight47CRetirement(owner):!storage_->poisoned)&&
  owner.controller47C_.resources.get()==this&&owner.nativeMaterialRetirementCurrent();
}
bool OriginalUiControllerPopupResources50KG::transferInitialHistoryTo(const OriginalUiSharedDialogs50KG& owner,
 const std::shared_ptr<OriginalUiSharedDialogMaterialHistory50KG>& target,D& d)noexcept {
 d={};if(!storage_||!target||storage_->sharedHistory||storage_->sharedOwner!=&owner||
  storage_->lease||storage_->revision||storage_->poisoned||storage_->retiring||storage_->retiredSharedParts||
  !sharedDialogBoundCurrent(owner)||owner.controller47C_.resources.get()!=this||
  !target->source47CTransferCurrent(owner,*this))return fail(d,E::history,0x2055B0);
 OriginalUiOptionsColdInput50KG original;original.appStart=storage_->appStart;original.controllerPopup=storage_->popup;
 original.fonts=storage_->fonts;original.video=storage_->video;original.runtime=storage_->runtime;
 if(!target->preparedSourcesCurrent(original))return fail(d,E::identity,0x2476A0);
 if(!target->takeColdFive(owner,*this,storage_->keys,storage_->history,d))return false;
 // Shared-pointer retention cannot allocate or fail after the exact record
 // transfer. The genuine SharedDialogs bootstrap installs its second pin next.
 storage_->sharedHistory=target;return true;
}
bool OriginalUiControllerPopupResources50KG::retireSharedPayloadPart20DF80(OriginalUiSharedDialogs50KG& owner,
 std::size_t step,D& d)noexcept {
 d={};if(step>=9||!preflightSharedRetirement20DF80(owner)||storage_->retiredSharedParts!=step||
  !owner.controllerRetirementCurrent())return fail(d,E::busy,0x20DFAC);
 auto& g=storage_->geometry;
 if(!step){storage_->retiring=true;g.prepared_=false;g.packets_={};g.packetCount_=0;}
 if(step<3){for(auto& variant:g.textVariants_){auto& text=variant[2-step];std::string{}.swap(text.text);text.glyphs.atlas=nullptr;text.glyphs.count=0;}}
 else {
  // These are fixed native arrays, not nine guest allocation placeholders.
  // Actual packet/atlas borrows were unpublished at ref0. The caller has
  // completed both real renderer drains for THIS Dim/Quad before this step.
  if(owner.retirementDrainPhase_!=2)return fail(d,E::busy,step==3?0x2E19CC:0x2DF030);
 }
 ++storage_->retiredSharedParts;return true;
}
bool OriginalUiControllerPopupResources50KG::finishSharedPayload20E008(OriginalUiSharedDialogs50KG& owner)noexcept {
 if(!preflightSharedRetirement20DF80(owner)||storage_->retiredSharedParts!=9||!storage_->retiring||
  !owner.controllerRetirementCurrent())return false;
 // All three native text strings/atlas borrows and all six native Quad/Dim
 // packet uses are gone. Release this real bank/Video/archive ownership last.
 // Other legitimate image-bank owners keep their own references unchanged.
 const auto sharedHistory=storage_->sharedHistory;
 storage_.reset();
 return !sharedHistory||sharedHistory->finish47CPayload(owner,*this);
}
bool OriginalUiControllerPopupResources50KG::loaded()const noexcept{return storage_&&!storage_->retiring&&(!storage_->sharedOwner||
 storage_->sharedOwner->phase_==OriginalUiSharedDialogs50KG::Phase::loaded);}
bool OriginalUiControllerPopupResources50KG::poisoned()const noexcept{return storage_&&storage_->poisoned;}
const game::OriginalUiControllerPopupGeometry50KG* OriginalUiControllerPopupResources50KG::geometry()const noexcept{return loaded()?&storage_->geometry:nullptr;}
std::shared_ptr<OriginalUiSharedDialogMaterialHistory50KG> OriginalUiControllerPopupResources50KG::sharedHistory()const noexcept {
 return storage_?storage_->sharedHistory:nullptr;
}
bool OriginalUiControllerPopupResources50KG::prepareOrRejoin(const OriginalUiOptionsColdInput50KG& in,
 std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,const std::vector<OriginalUiOptionsMaterialBinding50KG>& bindings,D& d)noexcept{
 d={};if(!in.english||!in.english->loaded()||!images||!images->loaded()||!in.appStart||!in.controllerPopup||!in.fonts)
  return fail(d,E::input,0x2055B0);
 if(in.appStart->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::appStart)||
 in.controllerPopup->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::controllerPopup)||
 in.fonts->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::fonts))return fail(d,E::identity,0x2055B0);
 // Prospective banks may validate immutable construction while another manager
 // leases the owner. They cannot copy histories or acquire until retirement.
 if(storage_&&(storage_->poisoned||!loaded()))return fail(d,E::poisoned,0x1B8718);
 // A transferred owner cannot be "rejoined" through the old five-key API.
 // The accepted canonical/effective bridge must first replace that API.
 if(storage_&&storage_->sharedHistory)return storage_->sharedHistory->rejoinBank(in,std::move(images),bindings,d);
 if(storage_&&(!sameVideo(storage_->video,in.video)||!sameRuntime(storage_->runtime,in.runtime)))return fail(d,E::video,0x2476A0);
 try{
  std::array<assets::OriginalUiMaterialEquivalenceKey50KG,5> keys;
  std::array<assets::OriginalUiMaterialBindingState50KG,5> histories;
  std::array<std::uint32_t,5> imageIndices{};
  for(std::size_t i=0;i<registryTokens.size();++i){d.registryToken=registryTokens[i];
   const OriginalUiOptionsMaterialBinding50KG* binding=nullptr;
   for(const auto& candidate:bindings)if(candidate.registryToken==registryTokens[i]){if(binding)return fail(d,E::identity,0x29BEA0);binding=&candidate;}
   const auto authored=in.controllerPopup->find(std::string(names[i])+".psm");
   if(!binding||binding->registeredName!=names[i]||binding->registryIdentity!=in.appStart->find("AppStart/Mat_Stbl.txt").identity||
    !authored.identity||binding->material.identity!=authored.identity||binding->material.passes.size()!=1)
    return fail(d,E::identity,0x247110);
   const auto& pass=binding->material.passes[0];
   if(pass.identity!=authored.identity||pass.pass!=1||pass.provenance.archive!=in.controllerPopup->identity()||
    pass.provenance.origin!=assets::OriginalUiMaterialOrigin50KG::authoredPsm||pass.shaderPassGroup!="popup"||
    binding->images.size()!=(i?1u:0u))return fail(d,E::identity,0x247110);
   const auto* image=binding->images.empty()?nullptr:images->sourceProvenance(binding->images[0]);
   assets::OriginalUiMaterialEquivalenceDiagnostic50KG keyDetail;assets::OriginalUiMaterialBindingDiagnostic50KG historyDetail;
   if(!assets::prepareOriginalUiMaterialEquivalence50KG(pass,image,keys[i],keyDetail)||!histories[i].initialize(keys[i],historyDetail))
    return fail(d,E::material,keyDetail.sourcePC?keyDetail.sourcePC:historyDetail.sourcePC);
   if(storage_&&assets::compareOriginalUiMaterialEquivalence50KG(storage_->keys[i],keys[i])!=assets::OriginalUiMaterialEquivalence50KG::equivalent)
    return fail(d,E::history,0x249BA0);
   if(i)imageIndices[i]=binding->images[0];
  }
  if(storage_){d={};return true;}
  auto s=std::make_unique<Storage>();s->appStart=in.appStart;s->popup=in.controllerPopup;s->fonts=in.fonts;s->images=std::move(images);s->video=in.video;s->runtime=in.runtime;
  game::OriginalUiControllerPopupGeometryDiagnostic50KG geometryDetail;
  if(!s->geometry.prepare(*in.english,*s->images,imageIndices[2],imageIndices[3],imageIndices[4],geometryDetail))
   return fail(d,E::geometry,geometryDetail.sourcePC);
  s->keys=std::move(keys);s->history=std::move(histories);storage_=std::move(s);d={};return true;
 }catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiControllerPopupResources50KG::prepareOrRejoin(const OriginalUiMainColdInput50KG& in,
 std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,const std::vector<OriginalUiMainMaterialBinding50KG>& bindings,D& d) noexcept {
 // Adapt actual retained construction objects, not old bank indexes or history.
 try {
  OriginalUiOptionsColdInput50KG input;input.appStart=in.appStart;input.controllerPopup=in.controllerPopup;
  input.fonts=in.fonts;input.english=in.english;input.runtime=in.runtime;
  input.video={in.video.lifetime,in.video.width,in.video.height,in.video.colorBits,in.video.packetZMaximum,in.video.zBufferFormat,{}};
  std::vector<OriginalUiOptionsMaterialBinding50KG> mapped;
  for(const auto& b:bindings)for(auto token:OriginalUiSharedDialogMaterialHistory50KG::registryTokens)if(b.registryToken==token){
   OriginalUiOptionsMaterialBinding50KG copy;copy.registeredName=b.registeredName;copy.registryToken=b.registryToken;
   copy.registryIdentity=b.registryIdentity;copy.material=b.material;copy.images=b.images;mapped.push_back(std::move(copy));
  }
  return prepareOrRejoin(input,std::move(images),mapped,d);
 }catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiControllerPopupResources50KG::prepareOrRejoin(const OriginalUiRiderColdInput50KG& in,
 std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,const std::vector<OriginalUiRiderMaterialBinding50KG>& bindings,D& d) noexcept {
 d={};
 // Unlike the Main/Options construction path, Rider must bring the existing
 // root's exact47C, attached six-role history and same live SharedDialogs.
 // Never fall back to the legacy five-role initialization or a new owner.
 if(in.retainedControllerPopup.get()!=this||!loaded()||storage_->poisoned||!storage_->sharedOwner||
    !storage_->sharedHistory||!sharedDialogBoundCurrent(*storage_->sharedOwner))return fail(d,E::history,0x2055B0);
 if(!in.english||!in.english->loaded()||!images||!images->loaded())return fail(d,E::input,0x2055B0);
 try {
  // This is only a typed adapter to the existing immutable source checks.
  // Every source/Video/runtime field is copied from the actual Rider input;
  // no Options application, manager or runtime value is manufactured.
  OriginalUiOptionsColdInput50KG input;input.appStart=in.appStart;input.controllerPopup=in.controllerPopup;
  input.fonts=in.fonts;input.english=in.english;input.runtime=in.runtime;
  input.video={in.video.lifetime,in.video.width,in.video.height,in.video.colorBits,in.video.packetZMaximum,in.video.zBufferFormat,{}};
  constexpr std::size_t offsets[]={70804,70826,70853,70872,70891,70917};
  std::vector<OriginalUiOptionsMaterialBinding50KG> mapped;
  for(const auto& b:bindings)for(const auto token:OriginalUiSharedDialogMaterialHistory50KG::registryTokens)if(b.registryToken==token){
   d.registryToken=token;
   if(b.registryByteOffset!=offsets[token-3228])return fail(d,E::identity,0x298724);
   OriginalUiOptionsMaterialBinding50KG copy;copy.registeredName=b.registeredName;copy.registryToken=b.registryToken;
   copy.registryIdentity=b.registryIdentity;copy.material=b.material;copy.images=b.images;mapped.push_back(std::move(copy));
  }
  return storage_->sharedHistory->rejoinBank(input,std::move(images),mapped,d);
 }catch(...){return fail(d,E::allocation,0);}
}
bool OriginalUiControllerPopupResources50KG::project(const game::OriginalUiControllerPopupRender50KG& render,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::input,0x2064C0);if(storage_->poisoned)return fail(d,E::poisoned,0x2064C0);
 game::OriginalUiControllerPopupGeometryDiagnostic50KG detail;
 return storage_->geometry.project(render,detail)||fail(d,E::geometry,detail.sourcePC);
}
bool OriginalUiControllerPopupResources50KG::copyHistory(std::uint32_t token,const assets::OriginalUiMaterialEquivalenceKey50KG& key,
 assets::OriginalUiMaterialBindingState50KG& out,D& d)const noexcept{
 d={};d.registryToken=token;if(!loaded()||storage_->poisoned||storage_->lease)return fail(d,E::history,0x29BEA0);
 if(storage_->sharedHistory)return fail(d,E::canonicalBridgePending,0x249BA0);
 for(std::size_t i=0;i<registryTokens.size();++i)if(registryTokens[i]==token){
  if(assets::compareOriginalUiMaterialEquivalence50KG(storage_->keys[i],key)!=assets::OriginalUiMaterialEquivalence50KG::equivalent)return fail(d,E::history,0x249BA0);
  try{out=storage_->history[i];return true;}catch(...){return fail(d,E::allocation,0);}
 }return fail(d,E::identity,0x29BEA0);
}
bool OriginalUiControllerPopupResources50KG::acquire(const OriginalUiSourceOwner50KM* owner,std::uint64_t& revision)noexcept{
 if(!loaded()||!owner||storage_->poisoned||storage_->lease||storage_->sharedHistory)return false;storage_->lease=owner;revision=storage_->revision;return true;
}
void OriginalUiControllerPopupResources50KG::release(const OriginalUiSourceOwner50KM* owner,bool retired)noexcept{
 if(!storage_||storage_->lease!=owner)return;if(!retired)storage_->poisoned=true;storage_->lease=nullptr;
}
bool OriginalUiControllerPopupResources50KG::current(const OriginalUiSourceOwner50KM* owner,std::uint64_t revision)const noexcept{
 return loaded()&&!storage_->sharedHistory&&storage_->lease==owner&&!storage_->poisoned&&storage_->revision==revision;
}
bool OriginalUiControllerPopupResources50KG::commit(const OriginalUiSourceOwner50KM* owner,std::uint64_t& revision,
 std::array<assets::OriginalUiMaterialBindingState50KG,5>& staged)noexcept{
 if(!current(owner,revision)||revision==std::numeric_limits<std::uint64_t>::max())return false;
 for(const auto& state:staged)if(!state.initialized())return false;
 storage_->history.swap(staged);revision=++storage_->revision;return true;
}
void OriginalUiControllerPopupResources50KG::poison(const OriginalUiSourceOwner50KM* owner)noexcept{
 if(storage_&&storage_->lease==owner)storage_->poisoned=true;
}
}
