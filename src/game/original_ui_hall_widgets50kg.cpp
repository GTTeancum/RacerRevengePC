#include "original_ui_hall_widgets50kg.h"
#include <utility>
namespace rr::game { namespace {
using E=OriginalUiHallWidgetError50KG;using D=OriginalUiHallWidgetDiagnostic50KG;using K=OriginalUiHallWidgetKind50KG;using O=OriginalUiControlOperation50KG;using T=OriginalUiTextWidgetStatus50KG;
bool fail(D& d,E e)noexcept{d.error=e;return false;}
bool idOk(std::uint16_t id)noexcept{return id>0&&id<originalUiHallWidgetStorageCount50KG;}
bool operation(O o)noexcept{switch(o){case O::show:case O::hide:case O::select:case O::deselect:case O::clearSelection:case O::setEnabled:case O::reset:return true;}return false;}
}
struct OriginalUiHallWidgets50KG::Storage {
 std::shared_ptr<const assets::OriginalUiArchive50KG> archive;assets::OriginalUiDat50KG uiDat,pageDat,widgets;assets::OriginalUiPageIndex50KG index;assets::OriginalUiPage50KG page;assets::OriginalUiMovementControl50KG movement;
 std::array<OriginalUiHallWidgetState50KG,31> states{};std::array<assets::OriginalUiOverlayText50KG,31> texts;std::array<assets::OriginalUiIcon50KG,31> icons;std::array<OriginalUiTextWidget50KG,31> textWidgets;
 struct Image{std::shared_ptr<const assets::OriginalUiScreenAssets50KG> owner;std::uint32_t index=0;OriginalUiIconLayout50KG layout{};};std::array<Image,31> images;
 std::vector<std::uint16_t> ordinary,top;
};
OriginalUiHallWidgets50KG::OriginalUiHallWidgets50KG()noexcept=default;OriginalUiHallWidgets50KG::~OriginalUiHallWidgets50KG()=default;
bool OriginalUiHallWidgets50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,const assets::OriginalUiEnglish50KG* english,D& d)noexcept{
 d={};if(storage_)return fail(d,E::alreadyBound);
 if(!archive||!archive->loaded()||archive->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::hallOfFame))return fail(d,E::archive);
 try{auto s=std::make_unique<Storage>();s->archive=std::move(archive);
 if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UIHOF/ui.dat",s->uiDat,d.parse)||!assets::readOriginalUiPageIndex50KG(s->uiDat,s->index,d.parse)||s->index.pageFiles!=std::vector<std::string>{"hall.dat"})return fail(d,E::parse);
 if(!assets::readOriginalUiDat50KG(*s->archive,"UI/UIHOF/hall.dat",s->pageDat,d.parse)||!assets::readOriginalUiDat50KG(*s->archive,"UI/UIHOF/Widgets.Dat",s->widgets,d.parse,assets::OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)||!assets::readOriginalUiPage50KG(s->pageDat,s->widgets,s->page,d.parse))return fail(d,E::parse);
 if(s->page.pageNo!=1||s->page.widgetCount!=30||s->page.groups.size()!=3||s->page.controls.size()!=17||s->page.groups[1].controls.size()!=13)return fail(d,E::population);
 for(std::size_t i=0;i<13;++i){auto c=s->page.groups[1].controls[i];if(c<0||std::size_t(c)>=s->page.controls.size()||s->page.controls[c].widgets!=std::vector<std::uint16_t>{originalUiHallCourseWidgets50KG[i]})return fail(d,E::population);}
 for(std::uint16_t id=1;id<=30;++id){d.widget=id;const auto* section=s->widgets.section("Widget "+std::to_string(id));if(!section||!section->find("Type"))return fail(d,E::population);const auto& type=section->find("Type")->text;auto& state=s->states[id];state.kind=type=="Empty"?K::empty:type=="Overlay_Text"?K::text:type=="Icon"?K::icon:type=="Movement_Control"?K::movement:K::absent;
 if(state.kind==K::absent||(state.kind==K::empty&&id!=4&&id!=19))return fail(d,E::population);
 if(state.kind==K::empty)continue;
 if(!assets::originalUiBoolean50KG(*section,"OnTop",false,state.onTop,d.parse,0x1CD890))return fail(d,E::parse);
 if(state.kind==K::text){if(!assets::readOriginalUiOverlayText50KG(s->widgets,id,english,s->texts[id],d.parse))return fail(d,E::parse);const auto& text=s->texts[id];if(text.wantsJoystickInput[0]||text.wantsJoystickInput[1]||text.textSource==assets::OriginalUiTextSource50KG::dynamic)return fail(d,E::input);if(s->textWidgets[id].bind(text)!=T::ok)return fail(d,E::text);}
 else if(state.kind==K::icon){if(!assets::readOriginalUiIcon50KG(s->widgets,id,s->icons[id],d.parse))return fail(d,E::parse);if(s->icons[id].wantsJoystickInput[0]||s->icons[id].wantsJoystickInput[1])return fail(d,E::input);}
 else if(state.kind==K::movement){if(id!=5||!assets::readOriginalUiMovementControl50KG(s->widgets,id,s->movement,d.parse)||!s->movement.wantsJoystickInput[0]||s->movement.wantsJoystickInput[1]||s->movement.gridSize)return fail(d,E::input);for(auto m:s->movement.mappings)if(m)return fail(d,E::input);}
 if(!state.onTop)s->ordinary.push_back(id);}
 for(std::uint16_t id=30;id;--id)if(s->states[id].kind!=K::empty&&s->states[id].onTop)s->top.push_back(id);
 for(const auto& c:s->page.controls)for(auto id:c.widgets)if(!idOk(id)||s->states[id].kind==K::empty||id==5)return fail(d,E::population);
 storage_=std::move(s);d={};return true;}catch(...){return fail(d,E::allocation);}}
