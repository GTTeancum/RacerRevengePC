#include "original_ui_game_options_controller50kg.h"
#include <utility>
namespace rr::game {
namespace {
using Status=OriginalUiOptionsMenuStatus50KG;
using Phase=OriginalUiOptionsMenuPhase50KG;
using Plan=OriginalUiGameOptionsPlan50KG;
using Op=OriginalUiControlOperation50KG;
using Nav=OriginalUiNavigationStatus50KG;
bool valid(const OriginalUiGameSettings50KG& s) noexcept {
    if(s.soundMode>2)return false;for(auto v:s.volume)if(v>100)return false;return true;
}
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
        case Op::show:s.shown=true;break;case Op::hide:s.shown=false;break;
        case Op::setEnabled:s.enabled=value;break;case Op::reset:break;}
    return true;
}
bool expand(Plan& p,const OriginalUiCommands50KG& c) noexcept {
    for(std::size_t i=0;i<c.count;++i){const auto& x=c.values[i];if(!control(p,x.controlId,x.operation,x.value,x.sourcePC))return false;}
    return true;
}
bool enter(Plan& p,std::uint16_t group,std::uint16_t slot) noexcept {
    OriginalUiCommands50KG c;return enterOriginalUiGroup50KG(p.state.navigation,group,slot,0,c)==Nav::ok&&expand(p,c);
}
bool sync(Plan& p) noexcept {
    const auto& s=p.state.local;
    const std::array<std::uint16_t,8> slots{{static_cast<std::uint16_t>(s.autoSave?1:2),
        static_cast<std::uint16_t>(s.soundMode+1),1,1,1,1,static_cast<std::uint16_t>(s.verticalSplit?2:1),
        static_cast<std::uint16_t>(s.vibration?1:2)}};
    for(std::uint16_t g=1;g<=8;++g){OriginalUiCommands50KG c;
        if(selectOriginalUiGroupSlot50KG(p.state.navigation,g,slots[g-1],c)!=Nav::ok||!expand(p,c))return false;}
    const auto g=p.state.navigation.controllerGroup[0];
    return g>=1&&g<=8&&enter(p,static_cast<std::uint16_t>(g),p.state.navigation.groups[g].selectedSlot);
}
bool descriptions(Plan& p,std::uint32_t pc) noexcept {
    constexpr std::array<std::uint16_t,8> ids{{156,157,158,159,160,167,161,162}};
    for(auto id:ids){if(!widget(p,id,Op::deselect,false,pc))return false;pc+=0x1c;}return true;
}
void sound(Plan& p,std::uint8_t id,std::uint32_t pc) noexcept {
    p.soundRequested=true;p.flushRequested=true;p.soundId=id;p.soundPC=pc;p.flushPC=pc+0x18;
}
}
void resetOriginalUiGameSettingsProfileFields50KG(OriginalUiGameSettingsProfile50KG& p) noexcept {
    p.savedGroup6E8=1;p.soundMode1101C=1;p.volume11020={{50,100,80,80}};
    p.flags11050=static_cast<std::uint8_t>((p.flags11050&~7u)|5u);
}
bool validOriginalUiGameSettingsProfile50KG(const OriginalUiGameSettingsProfile50KG& p) noexcept {
    if(!p.known||p.savedGroup6E8<1||p.savedGroup6E8>8||p.soundMode1101C>2)return false;
    for(auto v:p.volume11020)if(v>100)return false;return true;
}
struct OriginalUiGameOptionsController50KG::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    assets::OriginalUiDat50KG pageDat,widgets;
    assets::OriginalUiPage50KG page;
};
OriginalUiGameOptionsController50KG::OriginalUiGameOptionsController50KG() noexcept=default;
OriginalUiGameOptionsController50KG::~OriginalUiGameOptionsController50KG()=default;
Status OriginalUiGameOptionsController50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> a,
    assets::OriginalUiPageDiagnostic50KG& d) noexcept {
    d={};if(storage_)return Status::alreadyBound;
    if(!a||!a->loaded()||a->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options))return Status::invalidArchive;
    try{auto s=std::make_unique<Storage>();s->archive=std::move(a);
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI\\UIOption\\options.dat",s->pageDat,d)||
            !assets::readOriginalUiDat50KG(*s->archive,"UI\\UIOption\\Widgets.Dat",s->widgets,d,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
            !assets::readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d))return Status::parseFailure;
        if(s->page.pageNo!=4||s->page.widgetCount!=357||s->page.groups.size()!=9||s->page.controls.size()!=22)return Status::invalidPage;
        constexpr std::array<std::size_t,9> counts{{1,2,3,1,1,1,1,2,2}};
        for(std::size_t i=0;i<counts.size();++i)if(s->page.groups[i].controls.size()!=counts[i]||!s->page.groups[i].keepEnabled)return Status::invalidPage;
        storage_=std::move(s);return Status::ok;
    }catch(...){return Status::allocation;}
}
const assets::OriginalUiPage50KG* OriginalUiGameOptionsController50KG::page() const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiGameOptionsController50KG::widgetsDat() const noexcept{return storage_?&storage_->widgets:nullptr;}
bool OriginalUiGameOptionsController50KG::initialState(OriginalUiGameOptionsState50KG& out) const noexcept {
    if(!storage_)return false;OriginalUiGameOptionsState50KG s;
    if(!bindOriginalUiNavigation50KG(storage_->page,s.navigation))return false;out=s;return true;
}
Status OriginalUiGameOptionsController50KG::phase(const OriginalUiGameOptionsState50KG& state,Phase phase,
    const OriginalUiGameSettingsProfile50KG& profile,Plan& out) const noexcept {
    if(!storage_)return Status::notBound;if(state.navigation.page!=page())return Status::invalidPage;
    Plan p;p.state=state;bool okay=true;
    switch(phase){
    case Phase::requestedPage90:case Phase::previousPage94:
        if(phase==Phase::requestedPage90)p.state.navigation.controllerGroup={{-1,-1}};
        for(const auto& g:page()->groups){
            for(auto c:g.controls)if(okay)okay=control(p,c,phase==Phase::requestedPage90?Op::reset:Op::hide,false,phase==Phase::requestedPage90?0x2A57D8:0x2A4D58);
            for(auto c:g.associations)if(okay)okay=control(p,c,phase==Phase::requestedPage90?Op::reset:Op::hide,false,phase==Phase::requestedPage90?0x2A5818:0x2A4D8C);
        }break;
    case Phase::entry74:
        if(!profile.known)return Status::invalidInput;
        p.state.local={bool(profile.flags11050&1),bool(profile.flags11050&4),bool(profile.flags11050&2),profile.soundMode1101C,profile.volume11020};
        if(!valid(p.state.local))return Status::invalidInput;p.state.loaded=true;
        p.writesPageInputFlags42C=true;
        {constexpr std::array<std::uint16_t,5> ids{{125,126,127,3,15}};
            std::uint32_t pc=0x1F5010;for(auto id:ids){okay=okay&&widget(p,id,Op::setEnabled,false,pc);pc+=0x14;}}
        break;
    case Phase::openingAC:
        if(!state.loaded||!valid(state.local)||!profile.known||profile.savedGroup6E8<1||profile.savedGroup6E8>8)return Status::invalidInput;
        okay=enter(p,static_cast<std::uint16_t>(profile.savedGroup6E8),1)&&sync(p)&&descriptions(p,0x1F5C38);break;
    case Phase::openedB0:
        if(!state.loaded||!valid(state.local))return Status::invalidInput;
        p.writesPageInputFlags42C=true;p.pageInputFlags42C=7;
        okay=widget(p,125,Op::setEnabled,true,0x1F5D44)&&widget(p,126,Op::setEnabled,true,0x1F5D58)&&
            widget(p,127,Op::setEnabled,true,0x1F5D6C)&&widget(p,3,Op::setEnabled,true,0x1F5D80)&&
            widget(p,3,Op::deselect,false,0x1F5D9C)&&widget(p,15,Op::setEnabled,true,0x1F5DB0)&&sync(p);break;
    case Phase::closingB4:
        p.writesPageInputFlags42C=true;
        okay=widget(p,125,Op::setEnabled,false,0x1F5DF0)&&widget(p,126,Op::setEnabled,false,0x1F5E04)&&
            widget(p,127,Op::setEnabled,false,0x1F5E18)&&widget(p,15,Op::setEnabled,false,0x1F5E2C)&&descriptions(p,0x1F5E48);break;
    default:return Status::invalidInput;
    }
    if(!okay)return Status::capacity;out=p;return Status::ok;
}
Status OriginalUiGameOptionsController50KG::input(const OriginalUiGameOptionsState50KG& state,
    std::uint8_t controller,assets::OriginalUiMovementInput50KG input,bool reached,bool enabled,Plan& out) const noexcept {
    if(!storage_)return Status::notBound;if(state.navigation.page!=page())return Status::invalidPage;
    const auto event=static_cast<std::uint8_t>(input);if(controller>=2||event>=16)return Status::invalidInput;
    Plan p;p.state=state;
    if(!reached||!enabled){out=p;return Status::noChange;}p.pageAccepted=true;
    if(controller||event>6){out=p;return Status::noChange;}
    if(!state.loaded||!valid(state.local))return Status::invalidInput;
    const auto group=state.navigation.controllerGroup[0];if(group<1||group>8)return Status::invalidInput;
    if(event==4||event==6){
        p.intent=event==4?OriginalUiGameOptionsIntent50KG::accept:OriginalUiGameOptionsIntent50KG::cancel;
        p.rememberedGroup6E8=static_cast<std::uint16_t>(group);
        p.rememberedGroupPC=event==4?0x1F5158:0x1F51C8;p.requestPC=event==4?0x1F5168:0x1F51D8;
        // Source sound follows successful profile/apply + manager request. The
        // plan cannot execute either effect; session acceptance must own this.
        p.soundId=event==4?1:5;p.soundPC=event==4?0x1F5188:0x1F51F8;p.flushPC=p.soundPC+0x18;
        out=p;return Status::unavailable;
    }
    if(event==5){p.state.local={true,true,false,1,{{50,100,80,80}}};
        if(!sync(p))return Status::navigationFailure;sound(p,2,0x1F5248);out=p;return Status::ok;}
    OriginalUiCommands50KG commands;
    if(event==2||event==3){
        const auto status=moveOriginalUiGroup50KG(p.state.navigation,0,event==3,commands);
        if(status!=Nav::ok||!expand(p,commands))return Status::navigationFailure;
        sound(p,event==2?3:4,event==2?0x1F5B58:0x1F5BA0);out=p;return Status::ok;
    }
    if(moveOriginalUiControl50KG(p.state.navigation,static_cast<std::uint16_t>(group),event==1,commands)!=Nav::ok||!expand(p,commands))return Status::navigationFailure;
    bool clamped=false;auto& s=p.state.local;
    if(group==1)s.autoSave=!s.autoSave;
    else if(group==2)s.soundMode=p.state.navigation.groups[2].selectedSlot-1;
    else if(group>=3&&group<=6){auto& v=s.volume[group-3];
        if(event==0){clamped=v<10;v=clamped?0:v-10;}else{clamped=v>90;v=clamped?100:v+10;}}
    else if(group==7)s.verticalSplit=!s.verticalSplit;else s.vibration=!s.vibration;
    constexpr std::array<std::uint32_t,8> left{{0x1F52F0,0x1F5384,0x1F5420,0x1F54BC,0x1F5558,0x1F55F4,0x1F5648,0x1F569C}};
    constexpr std::array<std::uint32_t,8> right{{0x1F5744,0x1F57D8,0x1F587C,0x1F5920,0x1F59C4,0x1F5A68,0x1F5ABC,0x1F5B10}};
    const auto pc=(event==0?left:right)[group-1];sound(p,clamped?7:(event==0?3:4),clamped?pc-0x40:pc);
    out=p;return Status::ok;
}
std::uint16_t OriginalUiGameOptionsController50KG::selectedValueWidget(const OriginalUiGameOptionsState50KG& s) const noexcept {
    if(!storage_||s.navigation.page!=page()||!s.loaded)return 0;
    const auto g=s.navigation.controllerGroup[0];if(g<1||g>8)return 0;
    const auto slot=s.navigation.groups[g].selectedSlot;
    if(!slot||slot>page()->groups[g].controls.size())return 0;
    const auto c=page()->groups[g].controls[slot-1];
    if(c<0||page()->controls[c].widgets.size()!=1)return 0;return page()->controls[c].widgets[0];
}
}
