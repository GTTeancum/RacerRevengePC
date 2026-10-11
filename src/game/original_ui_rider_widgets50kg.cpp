#include "original_ui_rider_widgets50kg.h"
#include <array>
#include <utility>
namespace rr::game {
namespace {
using K=OriginalUiRiderWidgetKind50KG;using E=OriginalUiRiderWidgetError50KG;
using O=OriginalUiControlOperation50KG;using D=OriginalUiRiderWidgetDiagnostic50KG;
bool fail(D& d,E e)noexcept{d.error=e;return false;}
bool idOk(std::uint16_t id)noexcept{return id>0&&id<120;}
bool dynamic(K k)noexcept{return k==K::pod||k==K::character;}
bool operation(O o)noexcept{switch(o){case O::show:case O::hide:case O::select:case O::deselect:
    case O::clearSelection:case O::setEnabled:case O::reset:return true;}return false;}
}
struct OriginalUiRiderWidgets50KG::Storage {
    std::shared_ptr<const assets::OriginalUiArchive50KG> archive;
    OriginalUiRiderSelectionData50KG selection;std::array<OriginalUiNavigation50KG,2> navigation;
    assets::OriginalUiDat50KG dat;assets::OriginalUiMovementControl50KG movement;
    std::array<OriginalUiRiderWidgetState50KG,120> states{};
    std::array<assets::OriginalUiOverlayText50KG,120> texts;
    std::array<assets::OriginalUiIcon50KG,120> icons;
    std::array<OriginalUiTextWidget50KG,120> textWidgets;
    // Live text contents/positions never mutate shared immutable descriptors.
    std::array<std::vector<std::string>,120> dynamicLines;
    std::array<std::array<std::int32_t,2>,120> positions{};
    std::array<std::uint32_t,120> colors{};
    std::array<bool,120> hasColor{};
    struct Image {std::shared_ptr<const assets::OriginalUiScreenAssets50KG> owner;
        std::uint32_t index=0;OriginalUiIconLayout50KG layout{};};
    std::array<Image,120> images;
    std::vector<std::uint16_t> ordinary,top;bool poisoned=false,inCallback=false;
};
OriginalUiRiderWidgets50KG::OriginalUiRiderWidgets50KG()noexcept=default;
OriginalUiRiderWidgets50KG::~OriginalUiRiderWidgets50KG()=default;
bool OriginalUiRiderWidgets50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,
    const assets::OriginalUiEnglish50KG* english,D& d)noexcept {
    d={};if(storage_)return fail(d,E::alreadyBound);
    try {
        auto s=std::make_unique<Storage>();s->archive=std::move(archive);
        if(!s->selection.load(s->archive,d.selection)||
           !bindOriginalUiNavigation50KG(*s->selection.page(),s->navigation[0])||
           !bindOriginalUiNavigation50KG(*s->selection.page(),s->navigation[1]))return fail(d,E::archive);
        if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UIRider/Widgets.Dat",s->dat,d.parse,
            assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,E::parse);
        const auto* general=s->dat.section("General");std::int32_t count=0;
        if(!general||!assets::originalUiInteger50KG(*general,"NumberOfWidgets",0,count,d.parse,0x1CD70C)||count!=119)
            return fail(d,E::population);
        for(std::uint16_t id=1;id<120;++id) {
            d.widget=id;const auto* section=s->dat.section("Widget "+std::to_string(id));
            if(!section)return fail(d,E::population);const auto* type=section->find("Type");
            if(!type)return fail(d,E::population);auto& state=s->states[id];
            const auto& t=type->text;
            state.kind=t=="Empty"?K::empty:t=="Overlay_Text"?K::text:t=="Icon"?K::icon:
                t=="Movement_Control"?K::movement:t=="User_Rotate"?K::pod:
                t=="Soultree_Character"?K::character:K::absent;
            if(state.kind==K::absent)return fail(d,E::population);
            if(state.kind==K::empty)continue; //1CD70C null, never a drawable/child.
            if(!assets::originalUiBoolean50KG(*section,"OnTop",false,state.onTop,d.parse,0x1CD890))return fail(d,E::parse);
            if(state.kind==K::text) {
                if(!assets::readOriginalUiOverlayText50KG(s->dat,id,english,s->texts[id],d.parse))return fail(d,E::parse);
                const auto& text=s->texts[id];
                if(text.wantsJoystickInput[0]||text.wantsJoystickInput[1])return fail(d,E::input);
                if(s->textWidgets[id].bind(text)!=OriginalUiTextWidgetStatus50KG::ok)return fail(d,E::text);
                s->positions[id]={{text.x,text.y}};
                if(text.textSource==assets::OriginalUiTextSource50KG::dynamic)s->dynamicLines[id]=text.lines;
            }else if(state.kind==K::icon) {
                if(!assets::readOriginalUiIcon50KG(s->dat,id,s->icons[id],d.parse))return fail(d,E::parse);
                if(s->icons[id].wantsJoystickInput[0]||s->icons[id].wantsJoystickInput[1])return fail(d,E::input);
            }else if(state.kind==K::movement) {
                if(id!=8||!assets::readOriginalUiMovementControl50KG(s->dat,id,s->movement,d.parse))return fail(d,E::input);
                if(s->movement.gridSize!=0||!s->movement.wantsJoystickInput[0]||!s->movement.wantsJoystickInput[1])return fail(d,E::input);
                for(unsigned n=0;n<16;++n)
                    if(s->movement.mappings[n]!=(n==0?-2:n==1?-1:0))return fail(d,E::input);
            }
            if(!state.onTop)s->ordinary.push_back(id);
        }
        for(std::uint16_t id=119;id>0;--id)
            if(s->states[id].kind!=K::empty&&s->states[id].onTop)s->top.push_back(id);
        std::size_t movement=0;
        for(const auto& c:s->selection.page()->controls)for(auto id:c.widgets) {
            if(!idOk(id)||s->states[id].kind==K::empty)return fail(d,E::population);
            if(id==8)++movement;
        }
        if(movement!=1)return fail(d,E::population);
        storage_=std::move(s);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
const OriginalUiRiderSelectionData50KG* OriginalUiRiderWidgets50KG::selectionData()const noexcept{return storage_?&storage_->selection:nullptr;}
const OriginalUiNavigation50KG* OriginalUiRiderWidgets50KG::navigation(std::uint8_t player)const noexcept{return storage_&&player<2?&storage_->navigation[player]:nullptr;}
const std::vector<std::uint16_t>* OriginalUiRiderWidgets50KG::order(bool top)const noexcept{return storage_?(top?&storage_->top:&storage_->ordinary):nullptr;}
bool OriginalUiRiderWidgets50KG::poisoned()const noexcept{return storage_&&storage_->poisoned;}
OriginalUiRiderWidgetState50KG OriginalUiRiderWidgets50KG::state(std::uint16_t id)const noexcept {
    if(!storage_||!idOk(id))return {};auto s=storage_->states[id];
    if(s.kind==K::text){s.selected=storage_->textWidgets[id].selected();s.shown=storage_->textWidgets[id].shown();}return s;
}
const assets::OriginalUiOverlayText50KG* OriginalUiRiderWidgets50KG::textDescriptor(std::uint16_t id)const noexcept{
    return storage_&&idOk(id)&&storage_->states[id].kind==K::text?&storage_->texts[id]:nullptr;
}
const assets::OriginalUiIcon50KG* OriginalUiRiderWidgets50KG::iconDescriptor(std::uint16_t id)const noexcept{
    return storage_&&idOk(id)&&storage_->states[id].kind==K::icon?&storage_->icons[id]:nullptr;
}
bool OriginalUiRiderWidgets50KG::apply(const OriginalUiRiderWidgetCallback50KG* calls,std::size_t count,
    const OriginalUiRiderDynamicHooks50KG& hooks,D& d)noexcept {
    d={};if(!storage_)return fail(d,E::notBound);auto& s=*storage_;
    if(s.poisoned||s.inCallback)return fail(d,E::poisoned);
    if((count&&!calls)||count>32768)return fail(d,E::callback);
    bool needs=false;
    for(std::size_t i=0;i<count;++i){const auto& c=calls[i];d.widget=c.widget;d.callback=i;
        if(!idOk(c.widget)||!operation(c.operation)||s.states[c.widget].kind==K::empty||s.states[c.widget].kind==K::absent)
            return fail(d,E::callback);
        needs=needs||dynamic(s.states[c.widget].kind);
    }
    if(needs&&(!hooks.preflight||!hooks.apply))return fail(d,E::missingDynamicOwner);
    s.inCallback=true;
    if(needs&&!hooks.preflight(hooks.context,calls,count)){s.inCallback=false;return fail(d,E::missingDynamicOwner);}
    for(std::size_t i=0;i<count;++i){const auto& c=calls[i];d.widget=c.widget;d.callback=i;auto& state=s.states[c.widget];
        bool ok=true;
        if(state.kind==K::text){auto& text=s.textWidgets[c.widget];
            const auto status=c.operation==O::setEnabled?text.setControlEnabled(c.value):text.apply(c.operation,c.value);
            ok=status==OriginalUiTextWidgetStatus50KG::ok;
        }else if(dynamic(state.kind)) {
            auto actual=state;ok=hooks.apply(hooks.context,c,actual);
            ok=ok&&actual.kind==state.kind&&actual.onTop==state.onTop;if(ok)state=actual;
        }else {
            const auto visibility=[&](bool show,bool hide){if(show!=hide)state.shown=show;};
            const bool movement=state.kind==K::movement;const auto& icon=s.icons[c.widget];
            switch(c.operation){
            case O::show:case O::reset:state.shown=true;break;
            case O::hide:state.shown=false;break;
            case O::select:state.selected=true;if(movement)state.shown=true;else visibility(icon.enableOnSelect,icon.disableOnSelect);break;
            case O::deselect:state.selected=false;if(movement)state.shown=false;else visibility(icon.enableOnDeselect,icon.disableOnDeselect);break;
            case O::clearSelection:state.selected=false;break;
            case O::setEnabled:state.selected=c.value;state.shown=c.value;break;
            }
        }
        if(!ok){s.inCallback=false;s.poisoned=true;return fail(d,E::callback);}
    }
    s.inCallback=false;d={};return true;
}
bool OriginalUiRiderWidgets50KG::expandNavigation(std::uint8_t player,const OriginalUiNavigation50KG& next,
    const OriginalUiCommands50KG& commands,std::vector<OriginalUiRiderWidgetCallback50KG>& output,D& d)const noexcept {
    d={};if(!storage_)return fail(d,E::notBound);
    if(storage_->poisoned||storage_->inCallback)return fail(d,E::poisoned);
    if(player>1||next.page!=storage_->selection.page()||commands.count>commands.values.size())return fail(d,E::callback);
    try {
        std::vector<OriginalUiRiderWidgetCallback50KG> calls;
        for(std::size_t i=0;i<commands.count;++i){const auto& c=commands.values[i];
            if(c.controlId>=next.page->controls.size()||!operation(c.operation))return fail(d,E::callback);
            for(auto id:next.page->controls[c.controlId].widgets){
                d.widget=id;d.callback=calls.size();
                if(calls.size()==32768||!idOk(id)||storage_->states[id].kind==K::empty||
                   storage_->states[id].kind==K::absent)return fail(d,E::callback);
                calls.push_back({id,c.operation,c.value,c.sourcePC});
            }
        }
        output=std::move(calls);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderWidgets50KG::preflightNavigation(std::uint8_t player,const OriginalUiNavigation50KG& next,
    const OriginalUiCommands50KG& commands,const OriginalUiRiderDynamicHooks50KG& hooks,D& d)noexcept {
    std::vector<OriginalUiRiderWidgetCallback50KG> calls;
    if(!expandNavigation(player,next,commands,calls,d))return false;
    bool needs=false;for(const auto& call:calls)needs=needs||dynamic(storage_->states[call.widget].kind);
    if(!needs)return true;
    if(!hooks.preflight||!hooks.apply)return fail(d,E::missingDynamicOwner);
    storage_->inCallback=true;const bool okay=hooks.preflight(hooks.context,calls.data(),calls.size());storage_->inCallback=false;
    return okay?true:fail(d,E::missingDynamicOwner);
}
bool OriginalUiRiderWidgets50KG::commitNavigation(std::uint8_t player,const OriginalUiNavigation50KG& next,
    const OriginalUiCommands50KG& commands,const OriginalUiRiderDynamicHooks50KG& hooks,D& d)noexcept {
    std::vector<OriginalUiRiderWidgetCallback50KG> calls;
    if(!expandNavigation(player,next,commands,calls,d)||!apply(calls.data(),calls.size(),hooks,d))return false;
    storage_->navigation[player]=next;return true;
}
bool OriginalUiRiderWidgets50KG::planMovement(std::uint8_t player,std::uint8_t controller,assets::OriginalUiMovementInput50KG input,
    bool reached,bool enabled,bool accepted,OriginalUiRiderMovementPlan50KG& output)const noexcept {
    if(!storage_||storage_->poisoned||player>1||controller>1||static_cast<unsigned>(input)>=16)return false;
    OriginalUiRiderMovementPlan50KG p;p.nextNavigation=storage_->navigation[player];
    const auto mapping=storage_->movement.mappings[static_cast<unsigned>(input)];
    const auto group=p.nextNavigation.controllerGroup[controller];
    if(!reached||!enabled||!accepted||!storage_->states[8].selected||
       !storage_->movement.wantsJoystickInput[controller]||!mapping||group==-1){output=p;return true;}
    if(group<0||std::size_t(group)>=p.nextNavigation.page->groups.size())return false;
    const auto status=moveOriginalUiControl50KG(p.nextNavigation,std::uint16_t(group),mapping==-1,p.commands);
    if(status!=OriginalUiNavigationStatus50KG::ok&&status!=OriginalUiNavigationStatus50KG::noChange)return false;
    p.notifyManager=true;p.mapping=mapping;output=p;return true;
}
bool OriginalUiRiderWidgets50KG::setDynamicLines(std::uint16_t id,const std::vector<std::string>& lines,D& d)noexcept{
    d={};d.widget=id;const auto* text=textDescriptor(id);
    if(!text||storage_->poisoned||storage_->inCallback||text->textSource!=assets::OriginalUiTextSource50KG::dynamic||lines.size()!=text->lines.size())return fail(d,E::text);
    for(const auto& line:lines)if(line.size()>255||line.size()>std::size_t(text->dynamicLineLength)||line.find('\0')!=std::string::npos)return fail(d,E::text);
    try{auto replacement=lines;storage_->dynamicLines[id].swap(replacement);return true;}catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderWidgets50KG::setTextPosition(std::uint16_t id,std::int32_t x,std::int32_t y,D& d)noexcept{
    d={};d.widget=id;if(!textDescriptor(id)||storage_->poisoned||storage_->inCallback)return fail(d,E::text);
    storage_->positions[id]={{x,y}};return true;
}
bool OriginalUiRiderWidgets50KG::setColor(std::uint16_t id,std::uint32_t color,D& d)noexcept {
    d={};d.widget=id;if(!storage_||storage_->poisoned||storage_->inCallback||!idOk(id))return fail(d,E::input);
    if(textDescriptor(id)){for(std::size_t i=0;i<storage_->textWidgets[id].lineCount();++i){
        const auto status=storage_->textWidgets[id].setLineColor(i,color);
        if(status!=OriginalUiTextWidgetStatus50KG::ok&&status!=OriginalUiTextWidgetStatus50KG::lockedOut)return fail(d,E::text);
    }return true;}
    if(!iconDescriptor(id))return fail(d,E::input);storage_->colors[id]=color;storage_->hasColor[id]=true;return true;
}
bool OriginalUiRiderWidgets50KG::updateText(std::uint16_t id,bool shown,D& d)noexcept{
    d={};d.widget=id;if(!textDescriptor(id)||storage_->poisoned||storage_->inCallback)return fail(d,E::text);
    const auto status=storage_->textWidgets[id].updateStatic(shown);
    return status==OriginalUiTextWidgetStatus50KG::ok?true:fail(d,status==OriginalUiTextWidgetStatus50KG::hidden?E::hidden:E::text);
}
const std::vector<std::string>* OriginalUiRiderWidgets50KG::textLines(std::uint16_t id)const noexcept {
    const auto* descriptor=textDescriptor(id);
    if(!descriptor||storage_->poisoned||storage_->inCallback)return nullptr;
    return descriptor->textSource==assets::OriginalUiTextSource50KG::dynamic?&storage_->dynamicLines[id]:&descriptor->lines;
}
const std::array<std::int32_t,2>* OriginalUiRiderWidgets50KG::textPosition(std::uint16_t id)const noexcept {
    return textDescriptor(id)&&!storage_->poisoned&&!storage_->inCallback?&storage_->positions[id]:nullptr;
}
bool OriginalUiRiderWidgets50KG::projectText(std::uint16_t id,std::size_t line,float fontHeight,std::int32_t width,
    std::int32_t height,bool shown,OriginalUiRiderTextProjection50KG& out,D& d)noexcept{
    d={};d.widget=id;const auto* text=textDescriptor(id);if(!text||storage_->poisoned||storage_->inCallback||line>=text->lines.size())return fail(d,E::text);
    try{
        auto positioned=*text;positioned.x=storage_->positions[id][0];positioned.y=storage_->positions[id][1];
        if(text->textSource==assets::OriginalUiTextSource50KG::dynamic)positioned.lines=storage_->dynamicLines[id];
        OriginalUiRiderTextProjection50KG result;result.descriptor=text;
        if(!positionOriginalUiText50KG({&positioned,line,fontHeight,width,height},result.position))return fail(d,E::text);
        const auto status=storage_->textWidgets[id].projectLine(line,result.line,shown);
        if(status!=OriginalUiTextWidgetStatus50KG::ok)return fail(d,status==OriginalUiTextWidgetStatus50KG::hidden?E::hidden:E::text);
        if(text->textSource==assets::OriginalUiTextSource50KG::dynamic)result.line.text=&storage_->dynamicLines[id][line];
        out=result;return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderWidgets50KG::prepareIcon(std::uint16_t id,std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,
    std::uint32_t index,std::int32_t depth,D& d)noexcept{
    d={};d.widget=id;const auto* icon=iconDescriptor(id);if(!icon||storage_->poisoned||storage_->inCallback)return fail(d,E::icon);
    if(storage_->images[id].owner||!images||!images->loaded()||!images->textures())return fail(d,E::missingResource);
    const auto* texture=images->textures()->texture(index);if(!texture||!images->decodedTexture(index))return fail(d,E::missingResource);
    OriginalUiIconLayout50KG layout;if(layoutOriginalUiIcon50KG(*icon,texture->texture,depth,layout)!=OriginalUiIconLayoutStatus50KG::complete)return fail(d,E::icon);
    auto& binding=storage_->images[id];binding.owner=std::move(images);binding.index=index;binding.layout=layout;return true;
}
bool OriginalUiRiderWidgets50KG::projectIcon(std::uint16_t id,bool shown,OriginalUiRiderIconProjection50KG& out,D& d)const noexcept{
    d={};d.widget=id;const auto* icon=iconDescriptor(id);if(!icon||storage_->poisoned||storage_->inCallback)return fail(d,E::icon);
    if(!shown||!storage_->states[id].shown)return fail(d,E::hidden);
    const auto& image=storage_->images[id];if(!image.owner)return fail(d,E::missingResource);
    OriginalUiRiderIconProjection50KG p{icon,image.owner.get(),image.index,image.layout};
    if(storage_->hasColor[id]){auto color=storage_->colors[id];
        for(unsigned i=0;i<3;++i)p.layout.quad.sourcePacketColor[i]=std::uint8_t(((color>>(16-8*i))&255u)>>1);
    }out=p;return true;
}
}
