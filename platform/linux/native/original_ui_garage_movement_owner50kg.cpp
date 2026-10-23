#include "original_ui_garage_movement_owner50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include <exception>

namespace rr::platform {
namespace {
template<class A,class B>bool sameOwner(const std::shared_ptr<A>& a,const std::shared_ptr<B>& b)noexcept {
 return a&&b&&a.get()==b.get()&&!a.owner_before(b)&&!b.owner_before(a);
}
constexpr const char* mappingKeys[]{"Lpad_Left","Lpad_Right","Lpad_Up","Lpad_Down",
 "Rpad_Down","Rpad_Left","Rpad_Up","Rpad_Right","Start","Select","L1","R1","L2","R2","L3","R3"};
constexpr std::uint32_t readPC[]{0x2AB058,0x2AB070,0x2AB088,0x2AB0A0,
 0x2AB0B8,0x2AB0D0,0x2AB0E8,0x2AB100,0x2AB118,0x2AB130,0x2AB148,0x2AB160,
 0x2AB178,0x2AB190,0x2AB1A8,0x2AB1C0};
constexpr std::int32_t mapping(std::size_t i)noexcept{return i==0?-2:i==1?-1:0;}
}
OriginalUiGarageMovementOwner50KG::~OriginalUiGarageMovementOwner50KG()noexcept {
 if(constructed_&&phase_!=Phase::retired)std::terminate();
}
bool OriginalUiGarageMovementOwner50KG::fail(D& d,S status,std::uint32_t pc)noexcept {
 d.status=status;d.sourcePC=pc;
 if(phase_==Phase::loading)phase_=Phase::failed;
 return false;
}
bool OriginalUiGarageMovementOwner50KG::identityCurrent()const noexcept {
 if(ownerThread_!=std::this_thread::get_id()||!grant_||!grant_->current())return false;
 std::uint32_t pause=0;
 if(!lifetime_.readPause11C(pause)||!issuer_||!constructed_||phase_==Phase::retired||
    !sameOwner(authority_,issuer_->entities())||grant_->issuer()!=issuer_||
    !sameOwner(authority_,grant_->application())||!widget_)return false;
 // A retained payload observation also runs after actual manager publication.
 // Its source-changing callers still use the separate cold Scope predicates.
 return grant_->garageMovementIdentityCurrent(widget_,this,entity_);
}
bool OriginalUiGarageMovementOwner50KG::fieldsCurrent()const noexcept {
 if(!constructed_||!grant_||manager30_!=grant_->manager()||type34_!=(typeStored_?9u:0u)||
    selected38_||enableOnSelect3C_||disableOnSelect40_||enableOnDeselect44_||
    disableOnDeselect48_||field4C_||field58_||gridSize60_||loadStores_>17||wantsStores_>2||
    entity_.category28!=0xff||entity_.descendants24||entity_.firstChild18||
    (entity_.flags29&15u)!=(hidden_?0u:3u))return false;
 for(std::size_t i=0;i<mappings64_.size();++i)
  if(mappings64_[i]!=(loadStores_>i+1?mapping(i):0))return false;
 for(std::size_t i=0;i<wantsJoystick50_.size();++i)
  if(wantsJoystick50_[i]!=(wantsStores_>i?1:0))return false;
 return true;
}
bool OriginalUiGarageMovementOwner50KG::payloadCurrent()const noexcept {
 return identityCurrent()&&fieldsCurrent()&&phase_==Phase::loaded&&typeStored_&&loadStores_==17;
}
bool OriginalUiGarageMovementOwner50KG::construct2AAF40(const Application& root,
 std::shared_ptr<Grant> grant,const Widget* widget,D& d)noexcept {
 d={};
 // Grant.current begins with the actual immutable authority's thread guard;
 // do not inspect mutable root/Scope state before it succeeds.
 if(ownerThread_!=std::this_thread::get_id()||!grant||!grant->current())return fail(d,S::authority,0x2AAF40);
 if(phase_!=Phase::empty||constructed_||!widget||grant->issuer()!=&root||
    !sameOwner(root.entities(),grant->application()))return fail(d,S::phase,0x2AAF40);
 const auto lifetime=root.animationClock();std::uint32_t pause=0;
 if(!lifetime.readPause11C(pause))return fail(d,S::authority,0x2AAF40);
 const auto step=grant->constructGarageMovement2AAF40(widget,this,entity_);
 if(!step.okay())return fail(d,S::scope,step.sourcePC);
 issuer_=&root;authority_=root.entities();lifetime_=lifetime;grant_=std::move(grant);
 widget_=widget;constructed_=true;phase_=Phase::constructed;
 // Genuine141300 is already counted/enrolled.2A7760 borrows30 and performs
 // these stores before2AAF5C installs the concrete3D4000 dispatch identity.
 manager30_=grant_->manager();selected38_=0;type34_=0;enableOnSelect3C_=0;
 disableOnSelect40_=0;enableOnDeselect44_=0;disableOnDeselect48_=0;field4C_=0;
 wantsJoystick50_[0]=0;wantsJoystick50_[1]=0;field58_=0;
 gridSize60_=0; //2AAF60, before the eight two-word iterations.
 for(std::size_t i=0;i<16;i+=2){mappings64_[i]=0;mappings64_[i+1]=0;}
 return true;
}
bool OriginalUiGarageMovementOwner50KG::load2AB010(const Archive& archive,const Dat& dat,D& d)noexcept {
 d={};
 if(!identityCurrent()||!fieldsCurrent()||phase_!=Phase::constructed)return fail(d,S::phase,0x2AB010);
 std::uint32_t pc=0x2AB010;
 try {
  // Native bounded source-envelope admission, before any load publication.
  // The actual population owner retains these immutable resources; this
  // leaf borrows them only for this call, never a dangling parsed-DAT pointer.
  if(!archive.loaded()||archive.identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::garage))
   return fail(d,S::source,pc);
  const auto member=archive.find("UI/UIGarage/Widgets.Dat");
  if(!member.identity||dat.identity()!=member.identity)return fail(d,S::source,pc);
  const auto* row=dat.section("Widget 6");
  const auto* type=row?row->find("Type"):nullptr;
  if(!type||type->text!="Movement_Control")return fail(d,S::source,pc);
  if(!identityCurrent()||!fieldsCurrent())return fail(d,S::authority,pc);
  phase_=Phase::loading;
  type34_=9;typeStored_=true; //2AB020, before the redundant source manager store.
  manager30_=grant_->manager(); //2AB02C, no additional source reference.
  std::int32_t value=0;pc=0x2AB040;
  if(!assets::originalUiInteger50KG(*row,"GridSize",0,value,d.parse,pc)||value)
   return fail(d,S::source,pc);
  if(!identityCurrent()||!fieldsCurrent())return fail(d,S::authority,pc);
  gridSize60_=value;++loadStores_;
  for(std::size_t i=0;i<16;++i){
   pc=readPC[i];value=0;
   if(!assets::originalUiInteger50KG(*row,mappingKeys[i],0,value,d.parse,pc)||value!=mapping(i))
    return fail(d,S::source,pc);
   if(!identityCurrent()||!fieldsCurrent())return fail(d,S::authority,pc);
   mappings64_[i]=value;++loadStores_;
  }
  //No UIWidget+74 load or resource creation is reached by2AB010.
  phase_=Phase::loaded;d.sourcePC=0x2AB1D8;return true;
 }catch(...){return fail(d,S::allocation,pc);}
}
bool OriginalUiGarageMovementOwner50KG::storeWants1CD8F4(std::uint8_t controller,std::int32_t value,D& d)noexcept {
 d={};if(!payloadCurrent()||hidden_||rowStored_||attached_||controller!=wantsStores_||controller>=2)
  return fail(d,S::phase,0x1CD8F4);
 if(value!=1)return fail(d,S::source,0x1CD8F4);
 wantsJoystick50_[controller]=value;++wantsStores_;
 d.sourcePC=0x1CD8F4;return true;
}
bool OriginalUiGarageMovementOwner50KG::hide1CD908(D& d)noexcept {
 d={};if(!payloadCurrent()||wantsStores_!=2||hidden_||rowStored_||attached_)
  return fail(d,S::phase,0x1CD908);
 const auto step=grant_->hideGarageMovement1CD908(widget_,this,entity_);
 if(!step.okay())return fail(d,S::scope,step.sourcePC);
 hidden_=true;d.sourcePC=0x1CD908;return true;
}
bool OriginalUiGarageMovementOwner50KG::noteRowStored1CD918(D& d)noexcept {
 d={};if(!payloadCurrent()||!hidden_||rowStored_||attached_)return fail(d,S::phase,0x1CD918);
 const auto step=grant_->publishGarageMovementRow1CD918(widget_,this,entity_);
 if(!step.okay())return fail(d,S::scope,step.sourcePC);
 rowStored_=true;d.sourcePC=0x1CD918;return true;
}
bool OriginalUiGarageMovementOwner50KG::attach1CD9F8(D& d)noexcept {
 d={};if(!payloadCurrent()||!hidden_||!rowStored_||attached_)return fail(d,S::phase,0x1CD9F8);
 const auto step=grant_->attachGarageMovement1CD9F8(widget_,this,entity_);
 if(!step.okay())return fail(d,S::scope,step.sourcePC);
 attached_=true;d.sourcePC=0x1CD9F8;return true;
}
bool OriginalUiGarageMovementOwner50KG::preflightRetirement(D& d)const noexcept {
 d={};if(ownerThread_!=std::this_thread::get_id()){d.status=S::authority;d.sourcePC=0x2AAFA0;return false;}
 if(phase_==Phase::retired)return true;
 if(!constructed_)return phase_==Phase::empty;
 if(!identityCurrent()||!fieldsCurrent()||phase_==Phase::loading||phase_==Phase::retiring){
  d.status=S::phase;d.sourcePC=0x2AAFA0;return false;}
 const auto step=grant_->preflightGarageMovementRetirement(widget_,this,entity_);
 if(!step.okay()){d.status=S::scope;d.sourcePC=step.sourcePC;return false;}
 return true;
}
bool OriginalUiGarageMovementOwner50KG::retire2AAFA0(D& d)noexcept {
 if(!preflightRetirement(d))return false;
 if(phase_==Phase::retired||!constructed_)return true;
 auto step=grant_->beginGarageMovementRetirement(widget_,this,entity_);
 if(!step.okay())return fail(d,S::scope,step.sourcePC);
 phase_=Phase::retiring;
 //142590 repaired surviving links and104970 reached ref0.2AAFA0 has no
 //derived owned payload; it installs the base dispatch then1413E0 decrements
 //the source count. The caller frees native backing only after this suffix.
 //Neither dying source links nor manager30 are cleared or recomputed.
 step=grant_->finishGarageMovementRetirement(widget_,this,entity_);
 if(!step.okay())std::terminate();
 phase_=Phase::retired;d.sourcePC=0x1413E0;return true;
}
}
