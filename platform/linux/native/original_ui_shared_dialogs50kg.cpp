#include "original_ui_shared_dialogs50kg.h"
#include "original_ui_main_session50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
#include "../../../tests/native/original_race_display_lifetime50la.h"
#include <exception>
#include <cmath>
#include <bit>
#include <limits>
#include <new>
#include <cstring>
#include <cstdio>
namespace rr::platform {
OriginalUiSharedDialogs50KG::~OriginalUiSharedDialogs50KG()noexcept {
    // Global UI shutdown must perform its actual two child callbacks. Neither
    // Garage Back nor destruction of a resource projection releases these.
    if(dialog470_.entity.reference00||controller47C_.entity.reference00)std::terminate();
}
bool OriginalUiSharedDialogs50KG::fail(Diagnostic& d,Status s,std::uint32_t pc)noexcept {
    d.status=s;d.sourcePC=pc;d.sourceEffectsCommitted=dialog470_.entity.reference00!=0||dialog470_.vtable!=0;return false;
}
bool OriginalUiSharedDialogs50KG::current()const noexcept {
    const auto app=application_.lock();return app&&app->sharedDialogs.get()==this&&scope_&&inputs_.root&&
        scope_->issuer_==inputs_.root&&scope_->live().okay();
}
bool OriginalUiSharedDialogs50KG::bindInitial1B7BD0(std::shared_ptr<Scope> scope,
 std::shared_ptr<OriginalUiMainApplication50KG> app,Inputs inputs,Diagnostic& d)noexcept {
    d={};if(phase_!=Phase::empty||!scope||!scope->live().okay()||!app||app->sharedDialogs.get()!=this||
       !inputs.root||scope->issuer_!=inputs.root)return fail(d,Status::owner,0x1B7BD0);
    // A native session that already published47C resources has real history.
    // Do not manufacture470's constructor history alongside that live UI.
    const auto& state=app->controllerPopupState;
    if(app->controllerPopupResources||(!successorConstructor_&&(state.delta3C0!=0||state.timer3C4!=0||state.active3CC||
       state.closing3C8||state.hasConfirm3D0||state.hasCancel3D4||state.result3D8)))
        return fail(d,Status::history,0x1B7BD0);
    scope_=std::move(scope);application_=app;inputs_=std::move(inputs);phase_=Phase::bound;
    return construct470203C90(d);
}
bool OriginalUiSharedDialogs50KG::construct470203C90(Diagnostic& d)noexcept {
    if(!current()||phase_!=Phase::bound)return fail(d,Status::phase,0x203C90);
    d.entity=scope_->constructSharedDialog203C90(*this,dialog470_.entity);
    if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);
    try {
        dialog470_.vtable=0x3D0910;
        for(auto& q:dialog470_.strips){if(!q.construct2DEF40(0x40+constructed470Quads_*0x60))
            return fail(d,Status::phase,0x203CD0);++constructed470Quads_;}
        dialog470_.dim160=std::make_unique<Dim>(); //Actual2E1920 fields.
        dialog470_.message1B0=std::unique_ptr<Text>(new Text);
        dialog470_.alternate220=std::unique_ptr<Text>(new Text);
        dialog470_.manager30=nullptr;dialog470_.active298=0;dialog470_.timer294=.25f;
        dialog470_.primaryPresent29C=0;dialog470_.secondaryPresent2A0=0;dialog470_.delta290=0;
        dialog470_.constructorStateOwned=true;phase_=Phase::dialog470Constructed;
        d.sourceEffectsCommitted=true;d.sourcePC=0x203D3C;return true;
    }catch(...){phase_=Phase::failed;return fail(d,Status::allocation,0x203C90);}
}
bool OriginalUiSharedDialogs50KG::load470203D40(Diagnostic& d)noexcept {
    d={};if(!current()||phase_!=Phase::dialog470Constructed||!dialog470_.dim160||
       !scope_->sharedDialogPrefixCurrent(*this,0,dialog470_.entity))return fail(d,Status::phase,0x203D40);
    Quad::RootInputs in(inputs_.root,inputs_.domain,inputs_.context,inputs_.video,inputs_.popup,inputs_.materialDirectory);
    if(!Quad::inputsCurrent(in)||!inputs_.fontDirectory||!inputs_.fonts)
        return fail(d,Status::resourcePending,0x203D78);
    try {
        //141C20 itself is the original identity no-op.203D7C stores this real
        //persistent dispatcher, not the active Garage manager.
        dialog470_.manager30=&scope_->dispatcher_;
        auto& p=*dialog470_.dim160;p.type0C=1;p.count04=p.count08=4;
        using Page=OriginalUiGaragePageOwner50KG;
        const auto firstUse=[](Page::PolygonAssertion& s)noexcept {
            if(!s.guard.load(std::memory_order_acquire)){s.word.store(1,std::memory_order_relaxed);s.guard.store(1,std::memory_order_release);}
        };
        firstUse(Page::polygonCount8F38());firstUse(Page::polygonLimit8F3C());firstUse(Page::polygonQuads8F44());
        p.vertices00=std::make_unique<Dim::Vertex[]>(4);p.owns38=1;p.dirty3C=1;p.count34=p.count08;
        p.count48=p.capacity4C=1;p.count34=1;p.packets44=std::make_unique<Dim::Descriptor[]>(1);
        auto* packet=static_cast<Quad::Qword*>(::operator new[](18*sizeof(Quad::Qword),std::align_val_t{64}));
        auto& desc=p.packets44[0];desc.allocation00.reset(packet);
        for(std::size_t i=0;i<18;++i)std::construct_at(packet+i);
        packet[1].words={0x11000000,0x03000000,0x02000200,0x6C0E8000};desc.vif04=packet+1;
        packet[2].words={0x8004,0x30064000,0x512,0};desc.gif08=packet+2;desc.tag0C=packet+3;
        desc.sourcePrimitive=12;desc.sourceVertices=4;std::size_t cursor=4;
        for(std::size_t i=0;i<4;++i){auto& vertex=p.vertices00[i];firstUse(Page::polygonAliases8F64());
            const auto slot=vertex.aliases10;vertex.rgba1C[slot]=packet+cursor++;vertex.stq14[slot]=packet+cursor++;
            packet[cursor].words={0,0,0x3F800000,0};vertex.xyz24[slot]=packet+cursor++;++vertex.aliases10;}
        packet[cursor].words={0x140000B0,0x10000000,0,0};packet[0].words={0x30000011,0,0,0};
        desc.dmaData4=packet+1;desc.pointersKnown=true;
        p.primitives30=std::make_unique<std::unique_ptr<Quad::Primitive>[]>(1);
        p.primitives30[0]=std::make_unique<Quad::Primitive>();
        if(!Quad::retainPrimitive2465B0(in,*p.primitives30[0],"dimpopup",d.quad)){
            phase_=Phase::failed;return fail(d,Status::resourcePending,d.quad.sourcePC);}
        p.primitives30[0]->packet10=packet;
        //203DA0..3F6C writes the four real vertices and all published aliases.
        //Only source RGBA/geometry is retained; no guest DMA is submitted.
        constexpr std::array<std::array<float,2>,4> xy{{{{0,0}},{{640,0}},{{0,448}},{{640,448}}}};
        for(std::size_t i=0;i<4;++i){auto& vertex=p.vertices00[i];vertex.authored00[0]=xy[i][0];vertex.authored00[1]=xy[i][1];
            for(std::size_t a=0;a<vertex.aliases10;++a){for(std::size_t c=0;c<4;++c)
                vertex.xyz24[a]->words[c]=std::bit_cast<std::uint32_t>(vertex.authored00[c]);
                vertex.rgba1C[a]->words={0,0,0,127};}}
        phase_=Phase::dialog470DimLoaded;
        const auto font=inputs_.fonts->find282210(Fonts::Name::frontendVenus16);
        const auto localization=inputs_.root->sourceLocalization32COwner_;
        if(!font||!localization){phase_=Phase::failed;return fail(d,Status::resourcePending,0x203FA4);}
        for(std::size_t i=0;i<2;++i){
            if(!inputs_.root->borrowLocalizationText18AC60(*localization,87+static_cast<std::int32_t>(i),inputs_.selected32C,d.text.localization)){
                phase_=Phase::failed;return fail(d,Status::resourcePending,d.text.localization.sourcePC);}
            std::string_view text;
            if(!assets::OriginalApplicationLocalization50KG::SelectedText::BorrowReader::readCurrent(
                inputs_.selected32C,*inputs_.root,*localization,87+static_cast<std::int32_t>(i),text)){
                phase_=Phase::failed;return fail(d,Status::source,i?0x20422C:0x204040);}
            auto& item=i?*dialog470_.alternate220:*dialog470_.message1B0;
            if(!item.loadColdSource2DFD90(*inputs_.root,inputs_.domain,inputs_.context,inputs_.fonts,font,inputs_.popup,
                *inputs_.materialDirectory,*inputs_.fontDirectory,Fonts::Name::frontendVenus16,"venus16popup",40,false,d.text,text)){
                phase_=Phase::failed;return fail(d,Status::resourcePending,d.text.sourcePC);}
            inputs_.selected32C.reset();
            if(item.vector10_[0]!=320||item.vector10_[1]!=224){item.vector10_[0]=320;item.vector10_[1]=224;item.flags5A_|=3;}
            if(item.justification50_!=3){item.justification50_=3;item.flags5A_|=3;} //Actual3C4750 enum.
            if(item.color54_!=0x00CCCCCC){for(std::size_t g=0;g<item.capacity5B_;++g)for(auto& vertex:item.glyphsC_[g].vertices)
                for(std::size_t a=0;a<vertex.aliases10;++a){auto& rgba=vertex.rgba1C[a]->words;rgba[0]=rgba[1]=rgba[2]=102;}
                item.color54_=0x00CCCCCC;}
            ++loaded470Texts_;
        }
        phase_=Phase::dialog470TextsLoaded;
        const auto dimension=[](Quad& q,std::size_t c,float value)noexcept {
            if(q.uv30[c]!=value){q.uv30[c]=value;q.flags5CValue|=1;q.flags5CKnownMask|=1;
                if(q.uv30[3]!=0){q.flags5CValue|=2;q.flags5CKnownMask|=2;}}
        };
        const auto packetFloat=[](Quad& q,std::size_t n,std::size_t c,float x)noexcept {q.resource58[n].words[c]=std::bit_cast<std::uint32_t>(x);};
        for(std::size_t i=0;i<3;++i){auto& q=dialog470_.strips[i];
            if(!q.load2DF0B0(in,d.quad)){phase_=Phase::failed;return fail(d,Status::resourcePending,d.quad.sourcePC);}
            dimension(q,1,640);dimension(q,2,i==2?14.f:16.f);
            if(i){volatile float s0=0.f*q.scale4C,s1=32.f*q.scale4C;
                volatile float t0=(i==1?32.f:18.f)*q.scale50,t1=(i==1?0.f:32.f)*q.scale50;
                packetFloat(q,12,0,s0);packetFloat(q,6,0,s0);packetFloat(q,15,0,s1);packetFloat(q,9,0,s1);
                packetFloat(q,9,1,t0);packetFloat(q,6,1,t0);packetFloat(q,15,1,t1);packetFloat(q,12,1,t1);}
            if(q.color44!=0xFFF2851Bu){for(auto n:{4u,7u,10u,13u}){auto& rgba=q.resource58[n].words;rgba[0]=121;rgba[1]=66;rgba[2]=13;}
                q.color44=0xFFF2851Bu;}
            ++loaded470Quads_;
        }
        dialog470_.loaded=true;phase_=Phase::dialog470Loaded;d.sourceEffectsCommitted=true;d.sourcePC=0x2048EC;return true;
    }catch(...){phase_=Phase::failed;return fail(d,Status::allocation,0x203D40);}
}
bool OriginalUiSharedDialogs50KG::loadedCurrent()const noexcept {
    const auto app=application_.lock();return current()&&phase_==Phase::loaded&&dialog470_.loaded&&
        dialog470_.constructorStateOwned&&controller47C_.loaded&&controller47C_.resources&&
        controller47C_.resources==app->controllerPopupResources&&controller47C_.resources->loaded()&&
        !controller47C_.resources->poisoned()&&controller47C_.resources->sharedDialogBoundCurrent(*this)&&game::OriginalUiControllerPopup50KG::valid(app->controllerPopupState)&&
        scope_->sharedDialogPrefixCurrent(*this,0,dialog470_.entity)&&
        scope_->sharedDialogPrefixCurrent(*this,1,controller47C_.entity);
}
bool OriginalUiSharedDialogs50KG::construct47C2054D0(Diagnostic& d)noexcept {
    if(!current()||phase_!=Phase::dialog470Loaded||!dialog470_.loaded)
        return fail(d,Status::phase,0x2054D0);
    d.entity=scope_->constructControllerPopup2054D0(*this,controller47C_.entity);
    if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);
    controller47C_.vtable=0x3D0890;
    //205564..20559C initializes the NEW 47C instance. A successor is issued
    // only after exact prior object retirement; Root/profile/audio history is
    // not reset. Initial boot keeps its existing constructor/loader projection.
    if(successorConstructor_){
        auto app=application_.lock();if(!app)std::terminate();
        auto& q=app->controllerPopupState;
        q.timer3C4=0;q.delta3C0=0;q.closing3C8=false;q.active3CC=false;
        q.hasConfirm3D0=false;q.hasCancel3D4=false;q.result3D8=0;
    }
    phase_=Phase::controller47CConstructed;d.sourceEffectsCommitted=true;d.sourcePC=0x2055AC;return true;
}
bool OriginalUiSharedDialogs50KG::bindLoaded47C2055B0(
 std::shared_ptr<OriginalUiControllerPopupResources50KG> resources,Diagnostic& d)noexcept {
    if(!current()||phase_!=Phase::controller47CConstructed||!resources||!resources->loaded()||resources->poisoned())
        return fail(d,Status::resourcePending,0x2055B0);
    const auto app=application_.lock();
    if(app->controllerPopupResources&&app->controllerPopupResources!=resources)
        return fail(d,Status::history,0x2055B0);
    // Resource provenance must also match this exact initial boot archive and
    // native Video owner. The concrete resource owner validates these inputs.
    if(!resources->bindSharedDialogOwner2055B0(*this))return fail(d,Status::owner,0x2055B0);
    if(successorConstructor_)app->controllerPopupState.strips=
        game::OriginalUiControllerPopup50KG::initialState().strips;
    app->controllerPopupResources=resources;controller47C_.resources=std::move(resources);
    controller47C_.loaded=true;phase_=Phase::loaded;d.sourceEffectsCommitted=true;d.sourcePC=0x2055B0;return true;
}
bool OriginalUiSharedDialogs50KG::reappendInitialMain1D7CF4(Diagnostic& d)noexcept {
 d={};if(initialMainAttached_)return loadedCurrent();
 if(!loadedCurrent()||scope_->managerEpoch_||scope_->managerLive_||completedReorders_||reorderingPage_)
  return fail(d,Status::phase,0x1D7CF4);
 const auto app=application_.lock();
 if(!reorderStep_){dialog470_.timer294=.25f;dialog470_.delta290=0;reorderStep_=1;}
 if(reorderStep_==1){d.entity=scope_->reappendSharedDialog1B86D0(*this,0,dialog470_.entity);
  if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);reorderStep_=2;}
 if(reorderStep_==2){app->controllerPopupState.timer3C4=.25f;app->controllerPopupState.delta3C0=0;reorderStep_=3;}
 if(reorderStep_==3){d.entity=scope_->reappendSharedDialog1B86D0(*this,1,controller47C_.entity);
  if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);reorderStep_=4;}
 // The caller retains the completed genuine Main cold resource owner. This
 // is its constructor's later1E5038/44 enable pair, not guessed pad presence.
 app->courseUi474=1;app->courseUi478=0;
 initialMainAttached_=true;initialMainResetPending_=true;++completedReorders_;reorderStep_=0;d.sourceEffectsCommitted=true;d.sourcePC=0x1E5044;return true;
}
bool OriginalUiSharedDialogs50KG::reappendNativeMenu1B86D0(Diagnostic& d)noexcept {
 d={};if(!loadedCurrent()||!initialMainAttached_||scope_->managerLive_||reorderingPage_||
    completedReorders_==std::numeric_limits<std::uint64_t>::max())return fail(d,Status::phase,0x1B86D0);
 const auto app=application_.lock();
 if(!reorderStep_){dialog470_.timer294=.25f;dialog470_.delta290=0;reorderStep_=1;}
 if(reorderStep_==1){d.entity=scope_->reappendSharedDialog1B86D0(*this,0,dialog470_.entity);
  if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);reorderStep_=2;}
 if(reorderStep_==2){app->controllerPopupState.timer3C4=.25f;app->controllerPopupState.delta3C0=0;reorderStep_=3;}
 if(reorderStep_==3){d.entity=scope_->reappendSharedDialog1B86D0(*this,1,controller47C_.entity);
  if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);reorderStep_=4;}
 ++completedReorders_;reorderStep_=0;reorderedPage_=nullptr;reorderedGrant_.reset();
 d.sourceEffectsCommitted=true;d.sourcePC=0x1B872C;return true;
}
bool OriginalUiSharedDialogs50KG::reappendMainEntry1D7CF4(Diagnostic& d)noexcept {
 if(initialMainResetPending_){d={};if(!loadedCurrent()||!initialMainAttached_)return fail(d,Status::phase,0x1D7CF4);
  initialMainResetPending_=false;d.sourcePC=0x1D7CF4;return true;}
 if(!reappendNativeMenu1B86D0(d))return false;
 const auto app=application_.lock();app->courseUi474=1;app->courseUi478=0;return true;
}
bool OriginalUiSharedDialogs50KG::reappendOptionsEntry1B86D0(Diagnostic& d)noexcept {
 if(initialMainResetPending_)return fail(d,Status::phase,0x1B86D0);
 return reappendNativeMenu1B86D0(d);
}
bool OriginalUiSharedDialogs50KG::reorderCompleteFor1B872C(const OriginalUiGaragePageOwner50KG& page)const noexcept {
    return loadedCurrent()&&completedReorders_&&reorderStep_==0&&reorderedPage_==&page&&reorderedGrant_.lock()==page.grant_&&
        page.currentConstructor()&&page.issuer_==inputs_.root&&scope_->garagePageOwner_==&page&&
        scope_->garagePage_==&page.entity_&&page.cameraHandoffPhase_==3;
}
bool OriginalUiSharedDialogs50KG::reappend1B86D0(const OriginalUiGaragePageOwner50KG& page,Diagnostic& d)noexcept {
    d={};if(!loadedCurrent()||!page.currentConstructor()||page.issuer_!=inputs_.root||
        scope_->garagePageOwner_!=&page||scope_->garagePage_!=&page.entity_||page.cameraHandoffPhase_!=3)
        return fail(d,Status::owner,0x1B86D0);
    if(reorderCompleteFor1B872C(page)){d.sourcePC=0x1B872C;return true;}
    if(reorderStep_&&reorderingPage_!=&page)return fail(d,Status::phase,0x1B86D0);
    if(completedReorders_==std::numeric_limits<std::uint64_t>::max())return fail(d,Status::phase,0x1B86D0);
    const auto app=application_.lock();
    if(reorderStep_==0){reorderingPage_=&page;dialog470_.timer294=.25f;dialog470_.delta290=0;reorderStep_=1;}
    if(reorderStep_==1){d.entity=scope_->reappendSharedDialog1B86D0(*this,0,dialog470_.entity);
        if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);reorderStep_=2;}
    if(reorderStep_==2){
        // Source207330 changes these two fields only, even when an existing
        // Main/Options session has updated every other popup field.
        app->controllerPopupState.timer3C4=.25f;app->controllerPopupState.delta3C0=0;reorderStep_=3;
    }
    if(reorderStep_==3){d.entity=scope_->reappendSharedDialog1B86D0(*this,1,controller47C_.entity);
        if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);reorderStep_=4;}
    ++completedReorders_;reorderedPage_=&page;reorderedGrant_=page.grant_;reorderingPage_=nullptr;reorderStep_=0;d.sourceEffectsCommitted=true;d.sourcePC=0x1B872C;return true;
}
bool OriginalUiSharedDialogs50KG::advance4702048F0(float delta,Diagnostic& d)noexcept {
 d={};if(!loadedCurrent()||!std::isfinite(delta)||delta<0||
    dialog470_.entity.parent1C!=&scope_->dispatcher_||controller47C_.entity.parent1C!=&scope_->dispatcher_)
  return fail(d,Status::owner,0x2048F0);
 auto& host=inputs_.video->host;
 if(!host.ownerThread()||!host.created()||!host.current()||!host.sharedDialogPollCurrent50KG_||
    !host.sharedDialogPollRevision50KG_)return fail(d,Status::source,0x204914);
 if(update470Count_==std::numeric_limits<std::uint64_t>::max())return fail(d,Status::phase,0x2048F0);
 auto& p=dialog470_;const auto app=application_.lock();
 if(!update470Pending_){
  p.delta290=delta; //2048F8 precedes both source device queries.
  update470SampledPrimary_=host.sharedDialogPrimaryPresent50KG_;
  update470Pending_=true;
 }else if(p.delta290!=delta)return fail(d,Status::phase,0x2048F8);
 const auto first=std::uint32_t(update470SampledPrimary_);
 const bool firstChanged=first!=p.primaryPresent29C&&app->courseUi474!=0;
 const auto second=host.sharedDialogSecondPresence50KG_;
 // The original always queries both devices. Native enumeration already did
 // that work. An unknown unused result has no retained references or effects:
 // the first changed enabled port skips the second comparison this frame.
 if(!firstChanged&&app->courseUi478&&second==WindowControllerPresence50DQ::Unknown)
  return fail(d,Status::source,0x204948);
 if(firstChanged){p.primaryPresent29C=first;p.timer294=.25f;} //20496C/978
 else if(app->courseUi478){const auto value=std::uint32_t(second==WindowControllerPresence50DQ::Present);
  if(value!=p.secondaryPresent2A0){p.secondaryPresent2A0=value;p.timer294=.25f;}} //204998/9A0
 // This deliberately tests cached PRIMARY presence only, exactly2049A4.
 // Do not fix the original's asymmetric second-port timer behavior.
 if(p.primaryPresent29C==0&&p.timer294>0)p.timer294-=p.delta290;
 // The actual two dialog Entity prefixes are childless;141ED0 has no child
 // callbacks here. This is not completion of the manager or Page update.
 update470Pending_=false;++update470Count_;update470PollRevision_=host.sharedDialogPollRevision50KG_;
 d.sourceEffectsCommitted=true;d.sourcePC=0x2049F4;return true;
}
bool OriginalUiSharedDialogs50KG::invisible470204AC0(Diagnostic& d)const noexcept {
 d={};if(!loadedCurrent()||update470Pending_||!update470Count_){d.status=Status::phase;d.sourcePC=0x204AC0;return false;}
 const auto& host=inputs_.video->host;const auto app=application_.lock();
 if(!host.ownerThread()||!host.created()||!host.current()||!host.sharedDialogPollCurrent50KG_||
    update470PollRevision_>host.sharedDialogPollRevision50KG_){d.status=Status::source;d.sourcePC=0x204AC0;return false;}
 if((!dialog470_.primaryPresent29C&&app->courseUi474)||(!dialog470_.secondaryPresent2A0&&app->courseUi478)){
  // This is the real observable remaining case:204A00 mutates the two strip
  // materials and204AC0 animates/draws the warning. It is not unknown history.
  d.status=Status::resourcePending;d.sourcePC=0x204A54;return false;
 }
 // Presence is the actual2048F0 cache, not the latest OS sample: native
 // hidden-frame waits may poll again without another source update.
 //204A00 and204AC0 both take their genuine no-warning branch. Their base
 // callbacks visit this actual childless Entity. No popup pixels are omitted.
 d.sourcePC=0x2054A4;return true;
}
bool OriginalUiSharedDialogs50KG::nativeMaterialRetirementCurrent()const noexcept {
 Quad::Diagnostic d;return dialog470_.dim160&&dialog470_.dim160->primitives30&&
  Quad::preflightPrimitive246260(dialog470_.dim160->primitives30[0],d);
}
bool OriginalUiSharedDialogs50KG::controllerRetirementCurrent()const noexcept {
 return current()&&phase_==Phase::retiring&&retiringOrdinal_==1&&
  scope_->sharedDialogRetirementCurrent(*this,1,controller47C_.entity);
}
bool OriginalUiSharedDialogs50KG::drain2DEFE0(Diagnostic& d,std::uint32_t firstPC,std::uint32_t secondPC)noexcept {
 if(phase_!=Phase::retiring||retirementDrainPhase_>2||!scope_->sharedDialogRetirementCurrent(
    *this,retiringOrdinal_,retiringOrdinal_?controller47C_.entity:dialog470_.entity))return fail(d,Status::phase,firstPC);
 if(!retirementDisplay_){const auto display=inputs_.root->raceDisplayOwner50LA_.lock();
  if(!display||display->application_!=inputs_.root||display->video_!=inputs_.video||
     !display->frontendOwnerCurrent50KG()||!display->canRetain())return fail(d,Status::retirementPending,firstPC);
  retirementDisplay_=display;display->pin();retirementDisplayPinned_=true;
 }
 auto& display=*retirementDisplay_;
 if(!retirementDisplayPinned_||display.application_!=inputs_.root||!display.frontendOwnerCurrent50KG()||!display.canRetain())
  return fail(d,Status::retirementPending,firstPC);
 if(!retirementDrainPhase_){std::int32_t actualV0=0;
  const auto r=game::serviceOriginalRenderer244D60(display.dispatch_,true,display.renderer_,d.renderer,actualV0,
   game::OriginalCameraDrainPolicy::initializedContext);
  if(r.status!=game::OriginalCameraStartStatus::complete)return fail(d,Status::retirementPending,r.sourcePC);
  retirementDrainPhase_=1;
 }
 if(retirementDrainPhase_==1){if(!display.renderer_.finishInitializedContextWork(d.renderer)||!d.renderer.contextWorkCompleted)
   return fail(d,Status::retirementPending,secondPC);retirementDrainPhase_=2;}
 return true;
}
bool OriginalUiSharedDialogs50KG::retireDim2E1980(Diagnostic& d)noexcept {
 if(!dialog470_.dim160)return fail(d,Status::phase,0x20E094);auto& p=*dialog470_.dim160;
 if(p.count34!=1||p.count48!=1||p.capacity4C!=1||p.count04!=4||p.count08!=4||p.owns38!=1||
    !p.vertices00||!p.primitives30||!p.packets44||!Quad::preflightPrimitive246260(p.primitives30[0],d.quad))
  return fail(d,Status::retirementPending,d.quad.sourcePC?d.quad.sourcePC:0x2E19D4);
 if(!drain2DEFE0(d,0x2E19B4,0x2E19CC))return false;
 if(!Quad::preflightPrimitive246260(p.primitives30[0],d.quad))return fail(d,Status::retirementPending,d.quad.sourcePC);
 Quad::releasePrimitive246260(p.primitives30[0]);p.primitives30.reset();
 p.packets44[0].allocation00.reset();p.packets44.reset();p.vertices00.reset();dialog470_.dim160.reset();return true;
}
bool OriginalUiSharedDialogs50KG::retireAttached142590(Diagnostic& d)noexcept {
 d={};if(phase_==Phase::retired){d.sourcePC=0x1413E0;return true;}
 if(!current()||reorderStep_)return fail(d,Status::phase,0x142590);
 const auto app=application_.lock();
 if(phase_==Phase::loaded){
  if(!loadedCurrent()||!controller47C_.resources->preflightSharedRetirement20DF80(*this))
   return fail(d,Status::retirementPending,0x20DFAC);
  d.entity=scope_->beginSharedDialogRetirement142590(*this,1,controller47C_.entity);
  if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);
  phase_=Phase::retiring;retiringOrdinal_=1;retirementPayloadStep_=retirementDrainPhase_=0;
 }
 if(phase_!=Phase::retiring)return fail(d,Status::phase,0x142590);
 if(retiringOrdinal_==1){
  if(!controllerRetirementCurrent())return fail(d,Status::owner,0x20DF80);
  while(retirementPayloadStep_<9){const auto step=retirementPayloadStep_;
   if(!controller47C_.resources->preflightSharedRetirement20DF80(*this))return fail(d,Status::retirementPending,0x20DFAC);
   if(step>=3&&!drain2DEFE0(d,step==3?0x2E19B4:0x2DF018,step==3?0x2E19CC:0x2DF030))return false;
   if(!controller47C_.resources->retireSharedPayloadPart20DF80(*this,step,d.popup))
    return fail(d,Status::retirementPending,d.popup.sourcePC);
   ++retirementPayloadStep_;retirementDrainPhase_=0;
  }
  if(!controller47C_.resources->finishSharedPayload20E008(*this))return fail(d,Status::retirementPending,0x20E008);
  d.entity=scope_->finishSharedDialogRetirement1413E0(*this,1,controller47C_.entity);
  if(!d.entity.okay())std::terminate();
  if(app->controllerPopupResources!=controller47C_.resources)std::terminate();
  app->controllerPopupResources.reset();controller47C_.resources.reset();controller47C_.loaded=false;
  retiringOrdinal_=0;retirementPayloadStep_=retirementDrainPhase_=0;
 }
 if(!scope_->sharedDialogRetirementCurrent(*this,0,dialog470_.entity)){
  d.entity=scope_->beginSharedDialogRetirement142590(*this,0,dialog470_.entity);
  if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);
 }
 //20E050 exact embedded order: Text220,Text1B0,Dim160,Quad100,A0,40.
 while(retirementPayloadStep_<6){const auto step=retirementPayloadStep_;
  if(step<2){auto& item=step==0?dialog470_.alternate220:dialog470_.message1B0;
   if(!item||!item->retire2E1340(d.text))return fail(d,Status::retirementPending,d.text.sourcePC);
   item.reset();--loaded470Texts_;
  }else if(step==2){if(!retireDim2E1980(d))return false;}
  else {auto& q=dialog470_.strips[5-step];
   if(!q.preflightRetirement2DEFE0(d.quad))return fail(d,Status::retirementPending,d.quad.sourcePC);
   if(!drain2DEFE0(d,0x2DF018,0x2DF030))return false;
   q.retirementDrainPhase_=retirementDrainPhase_;
   if(!q.retireResources2DF038(d.quad))return fail(d,Status::retirementPending,d.quad.sourcePC);
   --loaded470Quads_;--constructed470Quads_;
  }
  ++retirementPayloadStep_;retirementDrainPhase_=0;
 }
 d.entity=scope_->finishSharedDialogRetirement1413E0(*this,0,dialog470_.entity);
 if(!d.entity.okay())return fail(d,Status::owner,d.entity.sourcePC);
 dialog470_.loaded=false;phase_=Phase::retired;reorderedPage_=nullptr;reorderedGrant_.reset();
 if(retirementDisplayPinned_){retirementDisplay_->unpin();retirementDisplayPinned_=false;}retirementDisplay_.reset();
 inputs_.selected32C.reset();inputs_.fonts.reset();inputs_.popup.reset();inputs_.video.reset();inputs_.context.reset();inputs_.domain.reset();
 d.sourceEffectsCommitted=true;d.sourcePC=0x1413E0;return true;
}

}
namespace rr::game {
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::constructSharedDialog203C90(SharedDialogs& owner,C& c)noexcept {
    auto r=live();if(!r.okay())return r;
    if(!dispatcherLive_||!linked(dispatcher_,&application_->root10_)||managerLive_||managerEpoch_||
       sharedDialogsOwner_||sharedDialogs_[0]||sharedDialogs_[1]||sharedDialogRetired_[0]||sharedDialogRetired_[1]||owner.scope_.get()!=this||
       owner.phase_!=SharedDialogs::Phase::bound||&c!=&owner.dialog470_.entity||c.reference00)
        return {Status::phase,0x203C90};
    if(!application_->entityAuthority_.reserveAdditional(1)||!application_->entityAuthority_.canEnroll(c,key(&c)))
        return {Status::capacity,0x1B7BD4};
    const auto init=initializeOriginalWreckComponent50LA(c,K::entity141300,scopeGeneration_,key(&c),true,application_->objectCount423F60_);
    if(init.status!=OriginalWreckComponentStatus50LA::complete||!application_->entityAuthority_.enroll(c))std::terminate();
    sharedDialogsOwner_=&owner;sharedDialogs_[0]=&c;return {Status::complete,0x203C9C};
}
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::constructControllerPopup2054D0(SharedDialogs& owner,C& c)noexcept {
    auto r=live();if(!r.okay())return r;
    if(sharedDialogsOwner_!=&owner||sharedDialogs_[1]||owner.scope_.get()!=this||
       owner.phase_!=SharedDialogs::Phase::dialog470Loaded||!owner.dialog470_.loaded||
       !sharedDialogPrefixCurrent(owner,0,owner.dialog470_.entity)||&c!=&owner.controller47C_.entity||c.reference00)
        return {Status::phase,0x2054D0};
    if(!application_->entityAuthority_.reserveAdditional(1)||!application_->entityAuthority_.canEnroll(c,key(&c)))
        return {Status::capacity,0x1B7C00};
    const auto init=initializeOriginalWreckComponent50LA(c,K::entity141300,scopeGeneration_,key(&c),true,application_->objectCount423F60_);
    if(init.status!=OriginalWreckComponentStatus50LA::complete||!application_->entityAuthority_.enroll(c))std::terminate();
    sharedDialogs_[1]=&c;return {Status::complete,0x2054DC};
}
bool OriginalUiEntityScope50KG::sharedDialogPrefixCurrent(const SharedDialogs& owner,std::size_t ordinal,const C& c)const noexcept {
    return ordinal<2&&live().okay()&&dispatcherLive_&&sharedDialogsOwner_==&owner&&owner.scope_.get()==this&&
        sharedDialogs_[ordinal]==&c&&&c==(ordinal?&owner.controller47C_.entity:&owner.dialog470_.entity)&&
        c.kind==K::entity141300&&c.sessionGeneration==scopeGeneration_&&c.objectKey==key(&c)&&c.reference00==1&&
        !c.firstChild18&&application_->entityAuthority_.canRetire(c)&&
        ((c.parent1C==&dispatcher_&&c.mask20==0xffffffffu&&linked(c,&dispatcher_))||
         (!c.parent1C&&!c.previous10&&!c.next14&&
          ((!c.mask20&&!sharedDialogEverAttached_[ordinal])||
           (c.mask20==0xffffffffu&&sharedDialogEverAttached_[ordinal]&&sharedDialogDetached_&&
            sharedDialogReorderOrdinal_==ordinal))));
}
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::reappendSharedDialog1B86D0(SharedDialogs& owner,std::size_t ordinal,C& c)noexcept {
    if(ordinal>1||sharedDialogReorderOrdinal_!=ordinal||owner.reorderStep_!=(ordinal?3:1)||
       !owner.loadedCurrent()||!sharedDialogPrefixCurrent(owner,ordinal,c))return {Status::phase,0x1B86D0};
    // This is the real native dispatcher forest. No private replacement list,
    // no acceptance of unrelated children, no fabricated prior ordering.
    for(const C* node=dispatcher_.firstChild18;node;node=node->next14)
        if(node!=sharedDialogs_[0]&&node!=sharedDialogs_[1]&&!(managerLive_&&managerPublished_&&node==&manager_))
            return {Status::topology,0x141A70};
    if(!sharedDialogDetached_){
        const auto detached=application_->entityAuthority_.detach(c,application_->attachmentLock423F58_);
        if(detached.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,detached.sourcePC};
        sharedDialogDetached_=true;
    }
    const auto appended=application_->entityAuthority_.attachChildren(dispatcher_,c,0xffffffffu,application_->attachmentLock423F58_);
    if(appended.status!=OriginalWreckComponentStatus50LA::complete)return {Status::topology,appended.sourcePC};
    sharedDialogEverAttached_[ordinal]=true;sharedDialogDetached_=false;sharedDialogReorderOrdinal_=ordinal?0:1;
    return {Status::complete,ordinal?0x1B8718u:0x1B86F8u};
}
}
namespace rr::game {
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::beginSharedDialogRetirement142590(
 SharedDialogs& owner,std::size_t ordinal,C& c)noexcept {
 auto r=live();if(!r.okay())return r;
 if(ordinal>1||sharedDialogsOwner_!=&owner||owner.scope_.get()!=this||managerLive_||managerRetiring_||
    managerSearchOwner_||!garageChildren_.empty()||sharedDialogDetached_||sharedDialogReorderOrdinal_||
    sharedDialogRetiring_[ordinal]||sharedDialogRetired_[ordinal]||!sharedDialogPrefixCurrent(owner,ordinal,c)||
    c.parent1C!=&dispatcher_||c.next14)return {Status::phase,0x142590};
 if(ordinal==1){
  if(owner.phase_!=SharedDialogs::Phase::loaded||sharedDialogRetired_[0]||sharedDialogRetiring_[0]||
     !sharedDialogs_[0]||dispatcher_.firstChild18!=sharedDialogs_[0]||
     sharedDialogs_[0]->parent1C!=&dispatcher_||sharedDialogs_[0]->previous10||
     sharedDialogs_[0]->next14!=&c||c.previous10!=sharedDialogs_[0])return {Status::topology,0x1425B8};
 }else if(owner.phase_!=SharedDialogs::Phase::retiring||owner.retiringOrdinal_||!sharedDialogRetired_[1]||
           sharedDialogs_[1]||dispatcher_.firstChild18!=&c||c.previous10)return {Status::topology,0x1425B8};
 // Last reference repairs genuine dispatcher links. The dying source object's
 // parent/previous/next values are not invented-null stores.
 if(c.previous10)c.previous10->next14=c.next14;
 if(c.next14)c.next14->previous10=c.previous10;
 if(dispatcher_.firstChild18==&c)dispatcher_.firstChild18=c.next14;
 if(!application_->entityAuthority_.retire(c))std::terminate();--c.reference00;
 sharedDialogRetiring_[ordinal]=true;return {Status::complete,0x104970};
}
bool OriginalUiEntityScope50KG::sharedDialogRetirementCurrent(const SharedDialogs& owner,
 std::size_t ordinal,const C& c)const noexcept {
 if(ordinal>1||!live().okay()||sharedDialogsOwner_!=&owner||owner.scope_.get()!=this||
    sharedDialogs_[ordinal]!=&c||!sharedDialogRetiring_[ordinal]||sharedDialogRetired_[ordinal]||
    c.kind!=K::entity141300||c.sessionGeneration!=scopeGeneration_||c.objectKey!=key(&c)||
    c.reference00||c.firstChild18||c.parent1C!=&dispatcher_||c.mask20!=0xffffffffu||c.next14||
    contains(application_->entityAuthority_.view(),&c)||!application_->objectCount423F60_)return false;
 return ordinal?c.previous10==&owner.dialog470_.entity&&!sharedDialogRetired_[0]:
   !c.previous10&&sharedDialogRetired_[1]&&!dispatcher_.firstChild18;
}
bool OriginalUiEntityScope50KG::sharedDialogRetiredNodesAbsent(const SharedDialogs& owner)const noexcept {
 const auto view=application_->entityAuthority_.view();
 return !contains(view,&dispatcher_)&&!contains(view,&owner.dialog470_.entity)&&
        !contains(view,&owner.controller47C_.entity);
}
OriginalUiEntityScope50KG::Result OriginalUiEntityScope50KG::finishSharedDialogRetirement1413E0(
 SharedDialogs& owner,std::size_t ordinal,C& c)noexcept {
 if(!sharedDialogRetirementCurrent(owner,ordinal,c))return {Status::phase,0x1413E0};
 if((ordinal==1&&owner.retirementPayloadStep_!=9)||(ordinal==0&&owner.retirementPayloadStep_!=6))
  return {Status::phase,0x1413E0};
 --application_->objectCount423F60_;sharedDialogs_[ordinal]=nullptr;
 sharedDialogRetiring_[ordinal]=false;sharedDialogRetired_[ordinal]=true;
 if(!ordinal)sharedDialogsOwner_=nullptr;
 return {Status::complete,0x1413E0};
}
}

