#include "original_ui_controls_controller50kg.h"
#include <utility>

namespace rr::game {
namespace {
using Status=OriginalUiOptionsMenuStatus50KG;
using State=OriginalUiControlsState50KG;
using Plan=OriginalUiControlsPlan50KG;
using Phase=OriginalUiControlsPhase50KG;
using Intent=OriginalUiControlsIntent50KG;
using Kind=OriginalUiControlsEffectKind50KG;
using Model=OriginalUiControlsModelAction50KG;
using Scene=OriginalUiControlsSceneAction50KG;
using Op=OriginalUiControlOperation50KG;
bool effect(Plan& p,const OriginalUiControlsEffect50KG& e) noexcept {
    if(p.effectCount==p.effects.size())return false;
    p.effects[p.effectCount++]=e;return true;
}
bool widget(Plan& p,std::uint16_t id,Op op,bool value,std::uint32_t pc) noexcept {
    if(!id||id>357)return false;
    OriginalUiControlsEffect50KG e;e.widgetId=id;e.operation=op;e.value=value;e.sourcePC=pc;
    return effect(p,e);
}
bool text(Plan& p,std::uint16_t id,std::uint16_t string,std::uint32_t pc) noexcept {
    OriginalUiControlsEffect50KG e;e.kind=Kind::localizedText;e.widgetId=id;e.stringId=string;e.sourcePC=pc;
    return effect(p,e);
}
bool model(Plan& p,Model action,std::uint32_t pc) noexcept {
    OriginalUiControlsEffect50KG e;e.kind=Kind::model;e.widgetId=36;e.modelAction=action;e.sourcePC=pc;
    return effect(p,e);
}
bool scene(Plan& p,Scene action,std::uint32_t pc) noexcept {
    OriginalUiControlsEffect50KG e;e.kind=Kind::scene;e.sceneAction=action;e.sourcePC=pc;return effect(p,e);
}
bool control(Plan& p,std::int32_t id,Op op,bool value,std::uint32_t pc) noexcept {
    if(id==-1)return true;
    if(id<0||std::size_t(id)>=p.state.navigation.page->controls.size())return false;
    for(auto member:p.state.navigation.page->controls[std::size_t(id)].widgets)
        if(!widget(p,member,op,value,pc))return false;
    auto& s=p.state.navigation.controls[std::size_t(id)];
    switch(op){case Op::select:s.selected=true;break;case Op::deselect:case Op::clearSelection:s.selected=false;break;
    case Op::show:s.shown=true;break;case Op::hide:s.shown=false;break;case Op::setEnabled:s.enabled=value;break;case Op::reset:break;}
    return true;
}
bool group(Plan& p,std::size_t id,Op op,bool value,std::uint32_t memberPC,std::uint32_t assocPC) noexcept {
    if(id>=p.state.navigation.page->groups.size())return false;
    const auto& g=p.state.navigation.page->groups[id];
    for(auto c:g.controls)if(!control(p,c,op,value,memberPC))return false;
    for(auto c:g.associations)if(!control(p,c,op,value,assocPC))return false;
    return true;
}
bool enable(Plan& p,std::size_t id,bool value) noexcept {
    if(id>=p.state.navigation.page->groups.size())return false;
    p.state.navigation.groups[id].enabled=value;
    return group(p,id,Op::setEnabled,value,0x2A5044,0x2A5088);
}
bool enableSlot(Plan& p,std::size_t groupId,std::size_t oneBasedSlot,std::uint32_t pc) noexcept {
    if(groupId>=p.state.navigation.page->groups.size())return false;
    const auto& g=p.state.navigation.page->groups[groupId];
    if(!oneBasedSlot||oneBasedSlot>g.controls.size())return false;
    return control(p,g.controls[oneBasedSlot-1],Op::setEnabled,true,pc);
}
void sound(Plan& p,std::uint8_t id,std::uint32_t pc) noexcept {
    p.soundRequested=true;p.flushRequested=true;p.soundId=id;p.soundPC=pc;p.flushPC=pc+0x18;
}
std::int64_t signedWord(std::uint32_t v) noexcept {
    return v<=0x7fffffffu?std::int64_t(v):std::int64_t(v)-0x100000000ll;
}
// Source1F40F0. Row entries identify original button/action codes. Names and
// authored text are supplied by the real localized string owner, never guessed.
bool actionLabel(Plan& p,std::int32_t code,std::uint16_t label) noexcept {
    std::size_t g=0,slot=0;std::uint16_t id=0;std::uint32_t enablePC=0,textPC=0;
    if(p.state.frontView4CC){
        g=1;
        switch(code){
        case 2:slot=3;id=83;enablePC=0x1F4170;textPC=0x1F4184;break;
        case 1:slot=6;id=82;enablePC=0x1F41A8;textPC=0x1F41BC;break;
        case 0:slot=5;id=81;enablePC=0x1F41E0;textPC=0x1F41F4;break;
        case 3:if(p.state.configuration4D4){slot=13;id=155;enablePC=0x1F4224;textPC=0x1F4238;}
               else{slot=4;id=84;enablePC=0x1F425C;textPC=0x1F4270;}break;
        case 9:if(p.state.configuration4D4){slot=12;id=150;enablePC=0x1F42A0;textPC=0x1F42B4;}
               else{slot=2;id=80;enablePC=0x1F42D8;textPC=0x1F42EC;}break;
        case 8:if(p.state.configuration4D4){slot=11;id=145;enablePC=0x1F431C;textPC=0x1F4330;}
               else{slot=1;id=77;enablePC=0x1F4354;textPC=0x1F4368;}break;
        default:return true;
        }
    }else{
        g=2;
        switch(code){
        case 6:slot=1;id=93;enablePC=0x1F43C4;textPC=0x1F43D8;break;
        case 4:slot=2;id=94;enablePC=0x1F43FC;textPC=0x1F4410;break;
        case 7:slot=3;id=95;enablePC=0x1F4434;textPC=0x1F4448;break;
        case 5:slot=4;id=96;enablePC=0x1F446C;textPC=0x1F4480;break;
        default:return true;
        }
    }
    return enableSlot(p,g,slot,enablePC)&&text(p,id,label,textPC);
}
bool labels(Plan& p,const ControlPresets49D& current) noexcept {
    if(!text(p,47,p.state.configuration4D4?71:70,p.state.configuration4D4?0x1F3DE4:0x1F3E00))return false;
    constexpr std::array<std::uint32_t,8> pcs{{0x1F3E40,0x1F3E5C,0x1F3E78,0x1F3E94,0x1F3EB0,0x1F3ECC,0x1F3EE8,0x1F3F04}};
    if(p.state.preset4D8<8&&!text(p,130,std::uint16_t(62+p.state.preset4D8),pcs[p.state.preset4D8]))return false;
    if(!enable(p,1,false)||!enable(p,2,false))return false;
    p.previewBefore=current;p.previewAfter=current;
    p.previewRow=previewControlPreset49D(p.previewAfter,p.state.preset4D8);p.previewPerformed=true;
    OriginalUiControlsEffect50KG commit;commit.kind=Kind::previewPresets;commit.sourcePC=0x1F3F3C;
    if(!effect(p,commit))return false;
    const auto& row=p.previewRow;
    const std::array<std::int32_t,10> codes{{row.action690,row.action694,row.flag698,row.flag69C,row.action6A0,
        row.action6A4,row.action6A8,row.action6AC,row.action6B0,row.flag6B4}};
    constexpr std::array<std::uint16_t,10> ids{{81,80,85,84,77,74,79,78,83,82}};
    for(std::size_t i=0;i<codes.size();++i)if(!actionLabel(p,codes[i],ids[i]))return false;
    if(p.state.frontView4CC){
        if(p.state.configuration4D4)return enableSlot(p,1,9,0x1F4068)&&enableSlot(p,1,10,0x1F4084);
        return enableSlot(p,1,7,0x1F40BC)&&enableSlot(p,1,8,0x1F40D8);
    }
    return true;
}
}
struct OriginalUiControlsController50KG::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    assets::OriginalUiDat50KG pageDat,widgets;
    assets::OriginalUiPage50KG page;
    std::uint8_t controllerIndex=0;
};
OriginalUiControlsController50KG::OriginalUiControlsController50KG() noexcept=default;
OriginalUiControlsController50KG::~OriginalUiControlsController50KG()=default;
Status OriginalUiControlsController50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> a,
    std::uint8_t controllerIndex,assets::OriginalUiPageDiagnostic50KG& d) noexcept {
    d={};if(storage_)return Status::alreadyBound;if(controllerIndex>=2)return Status::invalidInput;
    if(!a||!a->loaded()||a->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::options))return Status::invalidArchive;
    try{auto s=std::make_unique<Storage>();s->archive=std::move(a);s->controllerIndex=controllerIndex;
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI\\UIOption\\config.dat",s->pageDat,d)||
           !assets::readOriginalUiDat50KG(*s->archive,"UI\\UIOption\\Widgets.Dat",s->widgets,d,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !assets::readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d))return Status::parseFailure;
        if(s->page.pageNo!=2||s->page.widgetCount!=357||s->page.controls.size()!=19||s->page.groups.size()!=4)return Status::invalidPage;
        const std::array<std::vector<std::int16_t>,4> exact{{{0},{1,2,3,4,5,6,7,8,15,9,16,17,18},{10,11,12,13},{14,-1,-1,-1,-1,-1,-1,-1,-1,-1}}};
        for(std::size_t g=0;g<exact.size();++g)
            if(s->page.groups[g].controls!=exact[g]||!s->page.groups[g].associations.empty()||s->page.groups[g].keepEnabled!=(g!=0))return Status::invalidPage;
        storage_=std::move(s);return Status::ok;
    }catch(...){return Status::allocation;}
}
const assets::OriginalUiPage50KG* OriginalUiControlsController50KG::page() const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiControlsController50KG::widgetsDat() const noexcept{return storage_?&storage_->widgets:nullptr;}
bool OriginalUiControlsController50KG::initialState(State& out) const noexcept {
    if(!storage_)return false;State s;s.controllerIndex4DC=storage_->controllerIndex;
    if(!bindOriginalUiNavigation50KG(storage_->page,s.navigation))return false;out=s;return true;
}
Status OriginalUiControlsController50KG::phase(const State& state,Phase phase,
    const ControlPresets49D& presets,Plan& out) const noexcept {
    if(!storage_)return Status::notBound;
    if(state.navigation.page!=page()||state.controllerIndex4DC!=storage_->controllerIndex)return Status::invalidPage;
    Plan p;p.state=state;bool okay=true;
    switch(phase){
    case Phase::requestedPage90:case Phase::previousPage94:
        if(phase==Phase::requestedPage90)p.state.navigation.controllerGroup={{-1,-1}};
        for(std::size_t g=0;g<page()->groups.size()&&okay;++g)
            okay=group(p,g,phase==Phase::requestedPage90?Op::reset:Op::hide,false,
                phase==Phase::requestedPage90?0x2A57D8:0x2A4D58,phase==Phase::requestedPage90?0x2A5818:0x2A4D8C);
        break;
    case Phase::entry74:
        p.writesPageInputFlags42C=true;p.state.modelBlocked4D0=false;
        okay=widget(p,3,Op::setEnabled,false,0x1F3844)&&widget(p,124,Op::setEnabled,false,0x1F3858)&&
            enable(p,1,false)&&enable(p,2,false)&&enable(p,3,false)&&
            model(p,Model::bindEntryModel,0x1F38B0)&&model(p,Model::restoreEntryTransforms,0x1F38C4);
        p.state.frontView4CC=true;
        p.state.preset4D8=presets.slots[state.controllerIndex4DC].preset11048;
        p.state.configuration4D4=presets.slots[state.controllerIndex4DC].configuration11040;
        p.state.loaded=true;
        okay=okay&&scene(p,Scene::activateBorrowedCamera,0x1F38DC)&&scene(p,Scene::clearSceneField3D8,0x1F3930)&&
            scene(p,Scene::applyPageSceneField430,0x1F393C);break;
    case Phase::openingAC:
        if(!state.loaded)return Status::invalidInput;
        if(state.controllerIndex4DC==1)okay=scene(p,Scene::routeToSecondController,0x1F44C0);break;
    case Phase::openedB0:
        if(!state.loaded)return Status::invalidInput;
        p.writesPageInputFlags42C=true;p.pageInputFlags42C=15;okay=enable(p,3,true);
        if(!state.controllerIndex4DC)okay=okay&&widget(p,14,Op::setEnabled,false,0x1F4520)&&
            widget(p,3,Op::setEnabled,true,0x1F4534)&&widget(p,3,Op::deselect,false,0x1F4550);
        else okay=okay&&widget(p,13,Op::setEnabled,false,0x1F456C)&&
            widget(p,124,Op::setEnabled,true,0x1F4580)&&widget(p,124,Op::deselect,false,0x1F459C);
        break;
    case Phase::closingB4:
        p.writesPageInputFlags42C=true;okay=enable(p,3,false);break;
    case Phase::closingThresholdB8:
        if(state.controllerIndex4DC==1)okay=scene(p,Scene::routeToFirstController,0x1F4600);break;
    default:return Status::invalidInput;
    }
    if(!okay)return Status::capacity;out=p;return Status::ok;
}
Status OriginalUiControlsController50KG::input(const State& state,std::uint8_t controller,
    assets::OriginalUiMovementInput50KG input,bool reached,bool enabled,Plan& out) const noexcept {
    if(!storage_)return Status::notBound;
    if(state.navigation.page!=page()||state.controllerIndex4DC!=storage_->controllerIndex)return Status::invalidPage;
    const auto event=static_cast<std::uint8_t>(input);if(controller>=2||event>=16)return Status::invalidInput;
    Plan p;p.state=state;
    if(!reached||!enabled){out=p;return Status::noChange;}p.pageAccepted=true;
    if(controller!=state.controllerIndex4DC||event>=8||event==2||event==3){out=p;return Status::noChange;}
    if(!state.loaded)return Status::invalidInput;
    if(event==4||event==6){
        p.intent=event==4?Intent::accept:Intent::cancel;p.requestPC=event==4?0x1F39AC:0x1F3A04;
        // Sound must happen AFTER the caller's profile/manager transaction.
        p.soundId=event==4?1:5;p.soundPC=event==4?0x1F39CC:0x1F3A24;p.flushPC=p.soundPC+0x18;
        out=p;return Status::unavailable;
    }
    if(state.modelBlocked4D0){out=p;return Status::noChange;}
    if(event==0){
        --p.state.preset4D8;
        if(p.state.configuration4D4){if(signedWord(p.state.preset4D8)<4)p.state.preset4D8=7;}
        else if(signedWord(p.state.preset4D8)<0)p.state.preset4D8=3;
        sound(p,3,0x1F3BF0);
    }else if(event==1){
        ++p.state.preset4D8;
        if(p.state.configuration4D4){if(signedWord(p.state.preset4D8)>=8)p.state.preset4D8=4;}
        else if(signedWord(p.state.preset4D8)>=4)p.state.preset4D8=0;
        sound(p,4,0x1F3C8C);
    }else if(event==7){
        p.state.configuration4D4=state.configuration4D4?0u:1u;
        p.state.preset4D8+=p.state.configuration4D4?4u:std::uint32_t(-4);
        sound(p,2,0x1F3AA8);
    }else if(event==5){
        if(!model(p,state.frontView4CC?Model::selectBackMotion:Model::selectFrontMotion,state.frontView4CC?0x1F3AF8:0x1F3B10)||
           !model(p,Model::advanceRotation001,0x1F3B34))return Status::capacity;
        p.state.frontView4CC=!state.frontView4CC;sound(p,2,0x1F3B54);
    }
    out=p;return Status::ok;
}
Status OriginalUiControlsController50KG::render(const State& state,const ControlPresets49D& presets,
    bool observationKnown,bool animationWord10Nonzero,Plan& out) const noexcept {
    if(!storage_)return Status::notBound;
    if(state.navigation.page!=page()||state.controllerIndex4DC!=storage_->controllerIndex)return Status::invalidPage;
    if(!state.loaded)return Status::invalidInput;if(!observationKnown)return Status::unavailable;
    Plan p;p.state=state;p.state.modelBlocked4D0=!animationWord10Nonzero;
    for(std::uint8_t i=0;i<4;++i){OriginalUiControlsEffect50KG e;e.kind=Kind::drawBackground;e.ordinal=i;e.sourcePC=0x1F3D28;if(!effect(p,e))return Status::capacity;}
    if(!p.state.modelBlocked4D0){if(!labels(p,presets))return Status::capacity;}
    else if(!enable(p,state.frontView4CC?2:1,false))return Status::capacity;
    OriginalUiControlsEffect50KG e;e.kind=Kind::renderAperture;e.sourcePC=0x1F3D94;
    if(!effect(p,e))return Status::capacity;out=p;return Status::ok;
}
bool describeOriginalUiControlsReturn50KG(const State& state,Intent intent,OriginalUiRouteDecision50KG& out) noexcept {
    if(!state.loaded||state.controllerIndex4DC>=2||!state.navigation.page||state.navigation.page->pageNo!=2||
       (intent!=Intent::accept&&intent!=Intent::cancel))return false;
    OriginalUiRouteDecision50KG d;d.action=OriginalUiRouteAction50KG::unavailable;
    d.request={OriginalUiRouteKind50KG::changePage,12,std::uint8_t(2+state.controllerIndex4DC),12,1,
        intent==Intent::accept?0x1F39ACu:0x1F3A04u};
    d.inputPC=0x1F3950;d.profileProposalComplete=true;
    d.blockers=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner);
    out=d;return true;
}
bool originalUiControlsReturnShape50KG(const OriginalUiRouteDecision50KG& d) noexcept {
    const auto& r=d.request;
    return r.kind==OriginalUiRouteKind50KG::changePage&&r.sourceScreen==12&&(r.sourcePage==2||r.sourcePage==3)&&
        r.targetScreen==12&&r.targetPage==1&&(r.sourcePC==0x1F39AC||r.sourcePC==0x1F3A04)&&
        d.inputPC==0x1F3950&&d.profileProposalComplete&&d.profileChanges.count==0&&d.selectedSlot==0&&d.selectedRider==-1;
}
bool applyOriginalUiControlsAccept50KG(const State& state,const ControlPresets49D& presets,ControlPresets49D& out) noexcept {
    if(!state.loaded||state.controllerIndex4DC>=2||!state.navigation.page||state.navigation.page->pageNo!=2)return false;
    auto next=presets;if(!selectControlPreset49D(next,state.controllerIndex4DC,state.preset4D8))return false;
    out=next;return true;
}
bool sameOriginalUiControlsPresets50KG(const ControlPresets49D& a,const ControlPresets49D& b) noexcept {
    for(std::size_t i=0;i<controlPresetSlots49D;++i){const auto& x=a.slots[i];const auto& y=b.slots[i];
        const auto& p=x.actions;const auto& q=y.actions;
        if(p.action690!=q.action690||p.action694!=q.action694||p.flag698!=q.flag698||p.flag69C!=q.flag69C||
           p.action6A0!=q.action6A0||p.action6A4!=q.action6A4||p.action6A8!=q.action6A8||p.action6AC!=q.action6AC||
           p.action6B0!=q.action6B0||p.flag6B4!=q.flag6B4||x.configuration11040!=y.configuration11040||x.preset11048!=y.preset11048||
           x.field10F94!=y.field10F94||x.field10F9C!=y.field10F9C||x.field10FA4!=y.field10FA4||
           x.field11030!=y.field11030||x.field11038!=y.field11038)return false;
    }
    return true;
}
} // namespace rr::game
