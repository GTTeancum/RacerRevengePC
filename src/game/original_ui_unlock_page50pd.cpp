#include "original_ui_unlock_page50pd.h"
#include <cmath>
#include <utility>

namespace rr::game {
namespace {
using S=OriginalUiOptionsMenuStatus50KG;using P=OriginalUiUnlockPlan50PD;using State=OriginalUiUnlockState50PD;
using O=OriginalUiControlOperation50KG;
bool callback(P& p,std::uint16_t id,O op,bool v,std::uint32_t pc) noexcept {
    if(!id||id>originalUiUnlockWidgetCount50PD||p.callbackCount==p.callbacks.size())return false;
    p.callbacks[p.callbackCount++]={id,op,v,pc};return true;
}
bool control(P& p,std::int32_t id,O op,bool v,std::uint32_t pc) noexcept {
    if(id==-1)return true;
    if(id<0||std::size_t(id)>=p.state.navigation.page->controls.size())return false;
    for(auto w:p.state.navigation.page->controls[std::size_t(id)].widgets)if(!callback(p,w,op,v,pc))return false;
    auto& c=p.state.navigation.controls[std::size_t(id)];
    switch(op){case O::select:c.selected=true;break;case O::deselect:case O::clearSelection:c.selected=false;break;
        case O::show:c.shown=true;break;case O::hide:c.shown=false;break;case O::setEnabled:c.enabled=v;break;case O::reset:break;}
    return true;
}
bool group(P& p,unsigned id,O op,bool v,std::uint32_t pc,std::uint32_t assoc) noexcept {
    const auto& g=p.state.navigation.page->groups[id];
    for(auto c:g.controls)if(!control(p,c,op,v,pc))return false;
    for(auto c:g.associations)if(!control(p,c,op,v,assoc))return false;
    if(op==O::setEnabled)p.state.navigation.groups[id].enabled=v;
    return true;
}
// 2A5000 group SetEnabled (controls 2A5044, associations 2A5088).
bool enabled(P& p,unsigned id,bool v) noexcept {return group(p,id,O::setEnabled,v,0x2A5044,0x2A5088);}
// 2A4E40: associations deselect, then controls deselect or (KeepEnabled) clear.
bool deselectGroup(P& p,unsigned id) noexcept {
    const auto& g=p.state.navigation.page->groups[id];
    for(auto c:g.associations)if(!control(p,c,O::deselect,false,0x2A4E78))return false;
    for(auto c:g.controls)if(!control(p,c,g.keepEnabled?O::clearSelection:O::deselect,false,g.keepEnabled?0x2A4F10:0x2A4EC4))return false;
    return true;
}
// 2A4F50(group,slot): zero-control groups return before +3C; +3C=slot,
// every control deselected (2A4FA8), then the slot's control selected (2A4FD4).
bool selectSlot(P& p,unsigned id,std::uint16_t slot) noexcept {
    const auto& g=p.state.navigation.page->groups[id];
    if(g.controls.empty())return true;
    if(!slot||slot>g.controls.size())return false;
    p.state.navigation.groups[id].selectedSlot=slot;
    for(auto c:g.controls)if(!control(p,c,O::deselect,false,0x2A4FA8))return false;
    return control(p,g.controls[slot-1u],O::select,false,0x2A4FD4);
}
bool valid(const State& s,const assets::OriginalUiPage50KG* page) noexcept {
    return page&&s.navigation.page==page&&s.pod2D40>=-1&&s.pod2D40<27&&(s.inputFlags42C==0||s.inputFlags42C==1)&&
        s.pending.galleryTier<=3&&(s.pending.pending[7]==(s.pending.galleryTier!=0));
}
bool checksValid(const OriginalUnlockChecks50PD& c) noexcept {return c.galleryTier<=3&&c.pending[7]==(c.galleryTier!=0);}
}
const char* originalUiUnlockGalleryTexture50PD(OriginalUiUnlockGallery50PD g) noexcept {
    // ELF 3B0AB0 / 3B0AC0 / 3B0AD0.
    switch(g){case OriginalUiUnlockGallery50PD::podracers:return "artpod01";
        case OriginalUiUnlockGallery50PD::pilots:return "artpil01";
        case OriginalUiUnlockGallery50PD::planets:return "artpla01";
        default:return nullptr;}
}
struct OriginalUiUnlockPage50PD::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    assets::OriginalUiDat50KG uiDat,pageDat,widgets;assets::OriginalUiPageIndex50KG index;assets::OriginalUiPage50KG page;
    std::array<std::int32_t,assets::originalUiControlLimit50KG> pods{};
};
OriginalUiUnlockPage50PD::OriginalUiUnlockPage50PD() noexcept=default;
OriginalUiUnlockPage50PD::~OriginalUiUnlockPage50PD()=default;
S OriginalUiUnlockPage50PD::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> a,assets::OriginalUiPageDiagnostic50KG& d) noexcept {
    d={};if(storage_)return S::alreadyBound;
    if(!a||!a->loaded()||a->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::unlock))return S::invalidArchive;
    try {
        auto s=std::make_unique<Storage>();s->archive=std::move(a);s->pods.fill(-1);
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UIUnlock/ui.dat",s->uiDat,d)||
           !assets::readOriginalUiPageIndex50KG(s->uiDat,s->index,d))return S::parseFailure;
        // 1D17D0 is the only page factory: ui.dat TotalPages=1, Page 1 unlock.dat.
        if(s->index.pageFiles!=std::vector<std::string>{"unlock.dat"})return S::invalidPage;
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UIUnlock/unlock.dat",s->pageDat,d)||
           !assets::readOriginalUiDat50KG(*s->archive,"UI/UIUnlock/Widgets.Dat",s->widgets,d,
               assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !assets::readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d))return S::parseFailure;
        const auto& pg=s->page;
        if(pg.pageNo!=1||pg.widgetCount!=originalUiUnlockWidgetCount50PD||pg.groups.size()!=3||pg.controls.size()!=17||
           pg.groups[0].controls!=std::vector<std::int16_t>{0}||pg.groups[1].controls!=std::vector<std::int16_t>{16}||
           pg.groups[2].controls.size()!=15||!pg.groups[0].keepEnabled||pg.groups[1].keepEnabled||pg.groups[2].keepEnabled)
            return S::invalidPage;
        // 20A850 control factory: PodUIPodControl reads PodIndex into +40.
        for(std::size_t c=1;c<=15;++c){
            const auto* section=s->pageDat.section("Control "+std::to_string(c));
            if(!section||!section->find("PodIndex")||
               !assets::originalUiInteger50KG(*section,"PodIndex",-1,s->pods[c],d,0x20A850))return S::parseFailure;
            if(s->pods[c]<0||s->pods[c]>26||pg.controls[c].widgets.size()!=3)return S::invalidPage;
            for(std::size_t o=1;o<c;++o)if(s->pods[o]==s->pods[c])return S::invalidPage;
        }
        for(auto c:pg.groups[2].controls)if(c<1||c>15)return S::invalidPage;
        storage_=std::move(s);return S::ok;
    }catch(...){return S::allocation;}
}
const assets::OriginalUiPage50KG* OriginalUiUnlockPage50PD::page() const noexcept {return storage_?&storage_->page:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiUnlockPage50PD::pageDat() const noexcept {return storage_?&storage_->pageDat:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiUnlockPage50PD::widgetsDat() const noexcept {return storage_?&storage_->widgets:nullptr;}
std::int32_t OriginalUiUnlockPage50PD::podIndex(std::size_t c) const noexcept {return storage_&&c<storage_->pods.size()?storage_->pods[c]:-1;}
std::int32_t OriginalUiUnlockPage50PD::podControl(std::int32_t pod) const noexcept {
    if(!storage_||pod<0)return -1;
    for(auto c:storage_->page.groups[2].controls)if(storage_->pods[std::size_t(c)]==pod)return c;
    return -1;
}
bool OriginalUiUnlockPage50PD::initialState(State& out) const noexcept {
    if(!storage_)return false;State s;if(!bindOriginalUiNavigation50KG(storage_->page,s.navigation))return false;
    s.pod2D40=-1; // ctor 209AC8..209AD0
    out=s;return true;
}
S OriginalUiUnlockPage50PD::phase(const State& s,OriginalUiUnlockPhase50PD phase,const OriginalUnlockChecks50PD& checks,
    std::int32_t rivalPod,P& out) const noexcept {
    if(!storage_)return S::notBound;if(!valid(s,page()))return S::invalidInput;
    P p;p.state=s;bool ok=true;
    switch(phase){
    case OriginalUiUnlockPhase50PD::requestedPage90:case OriginalUiUnlockPhase50PD::previousPage94:{
        const bool req=phase==OriginalUiUnlockPhase50PD::requestedPage90;
        if(req)p.state.navigation.controllerGroup={{-1,-1}};
        for(unsigned g=0;g<3&&ok;++g)ok=group(p,g,req?O::reset:O::hide,false,req?0x2A57D8:0x2A4D58,req?0x2A5818:0x2A4D8C);
        break;}
    case OriginalUiUnlockPhase50PD::entry74:{
        if(!checksValid(checks))return S::invalidInput;
        p.state.loaded=true;
        p.state.inputFlags42C=0;p.writesInputFlags=true;          // 20AE5C
        ok=deselectGroup(p,2);                                    // 20AEA4
        p.state.pod2D40=-1;                                        // 20AEB0
        p.state.pending=checks;p.state.refreshText2D64=true;       // 20AEC4..20AF58
        ++p.state.activations;
        const auto first=checks.first();
        if(ok&&first>=0&&first<7){
            // 20AF68: R = signed byte P+11280+12*byte(P+100C4+40*187B10(P));
            // checks 2..7 use their fixed pods. Every group-2 control whose
            // +40 equals the pod is selected through 2A4F50 (no early break).
            const auto pod=first==0?rivalPod:originalUnlockFixedPod50PD[std::size_t(first)];
            if(pod<0||pod>26)return S::invalidInput;
            p.state.pod2D40=pod;
            const auto& g=page()->groups[2];
            for(std::size_t i=0;i<g.controls.size()&&ok;++i)
                if(storage_->pods[std::size_t(g.controls[i])]==pod)ok=selectSlot(p,2,static_cast<std::uint16_t>(i+1));
        }else if(ok&&first==7){
            p.setsPageBackground8C=true;p.pageBackground8C=static_cast<OriginalUiUnlockGallery50PD>(checks.galleryTier);
        }
        if(ok)ok=enabled(p,1,false);                               // 20B348..20B350
        p.clearScene3D8=true;p.applyBackground430=true;            // 20B398..20B3B8
        break;}
    case OriginalUiUnlockPhase50PD::openingAC:
        if(!s.loaded)return S::invalidInput;
        p.startSong7=true;break;                                   // 20B3E0 -> 234900(stream,7,1)
    case OriginalUiUnlockPhase50PD::openedB0:
        if(!s.loaded)return S::invalidInput;
        p.state.inputFlags42C=1;p.writesInputFlags=true;ok=enabled(p,1,true);break; // 20B400
    case OriginalUiUnlockPhase50PD::closingB4:
        p.state.inputFlags42C=0;p.writesInputFlags=true;ok=enabled(p,1,false);p.fadeMusic=true;break; // 20B420
    case OriginalUiUnlockPhase50PD::closedB8:
        ok=enabled(p,2,false);break;                               // 20B470
    }
    if(!ok)return S::capacity;out=p;return S::ok;
}
S OriginalUiUnlockPage50PD::input(const State& s,std::uint8_t controller,std::uint8_t event,std::uint32_t mode34,P& out) const noexcept {
    if(!storage_)return S::notBound;if(!valid(s,page())||controller>1||event>15)return S::invalidInput;
    P p;p.state=s;p.pageAccepted=true;                             // 20B6F8 returns 1 on every path
    if(controller!=0||event!=4){out=p;return S::noChange;}         // 20B48C / 20B4A0
    // 20B4B0..20B5E4: grant ONLY the first pending flag and clear it.
    const auto first=s.pending.first();
    if(first>=0){p.grantRequested=true;p.grantCheck=std::size_t(first);p.grantTier=first==7?s.pending.galleryTier:0;
        p.state.pending.pending[std::size_t(first)]=false;if(first==7)p.state.pending.galleryTier=0;}
    auto& r=p.route;r.inputPC=0x20B480;r.profileProposalComplete=true;
    r.action=OriginalUiRouteAction50KG::unavailable;r.blockers=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner);
    if(p.state.pending.any()){
        // 20B648: vtable+A4(this,1) re-shows page 1 (manager page request).
        p.next=OriginalUiUnlockNext50PD::reshow;
        r.request={OriginalUiRouteKind50KG::changePage,originalUiUnlockScreen50PD,originalUiUnlockPage50PD,
            originalUiUnlockScreen50PD,originalUiUnlockPage50PD,0x20B654};
    }else{
        // 20B664..20B6B4: profile mode34==2 -> Garage 13, else Course 6.
        const bool garage=mode34==2;
        p.next=OriginalUiUnlockNext50PD::exit;
        r.request={OriginalUiRouteKind50KG::changeScreen,originalUiUnlockScreen50PD,originalUiUnlockPage50PD,
            static_cast<std::uint8_t>(garage?13:6),1,garage?0x20B690u:0x20B6B0u};
    }
    // 20B6B8..20B6EC: 292360(App2D8+40,1,0) then 267BE0, after either request.
    p.soundRequested=true;p.soundId=1;p.soundPC=0x20B6D0;p.flushPC=0x20B6E8;
    out=p;return S::unavailable;
}
S OriginalUiUnlockPage50PD::update(const State& s,float dt,P& out) const noexcept {
    if(!storage_)return S::notBound;if(!valid(s,page())||!std::isfinite(dt)||dt<0)return S::invalidInput;
    P p;p.state=s;p.saveUpdateDelta=true;p.updateDelta=dt;out=p;return S::ok; // 20CA34 swc1 f12,298
}
bool OriginalUiUnlockPage50PD::composeText(const State& s,OriginalUiUnlockTextPlan50PD& out,State& after) const noexcept {
    if(!storage_||!valid(s,page())||!s.refreshText2D64)return false;
    OriginalUiUnlockTextPlan50PD t;const auto first=s.pending.first();
    if(first==0){t.colorPod=s.pod2D40;t.rivalName=true;t.sourcePC=0x20B74C;if(t.colorPod<0)return false;}
    else if(first>=1&&first<7){t.colorPod=originalUnlockFixedPod50PD[std::size_t(first)];
        t.nameLoc=originalUiUnlockFixedNameLoc50PD[std::size_t(first)];t.sourcePC=0x20B954;}
    else if(first==7){
        if(s.pending.galleryTier<1||s.pending.galleryTier>3)return false;
        t.gallery=true;t.galleryTier=static_cast<OriginalUiUnlockGallery50PD>(s.pending.galleryTier);
        t.nameLoc=static_cast<std::int32_t>(originalUiUnlockGalleryLoc50PD[s.pending.galleryTier-1u]);t.sourcePC=0x20C44C;
    }else t.sourcePC=0x20C99C; // no flag: 20C450 skips straight to the clear
    after=s;after.refreshText2D64=false;out=t;return true;          // 20C99C
}
bool originalUiUnlockRouteShape50PD(const OriginalUiRouteDecision50KG& d,OriginalUiUnlockNext50PD next) noexcept {
    const auto& r=d.request;
    if(r.sourceScreen!=10||r.sourcePage!=1||d.inputPC!=0x20B480||!d.profileProposalComplete||d.profileChanges.count)return false;
    if(next==OriginalUiUnlockNext50PD::reshow)
        return r.kind==OriginalUiRouteKind50KG::changePage&&r.targetScreen==10&&r.targetPage==1&&r.sourcePC==0x20B654;
    if(next==OriginalUiUnlockNext50PD::exit)
        return r.kind==OriginalUiRouteKind50KG::changeScreen&&r.targetPage==1&&
            ((r.targetScreen==13&&r.sourcePC==0x20B690)||(r.targetScreen==6&&r.sourcePC==0x20B6B0));
    return false;
}
}
