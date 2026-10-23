#include "original_ui_garage_text_overlay50kg.h"
#include "original_ui_garage_widget1_owner50kg.h"
#include "original_ui_garage_renderer_lease50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <algorithm>
#include <cmath>
#include <exception>
#include <cstdio>
#include <limits>

namespace rr::platform {
namespace {
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
 return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
}
OriginalUiGarageTextOverlay50KG::~OriginalUiGarageTextOverlay50KG()noexcept {
 if(constructed_&&phase_!=Phase::retired)std::terminate();
}
bool OriginalUiGarageTextOverlay50KG::authorityThread(const std::shared_ptr<Authority>& a)noexcept {
 return Text::authorityThread(a); //Existing exact immutable authority accessor.
}
bool OriginalUiGarageTextOverlay50KG::fail(D& d,S s,std::uint32_t pc)noexcept {
 d.status=s;d.sourcePC=pc;
 if(phase_==Phase::loading)phase_=Phase::failed;
 return false;
}
bool OriginalUiGarageTextOverlay50KG::identityCurrent()const noexcept {
 if(ownerThread_!=std::this_thread::get_id()||!authorityThread(authority_))return false;
 std::uint32_t pause=0;
 if(!lifetime_.readPause11C(pause)||!issuer_||!sameOwner(authority_,issuer_->entities())||
    !grant_||!grant_->current()||grant_->issuer()!=issuer_||!sameOwner(authority_,grant_->application())||
    !constructed_||phase_==Phase::retired||!widget_||!outer_)return false;
 return grant_->garageTextOverlayIdentityCurrent(widget_,this,*outer_,entity_);
}
bool OriginalUiGarageTextOverlay50KG::payloadCurrent()const noexcept {
 return payloadOwnerAndResourcesCurrent()&&lineInputsCurrent();
}
bool OriginalUiGarageTextOverlay50KG::payloadOwnerAndResourcesCurrent()const noexcept {
 return identityCurrent()&&phase_==Phase::loaded&&screenText30_&&font34_&&
  screenText30_->phase_==Text::Phase::coldLoaded&&screenText30_->coldInputsCurrent()&&
  sameOwner(font34_,screenText30_->font4C_)&&scene8C_==nullptr;
}
bool OriginalUiGarageTextOverlay50KG::payloadCurrentAfterWidgetFields(
 const Widget& actualWidget,const C& actualOuter)const noexcept {
 // Preserve this leaf's complete independent identity/resource proof. The
 // Widget row caller supplied its exact owner and outer in the same readonly
 // pass; reject either mismatch before the unchanged local line-input scan.
 if(!payloadOwnerAndResourcesCurrent()||widget_!=&actualWidget||outer_!=&actualOuter)return false;
 bool dynamic=false;
 return actualWidget.lineInputsAfterFieldsCurrent(this,actualOuter,fontInput_,materialInput_,capacityInput_,dynamic)&&
  dynamic==dynamicCaller_;
}
bool OriginalUiGarageTextOverlay50KG::lineInputsCurrent()const noexcept {
 bool dynamic=false;
 return widget_&&outer_&&widget_->lineInputsCurrent(this,*outer_,fontInput_,materialInput_,capacityInput_,dynamic)&&
  dynamic==dynamicCaller_;
}
bool OriginalUiGarageTextOverlay50KG::construct29C850(const Application& root,
 std::shared_ptr<Grant> grant,const Widget* widget,C& outer,D& d)noexcept {
 d={};
 if(ownerThread_!=std::this_thread::get_id()||!authorityThread(root.entities()))return fail(d,S::authority,0x29C850);
 if(phase_!=Phase::empty||constructed_||!widget||!grant||!grant->current()||
    grant->issuer()!=&root||!sameOwner(root.entities(),grant->application()))return fail(d,S::phase,0x29C850);
 std::uint32_t pause=0;if(!root.animationClock().readPause11C(pause))return fail(d,S::authority,0x29C850);
 const auto step=grant->constructGarageTextOverlay29C850(widget,this,outer,entity_);
 if(!step.okay())return fail(d,S::scope,step.sourcePC);
 //The genuine141300 precedes all derived writes. These retained identities
 //are native safety, not another Entity/source reference.
 issuer_=&root;authority_=root.entities();lifetime_=root.animationClock();
 grant_=std::move(grant);widget_=widget;outer_=&outer;constructed_=true;phase_=Phase::constructed;
 //29C878 installs the concrete3D37F0 dispatch, represented by this typed owner.
 screenText30_.reset();font34_.reset();justification74_=3;
 enabled70_=0;blink7C_=0;timer78_=0;blinkOn80_=1;hold84_=-1;indefinite88_=0;scene8C_=nullptr;
 elapsed68_=0;alphaElapsed9C_=0;scaleElapsedAC_=0;rotationElapsedBC_=0;
 duration6C_=0;field98_=0;alphaDurationA0_=0;scaleDurationB0_=0;rotationDurationC0_=0;
 scaleC4_=1;scaleC8_=1;lockCC_=0;secondDelayD0_=-1;
 fieldsE0_[1]=0;fieldsE0_[2]=0;fieldsE0_[3]=0;fieldsE0_[0]=0;
 nameF0_[0]=0;name130_[0]=0;
 colorDirty180_=0;alphaDirty184_=0;scaleDirty188_=0;rotationDirty18C_=0;
 return true;
}
bool OriginalUiGarageTextOverlay50KG::load29CA40(std::shared_ptr<const Archive> fonts,
 std::shared_ptr<const Archive> garage,std::string_view fontName,std::string_view materialName,
 std::int32_t capacity,D& d)noexcept {
 d={};
 if(!identityCurrent()||phase_!=Phase::constructed)return fail(d,S::phase,0x29CA40);
 //Bind parameters to the actual retained manager row and published line slot.
 //The finite resources below do not authorize arbitrary caller-selected text.
 bool dynamic=false;
 if(!fonts||!garage||!widget_->lineInputsCurrent(this,*outer_,fontName,materialName,capacity,dynamic))
  return fail(d,S::source,0x29CA40);
 Fonts::Name fontId;
 std::string_view canonicalFont,canonicalMaterial;
 if(fontName=="denmrk16"&&materialName=="denmrk16Top"&&(capacity==6||capacity==21||capacity==36)&&!dynamic){
  fontId=Fonts::Name::frontendDenmrk16;canonicalFont="denmrk16";canonicalMaterial="denmrk16Top";
 }else if(fontName=="venus16"&&materialName=="venus16Top"&&capacity==14&&!dynamic){
  fontId=Fonts::Name::frontendVenus16;canonicalFont="venus16";canonicalMaterial="venus16Top";
 }else if(fontName=="denmrk16"&&materialName=="denmrk16"&&capacity==40&&dynamic){
  fontId=Fonts::Name::frontendDenmrk16;canonicalFont="denmrk16";canonicalMaterial="denmrk16";
 }else if(fontName=="denmrk14"&&materialName=="denmrk14"&&
    (capacity==12||capacity==9||capacity==8||capacity==7||capacity==6)&&!dynamic){
  fontId=Fonts::Name::frontendDenmrk14;canonicalFont="denmrk14";canonicalMaterial="denmrk14";
 }else return fail(d,S::source,0x29CA40);
 fontInput_=canonicalFont;materialInput_=canonicalMaterial;capacityInput_=capacity;dynamicCaller_=dynamic;
 phase_=Phase::loading;scene8C_=nullptr; //29CA58 before the scene-null branch.
 scaleC4_=1;scaleC8_=1; //29CAEC/F0.
 rectangle38_[0].store(502);rectangle38_[2].store(630);
 rectangle38_[1].store(315);rectangle38_[3].store(331);
 if(!issuer_->loadGarageTextFont29CB74(std::move(fonts),fontId,d.font))return fail(d,S::font,d.font.sourcePC);
 if(!identityCurrent()||!lineInputsCurrent())return fail(d,S::authority,0x29CB88);
 //Font registry cache lookup, NOT an extra numeric texture408 reference.
 const auto registry=issuer_->fonts328Owner_;
 if(!registry)return fail(d,S::font,0x29CB88);
 font34_=registry->find282210(fontId); //29CBAC.
 if(!font34_)return fail(d,S::font,0x29CBAC);
 try {screenText30_.reset(new Text);} //29CBB8 constructor,29CBC0 publication.
 catch(...){return fail(d,S::allocation,0x29CBA8);}
 if(!identityCurrent()||!lineInputsCurrent())return fail(d,S::authority,0x29CBD8);
 if(!screenText30_->loadCold2DFD90(*issuer_,issuer_->standardMaterialDomainOwner_,
    issuer_->materialContextOwner_,registry,font34_,std::move(garage),
    issuer_->materialDirectory36B970_,issuer_->fontDirectory36B420_,fontId,canonicalMaterial,capacity,d.payload))
  return fail(d,S::source,d.payload.sourcePC);
 if(!identityCurrent()||!lineInputsCurrent()||!screenText30_->coldInputsCurrent())return fail(d,S::authority,0x29CBE0);
 //29CBE8/29CC50 equality branches preserve the already-produced white/255
 //payload. Never recolor packets to the later Widget target here.
 auto& text=*screenText30_;
 if(text.color54_!=0xffffffffu||text.alpha58_!=255||text.scale40_!=1||text.angle44_!=0)
  return fail(d,S::source,0x29CBE0);
 const auto alpha=static_cast<std::int32_t>(static_cast<std::uint16_t>(text.alpha58_)&255u);
 alphaTarget94_.store(alpha);alphaFrom90_.store(alpha);secondAlphaD4_.store(alpha);
 scaleTargetA8_.store(text.scale40_);scaleFromA4_.store(text.scale40_);secondScaleD8_.store(text.scale40_);
 rotationTargetB4_.store(text.angle44_);rotationFromB8_.store(text.angle44_);secondRotationDC_.store(text.angle44_);
 //29CD78 calls genuine141C20: jr ra; return this. No update/reset/GL call.
 phase_=Phase::loaded;return true;
}
bool OriginalUiGarageTextOverlay50KG::attach2A9C64(D& d)noexcept {
 d={};if(!payloadCurrent()||dynamicCaller_)return fail(d,S::phase,0x2A9C64);
 return attachLine(0x2A9C64,d);
}
bool OriginalUiGarageTextOverlay50KG::attach2A9710(D& d)noexcept {
 d={};if(!payloadCurrent()||!dynamicCaller_)return fail(d,S::phase,0x2A9710);
 return attachLine(0x2A9710,d);
}
bool OriginalUiGarageTextOverlay50KG::attachLine(std::uint32_t pc,D& d)noexcept {
 if(attached_)return fail(d,S::phase,pc);
 const auto step=grant_->attachGarageTextOverlay2A9C64(widget_,this,*outer_,entity_);
 if(!step.okay())return fail(d,S::scope,step.sourcePC);
 attached_=true;d.sourcePC=pc;return true;
}
bool OriginalUiGarageTextOverlay50KG::setText29D940(std::string_view text,D& d)noexcept {
 d={};if(!payloadCurrent()){
  std::fprintf(stderr,"original_garage_text_setter_rejected stage=overlay_payload pc=2742592 phase=%u constructed=%u identity=%u resources=%u lines=%u screen=%u input_bytes=%zu\n",
   static_cast<unsigned>(phase_),unsigned(constructed_),unsigned(identityCurrent()),
   unsigned(payloadOwnerAndResourcesCurrent()),unsigned(lineInputsCurrent()),unsigned(bool(screenText30_)),text.size());
  return fail(d,S::phase,0x29D940);
 }
 if(lockCC_)return true; //Source lock gate precedes strlen/input inspection.
 if(!screenText30_->writeText29D990(text,d.payload))return fail(d,S::source,d.payload.sourcePC);
 return true;
}
bool OriginalUiGarageTextOverlay50KG::setAlpha29DBC0(std::int32_t alpha,float duration,D& d)noexcept {
 d={};if(!payloadCurrent())return fail(d,S::phase,0x29DBC0);
 if(lockCC_)return true;
 if(duration!=0)return fail(d,S::unsupported,0x29DBCC);
 alphaDurationA0_=duration;alphaElapsed9C_=0;alphaTarget94_.store(alpha);
 if(alphaTarget94_.value>255)alphaTarget94_.store(255);
 else if(alphaTarget94_.value<0)alphaTarget94_.store(0);
 alphaFrom90_.store(static_cast<std::uint16_t>(screenText30_->alpha58_)&255u);
 //Does not write current174, dirty184 or packet alpha.
 return true;
}
bool OriginalUiGarageTextOverlay50KG::setDepth29DC20(float depth,D& d)noexcept {
 d={};if(!payloadCurrent())return fail(d,S::phase,0x29DC20);
 if(!std::isfinite(depth))return fail(d,S::unsupported,0x29DC28);
 auto& t=*screenText30_;if(t.depth48_==depth)return true;
 t.depth48_=depth;t.flags5A_=std::uint8_t((t.flags5A_&~2u)|2u);return true;
}
bool OriginalUiGarageTextOverlay50KG::setJustification29DB30(std::int32_t justification,D& d)noexcept {
 d={};if(!payloadCurrent())return fail(d,S::phase,0x29DB30);
 if(lockCC_)return true;
 if(justification<0||justification>4)return fail(d,S::unsupported,0x29DB3C);
 justification74_=justification;return true;
}
bool OriginalUiGarageTextOverlay50KG::setColor29DA10(std::uint32_t color,D& d)noexcept {
 d={};if(!payloadCurrent())return fail(d,S::phase,0x29DA10);
 if(lockCC_)return true;
 colorDirty180_=1;color170_.store(color); //29DCB4 then29DCBC.
 return true;
}
bool OriginalUiGarageTextOverlay50KG::colorTargetCurrent(std::uint32_t color)const noexcept {
 // No recursive payload/Widget predicate here. The enclosing actual row
 // independently validates this leaf's live typed custody and resource owner.
 return phase_==Phase::loaded&&constructed_&&!lockCC_&&color170_.produced&&
  color170_.value==color&&colorDirty180_==1;
}
bool OriginalUiGarageTextOverlay50KG::fontLineHeight281730(float& out,D& d)const noexcept {
 d={};if(!payloadCurrent()){d.status=S::phase;d.sourcePC=0x281730;return false;}
 const auto* first=font34_->metrics().firstGlyph(); //281748..70 scans before281F90.
 const auto* texture=font34_->texture281F90();
 if(!first){out=-1;return true;}
 if(!texture||!texture->sourceHeight18()||*texture->sourceHeight18()<=0){d.status=S::font;d.sourcePC=0x281788;return false;}
 volatile float uvHeight=first->bottom-first->top;
 volatile float height=float(*texture->sourceHeight18())*uvHeight;
 if(!std::isfinite(height)){d.status=S::unsupported;d.sourcePC=0x2817AC;return false;}
 out=height;return true;
}
bool OriginalUiGarageTextOverlay50KG::setPosition29DB50(std::int32_t x,std::int32_t y,float duration,D& d)noexcept {
 d={};if(!payloadCurrent())return fail(d,S::phase,0x29DB50);
 if(lockCC_)return true;
 if(duration!=0)return fail(d,S::unsupported,0x29DB64);
 duration6C_=duration;elapsed68_=0;rectangle38_[8].store(x);rectangle38_[9].store(y);
 rectangle38_[4]=rectangle38_[0];rectangle38_[5]=rectangle38_[1];
 //29DBA8 memcpy16: source60/64 are not produced by29C850/29CA40.
 //Copy knownness, never read unknown native lanes or preserve old40/44.
 for(std::size_t i=0;i<4;++i){const auto& from=rectangle38_[8+i];
  if(from.produced)rectangle38_[i].store(from.value);else rectangle38_[i]=Known<std::int32_t>{};}
 return true;
}
bool OriginalUiGarageTextOverlay50KG::setup29DA40(const game::OriginalUiTextLineSetup50KG& s,D& d)noexcept {
 d={};if(!payloadCurrent())return fail(d,S::phase,0x29DA40);
 if(lockCC_)return true;
 //Reached static/dynamic cold setup. Unsupported animation is not settled early.
 if(!std::isfinite(s.holdDuration)||s.holdDuration>=0||s.blinkPeriod!=0||s.scale!=1||s.rotation!=0||
    s.secondPhaseDelay!=-1||s.alpha<std::numeric_limits<std::int16_t>::min()||
    s.alpha>std::numeric_limits<std::int16_t>::max())return fail(d,S::unsupported,0x29DA40);
 lockCC_=s.lock?1u:0u;enabled70_=1;timer78_=s.blinkPeriod;blink7C_=s.blinkPeriod;blinkOn80_=1;
 indefinite88_=1; //Negative hold deliberately leaves84 unchanged.
 alphaTarget94_.store(s.alpha);alpha174_.store(static_cast<float>(s.alpha));alphaDirty184_=1;
 scaleTargetA8_.store(s.scale);scale178_.store(s.scale);scaleDirty188_=1;
 rotationTargetB4_.store(s.rotation);rotation17C_.store(s.rotation);rotationDirty18C_=1;
 secondDelayD0_=s.secondPhaseDelay;
 secondAlphaD4_.store(s.alpha);secondScaleD8_.store(s.scale);secondRotationDC_.store(s.rotation);
 return true;
}
bool OriginalUiGarageTextOverlay50KG::setupStateCurrent(bool enabled)const noexcept {
 // Only the actual cold/entry setup words. No time advance, update or packet projection.
 return phase_==Phase::loaded&&constructed_&&!lockCC_&&enabled70_==(enabled?1u:0u)&&
  indefinite88_==(enabled?1u:0u)&&hold84_==-1&&
  timer78_==0&&blink7C_==0&&blinkOn80_==1&&
  alphaTarget94_.produced&&alphaTarget94_.value==255&&alpha174_.produced&&alpha174_.value==255&&alphaDirty184_==1&&
  scaleTargetA8_.produced&&scaleTargetA8_.value==1&&scale178_.produced&&scale178_.value==1&&scaleDirty188_==1&&
  rotationTargetB4_.produced&&rotationTargetB4_.value==0&&rotation17C_.produced&&rotation17C_.value==0&&rotationDirty18C_==1&&
  secondDelayD0_==-1&&secondAlphaD4_.produced&&secondAlphaD4_.value==255&&
  secondScaleD8_.produced&&secondScaleD8_.value==1&&secondRotationDC_.produced&&secondRotationDC_.value==0;
}
bool OriginalUiGarageTextOverlay50KG::retainedColorTargetCurrent(std::uint32_t color)const noexcept {
 if(updateStep_==UpdateStep::never)return colorTargetCurrent(color);
 return phase_==Phase::loaded&&constructed_&&!lockCC_&&color170_.produced&&
  color170_.value==color&&colorDirty180_<=1;
}
bool OriginalUiGarageTextOverlay50KG::retainedSetupStateCurrent(bool enabled)const noexcept {
 if(updateStep_==UpdateStep::never)return setupStateCurrent(enabled);
 // All writers remain private, source-ordered setters/update. The original
 // timer keeps decreasing with zero blink; it must NOT be reset to retain the
 // old constructor witness. Dirty words may be cleared by update or set by a
 // subsequent genuine setter. This is custody, not a frame/draw receipt.
 return phase_==Phase::loaded&&constructed_&&updatePublication_&&!lockCC_&&
  enabled70_==(enabled?1u:0u)&&indefinite88_==(enabled?1u:0u)&&hold84_==-1&&
  std::isfinite(timer78_)&&timer78_<=0&&blink7C_==0&&blinkOn80_==1&&
  alphaTarget94_.produced&&alphaTarget94_.value==255&&alpha174_.produced&&alpha174_.value==255&&alphaDirty184_<=1&&
  scaleTargetA8_.produced&&scaleTargetA8_.value==1&&scale178_.produced&&scale178_.value==1&&scaleDirty188_<=1&&
  rotationTargetB4_.produced&&rotationTargetB4_.value==0&&rotation17C_.produced&&rotation17C_.value==0&&rotationDirty18C_<=1&&
  secondDelayD0_==-1&&secondAlphaD4_.produced&&secondAlphaD4_.value==255&&
  secondScaleD8_.produced&&secondScaleD8_.value==1&&secondRotationDC_.produced&&secondRotationDC_.value==0;
}
bool OriginalUiGarageTextOverlay50KG::update29CDA0(OriginalUiGarageRendererLease50KG& renderer,
 float dt,std::uint64_t serial,D& d)noexcept {
 d={};
 if(updateFailed_)return fail(d,S::unsupported,updateFailurePC_);
 if(!payloadCurrent()||!renderer.currentFor(issuer_)||!serial||serial<=updatePublication_||
    !std::isfinite(dt)||dt<0)return fail(d,S::phase,0x29CDA0);
 // Source Entity dispatch has already established actual ancestry/eligibility
 // and serviced244D60. A direct child with callbacks is not this typed leaf.
 if(!attached_||!entity_.parent1C||(entity_.flags29&0x0du)!=1u||!(entity_.mask20&2u)||
    entity_.firstChild18||entity_.descendants24)return fail(d,S::scope,0x29CDA0);
 updatePublication_=serial;
 colorDirty180_=0;alphaDirty184_=0;scaleDirty188_=0;rotationDirty18C_=0;
 updateStep_=UpdateStep::dirtyCleared; //29CDB8..29CDC4 precede viewport reads.
 auto stop=[&](std::uint32_t pc)noexcept {
  updateFailed_=true;updateFailurePC_=pc;return fail(d,S::unsupported,pc);
 };
 std::array<std::int32_t,4> viewport{};std::array<std::int32_t,2> video{};std::uint32_t pc=0;
 if(scene8C_||!renderer.textViewport1159D0(viewport,video,pc))return stop(scene8C_?0x29CDDC:pc);
 volatile float xRatio=float(video[0])/float(viewport[2]-viewport[0]);
 volatile float yRatio=float(video[1])/float(viewport[3]-viewport[1]);
 if(!std::isfinite(xRatio)||!std::isfinite(yRatio))return stop(0x29CE38);
 if(scaleC4_!=xRatio)scaleC4_=xRatio; //29CE64
 if(scaleC8_!=yRatio)scaleC8_=yRatio; //29CE7C
 updateStep_=UpdateStep::viewportRead;
 // Canonical reached authored indefinite branch only. Do not settle a timed
 // hold, blink or secondary animation as though its callbacks had run.
 if(enabled70_){
  if(hold84_!=-1||indefinite88_!=1)return stop(0x29CE88);
  volatile float remaining=timer78_-dt;timer78_=remaining; //29CF2C, including7C=0.
  if(blink7C_!=0||blinkOn80_!=1)return stop(0x29CF30);
 }else lockCC_=0; //29CF64
 // In the authored zero-duration branch,29DB50 has already copied58/5C to
 //38/3C. Unknown60/64 lanes remain unknown and are never read by this test.
 if(!rectangle38_[0].produced||!rectangle38_[1].produced||!rectangle38_[8].produced||
    !rectangle38_[9].produced||rectangle38_[0].value!=rectangle38_[8].value||
    rectangle38_[1].value!=rectangle38_[9].value)return stop(0x29CF7C);
 const auto& text=*screenText30_;
 if(!alphaTarget94_.produced||alphaDurationA0_!=0||!scaleTargetA8_.produced||
    scaleDurationB0_!=0||!rotationTargetB4_.produced||rotationDurationC0_!=0)return stop(0x29D080);
 if(alphaTarget94_.value!=static_cast<int>(static_cast<std::uint16_t>(text.alpha58_)&255u)){
  alphaDirty184_=1;alpha174_.store(float(alphaTarget94_.value)); //29DCC0
 }
 if(scaleTargetA8_.value!=text.scale40_){scaleDirty188_=1;scale178_.store(scaleTargetA8_.value);}
 if(rotationTargetB4_.value!=text.angle44_){rotationDirty18C_=1;rotation17C_.store(rotationTargetB4_.value);}
 if(fieldsE0_[0])return stop(0x29D1F4);
 if(secondDelayD0_!=-1)return stop(0x29D33C);
 updateStep_=UpdateStep::returned;d.sourcePC=0x29D3E4;return true;
}
bool OriginalUiGarageTextOverlay50KG::draw29D3F0(OriginalUiGarageDrawContribution50KG& draw,D& d)noexcept {
 d={};
 if(!payloadCurrent()||!draw.textInvocation29D3F0Current(*this))return fail(d,S::authority,0x29D3F0);
 OriginalUiGarageDrawContribution50KG::TextRenderInputs input;std::uint32_t pc=0;
 if(!draw.readTextInputs29D3F0(*this,input,pc))return fail(d,S::source,pc);
 if(drawPublication_==input.publication||drawPending_)return fail(d,S::phase,0x29D3F0);
 drawPublication_=input.publication;drawPending_=true;
 if(scene8C_)return fail(d,S::unsupported,0x29D438);
 if(enabled70_&&blinkOn80_){
  if(!color170_.produced||!alpha174_.produced||!scale178_.produced||!rotation17C_.produced||
     !rectangle38_[0].produced||!rectangle38_[1].produced||!std::isfinite(alpha174_.value)||
     alpha174_.value<float(std::numeric_limits<std::int16_t>::min())||
     alpha174_.value>float(std::numeric_limits<std::int16_t>::max()))return fail(d,S::source,0x29D690);
  const auto alpha=static_cast<std::int16_t>(alpha174_.value);
  if(!screenText30_->applyOverlay29D690(color170_.value,alpha,scale178_.value,rotation17C_.value,
       rectangle38_[0].value,rectangle38_[1].value,justification74_,d.payload)||
     !screenText30_->draw2E0170(draw,*this,d.payload))return fail(d,S::source,d.payload.sourcePC);
  if(!draw.restoreTextState29D8E8(*this,14,1,0,pc)||
     !draw.restoreTextState29D8E8(*this,7,1,0,pc))return fail(d,S::source,pc);
 }
 // Actual source tail141D70 is dispatched by the retained enclosing walker.
 // These concrete authored TextOverlay instances are genuine childless leaves.
 if(entity_.firstChild18)return fail(d,S::unsupported,0x141D70);
 drawPending_=false;d.sourcePC=0x29D930;return true;
}
bool OriginalUiGarageTextOverlay50KG::stop29DB10(D& d)noexcept {
 d={};if(!payloadCurrent())return fail(d,S::phase,0x29DB10);
 // This source function has NO lock gate: unlike29DA10 it also clears CC.
 enabled70_=0;indefinite88_=0;hold84_=-1;lockCC_=0;d.sourcePC=0x29DB24;return true;
}
bool OriginalUiGarageTextOverlay50KG::preflightRetirement(D& d)const noexcept {
 d={};
 if(ownerThread_!=std::this_thread::get_id()){d.status=S::authority;d.sourcePC=0x29C9C0;return false;}
 if(phase_==Phase::retired)return true;
 if(!constructed_)return phase_==Phase::empty;
 if(!identityCurrent()||phase_==Phase::loading||phase_==Phase::retiring){d.status=S::phase;d.sourcePC=0x29C9C0;return false;}
 const auto step=grant_->preflightGarageTextOverlayRetirement(widget_,this,*outer_,entity_);
 if(!step.okay()){d.status=S::scope;d.sourcePC=step.sourcePC;return false;}
 if(screenText30_&&!screenText30_->preflightRetirement2E1340(d.payload)){
  d.status=S::payload;d.sourcePC=d.payload.sourcePC;return false;}
 return true;
}
bool OriginalUiGarageTextOverlay50KG::preflightSharedRetirement(
 std::span<const OriginalUiGarageTextOverlay50KG* const> owners,D& d)noexcept {
 d={};
 // The manager supplies its actual retained line owners, not ref totals. This
 // no-allocation native envelope precedes the FIRST source child retirement.
 for(std::size_t i=0;i<owners.size();++i){
  if(!owners[i]){d.status=S::authority;d.sourcePC=0x29C9C0;return false;}
  for(std::size_t j=0;j<i;++j)if(owners[j]==owners[i]){
   d.status=S::authority;d.sourcePC=0x29C9C0;return false;
  }
  const auto& owner=*owners[i];
  if(owner.ownerThread_!=std::this_thread::get_id()){
   d.status=S::authority;d.sourcePC=0x29C9C0;return false;
  }
  if(owner.phase_==Phase::retired)continue;
  if(!owner.constructed_){
   if(owner.phase_==Phase::empty)continue;
   d.status=S::phase;d.sourcePC=0x29C9C0;return false;
  }
  // Do not require every sibling to be the present Entity tail. Exact tail,
  // ref and link retirement validation remains at each actual source step.
  if(!owner.identityCurrent()||owner.phase_==Phase::loading||owner.phase_==Phase::retiring){
   d.status=S::phase;d.sourcePC=0x29C9C0;return false;
  }
  if(owner.screenText30_&&!owner.screenText30_->preflightRetirement2E1340(d.payload)){
   d.status=S::payload;d.sourcePC=d.payload.sourcePC;return false;
  }
 }
 for(const auto* owner:owners){
  const auto* text=owner->screenText30_.get();if(!text)continue;
  for(std::size_t p=0;p<=text->publishedPrimitives_;++p){
   const auto* primitive=p==text->publishedPrimitives_?text->pendingPrimitive_.get():text->primitives4_[p].get();
   if(!primitive||!primitive->materialRetained)continue;
   const auto table=text->domain_->materials382B38();
   std::size_t releases=0;
   for(const auto* sibling:owners){
    const auto* other=sibling->screenText30_.get();
    if(!other||!other->domain_||!sameOwner(table,other->domain_->materials382B38()))continue;
    const auto add=[&](const Text::Primitive* value)noexcept {
     if(value&&value->materialRetained&&value->material00==primitive->material00)++releases;
    };
    add(other->pendingPrimitive_.get());
    for(std::size_t q=0;q<other->publishedPrimitives_;++q)add(other->primitives4_[q].get());
   }
   assets::OriginalStandardMaterialRowView50LA row;
   if(!table->row(primitive->material00,row)||!row.references08||*row.references08<=0||
      static_cast<std::uint64_t>(*row.references08)<releases){
    d.status=S::payload;d.sourcePC=0x14D500;
    d.payload.status=OriginalUiGarageScreenTextStatus50KG::phase;d.payload.sourcePC=0x14D500;return false;
   }
  }
 }
 return true;
}
bool OriginalUiGarageTextOverlay50KG::retire29C9C0(D& d)noexcept {
 if(!preflightRetirement(d))return false;
 if(phase_==Phase::retired||!constructed_)return true;
 auto step=grant_->beginGarageTextOverlayRetirement(widget_,this,*outer_,entity_);
 if(!step.okay())return fail(d,S::scope,step.sourcePC);
 phase_=Phase::retiring;
 if(screenText30_){if(!screenText30_->retire2E1340(d.payload))std::terminate();screenText30_.reset();}
 //Font34 is a borrowed source pointer: dropping native lifetime retention
 //does not perform another source texture decrement or claim a source clear.
 font34_.reset();
 step=grant_->finishGarageTextOverlayRetirement(widget_,this,*outer_,entity_);
 if(!step.okay())std::terminate();
 attached_=false;phase_=Phase::retired;d.sourcePC=0x29CA00;return true;
}
}
