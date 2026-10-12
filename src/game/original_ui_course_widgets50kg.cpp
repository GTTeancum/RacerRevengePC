#include "original_ui_course_widgets50kg.h"
#include <array>
#include <utility>
namespace rr::game { namespace {
using K=OriginalUiCourseWidgetKind50KG;using E=OriginalUiCourseWidgetError50KG;using O=OriginalUiControlOperation50KG;using D=OriginalUiCourseWidgetDiagnostic50KG;using SK=OriginalUiCourseWidgetStepKind50KG;
bool fail(D& d,E e)noexcept{d.error=e;return false;}bool idOk(std::uint16_t id)noexcept{return id>0&&id<85;}bool iconKind(K k)noexcept{return k==K::icon||k==K::selectableIcon;}
bool operation(O o)noexcept{switch(o){case O::show:case O::hide:case O::select:case O::deselect:case O::clearSelection:case O::setEnabled:case O::reset:return true;}return false;}
bool selectable(const assets::OriginalUiDat50KG& dat,std::uint16_t id,bool selected,assets::OriginalUiIcon50KG& out,assets::OriginalUiPageDiagnostic50KG& d){
 const auto* s=dat.section("Widget "+std::to_string(id));if(!s||!s->find("Type")||s->find("Type")->text!="SelectableIcon"||!(id==7||id==8||(id>=53&&id<=56)))return false;
 const std::string prefix=selected?"Selected":"Deselected";assets::OriginalUiIcon50KG r;r.identity=dat.identity();r.widgetId=id;const auto* texture=s->find(prefix+"Texture");
 if(!texture||texture->text.empty()||texture->text=="*INVALID*"||texture->text.size()>259)return false;r.textureName=texture->text;r.materialName=texture->text;const auto dot=r.materialName.rfind('.');if(dot!=std::string::npos)r.materialName.resize(dot);
 if(r.materialName.empty()||s->find(prefix+"PartialRect")||s->find(prefix+"MapRect")||s->find(prefix+"Angle")||s->find(prefix+"PivotX")||s->find(prefix+"PivotY"))return false;
 if(!assets::originalUiInteger50KG(*s,prefix+"XLocation",0,r.x,d,selected?0x2AD1D8:0x2AD830)||!assets::originalUiInteger50KG(*s,prefix+"YLocation",0,r.y,d,selected?0x2AD1F0:0x2AD848)||!assets::originalUiInteger50KG(*s,prefix+"Alpha",255,r.alpha,d,selected?0x2AD4C4:0x2ADB1C)||!assets::originalUiNumber50KG(*s,prefix+"Depth",1,r.depth,d,selected?0x2AD520:0x2ADB78))return false;
 if(r.alpha<0||r.alpha>255)return false;std::array<float,3> rgb;if(!assets::originalUiTriple50KG(*s,prefix+"Color",{{200,200,200}},rgb,d,selected?0x2AD5EC:0x2ADC44))return false;r.color=0xff000000;
 for(unsigned i=0;i<3;++i){std::int32_t c=0;if(!roundOriginalUiTextMetric50KG(rgb[i],1,c)||c<0||c>255)return false;r.color|=std::uint32_t(c)<<(16-8*i);}
 if(!assets::originalUiBoolean50KG(*s,"EnableOnSelect",false,r.enableOnSelect,d,0x2AD010)||!assets::originalUiBoolean50KG(*s,"DisableOnSelect",false,r.disableOnSelect,d,0x2AD028)||!assets::originalUiBoolean50KG(*s,"EnableOnDeSelect",false,r.enableOnDeselect,d,0x2AD040)||!assets::originalUiBoolean50KG(*s,"DisableOnDeSelect",false,r.disableOnDeselect,d,0x2AD058)||!assets::originalUiBoolean50KG(*s,"WantsJoystickInput1",false,r.wantsJoystickInput[0],d,0x1CD8E0)||!assets::originalUiBoolean50KG(*s,"WantsJoystickInput2",false,r.wantsJoystickInput[1],d,0x1CD8E0))return false;
 out=std::move(r);return true;}
}
struct OriginalUiCourseWidgets50KG::Storage {
 std::shared_ptr<const assets::OriginalUiArchive50KG> archive;OriginalUiCourseWidgetController50KG controller;OriginalUiNavigation50KG navigation;assets::OriginalUiMovementControl50KG movement;
 std::array<OriginalUiCourseWidgetState50KG,85> states{};std::array<assets::OriginalUiOverlayText50KG,85> texts;std::array<std::array<assets::OriginalUiIcon50KG,2>,85> icons;std::array<OriginalUiTextWidget50KG,85> textWidgets;
 std::array<std::vector<std::string>,85> dynamicLines;std::array<std::array<std::int32_t,2>,85> positions{};std::array<std::uint32_t,85> colors{};std::array<bool,85> hasColor{};
 struct Image{std::shared_ptr<const assets::OriginalUiScreenAssets50KG> owner;std::uint32_t index=0;OriginalUiIconLayout50KG layout{};};std::array<std::array<Image,2>,85> images;
 std::vector<std::uint16_t> ordinary,top;bool poisoned=false,inCallback=false;std::uint32_t promptMask42C=0;
 std::array<std::vector<OriginalUiCoursePopupTextClone50KG>,9> popupClones;
 std::shared_ptr<const assets::OriginalUiScreenAssets50KG> popupImages;
 std::uint8_t popupEntityBits29=0;
};
OriginalUiCourseWidgets50KG::OriginalUiCourseWidgets50KG()noexcept=default;OriginalUiCourseWidgets50KG::~OriginalUiCourseWidgets50KG()=default;
bool OriginalUiCourseWidgets50KG::bind(std::shared_ptr<const assets::OriginalUiArchive50KG> archive,const assets::OriginalUiEnglish50KG* english,D& d)noexcept {
 d={};if(storage_)return fail(d,E::alreadyBound);try{auto s=std::make_unique<Storage>();s->archive=std::move(archive);d.controller=s->controller.bind(s->archive,d.parse);
 if(d.controller!=OriginalUiCourseWidgetControllerStatus50KG::ok||!s->controller.initialNavigation(s->navigation))return fail(d,E::controller);const auto& dat=*s->controller.widgetsDat();
 for(std::uint16_t id=1;id<85;++id){d.widget=id;const auto* section=dat.section("Widget "+std::to_string(id));if(!section)return fail(d,E::population);const auto* type=section->find("Type");if(!type)return fail(d,E::population);auto& state=s->states[id];const auto& t=type->text;
 state.kind=t=="Empty"?K::empty:t=="Overlay_Text"?K::text:t=="Icon"?K::icon:t=="SelectableIcon"?K::selectableIcon:t=="Movement_Control"?K::movement:t=="Soultree_Character"?K::character:K::absent;
 if(state.kind==K::absent)return fail(d,E::population);if(state.kind==K::empty)continue;if(!assets::originalUiBoolean50KG(*section,"OnTop",false,state.onTop,d.parse,0x1CD890))return fail(d,E::parse);
 if(state.kind==K::text){if(!assets::readOriginalUiOverlayText50KG(dat,id,english,s->texts[id],d.parse))return fail(d,E::parse);const auto& text=s->texts[id];if(text.wantsJoystickInput[0]||text.wantsJoystickInput[1])return fail(d,E::input);if(s->textWidgets[id].bind(text)!=OriginalUiTextWidgetStatus50KG::ok)return fail(d,E::text);s->positions[id]={{text.x,text.y}};if(text.textSource==assets::OriginalUiTextSource50KG::dynamic)s->dynamicLines[id]=text.lines;}
 else if(state.kind==K::icon){if(!assets::readOriginalUiIcon50KG(dat,id,s->icons[id][0],d.parse))return fail(d,E::parse);if(s->icons[id][0].wantsJoystickInput[0]||s->icons[id][0].wantsJoystickInput[1])return fail(d,E::input);}
 else if(state.kind==K::selectableIcon){for(unsigned i=0;i<2;++i){if(!selectable(dat,id,i!=0,s->icons[id][i],d.parse))return fail(d,E::parse);if(s->icons[id][i].wantsJoystickInput[0]||s->icons[id][i].wantsJoystickInput[1])return fail(d,E::input);}}
 else if(state.kind==K::movement){if(id!=5||!assets::readOriginalUiMovementControl50KG(dat,id,s->movement,d.parse))return fail(d,E::input);if(s->movement.gridSize||!s->movement.wantsJoystickInput[0]||!s->movement.wantsJoystickInput[1])return fail(d,E::input);for(unsigned i=0;i<16;++i)if(s->movement.mappings[i]!=(i==0?-2:i==1?-1:0))return fail(d,E::input);}
 if(!state.onTop)s->ordinary.push_back(id);}
 for(std::uint16_t id=84;id>0;--id)if(s->states[id].kind!=K::empty&&s->states[id].onTop)s->top.push_back(id);
 for(const auto& c:s->controller.page()->controls)for(auto id:c.widgets)if(!idOk(id)||s->states[id].kind==K::empty||id==5)return fail(d,E::population);
 const auto& page=*s->controller.page();
 if(!page.hasPopup)return fail(d,E::population);
 for(std::size_t i=0;i<page.popupWidgets.size();++i){const auto id=page.popupWidgets[i];
   if(id!=std::int32_t(68+i)||s->states[id].kind!=K::text||s->textWidgets[id].shown())return fail(d,E::population);
   for(const auto& c:page.controls)for(auto member:c.widgets)if(member==id)return fail(d,E::population);
 }
 storage_=std::move(s);d={};return true;}catch(...){return fail(d,E::allocation);}}
const OriginalUiCourseWidgetController50KG* OriginalUiCourseWidgets50KG::controller()const noexcept{return storage_?&storage_->controller:nullptr;}
const OriginalUiNavigation50KG* OriginalUiCourseWidgets50KG::navigation()const noexcept{return storage_?&storage_->navigation:nullptr;}
const std::vector<std::uint16_t>* OriginalUiCourseWidgets50KG::order(bool top)const noexcept{return storage_?(top?&storage_->top:&storage_->ordinary):nullptr;}
bool OriginalUiCourseWidgets50KG::poisoned()const noexcept{return storage_&&storage_->poisoned;}
std::uint32_t OriginalUiCourseWidgets50KG::promptMask42C()const noexcept{return storage_?storage_->promptMask42C:0;}
OriginalUiCourseWidgetState50KG OriginalUiCourseWidgets50KG::state(std::uint16_t id)const noexcept{if(!storage_||!idOk(id))return {};auto s=storage_->states[id];if(s.kind==K::text){s.selected=storage_->textWidgets[id].selected();s.shown=storage_->textWidgets[id].shown();}return s;}
const assets::OriginalUiOverlayText50KG* OriginalUiCourseWidgets50KG::textDescriptor(std::uint16_t id)const noexcept{return storage_&&idOk(id)&&storage_->states[id].kind==K::text?&storage_->texts[id]:nullptr;}
const assets::OriginalUiIcon50KG* OriginalUiCourseWidgets50KG::iconDescriptor(std::uint16_t id,bool selected)const noexcept{if(!storage_||!idOk(id)||!iconKind(storage_->states[id].kind)||(selected&&storage_->states[id].kind!=K::selectableIcon))return nullptr;return &storage_->icons[id][selected?1:0];}
bool OriginalUiCourseWidgets50KG::apply(const OriginalUiCourseWidgetStep50KG* steps,std::size_t count,const OriginalUiCourseWidgetHooks50KG& hooks,D& d)noexcept {
 d={};if(!storage_)return fail(d,E::notBound);auto& s=*storage_;if(s.poisoned||s.inCallback)return fail(d,E::poisoned);if((count&&!steps)||count>32768)return fail(d,E::callback);bool characters=false,audioSteps=false;
 for(std::size_t i=0;i<count;++i){const auto& c=steps[i];d.widget=c.widget;d.callback=i;if(c.kind==SK::widget){if(!idOk(c.widget)||(c.widget>=68&&c.widget<=76)||!operation(c.operation)||s.states[c.widget].kind==K::empty||s.states[c.widget].kind==K::absent)return fail(d,E::callback);characters=characters||s.states[c.widget].kind==K::character;}else if(c.kind==SK::audioStartLoop11||c.kind==SK::audioFade1)audioSteps=true;else if(c.kind==SK::promptMask42C){if(c.argument!=0&&c.argument!=3)return fail(d,E::callback);}else return fail(d,E::callback);}
 if((characters&&!hooks.character)||(audioSteps&&!hooks.audio)||((characters||audioSteps)&&!hooks.preflight))return fail(d,E::missingOwner);s.inCallback=true;if((characters||audioSteps)&&!hooks.preflight(hooks.context,steps,count)){s.inCallback=false;return fail(d,E::missingOwner);}
 for(std::size_t i=0;i<count;++i){const auto& c=steps[i];d.widget=c.widget;d.callback=i;bool ok=true;if(c.kind==SK::promptMask42C)s.promptMask42C=c.argument;else if(c.kind!=SK::widget)ok=hooks.audio(hooks.context,c);
 else{auto& state=s.states[c.widget];if(state.kind==K::text){auto& text=s.textWidgets[c.widget];const auto status=c.operation==O::setEnabled?text.setControlEnabled(c.value):text.apply(c.operation,c.value);ok=status==OriginalUiTextWidgetStatus50KG::ok;}
 else if(state.kind==K::character){auto actual=state;ok=hooks.character(hooks.context,c,actual);ok=ok&&actual.kind==state.kind&&actual.onTop==state.onTop;if(ok)state=actual;}
 else{const auto visibility=[&](bool show,bool hide){if(show!=hide)state.shown=show;};const auto& icon=s.icons[c.widget][0];const bool movement=state.kind==K::movement;switch(c.operation){case O::show:case O::reset:state.shown=true;break;case O::hide:state.shown=false;break;case O::select:state.selected=true;if(movement)state.shown=true;else visibility(icon.enableOnSelect,icon.disableOnSelect);break;case O::deselect:state.selected=false;if(movement)state.shown=false;else visibility(icon.enableOnDeselect,icon.disableOnDeselect);break;case O::clearSelection:state.selected=false;break;case O::setEnabled:state.selected=c.value;state.shown=c.value;break;}}}
 if(!ok){s.inCallback=false;s.poisoned=true;return fail(d,E::callback);}}
 s.inCallback=false;d={};return true;}
bool OriginalUiCourseWidgets50KG::moveCourse(bool forward,const OriginalUiCourseWidgetProfile50KG& p,const OriginalUiCourseWidgetHooks50KG& h,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::notBound);if(storage_->poisoned||storage_->inCallback)return fail(d,E::poisoned);
 OriginalUiCourseWidgetPlan50KG plan;d.controller=storage_->controller.moveCourse(storage_->navigation,forward,p,plan);
 if(d.controller!=OriginalUiCourseWidgetControllerStatus50KG::ok)return fail(d,E::controller);
 if(!apply(plan.steps.data(),plan.count,h,d))return false;storage_->navigation=plan.navigation;return true;
}
bool OriginalUiCourseWidgets50KG::phase(OriginalUiCourseWidgetPhase50KG phase,const OriginalUiCourseWidgetProfile50KG& p,const OriginalUiCourseWidgetHooks50KG& h,D& d)noexcept {
 d={};if(!storage_)return fail(d,E::notBound);if(storage_->poisoned||storage_->inCallback)return fail(d,E::poisoned);OriginalUiCourseWidgetPlan50KG plan;d.controller=storage_->controller.phase(storage_->navigation,phase,p,plan);if(d.controller!=OriginalUiCourseWidgetControllerStatus50KG::ok)return fail(d,E::controller);if(!apply(plan.steps.data(),plan.count,h,d))return false;storage_->navigation=plan.navigation;if(plan.writesPromptMask42C)storage_->promptMask42C=plan.promptMask42C;return true;}
