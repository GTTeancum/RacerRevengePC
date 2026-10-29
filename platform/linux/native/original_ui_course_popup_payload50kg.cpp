#include "original_ui_course_popup_payload50kg.h"
#include "original_ui_course_owned_lifecycle50kg.h"
#include "../../../src/assets/original_course_popup_resource_proof50kg.h"
#include <new>
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <exception>
namespace rr::platform {
namespace {
constexpr std::array<std::size_t,7> quadIndexes{{3,4,5,6,0,1,2}};
constexpr std::array<const char*,7> quadNames{{"icon_x","icon_tri","icon_sqr","icon_cir","overlay1top","overlay1top","overlay1top"}};
constexpr std::array<std::uint32_t,7> quadCalls{{0x1FF058,0x1FF110,0x1FF1C8,0x1FF280,0x1FF338,0x1FF4AC,0x1FF68C}};
}
OriginalUiCoursePopupPayload50KG::~OriginalUiCoursePopupPayload50KG()noexcept {
 if(displayPinned_||constructedQuads_!=retiredQuads_||phase_==Phase::loading||phase_==Phase::loaded)std::terminate();
 for(const auto& row:clones_)if(row.constructed!=row.retired)std::terminate();
}
bool OriginalUiCoursePopupPayload50KG::stop(Diagnostic& d,std::uint32_t pc)noexcept {
 stopPC_=d.sourcePC=pc;d.sourceEffectsCommitted=phase_!=Phase::empty&&phase_!=Phase::bound;return false;
}
bool OriginalUiCoursePopupPayload50KG::bindNative(OriginalUiCourseOwnedLifecycle50KG& owner,C& entity,
 std::shared_ptr<Display> display,Diagnostic& d)noexcept {
 d={};if(thread_!=std::this_thread::get_id()||phase_!=Phase::empty||!owner.issuer_||!display||
  display->application_!=owner.issuer_||display->video_!=owner.video_||!display->frontendOwnerCurrent50KG()||!display->canRetain())return false;
 const auto& root=*owner.issuer_;
 const auto course=owner.coursePopupArchive();
 if(!root.standardMaterialDomainOwner_||!root.standardMaterialDomainOwner_->prepared()||
    !root.materialContextOwner_||!root.materialContextOwner_->live()||!root.materialApplication||
    root.materialContextOwner_->materialApplication()!=root.materialApplication||
    !root.materialContextOwner_->applicationAnimationClock().sameOwner(root.animationClock())||
    !root.standardMaterialDomainOwner_->materials382B38()->screenApplicationMatches(*root.materialApplication)||
    !course||!course->loaded()||
    course->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course))return false;
 try {occurrence_=std::make_shared<const int>(0);}catch(...){return false;}
 domain_=root.standardMaterialDomainOwner_;context_=root.materialContextOwner_;course_=course;
 owner_=&owner;root_=owner.issuer_;entity_=&entity;display_=std::move(display);display_->pin();displayPinned_=true;phase_=Phase::bound;return true;
}
bool OriginalUiCoursePopupPayload50KG::current(bool retiring)const noexcept {
 return thread_==std::this_thread::get_id()&&owner_&&root_&&entity_&&display_&&displayPinned_&&
  domain_&&domain_==root_->standardMaterialDomainOwner_&&domain_->prepared()&&
  context_&&context_==root_->materialContextOwner_&&context_->live()&&root_->materialApplication&&
  context_->materialApplication()==root_->materialApplication&&context_->applicationAnimationClock().sameOwner(root_->animationClock())&&
  domain_->materials382B38()->screenApplicationMatches(*root_->materialApplication)&&
  course_&&course_->loaded()&&course_->identity()==assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course)&&
  display_->application_==root_&&display_->video_==owner_->video_&&!display_->retiring_&&
  owner_->coursePopupOccurrenceCurrent(*this,*entity_,retiring);
}
bool OriginalUiCoursePopupPayload50KG::construct1FEC80AfterCount(Diagnostic& d)noexcept {
 d={};if(phase_!=Phase::bound||!current())return stop(d,0x1FEC80);
 vtable_=0x3D0990;
 constexpr std::array<std::uint32_t,7> offsets{{0x40,0xA0,0x100,0x160,0x1C0,0x220,0x280}};
 for(std::size_t i=0;i<7;++i){if(!quads_[i].initializeColdPrefix2DEF40(offsets[i],false))std::terminate();quads_[i].coursePrefixOnly_=true;++constructedQuads_;}
 ids3BC_.fill(-1);completed3A4_=false;timer398_=0;dynamicArrays_.fill(nullptr);dynamicCounts_.fill(0);animate3B0_=true;
 for(auto& row:clones_)row.sourceArray34C=nullptr; // Counts370 and manager394 remain unknown.
 phase_=Phase::constructed;d.sourceEffectsCommitted=true;d.sourcePC=0x1FEE38;return true;
}
bool OriginalUiCoursePopupPayload50KG::loadQuadRole(std::size_t role,Diagnostic& d)noexcept {
 using Provider=assets::OriginalStandardArchiveProvider50LA;
 if(role!=loadedRoles_||role>=7||!current()||phase_!=Phase::loading||!occurrence_)
  return stop(d,role<7?quadCalls[role]:0x1FF868);
 auto& q=quads_[quadIndexes[role]];const auto domain=domain_;
 q.domain_=domain;q.context_=context_;q.video_=owner_->video_;q.garage_=course_;
 try {q.primitiveOwner_=std::make_unique<Quad::Primitive>();}catch(...){q.material54=nullptr;q.stage_=Quad::Stage::primitivePublished;return stop(d,0x2DF114);}
 q.material54=q.primitiveOwner_.get();q.stage_=Quad::Stage::primitivePublished;auto& p=*q.material54;p.domain=domain;p.context=q.context_;
 const auto table=domain->materials382B38();const auto provider=domain->archiveProvider3E1A70();
 if(!provider||!provider->sealed())return stop(d,0x2465B0);
 try {
  std::string requested=root_->materialDirectory36B970_;if(requested.empty())return stop(d,0x299BE0);
  if(requested.back()!='\\')requested+='\\';requested+=quadNames[role];
  if(!Provider::sameName(requested,std::string("ui\\textures\\")+quadNames[role]))return stop(d,0x299BE0);
  std::int32_t row=-1;const bool found=table->find298C80(requested,row,d.quad.material);
  p.material00=row;p.materialStored=true;
  auto& guard=Text::guard8CD8();auto& assertion=Text::assertion443640();
  if(!guard.load(std::memory_order_acquire)){assertion.store(1,std::memory_order_relaxed);guard.store(1,std::memory_order_release);}
  if((assertion.load()&&row==-1)||!found){if(row>=0){failedRetains_[quadIndexes[role]]=&p;failedRetainRows_[quadIndexes[role]]=row;}return stop(d,0x299CF8);}
  if(!table->retainScreen14D590(row,d.quad.material)){failedRetains_[quadIndexes[role]]=&p;failedRetainRows_[quadIndexes[role]]=row;return stop(d,d.quad.material.sourcePC);}
  p.materialRetained=true;p.type0C=15;p.flags0F&=std::uint8_t(~1u);q.stage_=Quad::Stage::materialRetained;
  assets::OriginalStandardMaterialRowView50LA retained;if(!table->row(row,retained))return stop(d,0x298E60);
  std::vector<std::shared_ptr<const assets::OriginalStandardMaterialSource50LA>> sources;
  std::vector<assets::OriginalStandardTextureSourceInput50LA> textures;
  if(retained.object04){if(!table->preflightCourseLoadedWidget(row,q.context_,domain->textures383400(),d.quad.material))return stop(d,0x2DF144);}
  else {
   std::shared_ptr<assets::OriginalCoursePopupResourceProof50KG> popup(new assets::OriginalCoursePopupResourceProof50KG);
   popup->occurrence_=occurrence_;popup->role_=role;popup->row_=row;popup->primitive_=&p;popup->offset_=q.offset;popup->callerPC_=quadCalls[role];popup->course_=q.garage_;popup->provider_=provider;
   popup->context_=q.context_;popup->textures_=domain->textures383400();
   const auto psmName=requested+".psm";Provider::MemberView psm,pst;
   if(role<4){
    if(!popup->current(psmName,row,q.context_,domain->textures383400(),false))return stop(d,0x29B638);
    if(provider->lookup132820(requested+".pst",pst)!=Provider::Lookup::present||!pst.member||!pst.data)return stop(d,0x29B638);
    std::shared_ptr<assets::OriginalCourseScreenFallback50KG> proof(new assets::OriginalCourseScreenFallback50KG);
    proof->popup_=popup;proof->occurrence_=occurrence_;proof->row_=row;proof->name_=requested;proof->course_=q.garage_;
    proof->provider_=provider;proof->context_=q.context_;proof->textures_=domain->textures383400();
    if(!table->bindCourseScreenFallbackAfterRetain(row,proof,d.quad.material))return stop(d,d.quad.material.sourcePC);
   }else{
    if(!popup->current(psmName,row,q.context_,domain->textures383400(),true)||
       provider->lookup132820(psmName,psm)!=Provider::Lookup::present||!psm.member||!psm.data)return stop(d,0x2DF144);
    auto source=std::make_shared<assets::OriginalStandardMaterialSource50LA>();
    if(!source->prepare(*psm.member,std::vector<std::uint8_t>(psm.data,psm.data+psm.size),d.quad.material.document))return stop(d,0x2DF144);
    std::shared_ptr<assets::OriginalCourseScreenAuthored50KG> proof(new assets::OriginalCourseScreenAuthored50KG);
    proof->popup_=popup;proof->occurrence_=occurrence_;proof->row_=row;proof->source_=source;proof->course_=q.garage_;
    proof->provider_=provider;proof->context_=q.context_;proof->textures_=domain->textures383400();
    sources.push_back(source);
    if(!table->bindCourseScreenAuthoredAfterRetain(row,proof,d.quad.material))return stop(d,d.quad.material.sourcePC);
    if(provider->lookup132820("ui\\textures\\overlay1.pst",pst)!=Provider::Lookup::present||!pst.member||!pst.data)return stop(d,0x2DF144);
   }
   assets::OriginalUiEncodedArchiveView50KG encoded;
   if(!provider->encodedUiInput(pst.mount,encoded)||encoded.providerIdentity()!=provider.get()||!encoded.identity())return stop(d,0x2DF144);
   const auto* identity=assets::originalUiMemberIdentity50KG(encoded.identity()->id,pst.ordinal);
   if(!identity||identity->bytes!=pst.size||!Provider::sameName(identity->name,*pst.member)||pst.revision!=provider->revision())return stop(d,0x2DF144);
   textures.push_back({*pst.member,std::vector<std::uint8_t>(pst.data,pst.data+pst.size)});
  }
  if(!domain->appendOriginalSources(sources,textures,d.quad.material)||!domain->populateCourseResources2DF144(q.context_,d.quad.material))return stop(d,d.quad.material.sourcePC);
  q.stage_=Quad::Stage::populated;
  if(!q.completeZeroPosition2DF158(d.quad)){
   if(d.quad.sourcePC==0x2DF1CC&&q.stage_==Quad::Stage::dimensionsStored&&!q.packetOwner_){q.resource58=nullptr;return stop(d,0x2DF1E0);}
   return stop(d,d.quad.sourcePC);
  }
  if(std::fegetround()!=FE_TONEAREST||!q.resource58||!std::isfinite(q.scale4C)||!std::isfinite(q.scale50))return stop(d,quadCalls[role]+8);
  if(role<4){constexpr float x[]{514,427,305,305};if(q.position10[0]!=x[role]||q.position10[1]!=378){
    q.position10[0]=x[role];q.position10[1]=378;q.flags5CValue|=1;q.flags5CKnownMask|=1;
    if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}
  }}else{
   for(std::size_t axis=1;axis<3;++axis){const float v=axis==1?640.f:role==6?14.f:16.f;
    if(q.uv30[axis]!=v){q.uv30[axis]=v;q.flags5CValue|=1;q.flags5CKnownMask|=1;if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}}}
   if(role>4){volatile float sl=0.f*q.scale4C,sr=32.f*q.scale4C,tt=(role==5?32.f:18.f)*q.scale50,tb=(role==5?0.f:32.f)*q.scale50;
    q.resource58[12].words[0]=std::bit_cast<std::uint32_t>(float(sl));q.resource58[6].words[0]=std::bit_cast<std::uint32_t>(float(sl));
    q.resource58[15].words[0]=std::bit_cast<std::uint32_t>(float(sr));q.resource58[9].words[0]=std::bit_cast<std::uint32_t>(float(sr));
    q.resource58[9].words[1]=std::bit_cast<std::uint32_t>(float(tt));q.resource58[6].words[1]=std::bit_cast<std::uint32_t>(float(tt));
    q.resource58[15].words[1]=std::bit_cast<std::uint32_t>(float(tb));q.resource58[12].words[1]=std::bit_cast<std::uint32_t>(float(tb));}
   if(q.color44!=0xFFF2851Bu){for(std::size_t channel=0;channel<3;++channel){constexpr std::uint32_t rgb[]{121,66,13};for(auto vertex:{13,10,7,4})q.resource58[vertex].words[channel]=rgb[channel];}q.color44=0xFFF2851Bu;}
  }
  ++loadedRoles_;return true;
 }catch(...){return stop(d,0x2DF144);}
}
bool OriginalUiCoursePopupPayload50KG::ownsClone(const Text* text,std::size_t& row,std::size_t& line)const noexcept {
 row=9;line=10;
 if(!text||!current()||(phase_!=Phase::loading&&phase_!=Phase::loaded)||!templateIds328_)return false;
 for(std::size_t i=0;i<9;++i){const auto& r=clones_[i];if(!r.backing||!r.sourceArray34C||!*r.sourceArray34C||!r.sourceCount370)continue;
  if(*r.sourceCount370!=sourceLineCounts_[i]||*r.sourceArray34C!=r.backing[0].text)return false;
  for(std::size_t j=0;j<r.constructed;++j)if(r.backing[j].text==text&&!r.backing[j].retired){row=i;line=j;return true;}
 }
 return false;
}
bool OriginalUiCoursePopupPayload50KG::cloneInputs(const Text* text,std::int32_t& capacity,std::shared_ptr<const Font>& font)const noexcept {
 capacity=0;font.reset();std::size_t i=9,j=10;if(!ownsClone(text,i,j))return false;
 const auto& clone=clones_[i].backing[j];
 if(clone.capacity<=0||clone.capacity>=101||!clone.font||!root_->fonts328Owner_||
    root_->fonts328Owner_->find282210(assets::OriginalApplicationFonts50LA::Name::frontendDenmrk16)!=clone.font)return false;
 capacity=clone.capacity;font=clone.font;return true;
}
bool OriginalUiCoursePopupPayload50KG::loadClones2037D0(Diagnostic& d)noexcept {
 if(!current()||phase_!=Phase::loading||!templateIds328_||*templateIds328_!=sourceWidgetIds_||loadedRoles_!=7)return stop(d,0x2037D0);
 for(std::size_t i=0;i<9;++i){auto& row=clones_[i];SourceLine source;
  if(!owner_->coursePopupTextLine(sourceWidgetIds_[i],0,source)||!source.outer||!source.screen||
     source.row!=std::size_t(sourceWidgetIds_[i]-1)||source.lineOrdinal!=0||
     source.outer->lineCount168!=std::int32_t(sourceLineCounts_[i]))return stop(d,0x203824);
  row.sourceCount370=std::uint32_t(source.outer->lineCount168); //203828, BEFORE allocation.
  row.allocationAttempted=true;
  try {row.backing=std::make_unique<Clone[]>(*row.sourceCount370);}catch(...){
   row.sourceArray34C=nullptr;return stop(d,0x203878); //2E8990(NULL) returnsNULL,203864 publishes it.
  }
  // Native-width array storage represents the exact2E8990 element constructor
  // loop. Source callback is2E02F0, count*70+10 header allocation is20383C.
  for(std::size_t j=0;j<*row.sourceCount370;++j){auto& clone=row.backing[j];
   clone.text=new(clone.storage.data()) Text;++row.constructed;
  }
  row.sourceArray34C=row.backing[0].text; //203864, before first source line read.
  for(std::size_t j=0;j<*row.sourceCount370;++j){auto& clone=row.backing[j];SourceLine line;
   if(!owner_->coursePopupTextLine(sourceWidgetIds_[i],j,line)||!line.outer||!line.line||!line.screen||!line.font||
      line.row!=std::size_t(sourceWidgetIds_[i]-1)||line.lineOrdinal!=j||!line.screen->coldInputsCurrent()||
      !line.screen->string8_||line.screen->length5C_>line.screen->capacity5B_||line.screen->string8_[line.screen->length5C_]||
      line.font!=line.screen->font4C_||line.screen->fontName_!=assets::OriginalApplicationFonts50LA::Name::frontendDenmrk16||
      std::string_view(line.outer->materialE8.data())!="denmrk16Top")return stop(d,0x203878);
   const std::string_view original(line.screen->string8_.get(),line.screen->length5C_);
   if(original.find('\0')!=std::string_view::npos)return stop(d,0x203888);
   clone.capacity=std::int32_t(original.size()+1);clone.font=line.font; // Actual strlen+1, not source capacity.
   if(!clone.text->loadCoursePopupClone2DFD90(*this,d.text))return stop(d,d.text.sourcePC);
   auto& text=*clone.text;
   // Source2038D4 compares full string length before clipping. InitialNULL is
   // real, so this writes the actual borrowed source string, never a literal.
   if(!text.writeText29D990(original,d.text))return stop(d,d.text.sourcePC);
   volatile float step=float(j)*21.f;volatile float y=float(line.outer->y184)+float(step);
   const float x=float(line.outer->x180);
   if(text.vector10_[0]!=x||text.vector10_[1]!=float(y)){
    text.vector10_[0]=x;text.vector10_[1]=float(y);text.flags5A_|=1;text.flags5A_|=2;
   }
   if(text.justification50_!=line.outer->justify170){text.justification50_=line.outer->justify170;text.flags5A_|=1;text.flags5A_|=2;}
   if(text.color54_!=line.outer->color17C){
    const auto color=line.outer->color17C;
    for(std::size_t glyph=0;glyph<text.capacity5B_;++glyph)for(auto& vertex:text.glyphsC_[glyph].vertices){
     for(std::size_t alias=0;alias<vertex.aliases10;++alias){auto& rgba=vertex.rgba1C[alias]->words;
      rgba[0]=((color>>16)&255u)>>1;rgba[1]=((color>>8)&255u)>>1;rgba[2]=(color&255u)>>1;}
    }
    text.color54_=color;
   }
   clone.loaded=true;++row.completed; // Source borrows end here; no raw source pointer retained.
  }
 }
 flags34_|=3;flags34KnownMask_|=3; //203AF4: upper bits remain source-unknown.
 return true;
}
bool OriginalUiCoursePopupPayload50KG::load1FF020(C& manager,const std::array<std::int32_t,9>& ids,Diagnostic& d)noexcept {
 d={};if(!current()||phase_!=Phase::constructed||loadAttempted_||!owner_->coursePopupManagerCurrent(manager)||ids!=sourceWidgetIds_||std::fegetround()!=FE_TONEAREST)
  return stop(d,0x1FF020);
 loadAttempted_=true;phase_=Phase::loading;
 //141C20 is a genuine no-op. First Quad argument setup delay-store1FF05C.
 manager394_=&manager;manager394Known_=true;
 for(std::size_t role=0;role<7;++role)if(!loadQuadRole(role,d))return false;
 if(!current()||display_->localizationServiceActive_||display_->localizationServiceFaulted_||
    display_->dispatch_.pendingCallbackDC||display_->dispatch_.count54||display_->dispatch_.count60)return stop(d,0x1FF874);
 display_->sourceClearMode2C_=0;rendererStoreReached_=true; // No native rollback of source scalar.
 templateIds328_=ids; //1FF880 memcpy of the nine actual caller stack IDs.
 if(!loadClones2037D0(d))return false;
 phase_=Phase::loaded;d.sourcePC=0x1FF890;d.sourceEffectsCommitted=true;return true;
}
bool OriginalUiCoursePopupPayload50KG::preflightNativeAbort(Diagnostic& d)const noexcept {
 d={};if(phase_==Phase::nativeRetired)return true;
 if(thread_!=std::this_thread::get_id()||!owner_||!entity_||!display_||!displayPinned_)return false;
 if(phase_!=Phase::bound&&!current()&&!current(true))return false;
 if(constructedQuads_&&(!display_->memoryCardConstructorDrainCurrent50KG()||display_->dispatch_.pendingCallbackDC||
    display_->dispatch_.count54||display_->dispatch_.count60))return false;
 const auto table=domain_?domain_->materials382B38():nullptr;
 const auto totalReleases=[&](std::int32_t material)noexcept {
  std::uint64_t total=0;
  for(const auto& q:quads_)if(q.stage_!=Quad::Stage::retired&&q.material54&&q.material54->materialRetained&&q.material54->material00==material)++total;
  for(const auto& r:clones_)if(r.backing)for(std::size_t j=0;j<r.constructed;++j){const auto& c=r.backing[j];
   if(c.retired||!c.text)continue;const auto& t=*c.text;
   if(t.pendingPrimitive_&&t.pendingPrimitive_->materialRetained&&t.pendingPrimitive_->material00==material)++total;
   for(std::size_t n=0;n<t.publishedPrimitives_;++n)if(t.primitives4_&&t.primitives4_[n]&&
      t.primitives4_[n]->materialRetained&&t.primitives4_[n]->material00==material)++total;
  }
  return total;
 };
 const auto releaseValid=[&](std::int32_t material)noexcept {
  assets::OriginalStandardMaterialRowView50LA row;
  return table&&table->canRetireNativeCollector29C300()&&table->row(material,row)&&row.references08&&
   *row.references08>0&&std::uint64_t(*row.references08)>=totalReleases(material);
 };
 for(const auto& q:quads_)if(q.stage_!=Quad::Stage::retired&&q.material54&&q.material54->materialRetained&&!releaseValid(q.material54->material00))return false;
 for(const auto& r:clones_)if(r.backing)for(std::size_t j=0;j<r.constructed;++j){const auto& c=r.backing[j];
  if(c.retired||!c.text)continue;const auto& t=*c.text;
  if(t.pendingPrimitive_&&t.pendingPrimitive_->materialRetained&&!releaseValid(t.pendingPrimitive_->material00))return false;
  for(std::size_t n=0;n<t.publishedPrimitives_;++n)if(t.primitives4_&&t.primitives4_[n]&&
   t.primitives4_[n]->materialRetained&&!releaseValid(t.primitives4_[n]->material00))return false;
 }

 for(const auto& row:clones_)if(row.backing)for(std::size_t j=0;j<row.constructed;++j){const auto& c=row.backing[j];
  if(!c.retired&&c.text&&!c.text->preflightRetirement2E1340(d.text)){d.sourcePC=d.text.sourcePC;return false;}
 }
 for(std::size_t i=0;i<constructedQuads_;++i){const auto& q=quads_[i];if(q.stage_==Quad::Stage::retired)continue;
  if(failedRetains_[i]){const auto* p=q.primitiveOwner_.get();if(!p||p!=failedRetains_[i]||p!=q.material54||
    !p->materialStored||p->material00!=failedRetainRows_[i]||p->materialRetained||p->optional04||p->optional14||
    (p->flags0F&1u)||p->packet10||q.resource58||q.packetOwner_||q.nativeGarageFifoPins_||q.nativeMenuWarningPins_)return false;
  }else if(!q.preflightRetirement2DEFE0(d.quad)){d.sourcePC=d.quad.sourcePC;return false;}
 }
 return true;
}
bool OriginalUiCoursePopupPayload50KG::drainQuad(Quad& q,Diagnostic& d)noexcept {
 if(!display_->memoryCardConstructorDrainCurrent50KG())return stop(d,0x2DEFE0);
 if(!q.retirementDrainPhase_){std::int32_t actualV0=0;
  const auto result=game::serviceOriginalRenderer244D60(display_->dispatch_,true,display_->renderer_,d.renderer,actualV0,game::OriginalCameraDrainPolicy::initializedContext);
  if(result.status!=game::OriginalCameraStartStatus::complete)return stop(d,0x2DEFE0);q.retirementDrainPhase_=1;
 }
 if(q.retirementDrainPhase_==1){if(display_->dispatch_.pendingCallbackDC||display_->dispatch_.count54||display_->dispatch_.count60||
    !display_->renderer_.finishInitializedContextWork(d.renderer)||!d.renderer.contextWorkCompleted)return stop(d,0x2DF028);
  q.retirementDrainPhase_=2;
 }
 return q.retireResources2DF038(d.quad);
}
bool OriginalUiCoursePopupPayload50KG::nativeAbortResources(Diagnostic& d)noexcept {
 if(!preflightNativeAbort(d))return false;if(phase_==Phase::nativeRetired)return true;
 phase_=Phase::nativeAborting;
 // Dynamic arrays are sourceNULL (no input/update has been admitted).
 // Static rows retire in actual source row order, each array's elements reverse.
 for(auto& row:clones_){if(row.backing){while(row.retired<row.constructed){auto& clone=row.backing[row.constructed-1-row.retired];
    if(!clone.text->retire2E1340(d.text))return stop(d,d.text.sourcePC);
    clone.text->~Text();clone.retired=true;clone.font.reset();++row.retired;
   }
   row.backing.reset();row.arrayFreed=true; // Preserve reached source34C as comparison-only history.
  }
  // Native abort does not invent the later source1FEE9C count-clear effect.
 }
 while(retiredQuads_<constructedQuads_){const auto i=constructedQuads_-1-retiredQuads_;auto& q=quads_[i];
  if(failedRetains_[i]){q.primitiveOwner_.reset();q.constructed=false;q.stage_=Quad::Stage::retired;failedRetains_[i]=nullptr;}
  else if(!drainQuad(q,d))return false;
  ++retiredQuads_;
 }
 occurrence_.reset();if(displayPinned_){display_->unpin();displayPinned_=false;}display_.reset();phase_=Phase::nativeRetired;
 d.sourcePC=0;return true; // Native resource retirement only; parent still owns Entity count.
}
}
