#include "original_ui_memory_card50kg.h"
#include "scalar_minmax48o.h"
#include <cfenv>
#include <cmath>

namespace rr::game {
namespace {
using State=OriginalUiMemoryCardState50KG;
using Plan=OriginalUiMemoryCardPlan50KG;
using Status=OriginalUiMemoryCardStatus50KG;
using Phase=OriginalUiOptionsMenuPhase50KG;
using Op=OriginalUiControlOperation50KG;
float rounded(float value) noexcept { volatile float result=value;return result; }
void place(State& s,float half,float edgeHeight) noexcept {
    const float top=rounded(216.f-half),bottom=rounded(216.f+half);
    s.strips[0]={0,rounded(top-16.f),640,edgeHeight};
    s.strips[1]={0,bottom,640,edgeHeight};
    s.strips[2]={0,top,640,rounded(bottom-top)};
}
void widget(Plan& p,std::uint16_t id,Op op,bool value,std::uint32_t pc) noexcept {
    p.callbacks[p.callbackCount++]={id,op,value,pc};
}
Status begin(const State& s,Plan& p,std::uint32_t pc) noexcept {
    p={};p.state=s;p.sourcePC=pc;
    return OriginalUiMemoryCard50KG::valid(s)?Status::ok:Status::invalidState;
}
void inputFlags(Plan& p,std::uint32_t flags) noexcept {
    p.state.inputFlags42C=flags;p.writesPageInputFlags42C=true;p.pageInputFlags42C=flags;
}
}
State OriginalUiMemoryCard50KG::initialState() noexcept { return {}; }
bool OriginalUiMemoryCard50KG::valid(const State& s) noexcept {
    if(!std::isfinite(s.timer398)||s.timer398>.25f||!std::isfinite(s.timer39C)||s.timer39C>.75f||
       !std::isfinite(s.delta3A0)||s.delta3A0<0||!std::isfinite(s.updateDelta298)||s.updateDelta298<0||
       s.pollTimer4DC!=0||s.load4D4||s.save4D8||s.value3B8!=0)return false;
    for(auto id:s.textIds)if(id!=-1)return false;
    for(auto color:s.color3E0)if(color!=0)return false;
    for(const auto& q:s.strips)if(!std::isfinite(q.x)||!std::isfinite(q.y)||!std::isfinite(q.width)||
        !std::isfinite(q.height)||q.x!=0||q.width!=640||q.height<0)return false;
    if(!s.entered)return s.dialogState30==0&&!s.dialogActive&&!s.completed3A4&&!s.closing3AC&&
        !s.closeOnDone3A8&&!s.animate3B0&&!s.resetMode3B4&&!s.inputBlocked4CC&&!s.callback4C8&&s.inputFlags42C==0&&
        s.timer398==0&&s.timer39C==0&&s.delta3A0==0&&s.updateDelta298==0;
    if(s.dialogState30!=4||!s.dialogActive||!s.animate3B0||!s.resetMode3B4)return false;
    if(s.closing3AC){
        if(!s.closeOnDone3A8||!s.inputBlocked4CC||s.inputFlags42C!=0)return false;
        if(s.completed3A4!=(s.timer398<=0))return false;
    }else if(s.closeOnDone3A8||s.completed3A4||s.inputBlocked4CC||s.inputFlags42C!=2)return false;
    return true;
}
Status OriginalUiMemoryCard50KG::phase(const State& s,Phase phase,Plan& p) noexcept {
    auto status=begin(s,p,0);if(status!=Status::ok)return status;
    switch(phase){
    case Phase::requestedPage90:
        p.sourcePC=0x1D7390;
        widget(p,2,Op::reset,false,0x2A57D8);widget(p,16,Op::reset,false,0x2A57D8);break;
    case Phase::previousPage94:
        p.sourcePC=0x2A6810;
        widget(p,2,Op::hide,false,0x2A4D58);widget(p,16,Op::hide,false,0x2A4D58);break;
    case Phase::entry74:{
        p.sourcePC=0x1F6910;
        // Source reset does not reset retained strip positions/material flags.
        const auto retained=s.strips;p.state={};p.state.strips=retained;p.state.updateDelta298=s.updateDelta298;
        p.state.entered=true;p.state.dialogActive=true;p.state.dialogState30=4;
        p.state.timer398=.25f;p.state.timer39C=.75f;p.state.animate3B0=true;p.state.resetMode3B4=true;
        inputFlags(p,2);
        widget(p,2,Op::setEnabled,true,0x1F6948);widget(p,16,Op::setEnabled,true,0x1F695C);
        widget(p,34,Op::setEnabled,false,0x1F6970);widget(p,21,Op::setEnabled,false,0x1F6984);
        widget(p,35,Op::setEnabled,false,0x1F6998);widget(p,22,Op::setEnabled,false,0x1F69AC);
        p.activateBorrowedCamera=true;p.applyPageSceneField430=true;
        p.resetGlobalPopupResult3D8=true;p.enableOwnedDialog=true;p.resetOwnedDialogText=true;break;
    }
    case Phase::openingAC:p.sourcePC=0x1D7520;break;
    case Phase::openedB0:p.sourcePC=0x1D79E0;break;
    case Phase::closingB4:p.sourcePC=0x1D74D0;break;
    default:return Status::invalidInput;
    }
    return Status::ok;
}
Status OriginalUiMemoryCard50KG::callback(const State& s,std::uint32_t event,Plan& p) noexcept {
    auto status=begin(s,p,0x1F74D0);if(status!=Status::ok)return status;
    if(!s.entered)return Status::invalidState;
    p.pageAccepted=true;if(event==2)p.state.callback4C8=true;return Status::ok;
}
Status OriginalUiMemoryCard50KG::input(const State& s,std::uint8_t controller,
    assets::OriginalUiMovementInput50KG input,bool reached,bool enabled,Plan& p) noexcept {
    auto status=begin(s,p,0x1F6AB0);if(status!=Status::ok)return status;
    const auto code=static_cast<unsigned>(input);
    if(controller>1||code>15)return Status::invalidInput;
    if(!s.entered)return Status::invalidState;
    if(!reached||!enabled)return Status::noChange;
    p.pageAccepted=true;
    if(s.inputBlocked4CC||controller!=0)return Status::noChange;
    if(code==5||code==7)return Status::unavailableStorage;
    if(code!=6)return Status::noChange;
    constexpr std::uint16_t ids[]={16,34,21,35,22,2};
    constexpr std::uint32_t pcs[]={0x1F6C70,0x1F6C84,0x1F6C98,0x1F6CAC,0x1F6CC0,0x1F6CD4};
    for(std::size_t i=0;i<6;++i)widget(p,ids[i],Op::setEnabled,false,pcs[i]);
    inputFlags(p,0);p.state.inputBlocked4CC=true;
    p.state.closeOnDone3A8=true;p.state.closing3AC=true;p.state.timer398=.25f;
    p.soundRequested=true;p.flushRequested=true;p.soundForce=false;p.soundId=5;p.soundFlags=1;
    p.soundPC=0x1F6D08;p.flushPC=0x1F6D20;
    // The same raw event subsequently reaches the dialog, but its positive
    // timer398 early-exit prevents another action. Do not invent consumption.
    return Status::ok;
}
Status OriginalUiMemoryCard50KG::update(const State& s,float delta,Plan& p) noexcept {
    auto status=begin(s,p,0x1F6D40);if(status!=Status::ok)return status;
    if(!s.entered)return Status::invalidState;
    if(!std::isfinite(delta)||delta<0||std::fegetround()!=FE_TONEAREST)return Status::invalidDelta;
    // In this envelope pollTimer4DC is always zero; never invoke1866E0.
    if(s.dialogActive&&s.completed3A4&&!s.callback4C8){p.returnToOptions=true;p.requestPC=0x1F6DB0;}
    p.state.updateDelta298=delta;p.saveUpdateDelta298=true;
    p.state.delta3A0=delta;p.updateDialogChildren=true;
    return Status::ok;
}
Status OriginalUiMemoryCard50KG::render(const State& s,OriginalUiMemoryCardRender50KG& p) noexcept {
    p={};p.state=s;p.strips=s.strips;p.page.state=s;p.page.sourcePC=0x1F6DE0;
    if(!valid(s)||!s.entered)return Status::invalidState;
    if(std::fegetround()!=FE_TONEAREST)return Status::invalidDelta;
    // 203B90 is tested before child render. Opening's crossing frame does NOT
    // expose prompts early; they appear on the following page render.
    if(!s.closing3AC&&s.timer398<=0&&!s.inputBlocked4CC){
        widget(p.page,34,Op::setEnabled,true,0x1F7464);widget(p.page,21,Op::setEnabled,true,0x1F7478);
        widget(p.page,35,Op::setEnabled,true,0x1F748C);widget(p.page,22,Op::setEnabled,true,0x1F74A0);
    }
    p.drawDialog=true;p.drawStrips=true;p.requiresDirectVideoSprite=true;
    if(s.timer398>0){
        const float ratio=rounded(s.timer398/.25f);
        const float fraction=s.closing3AC?ratio:rounded(1.f-ratio);
        const float half=detail::scalarMin48O(124.f,rounded(124.f*fraction));
        place(p.state,half,16.f);
        p.state.timer398=rounded(s.timer398-s.delta3A0);
        if(p.state.timer398<=0){
            if(s.closing3AC){place(p.state,0,0);p.state.completed3A4=true;}
            else place(p.state,124.f,16.f);
        }
    }else if(s.timer39C>0){
        p.state.timer39C=rounded(s.timer39C-s.delta3A0);
        // State4 does not match source completion states0,11,12,13,14.
    }
    p.strips=p.state.strips;
    return valid(p.state)?Status::ok:Status::invalidState;
}
bool sameOriginalUiMemoryCardState50KG(const State& a,const State& b) noexcept {
    if(a.entered!=b.entered||a.inputFlags42C!=b.inputFlags42C||a.callback4C8!=b.callback4C8||
       a.inputBlocked4CC!=b.inputBlocked4CC||a.load4D4!=b.load4D4||a.save4D8!=b.save4D8||
       a.pollTimer4DC!=b.pollTimer4DC||a.updateDelta298!=b.updateDelta298||a.dialogActive!=b.dialogActive||
       a.dialogState30!=b.dialogState30||a.timer398!=b.timer398||a.timer39C!=b.timer39C||a.delta3A0!=b.delta3A0||
       a.completed3A4!=b.completed3A4||a.closeOnDone3A8!=b.closeOnDone3A8||a.closing3AC!=b.closing3AC||
       a.animate3B0!=b.animate3B0||a.resetMode3B4!=b.resetMode3B4||a.value3B8!=b.value3B8||
       a.textIds!=b.textIds||a.color3E0!=b.color3E0)return false;
    for(std::size_t i=0;i<a.strips.size();++i){const auto& x=a.strips[i];const auto& y=b.strips[i];
        if(x.x!=y.x||x.y!=y.y||x.width!=y.width||x.height!=y.height)return false;}
    return true;
}
bool describeOriginalUiMemoryCardReturn50KG(const Plan& p,OriginalUiRouteDecision50KG& out) noexcept {
    if(!OriginalUiMemoryCard50KG::valid(p.state)||!p.state.entered||!p.state.dialogActive||
       !p.state.completed3A4||p.state.callback4C8||!p.returnToOptions||p.requestPC!=0x1F6DB0||p.sourcePC!=0x1F6D40)return false;
    OriginalUiRouteDecision50KG d;d.action=OriginalUiRouteAction50KG::unavailable;
    d.request={OriginalUiRouteKind50KG::changePage,12,5,12,1,0x1F6DB0};d.inputPC=0x1F6D40;
    d.profileProposalComplete=true;d.blockers=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner);
    out=d;return true;
}
bool originalUiMemoryCardReturnShape50KG(const OriginalUiRouteDecision50KG& d) noexcept {
    const auto& r=d.request;
    return r.kind==OriginalUiRouteKind50KG::changePage&&r.sourceScreen==12&&r.sourcePage==5&&
        r.targetScreen==12&&r.targetPage==1&&r.sourcePC==0x1F6DB0&&d.inputPC==0x1F6D40&&
        d.profileProposalComplete&&d.profileChanges.count==0&&d.selectedSlot==0&&d.selectedRider==-1;
}
}