const assets::OriginalUiPage50KG* OriginalUiHallWidgets50KG::page()const noexcept{return storage_?&storage_->page:nullptr;}
const assets::OriginalUiDat50KG* OriginalUiHallWidgets50KG::widgetsDat()const noexcept{return storage_?&storage_->widgets:nullptr;}
const assets::OriginalUiMovementControl50KG* OriginalUiHallWidgets50KG::movement()const noexcept{return storage_?&storage_->movement:nullptr;}
const std::vector<std::uint16_t>* OriginalUiHallWidgets50KG::order(bool top)const noexcept{return storage_?(top?&storage_->top:&storage_->ordinary):nullptr;}
OriginalUiHallWidgetState50KG OriginalUiHallWidgets50KG::state(std::uint16_t id)const noexcept{if(!storage_||!idOk(id))return {};auto s=storage_->states[id];if(s.kind==K::text){s.selected=storage_->textWidgets[id].selected();s.shown=storage_->textWidgets[id].shown();}return s;}
const assets::OriginalUiOverlayText50KG* OriginalUiHallWidgets50KG::textDescriptor(std::uint16_t id)const noexcept{return storage_&&idOk(id)&&storage_->states[id].kind==K::text?&storage_->texts[id]:nullptr;}
const assets::OriginalUiIcon50KG* OriginalUiHallWidgets50KG::iconDescriptor(std::uint16_t id)const noexcept{return storage_&&idOk(id)&&storage_->states[id].kind==K::icon?&storage_->icons[id]:nullptr;}
const OriginalUiTextWidget50KG* OriginalUiHallWidgets50KG::textWidget(std::uint16_t id)const noexcept{return textDescriptor(id)?&storage_->textWidgets[id]:nullptr;}
bool OriginalUiHallWidgets50KG::preflight(const OriginalUiOptionsMenuCallback50KG* callbacks,std::size_t count,D& d)const noexcept{
 d={};if(!storage_)return fail(d,E::notBound);if((count&&!callbacks)||count>256)return fail(d,E::callback);
 for(std::size_t i=0;i<count;++i){const auto& c=callbacks[i];d.widget=c.widgetId;d.callback=i;d.sourcePC=c.sourcePC;
 if(!idOk(c.widgetId)||storage_->states[c.widgetId].kind==K::empty||!operation(c.operation))return fail(d,E::callback);}return true;}
