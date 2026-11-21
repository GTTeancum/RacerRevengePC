#include "original_ui_win_page50pd.h"
#include <cfenv>
#include <cmath>
#include <limits>
#include <new>
#include <utility>

namespace rr::game {
namespace {
using E=OriginalUiWinPageError50PD;using D=OriginalUiWinPageDiagnostic50PD;
using K=OriginalUiWinWidgetKind50PD;using O=OriginalUiControlOperation50KG;
using T=OriginalUiTextWidgetStatus50KG;using P=OriginalUiWinPhase50PD;
bool fail(D& d,E e,std::uint32_t pc=0)noexcept{d.error=e;if(pc)d.sourcePC=pc;return false;}
bool idOk(std::uint16_t id)noexcept{return id>0&&id<originalUiWinWidgetStorage50PD;}
bool operation(O o)noexcept{switch(o){case O::show:case O::hide:case O::select:case O::deselect:case O::clearSelection:case O::setEnabled:case O::reset:return true;}return false;}
bool stem(const std::string& value,std::string& out){
    const auto dot=value.find_last_of('.');
    if(dot==std::string::npos||value.substr(dot)!=".slt"||dot==0||dot>63)return false;
    out=value.substr(0,dot);
    for(const auto c:out)if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'))return false;
    return true;
}
}
bool resolveOriginalUiWinCircuit50PD(const OriginalUnlockFacts50PD& f,OriginalUiWinCircuit50PD& out,std::uint32_t& pc) noexcept {
    pc=0;
    if(f.career100C0>=f.diagonalMedals.size()){pc=0x2084F0;return false;}
    // 208564..208594: v1=187B10+1, compared against 13, 8 and 4.
    const auto next=f.order187B10+1u;std::uint8_t circuit=0,order=0;
    if(next==4u){circuit=1;order=3;}            //+193B
    else if(next==8u){circuit=2;order=7;}       //+193F
    else if(next==13u){circuit=3;order=12;}     //+1944
    else {pc=0x208668;return false;}             // "No Tier was completed..." assert
    // DLC career (PORT EXTENSION): an add-on selected career's own medal.
    const auto place=f.selectedAddonCareer?f.selectedAddonMedals[order]:f.diagonalMedals[f.career100C0][order];
    // 2086CC..208724: places 1..3 select a slot; anything else asserts.
    if(place<1||place>3){pc=0x20874C;return false;}
    out={circuit,place};return true;
}
bool buildOriginalUiWinMessage50PD(const assets::OriginalUiEnglish50KG& english,
    OriginalUiWinCircuit50PD c,std::string& combined,std::uint32_t& pc) noexcept {
    pc=0;
    if(c.circuit<1||c.circuit>3||c.place<1||c.place>3){pc=0x208BB8;return false;}
    // Place index by (place,circuit). Circuit 3 place 2 reads 147 (0x20946C).
    static constexpr std::uint16_t placeText[3][3]={{145,145,145},{146,146,147},{147,147,147}};
    static constexpr std::uint16_t circuitText[3]={161,162,163};
    const auto* won=english.at(160);
    const auto* place=english.at(placeText[c.place-1][c.circuit-1]);
    const auto* circuit=english.at(circuitText[c.circuit-1]);
    if(!won||!place||!circuit){pc=0x208C40;return false;}
    try{
        // ELF 3B09E0 "%s %s@%s" through 308C90 sprintf.
        std::string text=won->text;text.push_back(' ');text+=place->text;text.push_back('@');text+=circuit->text;
        if(text.find('\0')!=std::string::npos){pc=0x308C90;return false;}
        combined=std::move(text);return true;
    }catch(...){pc=0x308C90;return false;}
}

struct OriginalUiWinPage50PD::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    assets::OriginalUiDat50KG uiDat,pageDat,widgets;assets::OriginalUiPageIndex50KG index;assets::OriginalUiPage50KG page;
    std::array<OriginalUiWinWidgetState50PD,originalUiWinWidgetStorage50PD> states{};
    std::array<assets::OriginalUiOverlayText50KG,originalUiWinWidgetStorage50PD> texts;
    std::array<assets::OriginalUiIcon50KG,originalUiWinWidgetStorage50PD> icons;
    std::array<OriginalUiWinRotateConfig50PD,originalUiWinWidgetStorage50PD> rotates;
    std::array<OriginalUiTextWidget50KG,originalUiWinWidgetStorage50PD> textWidgets;
    std::array<std::vector<std::string>,originalUiWinWidgetStorage50PD> dynamic;
    std::array<std::uint64_t,originalUiWinWidgetStorage50PD> dynamicRevision{};
    std::array<bool,originalUiWinWidgetStorage50PD> iconPrepared{};
    std::array<OriginalUiIconLayout50KG,originalUiWinWidgetStorage50PD> iconLayouts{};
    std::vector<std::uint16_t> order;
    OriginalUiNavigation50KG navigation{};
    std::uint32_t prompt42C=0;OriginalUiWinCircuit50PD circuit650{};bool entered=false;
    bool readRotate(std::uint16_t id,const assets::OriginalUiDatSection50KG& s,OriginalUiWinRotateConfig50PD& c,D& d){
        const auto* slt=s.find("SLT");
        if(!slt||!stem(slt->text,c.stem))return fail(d,E::parse,0x1D31D0);
        c.slt=slt->text;
        // Every Win rotate widget authors these explicitly (Rider pod reader route).
        if(!s.find("Position")||!s.find("LookVector")||!s.find("UpVector")||
           !assets::originalUiTriple50KG(s,"Position",{},c.position,d.parse,0x1D31D0)||
           !assets::originalUiTriple50KG(s,"LookVector",{},c.look,d.parse,0x1D31D0)||
           !assets::originalUiTriple50KG(s,"UpVector",{},c.up,d.parse,0x1D31D0)||
           !assets::originalUiBoolean50KG(s,"IsLit",false,c.lit,d.parse,0x1D31D0)||
           !assets::originalUiBoolean50KG(s,"StartPaused",true,c.startPaused,d.parse,0x1D31D0)||
           !assets::originalUiBoolean50KG(s,"CleanFromMemory",false,c.cleanFromMemory,d.parse,0x1D31D0)||
           !assets::originalUiBoolean50KG(s,"DynamicLoad",false,c.dynamicLoad,d.parse,0x1CD5C0)||
           !assets::originalUiBoolean50KG(s,"EnableOnSelect",false,c.enableOnSelect,d.parse,0x2A7380)||
           !assets::originalUiBoolean50KG(s,"DisableOnSelect",false,c.disableOnSelect,d.parse,0x2A7380)||
           !assets::originalUiBoolean50KG(s,"EnableOnDeSelect",false,c.enableOnDeselect,d.parse,0x2A7380)||
           !assets::originalUiBoolean50KG(s,"DisableOnDeSelect",false,c.disableOnDeselect,d.parse,0x2A7380))
            {d.widget=id;return fail(d,E::parse,d.parse.sourcePC);}
        const char* flags[]{"XRotate","YRotate","ZRotate"};const char* speeds[]{"XVelocity","YVelocity","ZVelocity"};
        for(unsigned a=0;a<3;++a)if(!assets::originalUiBoolean50KG(s,flags[a],false,c.rotate[a],d.parse,0x1D31D0)||
            !assets::originalUiNumber50KG(s,speeds[a],0,c.velocity[a],d.parse,0x1D31D0)){d.widget=id;return fail(d,E::parse,d.parse.sourcePC);}
        for(unsigned a=0;a<3;++a)if(!std::isfinite(c.velocity[a])){d.widget=id;return fail(d,E::parse,0x1D31D0);}
        return true;
    }
    void visibility(OriginalUiWinWidgetState50PD& s,bool show,bool hide)noexcept{if(show!=hide)s.shown=show;}
};
OriginalUiWinPage50PD::OriginalUiWinPage50PD() noexcept=default;
OriginalUiWinPage50PD::~OriginalUiWinPage50PD()=default;
bool OriginalUiWinPage50PD::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const assets::OriginalUiEnglish50KG* english,D& d) noexcept {
    d={};if(storage_)return fail(d,E::alreadyBound);
    if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::win))return fail(d,E::archive,0x1CCBA0);
    if(!english||!english->loaded())return fail(d,E::localization,0x2A9000);
    try{
        auto s=std::make_unique<Storage>();s->archive=std::move(archive);
        // Loader 1CCBA0 "UIWin": ui.dat [Page 1] PageFile=win.dat (1CEC8C path).
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UIWin/ui.dat",s->uiDat,d.parse,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !assets::readOriginalUiPageIndex50KG(s->uiDat,s->index,d.parse)||s->index.pageFiles!=std::vector<std::string>{"win.dat"})return fail(d,E::parse,0x1CEC8C);
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UIWin/win.dat",s->pageDat,d.parse)||
           !assets::readOriginalUiDat50KG(*s->archive,"UI/UIWin/Widgets.Dat",s->widgets,d.parse,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||
           !assets::readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d.parse))return fail(d,E::parse,0x1D63B0);
        const auto& p=s->page;
        // Authored win.dat membership; any other shape is not this page.
        if(p.pageNo!=1||p.widgetCount!=14||p.groups.size()!=2||p.controls.size()!=4||
           p.groups[0].controls!=std::vector<std::int16_t>{0}||!p.groups[0].keepEnabled||
           p.groups[1].controls!=std::vector<std::int16_t>{1,2,3}||p.groups[1].keepEnabled||
           !p.groups[1].associations.empty()||!p.groups[0].associations.empty()||
           p.controls[0].widgets!=std::vector<std::uint16_t>{1,3,4,5,6,7,8}||
           p.controls[1].widgets!=std::vector<std::uint16_t>{9,10}||p.controls[2].widgets!=std::vector<std::uint16_t>{11,12}||
           p.controls[3].widgets!=std::vector<std::uint16_t>{13,14}||p.hasPopup)return fail(d,E::population,0x1D6DEC);
        for(std::uint16_t id=1;id<=14;++id){
            d.widget=id;const auto* section=s->widgets.section("Widget "+std::to_string(id));
            const auto* type=section?section->find("Type"):nullptr;if(!type)return fail(d,E::population,0x2A4798);
            auto& st=s->states[id];
            st.kind=type->text=="Empty"?K::empty:type->text=="Overlay_Text"?K::text:type->text=="Icon"?K::icon:type->text=="User_Rotate"?K::rotate:K::absent;
            const bool expected=(id==2&&st.kind==K::empty)||((id==1||(id>=3&&id<=5))&&st.kind==K::text)||
                (id>=6&&id<=8&&st.kind==K::icon)||(id>=9&&st.kind==K::rotate);
            if(!expected)return fail(d,E::population,0x2A4798);
            if(st.kind==K::empty)continue;
            bool onTop=false;if(!assets::originalUiBoolean50KG(*section,"OnTop",false,onTop,d.parse,0x1CD890))return fail(d,E::parse,0x1CD890);
            if(onTop)return fail(d,E::population,0x1CD890);
            if(st.kind==K::text){
                if(!assets::readOriginalUiOverlayText50KG(s->widgets,id,english,s->texts[id],d.parse))return fail(d,E::parse,0x2A9000);
                const auto& t=s->texts[id];
                if(t.wantsJoystickInput[0]||t.wantsJoystickInput[1])return fail(d,E::input,0x2A44C0);
                if((id==originalUiWinMessage50PD)!=(t.textSource==assets::OriginalUiTextSource50KG::dynamic))return fail(d,E::population,0x2A9174);
                if(t.textSource==assets::OriginalUiTextSource50KG::dynamic&&(t.lines.size()!=2||t.dynamicLineLength!=60))return fail(d,E::population,0x2A9190);
                if(s->textWidgets[id].bind(t)!=T::ok)return fail(d,E::text,0x2A9000);
                if(t.textSource==assets::OriginalUiTextSource50KG::dynamic)s->dynamic[id].assign(t.lines.size(),std::string());
            }else if(st.kind==K::icon){
                if(!assets::readOriginalUiIcon50KG(s->widgets,id,s->icons[id],d.parse))return fail(d,E::parse,0x2AC590);
                if(s->icons[id].wantsJoystickInput[0]||s->icons[id].wantsJoystickInput[1])return fail(d,E::input,0x2A44C0);
            }else if(!s->readRotate(id,*section,s->rotates[id],d))return false;
            s->order.push_back(id);
        }
        if(!bindOriginalUiNavigation50KG(s->page,s->navigation))return fail(d,E::population,0x2A5EA0);
        storage_=std::move(s);d={};return true;
    }catch(const std::bad_alloc&){return fail(d,E::allocation);}
    catch(...){return fail(d,E::parse);}
}
bool OriginalUiWinPage50PD::bound()const noexcept{return bool(storage_);}
const assets::OriginalUiPage50KG* OriginalUiWinPage50PD::page()const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiWinPage50PD::widgetsDat()const noexcept{return storage_?&storage_->widgets:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiWinPage50PD::uiDat()const noexcept{return storage_?&storage_->uiDat:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiWinPage50PD::pageDat()const noexcept{return storage_?&storage_->pageDat:nullptr;}
const std::vector<std::uint16_t>* OriginalUiWinPage50PD::order()const noexcept{return storage_?&storage_->order:nullptr;}
OriginalUiWinWidgetState50PD OriginalUiWinPage50PD::state(std::uint16_t id)const noexcept{
    if(!storage_||!idOk(id))return {};auto s=storage_->states[id];
    if(s.kind==K::text){s.selected=storage_->textWidgets[id].selected();s.shown=storage_->textWidgets[id].shown();}
    return s;
}
const assets::OriginalUiOverlayText50KG* OriginalUiWinPage50PD::textDescriptor(std::uint16_t id)const noexcept{return storage_&&idOk(id)&&storage_->states[id].kind==K::text?&storage_->texts[id]:nullptr;}
const assets::OriginalUiIcon50KG* OriginalUiWinPage50PD::iconDescriptor(std::uint16_t id)const noexcept{return storage_&&idOk(id)&&storage_->states[id].kind==K::icon?&storage_->icons[id]:nullptr;}
const OriginalUiWinRotateConfig50PD* OriginalUiWinPage50PD::rotateConfig(std::uint16_t id)const noexcept{return storage_&&idOk(id)&&storage_->states[id].kind==K::rotate?&storage_->rotates[id]:nullptr;}
const OriginalUiTextWidget50KG* OriginalUiWinPage50PD::textWidget(std::uint16_t id)const noexcept{return textDescriptor(id)?&storage_->textWidgets[id]:nullptr;}
const OriginalUiNavigation50KG* OriginalUiWinPage50PD::navigation()const noexcept{return storage_?&storage_->navigation:nullptr;}
std::uint32_t OriginalUiWinPage50PD::promptMask42C()const noexcept{return storage_?storage_->prompt42C:0;}
OriginalUiWinCircuit50PD OriginalUiWinPage50PD::circuit()const noexcept{return storage_?storage_->circuit650:OriginalUiWinCircuit50PD{};}
bool OriginalUiWinPage50PD::entered()const noexcept{return storage_&&storage_->entered;}
std::uint16_t OriginalUiWinPage50PD::selectedPlace()const noexcept{
    if(!storage_)return 0;const auto slot=storage_->navigation.groups[1].selectedSlot;
    return slot>=1&&slot<=3&&storage_->navigation.controls[std::size_t(slot)].selected?slot:0;
}
bool OriginalUiWinPage50PD::phase(P kind,const OriginalUiWinCircuit50PD* entry,OriginalUiWinPlan50PD& out,D& d)const noexcept{
    d={};if(!storage_)return fail(d,E::notBound);
    OriginalUiWinPlan50PD p;p.navigation=storage_->navigation;const auto& page=storage_->page;
    const auto callback=[&](std::uint16_t id,O op,bool v,std::uint32_t pc,bool controlEnable=false){
        if(!idOk(id)||p.callbackCount==p.callbacks.size())return false;
        p.callbacks[p.callbackCount++]={id,op,v,controlEnable,pc};return true;};
    const auto control=[&](std::int32_t id,O op,bool v,std::uint32_t pc){
        if(id==-1)return true;if(id<0||std::size_t(id)>=page.controls.size())return false;
        for(auto w:page.controls[std::size_t(id)].widgets)if(!callback(w,op,v,pc))return false;
        auto& c=p.navigation.controls[std::size_t(id)];
        switch(op){case O::select:c.selected=true;break;case O::deselect:case O::clearSelection:c.selected=false;break;
        case O::show:c.shown=true;break;case O::hide:c.shown=false;break;case O::setEnabled:c.enabled=v;break;case O::reset:break;}
        return true;};
    const auto group=[&](std::size_t id,O op,std::uint32_t pc,std::uint32_t assoc){
        const auto& g=page.groups[id];for(auto c:g.controls)if(!control(c,op,false,pc))return false;
        for(auto c:g.associations)if(!control(c,op,false,assoc))return false;return true;};
    bool ok=true;
    switch(kind){
    case P::requestedPage90:
        // Manager 2A57D8 page show: reset every group (and its associations).
        p.navigation.controllerGroup={{-1,-1}};
        for(std::size_t g=0;g<page.groups.size()&&ok;++g)ok=group(g,O::reset,0x2A57D8,0x2A5818);break;
    case P::previousPage94:
        for(std::size_t g=0;g<page.groups.size()&&ok;++g)ok=group(g,O::hide,0x2A4D58,0x2A4D8C);break;
    case P::entry74:{
        if(!entry||entry->circuit<1||entry->circuit>3||entry->place<1||entry->place>3)return fail(d,E::circuit,0x208668);
        p.writesPromptMask42C=true;p.promptMask42C=0;          //2084D4
        p.writesCircuit650=true;p.circuit=*entry;             //208598/2085D0/208608
        ok=callback(originalUiWinContinue50PD,O::setEnabled,false,0x2086B0,true)&&
           callback(originalUiWinTitle50PD,O::setEnabled,false,0x2086C4,true);
        OriginalUiCommands50KG commands;
        if(ok)ok=selectOriginalUiGroupSlot50KG(p.navigation,1,entry->place,commands)==OriginalUiNavigationStatus50KG::ok; //2A6BC0
        for(std::size_t i=0;ok&&i<commands.count;++i){
            const auto& c=commands.values[i];
            if(c.controlId>=page.controls.size()){ok=false;break;}
            for(auto w:page.controls[c.controlId].widgets)if(!(ok=callback(w,c.operation,c.value,c.sourcePC)))break;
        }
        p.activateCamera=true;p.clearScene3D8=true;p.applyBackground430=true; //208794..2087E8
        break;}
    case P::openingAC:
        if(!storage_->entered)return fail(d,E::input,0x208810);
        p.music=storage_->circuit650.circuit==1?OriginalUiWinMusic50PD::song10Once:
            storage_->circuit650.circuit==2?OriginalUiWinMusic50PD::song8Once:OriginalUiWinMusic50PD::none;
        p.musicPC=storage_->circuit650.circuit==1?0x208854:storage_->circuit650.circuit==2?0x208874:0;
        break;
    case P::openedB0:
        if(!storage_->entered)return fail(d,E::input,0x208900);
        p.writesPromptMask42C=true;p.promptMask42C=1;          //208910
        ok=callback(originalUiWinContinue50PD,O::setEnabled,true,0x208920,true)&&
           callback(originalUiWinTitle50PD,O::setEnabled,true,0x208934,true);
        break;
    case P::closingB4:
        p.writesPromptMask42C=true;p.promptMask42C=0;          //20895C
        ok=callback(originalUiWinContinue50PD,O::setEnabled,false,0x208970,true)&&
           callback(originalUiWinTitle50PD,O::setEnabled,false,0x208984,true);
        p.music=OriginalUiWinMusic50PD::fade1s;p.musicPC=0x20899C;break;
    case P::closingThresholdB8:case P::closedBC:break;         //2089C0 / 1D7A00
    }
    if(!ok)return fail(d,E::callback);
    out=p;return true;
}
bool OriginalUiWinPage50PD::preflight(const OriginalUiWinPlan50PD& p,D& d)const noexcept{
    d={};if(!storage_)return fail(d,E::notBound);
    if(p.callbackCount>p.callbacks.size()||p.navigation.page!=&storage_->page)return fail(d,E::callback);
    if(p.writesPromptMask42C&&p.promptMask42C>1)return fail(d,E::callback);
    for(std::size_t i=0;i<p.callbackCount;++i){const auto& c=p.callbacks[i];d.widget=c.widgetId;d.callback=i;d.sourcePC=c.sourcePC;
        if(!idOk(c.widgetId)||storage_->states[c.widgetId].kind==K::empty||storage_->states[c.widgetId].kind==K::absent||!operation(c.operation))return fail(d,E::callback);
        if(c.controlEnable&&(c.operation!=O::setEnabled||storage_->states[c.widgetId].kind!=K::text))return fail(d,E::callback);}
    d={};return true;
}
bool OriginalUiWinPage50PD::apply(const OriginalUiWinPlan50PD& p,D& d) noexcept{
    if(!preflight(p,d))return false;auto& s=*storage_;
    for(std::size_t i=0;i<p.callbackCount;++i){const auto& c=p.callbacks[i];auto& st=s.states[c.widgetId];d.widget=c.widgetId;d.callback=i;d.sourcePC=c.sourcePC;
        if(st.kind==K::text){auto& t=s.textWidgets[c.widgetId];
            const auto r=c.controlEnable?t.setControlEnabled(c.value):(c.operation==O::setEnabled?t.setControlEnabled(c.value):t.apply(c.operation,c.value));
            if(r!=T::ok)return fail(d,E::text);continue;}
        if(st.kind==K::icon){const auto& icon=s.icons[c.widgetId];
            switch(c.operation){case O::show:case O::reset:st.shown=true;break;case O::hide:st.shown=false;break;
            case O::select:st.selected=true;s.visibility(st,icon.enableOnSelect,icon.disableOnSelect);break;
            case O::deselect:st.selected=false;s.visibility(st,icon.enableOnDeselect,icon.disableOnDeselect);break;
            case O::clearSelection:st.selected=false;break;case O::setEnabled:st.selected=c.value;st.shown=c.value;break;}
            continue;}
        // User_Rotate: DynamicLoad creates on SELECT; CleanFromMemory releases on DESELECT.
        const auto& r=s.rotates[c.widgetId];
        switch(c.operation){case O::show:case O::reset:st.shown=true;break;case O::hide:st.shown=false;break;
        case O::select:st.selected=true;if(r.dynamicLoad)st.loaded=true;s.visibility(st,r.enableOnSelect,r.disableOnSelect);break;
        case O::deselect:st.selected=false;s.visibility(st,r.enableOnDeselect,r.disableOnDeselect);if(r.cleanFromMemory)st.loaded=false;break;
        case O::clearSelection:st.selected=false;break;case O::setEnabled:st.selected=c.value;st.shown=c.value;break;}
    }
    s.navigation=p.navigation;
    if(p.writesPromptMask42C)s.prompt42C=p.promptMask42C;
    if(p.writesCircuit650){s.circuit650=p.circuit;s.entered=true;}
    d={};return true;
}
bool OriginalUiWinPage50PD::input(std::uint8_t controller,std::uint8_t event,bool unlock,
    OriginalUiWinInputResult50PD& out,D& d)const noexcept{
    d={};out={};if(!storage_)return fail(d,E::notBound);if(controller>1||event>15)return fail(d,E::input);
    // 2089D0 returns 1 for every delivered event; only device0 event4 acts.
    out.pageAccepted=true;if(controller!=0||event!=4)return true;
    if(!storage_->entered)return fail(d,E::input,0x2089D0);
    auto& r=out.route;r.action=OriginalUiRouteAction50KG::request;
    r.request={OriginalUiRouteKind50KG::changeScreen,originalUiWinScreen50PD,originalUiWinPageId50PD,
        static_cast<std::uint8_t>(unlock?10:13),1,unlock?0x208A28u:0x208A48u};
    r.inputPC=0x2089D0;r.profileProposalComplete=true;r.selectedSlot=selectedPlace();
    r.blockers=static_cast<std::uint32_t>(OriginalUiRouteBlocker50KG::destinationOwner);
    out.routeRequested=true;out.soundId=1;out.soundPC=0x208A68;out.flushPC=0x208A80;
    return true;
}
bool OriginalUiWinPage50PD::setWidgetColor(std::uint16_t id,std::uint32_t color,D& d) noexcept{
    d={};d.widget=id;if(!storage_||!idOk(id))return fail(d,E::notBound);auto& s=*storage_;
    if(s.states[id].kind==K::text){if(s.textWidgets[id].setWidgetColor(color)!=T::ok)return fail(d,E::text,0x2AA7F0);return true;}
    if(s.states[id].kind==K::icon){if(!s.iconPrepared[id])return fail(d,E::icon,0x2ACE50);
        s.iconLayouts[id].quad.sourcePacketColor=originalUiTextPacketColor50KG(color,static_cast<std::int16_t>(s.icons[id].alpha));return true;}
    return fail(d,E::input);
}
bool OriginalUiWinPage50PD::setDynamicText(std::uint16_t id,const std::string& text,D& d) noexcept{
    d={};d.widget=id;const auto* desc=textDescriptor(id);
    if(!desc||desc->textSource!=assets::OriginalUiTextSource50KG::dynamic)return fail(d,E::text,0x2A9DE0);
    if(text.find('\0')!=std::string::npos)return fail(d,E::text,0x2A9DE0);
    auto& s=*storage_;const auto lines=desc->lines.size();
    try{
        std::vector<std::string> slices;std::size_t at=0;
        for(;;){if(slices.size()>=lines)return fail(d,E::text,0x2A9ED0);
            const auto end=text.find('@',at);slices.push_back(text.substr(at,end==std::string::npos?std::string::npos:end-at));
            if(end==std::string::npos)break;at=end+1;}
        for(const auto& l:slices)if(l.size()>static_cast<std::size_t>(desc->dynamicLineLength))return fail(d,E::text,0x29D940);
        auto& widget=s.textWidgets[id];
        for(std::size_t i=0;i<slices.size();++i)if(widget.line(i)&&widget.line(i)->locked)return fail(d,E::text,0x29DA40);
        std::vector<std::string> next(lines);
        for(std::size_t i=0;i<slices.size();++i)next[i]=slices[i];
        for(std::size_t i=0;i<slices.size();++i)if(widget.setupLine(i,OriginalUiTextLineSetup50KG{})!=T::ok)return fail(d,E::text,0x29DA40);
        if(next!=s.dynamic[id]){s.dynamic[id].swap(next);++s.dynamicRevision[id];}
        return true;
    }catch(...){return fail(d,E::allocation);}
}
const std::vector<std::string>* OriginalUiWinPage50PD::dynamicLines(std::uint16_t id)const noexcept{
    const auto* t=textDescriptor(id);return t&&t->textSource==assets::OriginalUiTextSource50KG::dynamic?&storage_->dynamic[id]:nullptr;}
std::uint64_t OriginalUiWinPage50PD::dynamicRevision(std::uint16_t id)const noexcept{return storage_&&idOk(id)?storage_->dynamicRevision[id]:0;}
bool OriginalUiWinPage50PD::updateText(std::uint16_t id,bool shown,D& d) noexcept{
    d={};d.widget=id;if(!textDescriptor(id))return fail(d,E::text);
    const auto s=storage_->textWidgets[id].updateStatic(shown);
    return s==T::ok?true:fail(d,s==T::hidden?E::hidden:E::text);
}
bool OriginalUiWinPage50PD::projectText(std::uint16_t id,std::size_t line,float fontHeight,std::int32_t width,std::int32_t height,
    bool shown,OriginalUiTextPosition50KG& position,OriginalUiTextLineProjection50KG& projection,D& d) noexcept{
    d={};d.widget=id;const auto* text=textDescriptor(id);if(!text)return fail(d,E::text);
    // Dynamic lines are positioned from the live SetText string, never the empty descriptor.
    assets::OriginalUiOverlayText50KG live;const assets::OriginalUiOverlayText50KG* source=text;
    try{if(text->textSource==assets::OriginalUiTextSource50KG::dynamic){live=*text;live.lines=storage_->dynamic[id];source=&live;}}
    catch(...){return fail(d,E::allocation);}
    OriginalUiTextPosition50KG p;
    if(!positionOriginalUiText50KG({source,line,fontHeight,width,height},p))return fail(d,E::text,0x2E13A0);
    OriginalUiTextLineProjection50KG l;const auto status=storage_->textWidgets[id].projectLine(line,l,shown);
    if(status!=T::ok)return fail(d,status==T::hidden?E::hidden:E::text);
    position=p;projection=l;return true;
}
bool OriginalUiWinPage50PD::prepareIcon(std::uint16_t id,const assets::Texture48B& atlas,std::int32_t depth,D& d) noexcept{
    d={};d.widget=id;const auto* icon=iconDescriptor(id);if(!icon)return fail(d,E::icon);if(storage_->iconPrepared[id])return fail(d,E::alreadyBound);
    OriginalUiIconLayout50KG layout;
    if(layoutOriginalUiIcon50KG(*icon,atlas,depth,layout)!=OriginalUiIconLayoutStatus50KG::complete)return fail(d,E::icon,0x2AC590);
    storage_->iconLayouts[id]=layout;storage_->iconPrepared[id]=true;return true;
}
const OriginalUiIconLayout50KG* OriginalUiWinPage50PD::iconLayout(std::uint16_t id)const noexcept{
    return iconDescriptor(id)&&storage_->iconPrepared[id]?&storage_->iconLayouts[id]:nullptr;}

}
