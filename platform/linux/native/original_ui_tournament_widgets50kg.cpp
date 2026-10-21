#include "original_ui_tournament_widgets50kg.h"
namespace rr::platform { namespace {
using E=OriginalUiTournamentWidgetsError50KG;using D=OriginalUiTournamentWidgetsDiagnostic50KG;
using K=OriginalUiTournamentWidgetKind50KG;using O=game::OriginalUiControlOperation50KG;using TS=game::OriginalUiTextWidgetStatus50KG;
bool fail(D& d,E e)noexcept{d.error=e;return false;}
bool idOk(std::uint16_t id)noexcept{return id>0&&id<originalUiTournamentWidgetSlots50KG;}
bool opOk(O op)noexcept{return op==O::show||op==O::hide||op==O::select||op==O::deselect||op==O::clearSelection||op==O::setEnabled||op==O::reset;}
}
struct OriginalUiTournamentWidgets50KG::Storage {
 std::shared_ptr<const OriginalUiTournamentWidgetResources50KG> resources;
 std::array<OriginalUiTournamentWidgetState50KG,originalUiTournamentWidgetSlots50KG> states{};
 std::array<assets::OriginalUiOverlayText50KG,originalUiTournamentWidgetSlots50KG> descriptors;
 std::array<std::unique_ptr<game::OriginalUiTextWidget50KG>,originalUiTournamentWidgetSlots50KG> texts;
 std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,originalUiTournamentWidgetSlots50KG> geometry;
 std::array<bool,originalUiTournamentWidgetSlots50KG> dynamic{},populated{},prepared{};
 std::array<std::uint32_t,originalUiTournamentWidgetSlots50KG> colors{};std::array<bool,originalUiTournamentWidgetSlots50KG> hasColor{};
 std::array<bool,originalUiTournamentWidgetSlots50KG> callbacksStarted{};bool inCallback=false,poisoned=false;
};
OriginalUiTournamentWidgets50KG::OriginalUiTournamentWidgets50KG()noexcept=default;
OriginalUiTournamentWidgets50KG::~OriginalUiTournamentWidgets50KG()=default;
bool OriginalUiTournamentWidgets50KG::bind(std::shared_ptr<const OriginalUiTournamentWidgetResources50KG> resources,D& d)noexcept{
 d={};if(storage_)return fail(d,E::alreadyBound);if(!resources||!resources->loaded()||!resources->widgetsDat())return fail(d,E::owner);
 try{auto s=std::make_unique<Storage>();s->resources=std::move(resources);
 for(std::uint16_t id=1;id<originalUiTournamentWidgetSlots50KG;++id){d.widget=id;auto& state=s->states[id];state.kind=s->resources->kind(id);
  if(state.kind==K::absent)return fail(d,E::resource);if(state.kind==K::empty)continue;
  const auto* section=s->resources->widgetsDat()->section("Widget "+std::to_string(id));if(!section)return fail(d,E::resource);
  assets::OriginalUiPageDiagnostic50KG parse;if(!assets::originalUiBoolean50KG(*section,"OnTop",false,state.onTop,parse,0x1CD890))return fail(d,E::resource);
  if(state.kind==K::text){const auto* desc=s->resources->text(id);if(!desc)return fail(d,E::resource);s->descriptors[id]=*desc;s->dynamic[id]=desc->textSource==assets::OriginalUiTextSource50KG::dynamic;
   if(!s->dynamic[id]){s->texts[id]=std::make_unique<game::OriginalUiTextWidget50KG>();d.text=s->texts[id]->bind(s->descriptors[id]);if(d.text!=TS::ok)return fail(d,E::text);s->populated[id]=true;}}
 }
 storage_=std::move(s);d={};return true;}catch(...){return fail(d,E::allocation);}
}
bool OriginalUiTournamentWidgets50KG::prepareDynamic(std::uint16_t id,const std::vector<std::string>& lines,D& d)noexcept{
 d={};d.widget=id;if(!storage_||!idOk(id))return fail(d,E::input);auto& s=*storage_;
 if(s.poisoned||s.inCallback||s.callbacksStarted[id]||!s.dynamic[id]||s.prepared[id]||s.populated[id])return fail(d,E::phase);
 try{auto descriptor=s.descriptors[id];descriptor.lines=lines;
  auto geom=std::make_unique<game::OriginalUiTextGeometry50KG>();if(!s.resources->prepareDynamicText(id,lines,*geom,d.resource))return fail(d,E::resource);
  // The immutable descriptor address is stable; only this pre-callback
  // population is allowed, before any line/projection borrow can escape.
  s.descriptors[id]=std::move(descriptor);auto text=std::make_unique<game::OriginalUiTextWidget50KG>();d.text=text->bind(s.descriptors[id]);
  if(d.text!=TS::ok){s.poisoned=true;return fail(d,E::text);}s.texts[id]=std::move(text);s.geometry[id]=std::move(geom);s.prepared[id]=true;return true;
 }catch(...){s.poisoned=true;return fail(d,E::allocation);}
}
bool OriginalUiTournamentWidgets50KG::publishDynamic(std::uint16_t id,D& d)noexcept{
 d={};d.widget=id;if(!storage_||!idOk(id))return fail(d,E::input);auto& s=*storage_;
 if(s.poisoned||s.inCallback||s.callbacksStarted[id]||!s.dynamic[id]||!s.prepared[id]||s.populated[id]||!s.texts[id]||!s.geometry[id])return fail(d,E::phase);
 s.populated[id]=true;return true;
}
bool OriginalUiTournamentWidgets50KG::populateDynamic(std::uint16_t id,const std::vector<std::string>& lines,D& d)noexcept{return prepareDynamic(id,lines,d)&&publishDynamic(id,d);}
bool OriginalUiTournamentWidgets50KG::dynamicComplete()const noexcept{if(!storage_||storage_->poisoned)return false;for(std::size_t i=1;i<originalUiTournamentWidgetSlots50KG;++i)if(storage_->dynamic[i]&&!storage_->populated[i])return false;return true;}
bool OriginalUiTournamentWidgets50KG::apply(const game::OriginalUiOptionsMenuCallback50KG* steps,std::size_t count,const OriginalUiTournamentWidgetHooks50KG& hooks,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::owner);auto& s=*storage_;if(s.poisoned||s.inCallback)return fail(d,E::poisoned);
 if((count&&!steps)||count>32768)return fail(d,E::phase);bool pods=false;
 for(std::size_t i=0;i<count;++i){const auto& c=steps[i];d.widget=c.widgetId;d.callback=i;if(!idOk(c.widgetId)||!opOk(c.operation))return fail(d,E::input);if(s.states[c.widgetId].kind==K::text&&(!s.texts[c.widgetId]||!s.populated[c.widgetId]))return fail(d,E::phase);pods=pods||s.states[c.widgetId].kind==K::userRotate;}
 if(pods&&(!hooks.preflight||!hooks.pod))return fail(d,E::missingOwner);
 s.inCallback=true;if(pods&&!hooks.preflight(hooks.context,steps,count)){s.inCallback=false;return fail(d,E::missingOwner);}
 for(std::size_t i=0;i<count;++i){const auto& c=steps[i];d.widget=c.widgetId;d.callback=i;auto& state=s.states[c.widgetId];bool okay=true;
  if(state.kind==K::empty)continue; // Original null factory slot is skipped.
  s.callbacksStarted[c.widgetId]=true;
  if(state.kind==K::text){auto& t=*s.texts[c.widgetId];d.text=c.operation==O::setEnabled?t.setControlEnabled(c.value):t.apply(c.operation,c.value);okay=d.text==TS::ok;}
  else if(state.kind==K::userRotate){auto next=state;okay=hooks.pod(hooks.context,c,next)&&next.kind==state.kind&&next.onTop==state.onTop;if(okay)state=next;}
  else{const auto visibility=[&](bool show,bool hide){if(show!=hide)state.shown=show;};const auto* icon=s.resources->icon(c.widgetId);const bool movement=state.kind==K::movement;
   if(!movement&&!icon)okay=false;else switch(c.operation){case O::show:case O::reset:state.shown=true;break;case O::hide:state.shown=false;break;case O::select:state.selected=true;if(movement)state.shown=true;else visibility(icon->enableOnSelect,icon->disableOnSelect);break;case O::deselect:state.selected=false;if(movement)state.shown=false;else visibility(icon->enableOnDeselect,icon->disableOnDeselect);break;case O::clearSelection:state.selected=false;break;case O::setEnabled:state.selected=c.value;state.shown=c.value;break;}}
  if(!okay){s.inCallback=false;s.poisoned=true;return fail(d,E::callback);}
 }
 s.inCallback=false;d={};return true;
}
OriginalUiTournamentWidgetState50KG OriginalUiTournamentWidgets50KG::state(std::uint16_t id)const noexcept{if(!storage_||!idOk(id))return {};auto v=storage_->states[id];if(v.kind==K::text&&storage_->texts[id]&&storage_->populated[id]){v.selected=storage_->texts[id]->selected();v.shown=storage_->texts[id]->shown();}return v;}
const game::OriginalUiTextWidget50KG* OriginalUiTournamentWidgets50KG::text(std::uint16_t id)const noexcept{return storage_&&idOk(id)&&storage_->populated[id]&&!storage_->poisoned?storage_->texts[id].get():nullptr;}
const game::OriginalUiTextGeometry50KG* OriginalUiTournamentWidgets50KG::textGeometry(std::uint16_t id)const noexcept{if(!storage_||!idOk(id)||storage_->poisoned||(storage_->dynamic[id]&&!storage_->populated[id]))return nullptr;return storage_->dynamic[id]?storage_->geometry[id].get():storage_->resources->staticTextGeometry(id);}
bool OriginalUiTournamentWidgets50KG::poisoned()const noexcept{return storage_&&storage_->poisoned;}
const std::vector<std::uint16_t>* OriginalUiTournamentWidgets50KG::order(bool top)const noexcept{return storage_&&!storage_->poisoned?storage_->resources->order(top):nullptr;}
const assets::OriginalUiOverlayText50KG* OriginalUiTournamentWidgets50KG::textDescriptor(std::uint16_t id)const noexcept{return storage_&&idOk(id)&&storage_->texts[id]&&storage_->populated[id]&&!storage_->poisoned?&storage_->descriptors[id]:nullptr;}
bool OriginalUiTournamentWidgets50KG::updateText(std::uint16_t id,bool shown,D& d)noexcept{
 d={};d.widget=id;if(!textDescriptor(id)||storage_->inCallback)return fail(d,E::text);
 d.text=storage_->texts[id]->updateStatic(shown);return d.text==TS::ok||d.text==TS::hidden?true:fail(d,E::text);
}
bool OriginalUiTournamentWidgets50KG::projectText(std::uint16_t id,std::size_t line,float fontHeight,std::int32_t width,std::int32_t height,bool shown,OriginalUiTournamentTextProjection50KG& out,D& d)noexcept{
 d={};d.widget=id;const auto* descriptor=textDescriptor(id);if(!descriptor||storage_->inCallback||line>=descriptor->lines.size())return fail(d,E::text);
 OriginalUiTournamentTextProjection50KG r;r.descriptor=descriptor;
 if(!game::positionOriginalUiText50KG({descriptor,line,fontHeight,width,height},r.position))return fail(d,E::text);
 d.text=storage_->texts[id]->projectLine(line,r.line,shown);if(d.text!=TS::ok)return fail(d,E::text);out=r;return true;
}
bool OriginalUiTournamentWidgets50KG::projectIcon(std::uint16_t id,bool shown,OriginalUiTournamentIconProjection50KG& out,D& d)const noexcept{
 d={};d.widget=id;if(!storage_||!idOk(id)||storage_->poisoned||storage_->inCallback)return fail(d,E::input);
 const auto& s=*storage_;const auto* desc=s.resources->icon(id);const auto* layout=s.resources->iconLayout(id);const auto* materials=s.resources->materials();const auto image=s.resources->images();const auto m=s.resources->widgetMaterial(id);
 if(!desc||!layout||!shown||!s.states[id].shown||!materials||m>=materials->size()||!image)return fail(d,E::resource);
 OriginalUiTournamentIconProjection50KG r{desc,image.get(),(*materials)[m].image,*layout};
 if(s.hasColor[id])for(unsigned i=0;i<3;++i)r.layout.quad.sourcePacketColor[i]=std::uint8_t(((s.colors[id]>>(16-8*i))&255u)>>1);
 out=r;return true;
}
bool OriginalUiTournamentWidgets50KG::setColor(std::uint16_t id,std::uint32_t color,D& d)noexcept{
 d={};d.widget=id;if(!storage_||!idOk(id)||storage_->poisoned||storage_->inCallback)return fail(d,E::input);
 auto& s=*storage_;if(s.states[id].kind==K::text){if(!s.texts[id]||!s.populated[id])return fail(d,E::phase);d.text=s.texts[id]->setWidgetColor(color);return d.text==TS::ok?true:fail(d,E::text);}
 if(s.states[id].kind!=K::icon)return fail(d,E::input);s.colors[id]=color;s.hasColor[id]=true;return true;
}
}