bool OriginalUiHallWidgets50KG::apply(const OriginalUiOptionsMenuCallback50KG* callbacks,std::size_t count,D& d)noexcept{
 if(!preflight(callbacks,count,d))return false;
 for(std::size_t i=0;i<count;++i){const auto& c=callbacks[i];auto& state=storage_->states[c.widgetId];d.widget=c.widgetId;d.callback=i;d.sourcePC=c.sourcePC;
 if(state.kind==K::text){auto& text=storage_->textWidgets[c.widgetId];if((c.operation==O::setEnabled?text.setControlEnabled(c.value):text.apply(c.operation,c.value))!=T::ok)return fail(d,E::text);}
 else{const auto visibility=[&](bool show,bool hide){if(show!=hide)state.shown=show;};const auto& icon=storage_->icons[c.widgetId];switch(c.operation){case O::show:case O::reset:state.shown=true;break;case O::hide:state.shown=false;break;case O::select:state.selected=true;if(state.kind==K::icon)visibility(icon.enableOnSelect,icon.disableOnSelect);break;case O::deselect:state.selected=false;if(state.kind==K::icon)visibility(icon.enableOnDeselect,icon.disableOnDeselect);break;case O::clearSelection:state.selected=false;break;case O::setEnabled:state.selected=c.value;state.shown=c.value;break;}}}
 d={};return true;}
bool OriginalUiHallWidgets50KG::setWidgetColor(std::uint16_t id,std::uint32_t color,D& d)noexcept{
 d={};d.widget=id;if(!storage_||!idOk(id))return fail(d,E::notBound);
 if(storage_->states[id].kind==K::text){if(storage_->textWidgets[id].setWidgetColor(color)!=T::ok)return fail(d,E::text);}
 else if(storage_->states[id].kind==K::icon){auto& image=storage_->images[id];if(!image.owner)return fail(d,E::missingResource);
  image.layout.quad.sourcePacketColor=originalUiTextPacketColor50KG(color,static_cast<std::int16_t>(storage_->icons[id].alpha));}
 else return fail(d,E::input);return true;}
bool OriginalUiHallWidgets50KG::updateText(std::uint16_t id,bool shown,D& d)noexcept{d={};d.widget=id;if(!textDescriptor(id))return fail(d,E::text);auto s=storage_->textWidgets[id].updateStatic(shown);return s==T::ok?true:fail(d,s==T::hidden?E::hidden:E::text);}
bool OriginalUiHallWidgets50KG::projectText(std::uint16_t id,std::size_t line,float fontHeight,std::int32_t width,std::int32_t height,bool shown,OriginalUiHallTextProjection50KG& out,D& d)noexcept{
 d={};d.widget=id;const auto* text=textDescriptor(id);if(!text)return fail(d,E::text);OriginalUiHallTextProjection50KG p;p.descriptor=text;
 if(!positionOriginalUiText50KG({text,line,fontHeight,width,height},p.position))return fail(d,E::text);auto status=storage_->textWidgets[id].projectLine(line,p.line,shown);if(status!=T::ok)return fail(d,status==T::hidden?E::hidden:E::text);out=p;return true;}
bool OriginalUiHallWidgets50KG::prepareIcon(std::uint16_t id,std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,std::uint32_t index,std::int32_t depth,D& d)noexcept{
 d={};d.widget=id;const auto* icon=iconDescriptor(id);if(!icon)return fail(d,E::icon);if(storage_->images[id].owner||!images||!images->loaded()||!images->textures())return fail(d,E::missingResource);const auto* image=images->textures()->texture(index);if(!image||!images->decodedTexture(index))return fail(d,E::missingResource);
 OriginalUiIconLayout50KG layout;if(layoutOriginalUiIcon50KG(*icon,image->texture,depth,layout)!=OriginalUiIconLayoutStatus50KG::complete)return fail(d,E::icon);auto& b=storage_->images[id];b.owner=std::move(images);b.index=index;b.layout=layout;return true;}
bool OriginalUiHallWidgets50KG::projectIcon(std::uint16_t id,bool shown,OriginalUiHallIconProjection50KG& out,D& d)const noexcept{
 d={};d.widget=id;const auto* icon=iconDescriptor(id);if(!icon)return fail(d,E::icon);if(!shown||!storage_->states[id].shown)return fail(d,E::hidden);const auto& image=storage_->images[id];if(!image.owner)return fail(d,E::missingResource);out={icon,image.owner.get(),image.index,image.layout};return true;}
}
