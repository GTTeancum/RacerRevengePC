#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_draw_contribution50kg.h"
#include "original_ui_garage_pod_scene50kg.h"
#include "original_ui_main_audio50kg.h"
#include "audio_mixer50hd.h"
#include "../../../src/game/original_ui_aperture_owner50kg.h"
#include <cmath>
#include <limits>
namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::runSharedHook1FAF10(std::uint32_t pc,Diagnostic& d)noexcept {
    if(sharedHookPending_||!sourceEntryReturned1FAE00()||!sceneBacking_||!sceneBacking_->main_)
        return fail(d,Status::runtimePending,pc);
    if(pc!=0x1FAF10&&pc!=0x1FAF70&&pc!=0x1FAFC0&&pc!=0x1FB0C0&&pc!=0x1D7A00)
        return fail(d,Status::runtimePending,pc);
    auto& main=*sceneBacking_->main_;
    sharedHookPending_=true;sharedHookPC_=pc;
    // A failure keeps the exact source prefix and the pending hook. No later
    // call can replay master, music, group, or child effects to hide it.
    if(pc==0x1FAF10){
        if(!main.mixer_||!main.canStartGaragePageSong6_234900(*this))
            return fail(d,Status::runtimePending,0x1FAF40);
        sharedHookPC_=0x1FAF40;
        if(!main.applyGarageMasterReverb26A820(*this,25,main.mixer_->mixedFrames()))
            return fail(d,Status::runtimePending,sharedHookPC_);
        d.sourceEffectsCommitted=true;sharedHookPC_=0x1FAF58;
        if(!main.startGaragePageSong6_234900(*this))return fail(d,Status::runtimePending,sharedHookPC_);
    }else if(pc==0x1FAF70){
        shared_.field42C=3;d.sourceEffectsCommitted=true;sharedHookPC_=0x1FAF90;
        if(!runtimeSetActiveGroup2A6BE0(2,1,0,d))return false;
        sharedHookPC_=0x1FAFA0;
        if(!runtimeGroupEnable(1,true,d))return false;
    }else if(pc==0x1FAFC0){
        shared_.field42C=0;d.sourceEffectsCommitted=true;sharedHookPC_=0x1FAFDC;
        if(!runtimeGroupEnable(1,false,d))return false;
        constexpr std::uint32_t calls[6]{0x1FAFF8,0x1FB014,0x1FB030,0x1FB04C,0x1FB068,0x1FB084};
        for(std::size_t i=0;i<6;++i){sharedHookPC_=calls[i];
            if(!widgets_||!widgets_->managerTable154_||!widgets_->slotsKnown_[10+i]||
               !runtimeWidgetCall(widgets_->managerTable154_[10+i],0x7C,0,calls[i],d))return false;
        }
        sharedHookPC_=0x1FB09C;
        if(!main.fadeGaragePageSong6_234440(*this))return fail(d,Status::runtimePending,sharedHookPC_);
    }else if(pc==0x1FB0C0){
        sharedHookPC_=0x1FB0E0;
        if(!main.mixer_||!main.applyGarageMasterReverb26A820(*this,0,main.mixer_->mixedFrames()))
            return fail(d,Status::runtimePending,sharedHookPC_);
        d.sourceEffectsCommitted=true;
    }
    // Actual Page vtable3D0A10+BC is the literal jr ra at1D7A00.
    sharedHookPending_=false;sharedHookPC_=0;d.sourcePC=pc;return true;
}
bool OriginalUiGaragePageOwner50KG::beginSharedManagerTransition1FEA10(
    const OriginalUiGarageSession50KG& session,Diagnostic& d)noexcept {
    d={};if(session.page_.get()!=this||!sourceEntryReturned1FAE00()||!shared_.state4B4||sharedHookPending_)
        return fail(d,Status::runtimePending,0x1FEA10);
    // The callback argument is the actual manager170, not a UI readiness bit.
    if(session.managerTransition170_!=1){d.sourcePC=0x1FEA4C;return true;}
    const auto state=*shared_.state4B4;
    if(state==0||state==1){d.sourcePC=0x1D7528;return true;}
    if(state!=2&&state!=3)return fail(d,Status::runtimePending,0x1D74E0);
    shared_.state4B4=1;d.sourceEffectsCommitted=true;
    if(!runSharedHook1FAF10(0x1FAF10,d))return false;
    d.sourcePC=0x1FEA4C;return true;
}
bool OriginalUiGaragePageOwner50KG::pollSharedManagerTransition1FE7C0(
    const OriginalUiGarageSession50KG& session,bool& done,Diagnostic& d)const noexcept {
    d={};done=false;
    if(session.page_.get()!=this||!sourceEntryReturned1FAE00()||!shared_.state4B4||
       sharedHookPending_||!std::isfinite(panelOffset4D8_))return fail(d,Status::runtimePending,0x1FE7C0);
    if(session.managerTransition170_==1)done=*shared_.state4B4==0&&panelOffset4D8_==0.f;
    else if(session.managerTransition170_==2)done=*shared_.state4B4==2&&panelOffset4D8_==140.f;
    d.sourcePC=0x1FE848;return true;
}
bool OriginalUiGaragePageOwner50KG::advanceSharedApertures1D7550(
    const OriginalUiGarageSession50KG& session,OriginalUiGarageDrawContribution50KG& draw,Diagnostic& d)noexcept {
    if(session.page_.get()!=this||!shared_.state4B4||!shared_.speed4B8||
       !shared_.top284||!shared_.bottom28C||sharedHookPending_||
       !std::isfinite(shared_.savedDelta298)||shared_.savedDelta298<0||
       !std::isfinite(*shared_.speed4B8)||*shared_.speed4B8<0||
       shared_.openSize274<0||shared_.openSize274>224)
        return fail(d,Status::runtimePending,0x1D7550);
    const auto state=*shared_.state4B4;
    if(state==0||state==2)return true;
    if(state!=1&&state!=3)return fail(d,Status::runtimePending,0x1D7550);
    const auto writeStep=[&](bool opening)noexcept {
        volatile float product=*shared_.speed4B8*shared_.savedDelta298;std::int32_t step=0;
        if(!game::convertOriginalUiApertureCvtWS50KG(product,step))return false;
        const std::int64_t top=std::int64_t(*shared_.top284)+(opening?-std::int64_t(step):step);
        const std::int64_t bottom=std::int64_t(*shared_.bottom28C)+(opening?std::int64_t(step):-std::int64_t(step));
        if(top<std::numeric_limits<std::int32_t>::min()||top>std::numeric_limits<std::int32_t>::max()||
           bottom<std::numeric_limits<std::int32_t>::min()||bottom>std::numeric_limits<std::int32_t>::max())return false;
        shared_.top284=static_cast<std::int32_t>(top);shared_.bottom28C=static_cast<std::int32_t>(bottom);
        d.sourceEffectsCommitted=true;return true;
    };
    if(state==3){
        if(*shared_.bottom28C<=224&&*shared_.top284>=224){
            //16C==3 is the genuine wait-screen-exit state, including Back.
            //1D75A4 draws the retained loading ScreenText440 BEFORE the bars.
            if(session.managerLifecycle16C_==3){
                if(!sharedText440_.value||!sharedText440_.value->coldInputsCurrent()||
                   sharedText440_.value->nativeGarageFifoPins_||draw.activePageCostText_||draw.activePageQuad_)
                    return fail(d,Status::runtimePending,0x1D75A4);
                drawSourcePC_=0x1D75A4;draw.activePageCostText_=sharedText440_.value;
                if(!sharedText440_.value->draw2E0170(draw,d.text))
                    return fail(d,Status::runtimePending,d.text.sourcePC);
                draw.activePageCostText_=nullptr;
            }
            shared_.bottom28C=224;shared_.top284=224;shared_.state4B4=2;d.sourceEffectsCommitted=true;
            return runSharedHook1FAF10(0x1D7A00,d);
        }
        if(!writeStep(false))return fail(d,Status::runtimePending,0x1D75D0);
        if(*shared_.bottom28C<=224&&*shared_.top284>=224)return runSharedHook1FAF10(0x1FB0C0,d);
    }else{
        if(*shared_.bottom28C>=448-shared_.openSize274&&*shared_.top284<=shared_.openSize274){
            shared_.bottom28C=448-shared_.openSize274;shared_.top284=shared_.openSize274;
            shared_.state4B4=0;base_.input268=1;d.sourceEffectsCommitted=true;
            return runSharedHook1FAF10(0x1FAF70,d);
        }
        if(!writeStep(true))return fail(d,Status::runtimePending,0x1D76D0);
    }
    return true;
}
}