bool OriginalUiCourseWidgets50KG::setDynamicLines(std::uint16_t id,const std::vector<std::string>& lines,D& d)noexcept {d={};d.widget=id;const auto* text=textDescriptor(id);if(!text||storage_->poisoned||storage_->inCallback||text->textSource!=assets::OriginalUiTextSource50KG::dynamic||lines.size()!=text->lines.size())return fail(d,E::text);for(const auto& line:lines)if(line.size()>255||line.size()>std::size_t(text->dynamicLineLength)||line.find('\0')!=std::string::npos)return fail(d,E::text);try{auto r=lines;storage_->dynamicLines[id].swap(r);return true;}catch(...){return fail(d,E::allocation);}}
bool OriginalUiCourseWidgets50KG::setTextPosition(std::uint16_t id,std::int32_t x,std::int32_t y,D& d)noexcept{d={};d.widget=id;if(!textDescriptor(id)||storage_->poisoned||storage_->inCallback)return fail(d,E::text);storage_->positions[id]={{x,y}};return true;}
bool OriginalUiCourseWidgets50KG::setColor(std::uint16_t id,std::uint32_t c,D& d)noexcept{d={};d.widget=id;if(!storage_||storage_->poisoned||storage_->inCallback||!idOk(id))return fail(d,E::input);if(textDescriptor(id)){for(std::size_t i=0;i<storage_->textWidgets[id].lineCount();++i){const auto status=storage_->textWidgets[id].setLineColor(i,c);if(status!=OriginalUiTextWidgetStatus50KG::ok&&status!=OriginalUiTextWidgetStatus50KG::lockedOut)return fail(d,E::text);}return true;}if(!iconDescriptor(id))return fail(d,E::input);storage_->colors[id]=c;storage_->hasColor[id]=true;return true;}
bool OriginalUiCourseWidgets50KG::updateText(std::uint16_t id,bool shown,D& d)noexcept{d={};d.widget=id;if(!textDescriptor(id)||storage_->poisoned||storage_->inCallback)return fail(d,E::text);const auto status=storage_->textWidgets[id].updateStatic(shown);return status==OriginalUiTextWidgetStatus50KG::ok?true:fail(d,status==OriginalUiTextWidgetStatus50KG::hidden?E::hidden:E::text);}
const std::vector<std::string>* OriginalUiCourseWidgets50KG::textLines(std::uint16_t id)const noexcept{const auto* d=textDescriptor(id);if(!d||storage_->poisoned||storage_->inCallback)return nullptr;return d->textSource==assets::OriginalUiTextSource50KG::dynamic?&storage_->dynamicLines[id]:&d->lines;}
const std::array<std::int32_t,2>* OriginalUiCourseWidgets50KG::textPosition(std::uint16_t id)const noexcept{return textDescriptor(id)&&!storage_->poisoned&&!storage_->inCallback?&storage_->positions[id]:nullptr;}
bool OriginalUiCourseWidgets50KG::projectText(std::uint16_t id,std::size_t line,float fontHeight,std::int32_t width,std::int32_t height,bool shown,OriginalUiCourseTextProjection50KG& out,D& d)noexcept{d={};d.widget=id;const auto* text=textDescriptor(id);if(!text||storage_->poisoned||storage_->inCallback||line>=text->lines.size())return fail(d,E::text);try{auto positioned=*text;positioned.x=storage_->positions[id][0];positioned.y=storage_->positions[id][1];if(text->textSource==assets::OriginalUiTextSource50KG::dynamic)positioned.lines=storage_->dynamicLines[id];OriginalUiCourseTextProjection50KG r;r.descriptor=text;if(!positionOriginalUiText50KG({&positioned,line,fontHeight,width,height},r.position))return fail(d,E::text);const auto status=storage_->textWidgets[id].projectLine(line,r.line,shown);if(status!=OriginalUiTextWidgetStatus50KG::ok)return fail(d,status==OriginalUiTextWidgetStatus50KG::hidden?E::hidden:E::text);if(text->textSource==assets::OriginalUiTextSource50KG::dynamic)r.line.text=&storage_->dynamicLines[id][line];out=r;return true;}catch(...){return fail(d,E::allocation);}}
bool OriginalUiCourseWidgets50KG::prepareIcon(std::uint16_t id,bool selected,std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,std::uint32_t index,std::int32_t depth,D& d)noexcept{d={};d.widget=id;const auto* icon=iconDescriptor(id,selected);if(!icon||storage_->poisoned||storage_->inCallback)return fail(d,E::icon);const auto v=selected?1u:0u;if(storage_->images[id][v].owner||!images||!images->loaded()||!images->textures())return fail(d,E::missingResource);const auto* texture=images->textures()->texture(index);if(!texture||!images->decodedTexture(index))return fail(d,E::missingResource);OriginalUiIconLayout50KG layout;if(layoutOriginalUiIcon50KG(*icon,texture->texture,depth,layout)!=OriginalUiIconLayoutStatus50KG::complete)return fail(d,E::icon);layout.quad.sourcePC=0x2DF80C;auto& b=storage_->images[id][v];b.owner=std::move(images);b.index=index;b.layout=layout;return true;}
bool OriginalUiCourseWidgets50KG::projectIcon(std::uint16_t id,bool shown,OriginalUiCourseIconProjection50KG& out,D& d)const noexcept{d={};d.widget=id;if(!storage_||!idOk(id)||storage_->poisoned||storage_->inCallback)return fail(d,E::icon);const auto& state=storage_->states[id];const bool selected=state.kind==K::selectableIcon&&state.selected;const auto* icon=iconDescriptor(id,selected);if(!icon)return fail(d,E::icon);if(!shown||!state.shown)return fail(d,E::hidden);const auto& image=storage_->images[id][selected?1u:0u];if(!image.owner)return fail(d,E::missingResource);OriginalUiCourseIconProjection50KG r{icon,image.owner.get(),image.index,image.layout};if(storage_->hasColor[id]&&(state.kind==K::icon||selected)){const auto color=storage_->colors[id];for(unsigned i=0;i<3;++i)r.layout.quad.sourcePacketColor[i]=std::uint8_t(((color>>(16-8*i))&255u)>>1);}out=r;return true;}
bool OriginalUiCourseWidgets50KG::popupTemplatesHidden438()const noexcept {
 if(!storage_||storage_->poisoned)return false;
 const auto* page=storage_->controller.page();if(!page||!page->hasPopup)return false;
 for(auto id:page->popupWidgets)if(id<1||id>=85||storage_->textWidgets[std::size_t(id)].shown())return false;
 return true;
}
bool OriginalUiCourseWidgets50KG::preparePopupClones438(std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images,
 const std::array<std::uint32_t,9>& fontImages,D& d)noexcept {
 d={};if(!storage_||storage_->popupImages||storage_->inCallback||!popupTemplatesHidden438()||!images||!images->loaded()||!images->textures())return fail(d,E::missingResource);
 try{std::array<std::vector<OriginalUiCoursePopupTextClone50KG>,9> clones;
 const auto& ids=storage_->controller.page()->popupWidgets;
 for(std::size_t i=0;i<ids.size();++i){const auto id=static_cast<std::uint16_t>(ids[i]);d.widget=id;const auto& text=storage_->texts[id];
 const auto image=fontImages[i];const auto* provenance=images->sourceProvenance(image);const auto* font=images->font(image);const auto* atlas=images->textures()->texture(image);
 const auto material=storage_->archive->find("font/"+text.materialName+".psm");
 if(!provenance||!font||!atlas||!material.identity||text.fontName!="denmrk16"||
    provenance->memberIdentity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::fonts,3)||
    provenance->celIdentity!=assets::originalUiMemberIdentity50KG(assets::OriginalUiArchiveId50KG::fonts,2))return fail(d,E::missingResource);
 for(std::size_t line=0;line<text.lines.size();++line){const auto& bytes=text.lines[line];if(bytes.size()>99||bytes.find('\0')!=std::string::npos)return fail(d,E::text);
 OriginalUiCoursePopupTextClone50KG clone;clone.sourceWidget=id;clone.materialIdentity=material.identity;clone.font=font;clone.atlas=&atlas->texture;clone.text=bytes;
 clone.x10=float(text.x);clone.y14=float(text.y)+21.f*float(line);clone.justification50=text.justification;clone.color54=text.color;
 clone.capacity5B=static_cast<std::uint8_t>(bytes.size()+1);clone.characterCount5C=static_cast<std::uint8_t>(bytes.size());
 clone.allocatedPackets5D=static_cast<std::uint8_t>((clone.capacity5B+35u)/36u);clone.activePackets5E=clone.allocatedPackets5D;
 clones[i].push_back(std::move(clone));}}
 storage_->popupClones=std::move(clones);storage_->popupImages=std::move(images);
 //1D66AC invokes popup v18=141520 after construction. Bits0/1 clear;
 // its inherited141460 child notification has no reparented template children.
 storage_->popupEntityBits29=0;d={};return true;
 }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiCourseWidgets50KG::disableFreshPopup438(D& d)noexcept {
 d={};if(!storage_||!storage_->popupImages||storage_->inCallback||!popupTemplatesHidden438())return fail(d,E::missingOwner);
 // Actual fresh-profile1DA714 dispatch to141520. Own entity state only.
 storage_->popupEntityBits29=static_cast<std::uint8_t>(storage_->popupEntityBits29&~3u);return true;
}
bool OriginalUiCourseWidgets50KG::popupEnabled438()const noexcept{return storage_&&(storage_->popupEntityBits29&1u);}
const std::array<std::vector<OriginalUiCoursePopupTextClone50KG>,9>* OriginalUiCourseWidgets50KG::popupClones438()const noexcept{return storage_&&storage_->popupImages?&storage_->popupClones:nullptr;}

}
