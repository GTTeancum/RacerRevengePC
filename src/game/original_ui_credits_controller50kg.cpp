#include "original_ui_credits_controller50kg.h"
#include <algorithm>
#include <cfenv>
#include <cmath>
#include <utility>

namespace rr::game {
namespace {
using Status=OriginalUiOptionsMenuStatus50KG;
using Phase=OriginalUiOptionsMenuPhase50KG;
using Plan=OriginalUiCreditsPlan50KG;
using State=OriginalUiCreditsState50KG;
using Op=OriginalUiControlOperation50KG;
using Nav=OriginalUiNavigationStatus50KG;
bool widget(Plan& p,std::uint16_t id,Op op,bool value,std::uint32_t pc) noexcept {
    if(!id||id>357||p.callbackCount==p.callbacks.size())return false;
    p.callbacks[p.callbackCount++]={id,op,value,pc};return true;
}
bool control(Plan& p,std::int32_t id,Op op,bool value,std::uint32_t pc) noexcept {
    if(id==-1)return true;
    if(id<0||static_cast<std::size_t>(id)>=p.state.navigation.page->controls.size())return false;
    for(auto w:p.state.navigation.page->controls[id].widgets)if(!widget(p,w,op,value,pc))return false;
    auto& s=p.state.navigation.controls[id];
    switch(op){case Op::select:s.selected=true;break;case Op::deselect:case Op::clearSelection:s.selected=false;break;
        case Op::show:s.shown=true;break;case Op::hide:s.shown=false;break;case Op::setEnabled:s.enabled=value;break;case Op::reset:break;}
    return true;
}
bool expand(Plan& p,const OriginalUiCommands50KG& c) noexcept {
    for(std::size_t i=0;i<c.count;++i){const auto& x=c.values[i];if(!control(p,x.controlId,x.operation,x.value,x.sourcePC))return false;}return true;
}
bool group(Plan& p,std::uint16_t id,Op op,bool value,std::uint32_t pc,std::uint32_t associationPC) noexcept {
    const auto& g=p.state.navigation.page->groups[id];
    for(auto c:g.controls)if(!control(p,c,op,value,pc))return false;
    for(auto c:g.associations)if(!control(p,c,op,value,associationPC))return false;
    if(op==Op::setEnabled)p.state.navigation.groups[id].enabled=value;
    return true;
}
bool alphaControl(Plan& p,std::int32_t c,std::uint8_t alpha,std::uint32_t pc) noexcept {
    if(c<0||static_cast<std::size_t>(c)>=p.state.navigation.page->controls.size())return false;
    for(auto w:p.state.navigation.page->controls[c].widgets){if(p.alphaCount==p.alphaWrites.size())return false;p.alphaWrites[p.alphaCount++]={w,alpha,pc};}return true;
}
bool validState(const State& s,const assets::OriginalUiPage50KG* p) noexcept {
    return p&&s.navigation.page==p&&s.phase7D4<=3&&std::isfinite(s.remaining7D8)&&
        s.navigation.groups[1].selectedSlot>=1&&s.navigation.groups[1].selectedSlot<=15;
}
}
struct OriginalUiCreditsController50KG::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    assets::OriginalUiDat50KG pageDat,widgets;
    assets::OriginalUiPage50KG page;
    OriginalUiCreditsConfig50KG config;
};
OriginalUiCreditsController50KG::OriginalUiCreditsController50KG() noexcept=default;
OriginalUiCreditsController50KG::~OriginalUiCreditsController50KG()=default;
Status OriginalUiCreditsController50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> a,assets::OriginalUiPageDiagnostic50KG& d) noexcept {
    d={};if(storage_)return Status::alreadyBound;
    if(!a||!a->loaded()||a->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options))return Status::invalidArchive;
    try{auto s=std::make_unique<Storage>();s->archive=std::move(a);
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI\\UIOption\\credits.dat",s->pageDat,d)||
            !assets::readOriginalUiDat50KG(*s->archive,"UI\\UIOption\\Widgets.Dat",s->widgets,d,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
            !assets::readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d))return Status::parseFailure;
        const auto* info=s->pageDat.section("PageInfo");if(!info)return Status::invalidPage;
        auto& c=s->config;
        if(!assets::originalUiInteger50KG(*info,"LECBeginControl",-1,c.lecBeginControl7DC,d,0x1F7610)||
            !assets::originalUiNumber50KG(*info,"FadeInTime",1,c.fadeIn7E0,d,0x1F762C)||
            !assets::originalUiNumber50KG(*info,"FadeOutTime",1,c.fadeOut7E4,d,0x1F7648)||
            !assets::originalUiNumber50KG(*info,"OnTime",3,c.on7E8,d,0x1F7664)||
            !assets::originalUiNumber50KG(*info,"OffTime",.25f,c.off7EC,d,0x1F7680))return Status::parseFailure;
        const std::array<std::int16_t,15> ids{{1,10,2,11,3,12,4,5,13,6,14,7,15,8,9}};
        const std::array<std::size_t,15> counts{{14,14,16,14,9,12,12,13,13,13,15,14,12,15,2}};
        if(s->page.pageNo!=6||s->page.widgetCount!=357||s->page.groups.size()!=2||s->page.controls.size()!=16||
            s->page.groups[0].controls!=std::vector<std::int16_t>{0}||s->page.controls[0].widgets!=std::vector<std::uint16_t>({11,17})||
            s->page.groups[1].controls.size()!=15||!s->page.groups[0].keepEnabled||!s->page.groups[1].keepEnabled||
            !s->page.groups[0].associations.empty()||!s->page.groups[1].associations.empty()||
            c.lecBeginControl7DC!=8||c.fadeIn7E0!=.5f||c.fadeOut7E4!=.5f||c.on7E8!=6||c.off7EC!=.5f)return Status::invalidPage;
        for(std::size_t i=0;i<ids.size();++i)if(s->page.groups[1].controls[i]!=ids[i]||s->page.controls[ids[i]].widgets.size()!=counts[i])return Status::invalidPage;
        storage_=std::move(s);return Status::ok;
    }catch(...){return Status::allocation;}
}
const assets::OriginalUiPage50KG* OriginalUiCreditsController50KG::page() const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiCreditsController50KG::widgetsDat() const noexcept{return storage_?&storage_->widgets:nullptr;}
const OriginalUiCreditsConfig50KG* OriginalUiCreditsController50KG::config() const noexcept{return storage_?&storage_->config:nullptr;}
bool OriginalUiCreditsController50KG::initialState(State& out) const noexcept {
    if(!storage_)return false;State s;if(!bindOriginalUiNavigation50KG(storage_->page,s.navigation))return false;out=s;return true;
}
Status OriginalUiCreditsController50KG::phase(const State& s,Phase phase,bool known,std::uint32_t mode34,Plan& out) const noexcept {
    if(!storage_)return Status::notBound;if(!validState(s,page()))return Status::invalidPage;
    Plan p;p.state=s;bool ok=true;
    switch(phase){
    case Phase::requestedPage90:case Phase::previousPage94:
        if(phase==Phase::requestedPage90)p.state.navigation.controllerGroup={{-1,-1}};
        for(std::uint16_t g=0;g<2&&ok;++g)ok=group(p,g,phase==Phase::requestedPage90?Op::reset:Op::hide,false,
            phase==Phase::requestedPage90?0x2A57D8:0x2A4D58,phase==Phase::requestedPage90?0x2A5818:0x2A4D8C);break;
    case Phase::entry74:{
        p.state.loaded=true;p.state.phase7D4=0;p.state.remaining7D8=config()->fadeIn7E0;
        p.writesPageInputFlags42C=true;p.state.inputFlags42C=0;
        ok=widget(p,11,Op::setEnabled,false,0x1F83C8)&&widget(p,3,Op::setEnabled,false,0x1F83DC)&&widget(p,17,Op::setEnabled,false,0x1F83F0);
        p.alphaAfterCallbackCount=3;
        for(auto c:page()->groups[1].controls)if(ok)ok=alphaControl(p,c,0,0x1F8428);
        ok=ok&&group(p,1,Op::setEnabled,true,0x2A5044,0x2A5088)&&group(p,1,Op::show,false,0x2A4CA8,0x2A4CDC);
        OriginalUiCommands50KG commands;if(ok)ok=selectOriginalUiGroupSlot50KG(p.state.navigation,1,1,commands)==Nav::ok&&expand(p,commands);
        p.activateBorrowedCamera=true;p.clearSceneField3D8=true;p.applyPageSceneField430=true;break;}
    case Phase::openingAC:
        if(!s.loaded)return Status::invalidInput;p.startMusic12=true;p.musicPC=0x1F8860;break;
    case Phase::openedB0:
        if(!s.loaded||!known)return Status::invalidInput;
        if(mode34!=2){p.state.inputFlags42C=2;p.writesPageInputFlags42C=true;p.pageInputFlags42C=2;ok=widget(p,11,Op::setEnabled,true,0x1F88A8);}
        ok=ok&&widget(p,17,Op::setEnabled,true,0x1F88BC)&&widget(p,3,Op::setEnabled,true,0x1F88D0)&&widget(p,3,Op::deselect,false,0x1F88EC);break;
    case Phase::closingB4:
        p.state.inputFlags42C=0;p.writesPageInputFlags42C=true;
        ok=widget(p,11,Op::setEnabled,false,0x1F8930)&&widget(p,17,Op::setEnabled,false,0x1F8944);
        p.fadeMusic=true;p.musicFadeSeconds=1;p.musicPC=0x1F895C;break;
    default:return Status::invalidInput;
    }
    if(!ok)return Status::capacity;out=p;return Status::ok;
}
Status OriginalUiCreditsController50KG::input(const State& s,std::uint8_t controller,assets::OriginalUiMovementInput50KG input,
    bool reached,bool enabled,Plan& out) const noexcept {
    if(!storage_)return Status::notBound;if(!validState(s,page()))return Status::invalidPage;
    if(controller>=2||static_cast<unsigned>(input)>=16)return Status::invalidInput;
    Plan p;p.state=s;if(!reached||!enabled){out=p;return Status::noChange;}p.pageAccepted=true;
    if(!(s.inputFlags42C&2)||controller||static_cast<unsigned>(input)!=6){out=p;return Status::noChange;}
    if(!s.loaded)return Status::invalidInput;
    p.intent=OriginalUiCreditsIntent50KG::optionsPage1;p.requestPC=0x1F856C;
    p.soundRequested=true;p.flushRequested=true;p.soundId=5;p.soundPC=0x1F858C;p.flushPC=0x1F85A4;
    out=p;return Status::unavailable;
}
Status OriginalUiCreditsController50KG::update(const State& s,float dt,std::uint32_t phase170,bool predicateKnown,bool predicate,Plan& out) const noexcept {
    if(!storage_)return Status::notBound;if(!validState(s,page()))return Status::invalidPage;
    if(!s.loaded||!std::isfinite(dt)||dt<0||std::fegetround()!=FE_TONEAREST)return Status::invalidInput;
    Plan p;p.state=s;p.saveUpdateDelta298=true;p.updateDelta298=dt;
    if(phase170==1||phase170==2){out=p;return Status::ok;}
    // Source tests remaining<0 BEFORE subtraction. Do not use <=, catch up in
    // a loop, or preserve overshoot: each update advances at most one phase.
    if(!(s.remaining7D8<0)){p.state.remaining7D8=s.remaining7D8-dt;if(!std::isfinite(p.state.remaining7D8))return Status::invalidInput;out=p;return Status::ok;}
    const auto& c=*config();
    if(s.phase7D4==0){p.state.phase7D4=1;p.state.remaining7D8=c.on7E8;}
    else if(s.phase7D4==1){p.state.phase7D4=2;p.state.remaining7D8=c.fadeOut7E4;}
    else if(s.phase7D4==2){p.state.phase7D4=3;p.state.remaining7D8=c.off7EC;}
    else {
        const auto slot=s.navigation.groups[1].selectedSlot;
        p.state.phase7D4=0;p.state.remaining7D8=c.fadeIn7E0;
        if(slot<15){
            if(!alphaControl(p,page()->groups[1].controls[slot],0,0x1F86C8))return Status::capacity;
            OriginalUiCommands50KG commands;
            if(moveOriginalUiControl50KG(p.state.navigation,1,true,commands)!=Nav::ok)return Status::navigationFailure;
            if(!expand(p,commands))return Status::capacity;
        }else{
            if(s.navigation.groups[1].enabled){
                if(!(s.inputFlags42C&2)&&!predicateKnown){Plan blocked;blocked.state=s;blocked.completionPredicateRequired=true;out=blocked;return Status::unavailable;}
                if(!group(p,1,Op::setEnabled,false,0x2A5044,0x2A5088)||!group(p,1,Op::hide,false,0x2A4D58,0x2A4D8C))return Status::capacity;
                p.intent=(s.inputFlags42C&2)?OriginalUiCreditsIntent50KG::optionsPage1:
                    predicate?OriginalUiCreditsIntent50KG::unlockScreen10:OriginalUiCreditsIntent50KG::garageScreen13;
                p.requestPC=(s.inputFlags42C&2)?0x1F8744:predicate?0x1F877C:0x1F879C;
            }
            p.state.phase7D4=3;p.state.remaining7D8=-1;
        }
    }
    out=p;return p.intent==OriginalUiCreditsIntent50KG::none?Status::ok:Status::unavailable;
}
Status OriginalUiCreditsController50KG::render(const State& s,std::uint32_t phase170,OriginalUiCreditsRender50KG& out) const noexcept {
    if(!storage_)return Status::notBound;if(!validState(s,page()))return Status::invalidPage;
    if(!s.loaded||std::fegetround()!=FE_TONEAREST)return Status::invalidInput;
    OriginalUiCreditsRender50KG r;if(phase170==1||phase170==2){out=r;return Status::ok;}
    const auto& c=*config();const float remaining=std::max(s.remaining7D8,0.0f);
    float alpha=0;
    if(s.phase7D4==0){const float ratio=remaining/c.fadeIn7E0;const float fraction=1.0f-ratio;alpha=255.0f*fraction;}
    else if(s.phase7D4==1)alpha=255;
    else if(s.phase7D4==2){const float ratio=remaining/c.fadeOut7E4;alpha=255.0f*ratio;}
    if(!std::isfinite(alpha)||alpha<0||alpha>255)return Status::invalidInput;
    // EE CVT.W.S truncates; alpha is nonnegative and bounded here.
    r.cardAlpha=static_cast<std::uint8_t>(alpha);r.drawCredits=true;
    r.selectedSlot=s.navigation.groups[1].selectedSlot;r.controlId=page()->groups[1].controls[r.selectedSlot-1];
    const auto slot=r.selectedSlot;const auto state=s.phase7D4;const auto boundary=c.lecBeginControl7DC;
    if((slot==1&&state==0)||(slot+1==boundary&&state==2)){r.drawRainLogo=true;r.rainAlpha=r.cardAlpha;}
    else if(slot<boundary&&!(slot+1==boundary&&state==3)){r.drawRainLogo=true;r.rainAlpha=255;}
    if((slot==boundary&&state==0)||(slot==15&&(state==2||state==3))){r.drawLecLogo=true;r.lecAlpha=r.cardAlpha;}
    else if(slot>=boundary&&slot<=15){r.drawLecLogo=true;r.lecAlpha=255;}
    out=r;return Status::ok;
}
bool describeOriginalUiCreditsReturn50KG(const Plan& p,OriginalUiRouteDecision50KG& out) noexcept {
    if(!p.state.loaded||!p.state.navigation.page||p.state.navigation.page->pageNo!=6||p.intent==OriginalUiCreditsIntent50KG::none)return false;
    if(p.intent!=OriginalUiCreditsIntent50KG::optionsPage1&&p.intent!=OriginalUiCreditsIntent50KG::unlockScreen10&&p.intent!=OriginalUiCreditsIntent50KG::garageScreen13)return false;
    if((p.intent==OriginalUiCreditsIntent50KG::optionsPage1&&p.requestPC!=0x1F856C&&p.requestPC!=0x1F8744)||
        (p.intent==OriginalUiCreditsIntent50KG::unlockScreen10&&p.requestPC!=0x1F877C)||
        (p.intent==OriginalUiCreditsIntent50KG::garageScreen13&&p.requestPC!=0x1F879C))return false;
    OriginalUiRouteDecision50KG d;d.action=OriginalUiRouteAction50KG::unavailable;d.profileProposalComplete=true;
    d.inputPC=p.requestPC==0x1F856C?0x1F8520:0x1F85C0;
    if(p.intent==OriginalUiCreditsIntent50KG::optionsPage1)d.request={OriginalUiRouteKind50KG::changePage,12,6,12,1,p.requestPC};
    else d.request={OriginalUiRouteKind50KG::changeScreen,12,6,static_cast<std::uint8_t>(p.intent==OriginalUiCreditsIntent50KG::unlockScreen10?10:13),1,p.requestPC};
    d.blockers=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner);out=d;return true;
}
bool originalUiCreditsReturnShape50KG(const OriginalUiRouteDecision50KG& d) noexcept {
    const auto& r=d.request;
    return r.kind==OriginalUiRouteKind50KG::changePage&&r.sourceScreen==12&&r.sourcePage==6&&r.targetScreen==12&&r.targetPage==1&&
        ((r.sourcePC==0x1F856C&&d.inputPC==0x1F8520)||(r.sourcePC==0x1F8744&&d.inputPC==0x1F85C0))&&
        d.profileProposalComplete&&d.profileChanges.count==0&&d.selectedSlot==0&&d.selectedRider==-1;
}
}