namespace original_main_entry50KG {
bool Application::sharedUiRetirementReceiptCurrent17C1D8()const noexcept {
 using Scope=rr::game::OriginalUiEntityScope50KG;
 using Owner=rr::platform::OriginalUiSharedDialogs50KG;
 const auto& old=retiredSharedUiDialogs_;const auto& scope=sharedUiScopeOwner_;
 const auto rejected=[](const auto& observed,const char* guard)noexcept {
  const bool failed=static_cast<bool>(observed);
  if(failed)std::fprintf(stderr,"original_shared_ui_retirement_receipt_rejected guard=%s\n",guard);
  return failed;
 };
 if(rejected(garageAudioOwnerThread_!=std::this_thread::get_id(),"garageAudioOwnerThread_!=std::this_thread::get_id()")||
    rejected(!entitiesOwner_,"!entitiesOwner_")||
    rejected(!entitiesOwner_->application10(),"!entitiesOwner_->application10()")||
    rejected(!state,"!state")||
    rejected(state->sharedDialogs,"state->sharedDialogs")||
    rejected(state->controllerPopupResources,"state->controllerPopupResources")||
    rejected(garageCustodyOwner_,"garageCustodyOwner_")||
    rejected(nativeCollectorFifoOwner_,"nativeCollectorFifoOwner_")||
    rejected(uiCollector1B73BCAttempted_,"uiCollector1B73BCAttempted_")||
    rejected(!old,"!old")||
    rejected(!scope,"!scope")||
    rejected(scope.use_count()!=2,"scope.use_count()!=2")||
    rejected(old->inputs_.root!=this,"old->inputs_.root!=this")||
    rejected(old->application_.lock()!=state,"old->application_.lock()!=state")||
    rejected(old->scope_!=scope,"old->scope_!=scope")||
    rejected(old->phase_!=Owner::Phase::retired,"old->phase_!=Owner::Phase::retired")||
    rejected(old->dialog470_.entity.reference00,"old->dialog470_.entity.reference00")||
    rejected(old->controller47C_.entity.reference00,"old->controller47C_.entity.reference00")||
    rejected(old->dialog470_.loaded,"old->dialog470_.loaded")||
    rejected(old->controller47C_.loaded,"old->controller47C_.loaded")||
    rejected(old->controller47C_.resources,"old->controller47C_.resources")||
    rejected(old->retirementDisplay_,"old->retirementDisplay_")||
    rejected(old->retirementDisplayPinned_,"old->retirementDisplayPinned_")||
    rejected(old->reorderingPage_,"old->reorderingPage_")||
    rejected(old->reorderedPage_,"old->reorderedPage_")||
    rejected(!old->reorderedGrant_.expired(),"!old->reorderedGrant_.expired()")||
    rejected(old->loaded470Texts_,"old->loaded470Texts_")||
    rejected(old->loaded470Quads_,"old->loaded470Quads_")||
    rejected(old->constructed470Quads_,"old->constructed470Quads_")||
    rejected(scope->issuer_!=this,"scope->issuer_!=this")||
    rejected(scope->application_!=entitiesOwner_,"scope->application_!=entitiesOwner_")||
    rejected(!scope->closed_,"!scope->closed_")||
    rejected(scope->dispatcherLive_,"scope->dispatcherLive_")||
    rejected(scope->managerLive_,"scope->managerLive_")||
    rejected(scope->managerPublished_,"scope->managerPublished_")||
    rejected(scope->dispatcherCurrentManager30_,"scope->dispatcherCurrentManager30_")||
    rejected(scope->transitionDisplayPinned_,"scope->transitionDisplayPinned_")||
    rejected(scope->transitionDisplay_,"scope->transitionDisplay_")||
    rejected(scope->sharedDialogs_[0],"scope->sharedDialogs_[0]")||
    rejected(scope->sharedDialogs_[1],"scope->sharedDialogs_[1]")||
    rejected(scope->dispatcher_.reference00,"scope->dispatcher_.reference00")||
    rejected(scope->dispatcher_.firstChild18,"scope->dispatcher_.firstChild18")||
    rejected(scope->dispatcher_.kind!=rr::game::OriginalWreckComponentKind50LA::uiDispatcher1B7140,"scope->dispatcher_.kind!=rr::game::OriginalWreckComponentKind50LA::uiDispatcher1B7140")||
    rejected(scope->dispatcher_.sessionGeneration!=scope->scopeGeneration_,"scope->dispatcher_.sessionGeneration!=scope->scopeGeneration_")||
    rejected(scope->dispatcher_.objectKey!=Scope::key(&scope->dispatcher_),"scope->dispatcher_.objectKey!=Scope::key(&scope->dispatcher_)")||
    rejected(!scope->sharedDialogRetiredNodesAbsent(*old),"!scope->sharedDialogRetiredNodesAbsent(*old)"))return false;
 const auto& q=state->controllerPopupState;const auto& saved=retiredControllerPopupState_;
 const auto same=[](float a,float b)noexcept{return std::bit_cast<std::uint32_t>(a)==std::bit_cast<std::uint32_t>(b);};
 if(rejected(!rr::game::OriginalUiControllerPopup50KG::valid(q),"!rr::game::OriginalUiControllerPopup50KG::valid(q)")||
    rejected(q.active3CC,"q.active3CC")||
    rejected(!same(q.delta3C0,saved.delta3C0),"!same(q.delta3C0,saved.delta3C0)")||
    rejected(!same(q.timer3C4,saved.timer3C4),"!same(q.timer3C4,saved.timer3C4)")||
    rejected(q.active3CC!=saved.active3CC,"q.active3CC!=saved.active3CC")||
    rejected(q.closing3C8!=saved.closing3C8,"q.closing3C8!=saved.closing3C8")||
    rejected(q.hasConfirm3D0!=saved.hasConfirm3D0,"q.hasConfirm3D0!=saved.hasConfirm3D0")||
    rejected(q.hasCancel3D4!=saved.hasCancel3D4,"q.hasCancel3D4!=saved.hasCancel3D4")||
    rejected(q.result3D8!=saved.result3D8,"q.result3D8!=saved.result3D8"))return false;
 for(std::size_t i=0;i<q.strips.size();++i){const auto& a=q.strips[i];const auto& b=saved.strips[i];
  if(!same(a.x,b.x)||!same(a.y,b.y)||!same(a.width,b.width)||!same(a.height,b.height)){
   std::fprintf(stderr,"original_shared_ui_retirement_receipt_rejected guard=popup_strip index=%zu\n",i);return false;}}
 return true;
}
Application::SharedUiBootResult Application::prepareSharedDialogsInitial1B7BD0(
 std::shared_ptr<rr::platform::OriginalUiControllerPopupResources50KG> resources,
 std::shared_ptr<const rr::assets::OriginalUiArchive50KG> popup,
 rr::platform::VehicleWindowGl50DS& renderer,std::string& error)const noexcept {
 using Owner=rr::platform::OriginalUiSharedDialogs50KG;
 error.clear();
 if(!state||state->sharedDialogs){error="Persistent UI constructor already owned; no history adoption";return SharedUiBootResult::unavailable;}
 const bool successor=bool(sharedUiScopeOwner_);
 if(successor&&!sharedUiRetirementReceiptCurrent17C1D8()){
  error="Fresh UI dispatcher requires its exact completed predecessor retirement";return SharedUiBootResult::failed;
 }
 if(!successor&&retiredSharedUiDialogs_){error="UI predecessor receipt lost its closed scope";return SharedUiBootResult::failed;}
 const auto display=raceDisplayOwner50LA_.lock();
 // No counted prefix is installed until genuine same-Root dependencies and
 // its actual live renderer can also support ordered normal retirement.
 if(!resources||!resources->loaded()||resources->poisoned()||!popup||!videoOwner_||
    !materialContextOwner_||!materialContextOwner_->live()||!standardMaterialDomainOwner_||
    !fonts328Owner_||!fonts328Owner_->find282210(rr::assets::OriginalApplicationFonts50LA::Name::frontendVenus16)||
    !sourceLocalization32COwner_||!display||!display->frontendOwnerCurrent50KG()||!display->canRetain()||
    &display->renderer_!=&renderer){error="Shared UI boot awaits actual Root32C,328,canonical materials and retained native display";return SharedUiBootResult::unavailable;}
 // Missing/unsealed actual resource authority leaves the optional new join
 // unopened. Main/Options retain existing noChange behavior, not fake load.
 const auto globalsProvider=standardMaterialDomainOwner_->archiveProvider3E1A70();
 rr::assets::OriginalStandardArchiveProvider50LA::MemberView globalsMember;
 if(!globalsProvider||globalsProvider->lookup132820("gwidgets.dat",globalsMember)!=
    rr::assets::OriginalStandardArchiveProvider50LA::Lookup::absent){
  error="Shared UI boot awaits genuine absent global-widget lookup; present variants are not implemented";
  return SharedUiBootResult::unavailable;
 }
 // The retained Popup archive is real input authority, not a replacement
 // for the canonical provider mount. Establish it before counted children.
 // Only source bytes are registered here; original Quad/Text calls retain
 // and populate their canonical rows in the existing source order.
 try {
  rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic mountDiagnostic;
  if(!globalsProvider->mountAuthenticatedUi(popup,mountDiagnostic)||
     !globalsProvider->sealNativeMounts(mountDiagnostic)){
   error="Shared UI authenticated Popup mount/seal failed";return SharedUiBootResult::unavailable;
  }
  std::vector<std::shared_ptr<const rr::assets::OriginalStandardMaterialSource50LA>> materials;
  rr::assets::OriginalStandardMaterialDiagnostic50LA sourceDiagnostic;
  for(const char* name:{"ui\\textures\\dimpopup.psm","font\\venus16popup.psm"}){
   const auto member=popup->find(name);
   rr::assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
   if(!member.identity||!member.data||!member.size||
      globalsProvider->lookup132820(name,mounted)!=rr::assets::OriginalStandardArchiveProvider50LA::Lookup::present||
      !mounted.member||!mounted.data||mounted.size!=member.size||std::memcmp(mounted.data,member.data,member.size)!=0){
    error="Shared UI canonical Popup PSM source unavailable";return SharedUiBootResult::unavailable;
   }
   auto source=std::make_shared<rr::assets::OriginalStandardMaterialSource50LA>();
   if(!source->prepare(*mounted.member,std::vector<std::uint8_t>(mounted.data,mounted.data+mounted.size),sourceDiagnostic.document)){
    error="Shared UI canonical Popup PSM source rejected";return SharedUiBootResult::unavailable;
   }
   materials.push_back(std::move(source));
  }
  std::vector<rr::assets::OriginalStandardTextureSourceInput50LA> textures;
  for(std::size_t i=0;i<popup->memberCount();++i){
   const auto member=popup->member(i);if(!member.identity||!member.data)continue;
   std::string name=member.identity->name;for(char& c:name)if(c>='A'&&c<='Z')c=char(c-'A'+'a');
   if(name.size()<4||name.compare(name.size()-4,4,".pst"))continue;
   rr::assets::OriginalStandardArchiveProvider50LA::MemberView mounted;
   if(globalsProvider->lookup132820(member.identity->name,mounted)!=rr::assets::OriginalStandardArchiveProvider50LA::Lookup::present||
      !mounted.member||!mounted.data||mounted.size!=member.size||std::memcmp(mounted.data,member.data,member.size)!=0){
    error="Shared UI canonical Popup PST source unavailable";return SharedUiBootResult::unavailable;
   }
   textures.push_back({*mounted.member,std::vector<std::uint8_t>(mounted.data,mounted.data+mounted.size)});
  }
  if(!standardMaterialDomainOwner_->appendOriginalSources(materials,textures,sourceDiagnostic)){
   error="Shared UI canonical Popup corpus rejected at source PC "+std::to_string(sourceDiagnostic.sourcePC);
   return SharedUiBootResult::unavailable;
  }
 }catch(...){error="Shared UI Popup source admission allocation failed";return SharedUiBootResult::unavailable;}
 const auto& q=state->controllerPopupState;
 if(state->controllerPopupResources||(!successor&&(q.delta3C0!=0||q.timer3C4!=0||q.active3CC||q.closing3C8||q.hasConfirm3D0||q.hasCancel3D4||q.result3D8))){
  error="Existing47C native history cannot be replaced by a fresh shared constructor";return SharedUiBootResult::unavailable;}
 rr::platform::OriginalApplicationAural50KG::Diagnostic audioInitializer;
 if(!sourceAural00Owner_||
    !sourceAural00Owner_->canActivateUiInitializer1B7340(audioInitializer)||
    !(successor?sourceAural00Owner_->canInitializeUiReentryReverb232C70(audioInitializer):
      sourceAural00Owner_->canInitializeUiReverb232C70(audioInitializer))){
  error="Actual UI audio initializer preflight rejected at source PC "+std::to_string(audioInitializer.sourcePC);
  return SharedUiBootResult::unavailable;
 }
 try {
  auto owner=std::shared_ptr<Owner>(new Owner); //Native envelope allocation, not a source counted prefix.
  std::shared_ptr<rr::game::OriginalUiEntityScope50KG> nextScope;
  const auto result=rr::game::OriginalUiEntityScope50KG::construct(this,entitiesOwner_,nextScope);
  if(!result.okay()){error="Actual UI dispatcher construction failed";return SharedUiBootResult::failed;}
  // Publish the actual constructed successor. The old receipt stays owned;
  // failed later initialization retains this new prefix and cannot replay it.
  sharedUiScopeOwner_=std::move(nextScope);owner->successorConstructor_=successor;
  // Genuine UI initializer effects precede any activated native page, Song,
  // source update or draw. Earlier immutable language/font preparation does
  // not publish a frame or play audio. Preserve actual Aural identity/order.
  if(!sourceAural00Owner_->activateUiInitializer1B7340(audioInitializer)||
     !(successor?sourceAural00Owner_->initializeUiReentryReverb232C70(audioInitializer):
      sourceAural00Owner_->initializeUiReverb232C70(audioInitializer))){
   error="Actual UI audio initializer retained failure at source PC "+std::to_string(audioInitializer.sourcePC);
   return SharedUiBootResult::failed;
  }

  rr::assets::OriginalStandardMaterialDiagnostic50LA collectorInitializer;
  if(!constructUiCollector1B73BC(collectorInitializer)){
   error="Actual first UI collector retained failure at source PC "+std::to_string(collectorInitializer.sourcePC);
   return SharedUiBootResult::failed;
  }
  //2A35E0 source missing-file return precedes shared470/47C construction.
  const auto globals=loadDispatcherGlobalWidgets2A35E0();
  if(!globals.okay()){
   const auto closed=sharedUiScopeOwner_->close();
   if(closed.okay())sharedUiScopeOwner_.reset();
   error="Actual global-widget load did not reach its absent-file return";
   return SharedUiBootResult::failed;
  }
  state->sharedDialogs=owner;
  Owner::Inputs in;in.root=this;in.domain=standardMaterialDomainOwner_;in.context=materialContextOwner_;
  in.video=videoOwner_;in.popup=std::move(popup);in.fonts=fonts328Owner_;
  in.materialDirectory=&materialDirectory36B970_;in.fontDirectory=&fontDirectory36B420_;
  Owner::Diagnostic d;
  if(!owner->bindInitial1B7BD0(sharedUiScopeOwner_,state,std::move(in),d)||!owner->load470203D40(d)||
     !owner->construct47C2054D0(d)||!owner->bindLoaded47C2055B0(std::move(resources),d)||
     !owner->reappendInitialMain1D7CF4(d)){
   error="Actual shared UI prefix retained at source PC "+std::to_string(d.sourcePC);return SharedUiBootResult::failed;}
  return SharedUiBootResult::complete;
 }catch(...){error="Shared UI boot allocation failed; actual committed prefixes retained";return SharedUiBootResult::failed;}
}
bool Application::finishSharedUiDispatcherAfterBanks1B7140(std::string& error)const noexcept {
 error.clear();const auto owner=state?state->sharedDialogs:nullptr;
 if(!owner)return !sharedUiScopeOwner_;
 if(owner->inputs_.root!=this||owner->scope_!=sharedUiScopeOwner_||owner->phase_!=rr::platform::OriginalUiSharedDialogs50KG::Phase::retired){
  error="Global dialog payloads must retire before the native UI banks and dispatcher";return false;}
 const auto result=sharedUiScopeOwner_->close();if(!result.okay()){error="Actual UI dispatcher still owns live children";return false;}
 // Preserve only the last completed lifetime, including exact 47C state.
 // A fresh constructor may replace that state, never a failed/active owner.
 retiredSharedUiDialogs_=owner;retiredControllerPopupState_=state->controllerPopupState;
 state->sharedDialogs.reset();return true;
}
}

#include "original_ui_dispatcher_transition50kg.inl"
