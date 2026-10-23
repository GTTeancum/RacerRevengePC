#include "original_ui_garage_session50kg.h"
#include "../../../tests/native/original_ui_main_entry50kg.h"
namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::setRouterActivity1415A0(bool enabled,Diagnostic& d)noexcept {
    if(!sourceEntryReturned1FAE00()||!sharedCollectionsPayloadCurrent()||!router_||
       router_->sourceVtable!=0x3D3D00||base_.router134!=router_.get()||
       router_->entity.parent1C!=&entity_||router_->entity.firstChild18||
       !grant_->garageAttachedPageCurrent(*this,entity_))
        return fail(d,Status::runtimePending,enabled?0x141630:0x1415A0);
    // Actual Router vtable3D3D00 slots1C/24 are1415A0/141630. This
    // retained Router is genuinely childless; its eight routing HEADS are
    // not Entity children and must not be recursively toggled as widgets.
    auto& flags=router_->entity.flags29;
    flags=static_cast<std::uint8_t>((flags&~1u)|(enabled?1u:0u));
    flags=static_cast<std::uint8_t>((flags&~2u)|(enabled?2u:0u));
    d={};d.sourceEffectsCommitted=true;d.sourcePC=enabled?0x1416BC:0x14162C;return true;
}
bool OriginalUiGarageSession50KG::updateManagerState2A3C80(D& d)noexcept {
    d={};using Status=assets::OriginalStandardMaterialStatus50LA;
    auto reject=[&](std::uint32_t pc)noexcept{d.status=Status::source;d.sourcePC=pc;return false;};
    if(!page_||!canRequestPage1D6130(*page_)||runtimePhase_!=RuntimePhase::manager||
       managerLifecycleCallbackPending_||managerField15C_||!runtimeFrame_)
        return reject(0x2A3C80);
    if(managerRestart17C_){managerLifecycle16C_=2;managerRestart17C_=0;d.sourceEffectsCommitted=true;return true;}
    OriginalUiGaragePageOwner50KG::Diagnostic pd;
    auto pageFailure=[&]()noexcept{d.sourceEffectsCommitted=true;return reject(pd.sourcePC?pd.sourcePC:0x2A3D8C);};
    switch(managerLifecycle16C_){
    case 0: break;
    case 2:
        //Actual concrete manager3CF5A0+84=2A4330 is literal return.
        managerLifecycleCallbackPending_=true;
        managerLifecycle16C_=4;managerTransition170_=1;
        if(!page_->beginSharedManagerTransition1FEA10(*this,pd)||
           !page_->setRouterActivity1415A0(false,pd))return pageFailure();
        managerLifecycleCallbackPending_=false;d.sourceEffectsCommitted=true;break;
    case 6:
        managerLifecycleCallbackPending_=true;
        managerLifecycle16C_=3;managerTransition170_=2;
        if(!page_->beginSharedManagerTransition1FEA10(*this,pd))return pageFailure();
        managerLifecycleCallbackPending_=false;d.sourceEffectsCommitted=true;break;
    case 3:case 4:{
        const auto life=managerLifecycle16C_;
        if((life==3&&managerTransition170_!=2)||(life==4&&managerTransition170_!=1))
            return reject(life==3?0x2A3E50:0x2A3EA4);
        bool done=false;
        if(!page_->pollSharedManagerTransition1FE7C0(*this,done,pd))return pageFailure();
        if(!done)break;
        managerLifecycleCallbackPending_=true;
        if(!page_->setRouterActivity1415A0(life==4,pd))return pageFailure();
        managerLifecycle16C_=life==3?9u:0u;managerTransition170_=0;
        managerLifecycleCallbackPending_=false;d.sourceEffectsCommitted=true;break;
    }
    case 9:{
        //17DE10 queues type2 and the ACTUAL164/168, before15C/174 writes.
        //No successor constructor executes recursively inside this Root call.
        if(managerQueued17DE10_||!entryFrontend_||!managerPendingScreen164_||!managerPendingPage168_||
           *managerPendingScreen164_!=9||*managerPendingPage168_!=1||!page_->entryProfile_||
           !page_->entryProfile_->owner)return reject(0x2A3F70);
        const auto profile=page_->entryProfile_->owner;
        const auto stamp=profile->stamp();
        if(stamp.generation!=page_->entryProfile_->generation||stamp.mode34!=2||
           !entryFrontend_->coordinator.queueGarageScreen17DE10(*this,
               *managerPendingScreen164_,*managerPendingPage168_))return reject(0x2A3F70);
        returnProfile17DE10_=profile;returnProfileStamp17DE10_=stamp;
        managerQueued17DE10_=true;
        managerField15C_=1; //2A3F7C.
        managerField174_=0; //2A3F84.
        //The concrete3CF5A0+80 callback2A4320 is a literal return. The real
        //141ED0 child walk still follows this callback in the enclosing owner.
        d.sourceEffectsCommitted=true;break;
    }
    default:return reject(0x2A3C80);
    }
    d.sourcePC=0x2A3FBC;return true;
}
}
