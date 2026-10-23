#include "original_ui_garage_page_owner50kg.h"
#include "original_ui_garage_session50kg.h"
#include "original_ui_garage_watto_owner50kg.h"
#include <cmath>
namespace rr::platform {
bool OriginalUiGaragePageOwner50KG::updatePanel1FE860(
    const OriginalUiGarageSession50KG& session,float dt,Diagnostic& d)noexcept {
    if(session.page_.get()!=this||!sourceEntryReturned1FAE00()||!shared_.state4B4||
       !information4DC_||!entryField4E0_||sharedHookPending_||
       !std::isfinite(dt)||dt<0||!std::isfinite(panelOffset4D8_)||
       !std::isfinite(shared_.time4B0)||shared_.time4B0<=0)
        return fail(d,Status::runtimePending,0x1FE860);
    if(session.managerTransition170_==1){
        information4DC_=0;d.sourceEffectsCommitted=true;
        if(*shared_.state4B4!=0){d.sourcePC=0x1FE9FC;return true;}
        volatile float speed=140.f/shared_.time4B0;
        volatile float step=speed*dt;
        volatile float next=panelOffset4D8_-step;
        if(!std::isfinite(next))return fail(d,Status::runtimePending,0x1FE8C0);
        panelOffset4D8_=next<0?0.f:static_cast<float>(next);
    }else if(session.managerTransition170_==2){
        information4DC_=1;d.sourceEffectsCommitted=true;
        if(panelOffset4D8_<140.f){
            volatile float speed=140.f/shared_.time4B0;
            volatile float step=speed*dt;
            volatile float next=panelOffset4D8_+step;
            if(!std::isfinite(next))return fail(d,Status::runtimePending,0x1FE914);
            panelOffset4D8_=next; //Clamp is on the NEXT reached source branch.
        }else{
            //1FE920 stores v1 after1FE8E8 loaded0x430C0000: this is140.f,
            //not integer1 from the earlier1FE8DC assignment to the same reg.
            panelOffset4D8_=140.f;float remaining=0;OriginalUiGarageWattoOwner50KG::D wd;
            if(!watto33B0_||!watto33B0_->remainingMotion1D5C10(*this,remaining,wd))
                return fail(d,Status::runtimePending,wd.sourcePC?wd.sourcePC:0x1FE924);
            if(remaining<1.5f||*entryField4E0_){
                if(*shared_.state4B4!=2&&*shared_.state4B4!=3){
                    shared_.state4B4=3;base_.input268=0;
                    if(!runSharedHook1FAF10(0x1FAFC0,d))return false;
                }
            }
        }
    }else return fail(d,Status::runtimePending,0x1FE994);
    d.sourcePC=0x1FE9FC;return true;
}
}
