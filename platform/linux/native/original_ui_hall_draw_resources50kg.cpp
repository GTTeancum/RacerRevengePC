#include "original_ui_hall_draw_resources50kg.h"
#include "original_ui_hall_addon50dl.h"
#include "original_ui_material_adapter50kg.h"
#include <cmath>
#include <algorithm>
#include "../../../src/assets/original_ui_hall_background50kg.h"
#include <utility>
#include <charconv>
#include <cstring>
namespace rr::platform {namespace {
using Error=OriginalUiHallDrawError50KG;using Diagnostic=OriginalUiHallDrawDiagnostic50KG;using Archive=assets::OriginalUiArchiveId50KG;constexpr auto unresolved=assets::unresolvedMaterialTexture50EA;
bool fail(Diagnostic& d,Error e,std::uint32_t pc=0)noexcept{d.error=e;d.sourcePC=pc;return false;}
char fold(char c)noexcept{return c=='\\'?'/':c>='A'&&c<='Z'?char(c+32):c;}
bool same(const std::string& a,const std::string& b)noexcept{if(a.size()!=b.size())return false;for(std::size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;return true;}
bool authentic(const std::shared_ptr<const assets::OriginalUiArchive50KG>& a,Archive id)noexcept{return a&&a->loaded()&&a->identity()==assets::originalUiArchiveIdentity50KG(id);}
bool validVideo(const OriginalUiHallVideoOwner50KG& v)noexcept{return v.lifetime&&v.width==640&&v.height==448&&v.colorBits==32&&v.packetZMaximum==0xffffff&&v.zBufferFormat==0x31;}
std::uint64_t imageIdentity(Archive id,const assets::OriginalUiMemberView50KG& m,std::size_t off)noexcept{const auto* a=assets::originalUiArchiveIdentity50KG(id);if(!a||off>0xffffffffu)return 0;for(std::size_t i=0;i<a->memberCount;++i)if(assets::originalUiMemberIdentity50KG(id,i)==m.identity)return(std::uint64_t(static_cast<std::uint8_t>(id)+1u)<<56)|(std::uint64_t(i+1u)<<32)|std::uint64_t(off);return 0;}
bool sameSampler(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b)noexcept{return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;}
std::uint32_t find(const std::vector<OriginalUiHallMaterialBinding50KG>& l,const std::string& n)noexcept{for(std::size_t i=0;i<l.size();++i)if(same(l[i].registeredName,n))return static_cast<std::uint32_t>(i);return unresolved;}
bool registry(const assets::OriginalUiMemberView50KG& t,std::vector<OriginalUiHallMaterialBinding50KG>& b){
 if(!t.identity||!t.data||t.size!=70962)return false;const auto delim=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};std::vector<bool> found(b.size(),false);std::size_t cursor=0;std::uint32_t token=0;
 while(cursor<t.size){while(cursor<t.size&&delim(t.data[cursor]))++cursor;if(cursor==t.size)break;const auto start=cursor;while(cursor<t.size&&!delim(t.data[cursor]))++cursor;
 for(std::size_t i=0;i<b.size();++i){auto& row=b[i];if(row.registeredName.size()!=cursor-start)continue;std::size_t j=0;for(;j<row.registeredName.size()&&fold(char(t.data[start+j]))==fold(row.registeredName[j]);++j){}if(j!=row.registeredName.size())continue;if(found[i])return false;found[i]=true;row.registryToken=token;row.registryIdentity=t.identity;}++token;}
 if(token!=3235)return false;for(bool f:found)if(!f)return false;return true;}
bool animation(const assets::OriginalUiMaterialPass50KG& p,game::OriginalUiTextureAnimationConfig50KG& out,Diagnostic& d){const auto* mode=p.authored.find("TextureAnimation");if(!mode||!same(mode->text,"unique"))return fail(d,Error::animation,0x29b950);game::OriginalUiTextureAnimationConfig50KG c;assets::OriginalUiPageDiagnostic50KG pd;std::int32_t looping=1,reverse=0,loopTo=-1;
 if(!assets::originalUiNumber50KG(p.authored,"animatedMapInitialFrame",0,c.initialFrame,pd,0x29b950)||!assets::originalUiNumber50KG(p.authored,"animatedMapFramesPerSecond",1,c.framesPerSecond,pd,0x29b950)||!assets::originalUiInteger50KG(p.authored,"animatedMapLooping",1,looping,pd,0x29b950)||!assets::originalUiInteger50KG(p.authored,"animatedMapReverseLoopOnEnd",0,reverse,pd,0x29b950)||!assets::originalUiInteger50KG(p.authored,"animatedMapLoopOnEndToFrame",-1,loopTo,pd,0x29b950))return fail(d,Error::animation,0x29b950);
 c.looping=looping!=0;c.reverseLoopOnEnd=reverse!=0;c.loopOnEndToFrame=loopTo;out=c;return true;}
game::OriginalUiOverlayQuad50KG quad(const char* m,float x,float y,float w,float h,float u0,float v0,float u1,float v1,float depth,std::uint32_t pc)noexcept{game::OriginalUiOverlayQuad50KG q;q.materialName=m;q.authoredDepth=depth;q.sourcePC=pc;q.vertices={{{x,y,u0,v0},{x+w,y,u1,v0},{x,y+h,u0,v1},{x+w,y+h,u1,v1}}};return q;}
}
struct OriginalUiHallDrawResources50KG::Storage {
 OriginalUiHallColdInput50KG owners;assets::OriginalUiMaterialTemplates50KG templates;
 std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;assets::OriginalUiImageBank50KG imageBank;
 game::OriginalUiHallWidgets50KG widgets;std::vector<OriginalUiHallMaterialBinding50KG> bindings;
 std::vector<MaterialTextureSampler50EB> samplers;
 std::array<std::unique_ptr<game::OriginalUiTextGeometry50KG>,31> text;
 std::array<std::uint32_t,31> widgetMaterial{};
 std::array<std::uint32_t,6> backgroundImages{{unresolved,unresolved,unresolved,unresolved,unresolved,unresolved}};
 // Page+154 is null at 2A5CB0; absent PageInfo.Background defaults to
 // *INVALID* at 2A5F4C, which 2A6530 does not load. +1464 starts -1.
 std::uint32_t currentBackgroundImage=unresolved;
 game::OriginalUiExitText50KG exitText;game::OriginalUiHallCamera50KG camera;
 std::array<std::string,3> ranks;std::array<std::string,6> planetNames;
 std::array<game::OriginalUiOverlayQuad50KG,9> overlays;
 std::array<game::OriginalUiTextLayout50KG,28> rowGeometry,scratchGeometry;
 std::array<std::array<char,22>,28> rowText{};
 std::array<OriginalUiHallPagePacket50KG,37> pagePackets{};std::size_t pageCount=0;
};
OriginalUiHallDrawResources50KG::OriginalUiHallDrawResources50KG()noexcept=default;
OriginalUiHallDrawResources50KG::~OriginalUiHallDrawResources50KG()=default;
bool OriginalUiHallDrawResources50KG::load(const OriginalUiHallColdInput50KG& in,Diagnostic& d)noexcept{
 d={};if(storage_)return fail(d,Error::alreadyLoaded);
 if(!authentic(in.appStart,Archive::appStart)||!authentic(in.hall,Archive::hallOfFame)||!authentic(in.fonts,Archive::fonts)||!authentic(in.controllerPopup,Archive::controllerPopup)||!authentic(in.overlays,Archive::overlays))return fail(d,Error::archive);
 if(!validVideo(in.video))return fail(d,Error::videoOwner);
 if(!in.english||!in.english->loaded()||!in.runtime.overrideKnown)return fail(d,Error::input);
 try{auto s=std::make_unique<Storage>();s->owners=in;s->owners.english=nullptr;s->widgetMaterial.fill(unresolved);
 if(!s->widgets.bind(in.hall,in.english,d.widgets))return fail(d,Error::widgets);
 if(!game::prepareOriginalUiHallCamera50KG(*in.hall,s->camera,d.camera))return fail(d,Error::geometry,d.camera.sourcePC);
 if(!s->templates.load(*in.appStart,d.material))return fail(d,Error::material);
 s->bindings.reserve(15);std::vector<assets::OriginalUiImageSource50KG> sources;
 const auto addSource=[&](const assets::OriginalUiImageSource50KG& x)->std::uint32_t{
   for(std::size_t i=0;i<sources.size();++i){const auto& o=sources[i];if(o.member.identity==x.member.identity&&o.pstOffset==x.pstOffset&&o.pstBytes==x.pstBytes&&o.cel.identity==x.cel.identity)return static_cast<std::uint32_t>(i);}
   sources.push_back(x);return static_cast<std::uint32_t>(sources.size()-1);};
 const auto addImages=[&](OriginalUiHallMaterialBinding50KG& b,const assets::OriginalUiArchive50KG& owner,const std::string& name,bool font,bool animated)->bool{
   const auto member=owner.find((font?"font/":"ui/textures/")+name+(animated?".atm":".pst"));if(!member.identity||!member.data)return fail(d,Error::resource);b.animated=animated;
   if(animated){assets::OriginalUiAtm50KG atm;if(!assets::splitOriginalUiAtm50KG(member.data,member.size,atm,d.atm))return fail(d,Error::resource);
     for(const auto& f:atm.frames){const auto identity=imageIdentity(owner.identity()->id,member,f.pstOffset);if(!identity)return fail(d,Error::resource);b.images.push_back(addSource({identity,member,f.pstOffset,f.pstBytes,{}}));}}
   else{const auto cel=font?owner.find("font/"+name+".cel"):assets::OriginalUiMemberView50KG{};if(font&&(!cel.identity||!cel.data))return fail(d,Error::resource);
     const auto identity=imageIdentity(owner.identity()->id,member,0);if(!identity)return fail(d,Error::resource);b.images.push_back(addSource({identity,member,0,member.size,cel}));}
   if(b.material.passes.size()!=1)return fail(d,Error::material);const auto& pass=b.material.passes[0];
   if(pass.textureCount!=1||!same(pass.textureName,name))return fail(d,Error::material);
   if(animated){if(!animation(pass,b.animation,d)||!game::validOriginalUiTextureAnimationConfig50KG(b.animation,static_cast<std::uint32_t>(b.images.size())))return fail(d,Error::animation,0x29b950);}
   else if(pass.authored.find("TextureAnimation"))return fail(d,Error::animation,0x29b950);
   return true;};
 const auto authored=[&](const char* material,const char* texture,bool font,bool animated)->bool{
   OriginalUiHallMaterialBinding50KG b;b.registeredName=material;
   if(!assets::readOriginalUiMaterial50KG(*in.hall,b.registeredName+".psm",s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material);
   if(texture&&!addImages(b,font?*in.fonts:same(texture,"overlay1")?*in.controllerPopup:*in.hall,texture,font,animated))return false;
   if(!texture&&(b.material.passes.size()!=1||b.material.passes[0].textureCount!=0))return fail(d,Error::material);
   s->bindings.push_back(std::move(b));return true;};
 if(!authored("font/denmrk16top","denmrk16",true,false)||!authored("font/denmrk16","denmrk16",true,false)||
    !authored("font/denmrk14","denmrk14",true,false)||!authored("font/denmrk14top","denmrk14",true,false)||
    !authored("font/venus16top","venus16",true,false)||!authored("ui/textures/arrwlefttop","arrwleft",false,true)||
    !authored("ui/textures/arrwrghttop","arrwrght",false,true)||!authored("ui/textures/overlay1","overlay1",false,false)||
    !authored("ui/textures/notexture",nullptr,false,false))return false;
 {OriginalUiHallMaterialBinding50KG b;b.registeredName="ui/textures/nameblur";
  if(!assets::readOriginalUiHallNameBlurFallbackMaterial50KG(*in.hall,s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material);
  if(!addImages(b,*in.hall,"nameblur",false,false))return false;s->bindings.push_back(std::move(b));}
 {OriginalUiHallMaterialBinding50KG b;b.registeredName="font/venus16";
  if(!assets::readOriginalUiHallFontFallbackMaterial50KG(*in.hall,*in.fonts,s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material);
  if(!addImages(b,*in.fonts,"venus16",true,false))return false;s->bindings.push_back(std::move(b));}
 constexpr const char* prompts[]={"icon_x","icon_tri","icon_sqr","icon_cir"};
 for(std::size_t i=0;i<4;++i){OriginalUiHallMaterialBinding50KG b;b.registeredName=std::string("ui/textures/")+prompts[i];
  if(!assets::readOriginalUiHallPromptFallbackMaterial50KG(*in.hall,*in.overlays,static_cast<assets::OriginalUiMainPrompt50KG>(i),s->templates,assets::OriginalUiTemplate50KG::screenMt,in.runtime,b.material,d.material))return fail(d,Error::material);
  if(!addImages(b,i<2?*in.overlays:*in.hall,prompts[i],false,false))return false;s->bindings.push_back(std::move(b));}
 // All five real source backgrounds remain pinned in this one immutable bank.
 for(auto planet:{0u,1u,2u,4u,5u}){const auto raw=in.hall->find(assets::originalUiHallBackgroundName50KG(planet));
   const auto identity=imageIdentity(Archive::hallOfFame,raw,0);
   if(!assets::isOriginalUiHallBackground50KG(raw)||!identity)return fail(d,Error::resource,0x2a6588);
   s->backgroundImages[planet]=addSource({identity,raw,0,raw.size,{}});}
 if(!registry(in.appStart->find("AppStart/Mat_Stbl.txt"),s->bindings))return fail(d,Error::registry,0x298724);
 auto originalImages=std::make_shared<assets::OriginalUiScreenAssets50KG>();
 if(!originalImages->load(sources,d.resource))return fail(d,Error::resource);
 assets::OriginalUiImageBankInput50KG bankInput;bankInput.archives={in.appStart,in.hall,in.fonts,in.controllerPopup,in.overlays};bankInput.owners={originalImages};
 assets::OriginalUiImageBankDiagnostic50KG bankDiagnostic;
 if(!s->imageBank.load(bankInput,bankDiagnostic))return fail(d,Error::resource);
 for(std::uint32_t i=0;i<sources.size();++i){const auto* map=s->imageBank.mapping(0,i);if(!map||map->combinedIndex!=i)return fail(d,Error::resource);}
 s->images=s->imageBank.images();const auto* bank=s->images->textures();if(!bank)return fail(d,Error::resource);
 s->samplers.resize(bank->textureCount());std::vector<bool> samplerSet(bank->textureCount(),false);
 for(std::size_t i=0;i<s->bindings.size();++i){d.materialIndex=static_cast<std::uint32_t>(i);auto& b=s->bindings[i];const auto& pass=b.material.passes[0];
  if(b.images.empty()){if(!assets::prepareOriginalUiNativeUntexturedMaterial50KG(pass,b.untextured,d.material))return fail(d,Error::material);continue;}
  for(auto image:b.images){d.imageIndex=image;const auto* decoded=s->images->decodedTexture(image);assets::OriginalUiNativeMaterial50KG policy;
   if(!decoded||!assets::prepareOriginalUiNativeMaterial50KG(pass,*decoded,policy,d.material))return fail(d,Error::material);
   OriginalUiMaterialSpan50KG converted;if(!convertOriginalUiMaterialSpan50KG(policy,{0,1,1,image,bank->textureCount()},converted))return fail(d,Error::material);
   if(samplerSet[image]&&!sameSampler(s->samplers[image],converted.native.sampler))return fail(d,Error::samplerConflict);
   samplerSet[image]=true;s->samplers[image]=converted.native.sampler;b.policies.push_back(policy);}}
 for(auto image:s->backgroundImages)if(image!=unresolved){auto& sampler=s->samplers[image];
  sampler.minification=sampler.magnification=MaterialTextureFilter50EB::linear;
  // Explicit native raw-texture adaptation; source inherited CLAMP is unclosed.
  sampler.s=sampler.t=MaterialTextureWrap50EB::clampToEdge;samplerSet[image]=true;}
 for(bool initialized:samplerSet)if(!initialized)return fail(d,Error::resource);
 for(std::uint16_t id=1;id<31;++id){d.widgetId=id;
  if(const auto* text=s->widgets.textDescriptor(id)){const auto m=find(s->bindings,"font/"+text->materialName);if(m==unresolved)return fail(d,Error::material);
   const auto& b=s->bindings[m];if(b.images.size()!=1||!same(b.material.passes[0].textureName,text->fontName))return fail(d,Error::material);
   const auto* font=s->images->font(b.images[0]);const auto* atlas=bank->texture(b.images[0]);if(!font||!atlas)return fail(d,Error::resource);
   if(text->depth!=1)return fail(d,Error::geometry);auto geometry=std::make_unique<game::OriginalUiTextGeometry50KG>();
   if(!geometry->prepare(*text,*font,atlas->texture,d.text))return fail(d,Error::geometry);s->text[id]=std::move(geometry);s->widgetMaterial[id]=m;
  }else if(const auto* icon=s->widgets.iconDescriptor(id)){const auto m=find(s->bindings,"ui/textures/"+icon->materialName);if(m==unresolved)return fail(d,Error::material);
   const auto& b=s->bindings[m];if(b.images.empty()||icon->depth!=1)return fail(d,Error::resource);
   if(!s->widgets.prepareIcon(id,s->images,b.images[0],in.video.packetZMaximum,d.widgets))return fail(d,Error::widgets);s->widgetMaterial[id]=m;}}
 const auto denmark=find(s->bindings,"font/denmrk16top");
 if(denmark==unresolved||s->bindings[denmark].images.size()!=1)return fail(d,Error::resource);
 if(s->exitText.prepare({11,1},in.hall,*in.english,s->images,s->bindings[denmark].images[0],d.exitText)!=game::OriginalUiExitTextStatus50KG::ok)return fail(d,Error::geometry,0x1d6aec);
 for(std::size_t i=0;i<3;++i){const auto* localized=in.english->at(327+i);if(!localized||localized->text.size()>3)return fail(d,Error::resource,0x1e0f40);s->ranks[i]=localized->text;}
 constexpr std::uint32_t names[]={11,15,12,16,14,13};for(std::size_t i=0;i<6;++i){const auto* localized=in.english->at(names[i]);if(!localized||localized->text.size()>14)return fail(d,Error::resource,0x1e1850);s->planetNames[i]=localized->text;}
 const auto overlayIndex=find(s->bindings,"ui/textures/overlay1");
 const auto* overlayImage=bank->texture(s->bindings[overlayIndex].images[0]);
 if(!overlayImage||overlayImage->texture.width!=32||overlayImage->texture.height!=32)return fail(d,Error::geometry,0x1e0760);
 constexpr float y[]={77,163,249,149,235,321,109,195,281};
 for(std::size_t i=0;i<9;++i){const float v0=i<3?0:i<6?1:.75f,v1=i<3?1:i<6?0:1;
  s->overlays[i]=quad("ui/textures/overlay1",0,y[i],640,i<6?32:40,0,v0,1,v1,1,0x1e17f0+static_cast<std::uint32_t>(i)*8u);}
 storage_=std::move(s);d={};return true;
 }catch(...){return fail(d,Error::allocation);}}
bool OriginalUiHallDrawResources50KG::loaded()const noexcept{return bool(storage_);}
game::OriginalUiHallWidgets50KG* OriginalUiHallDrawResources50KG::widgets()noexcept{return storage_?&storage_->widgets:nullptr;}
const game::OriginalUiHallWidgets50KG* OriginalUiHallDrawResources50KG::widgets()const noexcept{return storage_?&storage_->widgets:nullptr;}
const game::OriginalUiHallCamera50KG* OriginalUiHallDrawResources50KG::camera()const noexcept{return storage_?&storage_->camera:nullptr;}
game::OriginalUiExitText50KG* OriginalUiHallDrawResources50KG::exitText()noexcept{return storage_?&storage_->exitText:nullptr;}
const assets::OriginalUiScreenAssets50KG* OriginalUiHallDrawResources50KG::images()const noexcept{return storage_?storage_->images.get():nullptr;}
const assets::OriginalUiImageBank50KG* OriginalUiHallDrawResources50KG::sourceImageBank()const noexcept{return storage_?&storage_->imageBank:nullptr;}
const assets::MaterialTextureAssets50EA* OriginalUiHallDrawResources50KG::textures()const noexcept{return storage_?storage_->images->textures():nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiHallDrawResources50KG::samplers()const noexcept{return storage_?&storage_->samplers:nullptr;}
const std::vector<OriginalUiHallMaterialBinding50KG>* OriginalUiHallDrawResources50KG::materials()const noexcept{return storage_?&storage_->bindings:nullptr;}
const OriginalUiHallVideoOwner50KG* OriginalUiHallDrawResources50KG::video()const noexcept{return storage_?&storage_->owners.video:nullptr;}
const game::OriginalUiTextGeometry50KG* OriginalUiHallDrawResources50KG::textGeometry(std::uint16_t id)const noexcept{return storage_&&id<31?storage_->text[id].get():nullptr;}
const game::OriginalUiPositionedLine50KG* OriginalUiHallDrawResources50KG::textGeometryLine(std::uint16_t id,std::size_t line)const noexcept{const auto* g=textGeometry(id);return g&&line<g->lines().size()?&g->lines()[line]:nullptr;}
std::uint32_t OriginalUiHallDrawResources50KG::widgetMaterial(std::uint16_t id)const noexcept{return storage_&&id<31?storage_->widgetMaterial[id]:unresolved;}
std::uint32_t OriginalUiHallDrawResources50KG::materialIndex(const char* name)const noexcept{if(!storage_||!name)return unresolved;for(std::size_t i=0;i<storage_->bindings.size();++i){const auto& a=storage_->bindings[i].registeredName;std::size_t j=0;for(;j<a.size()&&name[j]&&fold(a[j])==fold(name[j]);++j){}if(j==a.size()&&!name[j])return static_cast<std::uint32_t>(i);}return unresolved;}
std::uint32_t OriginalUiHallDrawResources50KG::backgroundImage(std::uint32_t planet)const noexcept{return storage_&&planet<6?storage_->backgroundImages[planet]:unresolved;}
std::uint32_t OriginalUiHallDrawResources50KG::currentBackgroundImage()const noexcept{return storage_?storage_->currentBackgroundImage:unresolved;}
const OriginalUiHallPagePacket50KG* OriginalUiHallDrawResources50KG::pagePackets()const noexcept{return storage_&&storage_->pageCount?storage_->pagePackets.data():nullptr;}
std::size_t OriginalUiHallDrawResources50KG::pagePacketCount()const noexcept{return storage_?storage_->pageCount:0;}
namespace {
// Embedded ScreenTexts are source-constructed objects, not invented DAT widgets.
// ScreenText capacity/range, source truncating metrics and scale1 placement are
// retained independently of the authenticated Overlay_Text widget manager.
bool embedded(const char* text,std::size_t length,std::size_t capacity,float x,float y,
 unsigned justification,const assets::OriginalUiFont50KG& font,const assets::Texture48B& atlas,
 game::OriginalUiTextLayout50KG& output,Diagnostic& d)noexcept{
 if(!text||length>capacity||capacity>21||justification>4)return fail(d,Error::geometry,0x2dfd90);
 game::OriginalUiTextInput50KG in;in.font=&font;in.atlas=&atlas;in.text=reinterpret_cast<const std::uint8_t*>(text);in.length=length;
 game::OriginalUiTextLayout50KG measured;
 if(!game::layoutOriginalUiText50KG(in,measured,d.glyph))return fail(d,Error::geometry,0x2e0a30);
 std::int32_t width=0,height=0,position=0;
 if(!game::roundOriginalUiTextMetric50KG(measured.widthBeforeScaleAndRounding,1,width)||
    !game::roundOriginalUiTextMetric50KG(measured.firstGlyphHeightBeforeScaleAndRounding,1,height))return fail(d,Error::geometry,0x2e13a0);
 if(length){
  if(justification==1||justification==3){volatile float half=float(width)*.5f;volatile float v=x-half;
   if(!game::roundOriginalUiTextMetric50KG(v,1,position))return fail(d,Error::geometry,0x2e13a0);x=float(position);}
  else if(justification==4){volatile float v=x-float(width);if(!game::roundOriginalUiTextMetric50KG(v,1,position))return fail(d,Error::geometry,0x2e13a0);x=float(position);}
  if(justification==2||justification==3){volatile float half=float(height)*.5f;volatile float v=y-half;
   if(!game::roundOriginalUiTextMetric50KG(v,1,position))return fail(d,Error::geometry,0x2e13a0);y=float(position);}}
 in.localX=x;in.localY=y;
 if(!game::layoutOriginalUiText50KG(in,output,d.glyph))return fail(d,Error::geometry,0x2e0a30);return true;
}
}
bool OriginalUiHallDrawResources50KG::projectPage(const OriginalUiHallPageInput50KG& in,Diagnostic& d)noexcept{
 d={};if(!storage_)return fail(d,Error::notLoaded);auto& s=*storage_;
 if(!in.records||!in.colorsKnown||in.courseIndex>=13||in.planetIndex>=6||backgroundImage(in.planetIndex)==unresolved)return fail(d,Error::input,0x1e17d0);
 const auto textMaterial=materialIndex("font/denmrk14"),headerMaterial=materialIndex("font/denmrk14top"),overlayMaterial=materialIndex("ui/textures/overlay1");
 if(textMaterial>=s.bindings.size()||headerMaterial>=s.bindings.size()||overlayMaterial>=s.bindings.size())return fail(d,Error::material);
 const auto image=s.bindings[textMaterial].images[0];const auto* font=s.images->font(image);const auto* atlas=textures()->texture(image);
 if(!font||!atlas)return fail(d,Error::resource);
 std::array<std::array<char,22>,28> text{};std::array<std::size_t,28> lengths{};
 const auto& records=*in.records;
 for(std::size_t category=0;category<3;++category)for(std::size_t rank=0;rank<3;++rank){const auto base=(category*3+rank)*3;
  const auto& rankText=s.ranks[rank];lengths[base]=rankText.size();std::copy(rankText.begin(),rankText.end(),text[base].begin());
  const auto& name=category==0?records.lapNames102FC[rank]:category==1?records.raceNames10348[rank]:records.knockoutNames10394[rank];
  const auto end=std::find(name.begin(),name.end(),'\0');if(end==name.end())return fail(d,Error::input,0x1e1e68);
  lengths[base+1]=static_cast<std::size_t>(end-name.begin());std::copy(name.begin(),end,text[base+1].begin());
  if(category<2){std::array<char,9> value{};if(!game::originalUiHallTime18900050KG(category==0?records.lap102F0[rank]:records.race1033C[rank],value))return fail(d,Error::input,0x189000);
   lengths[base+2]=8;std::copy_n(value.begin(),8,text[base+2].begin());}
  else{std::int32_t value=0;static_assert(sizeof(value)==sizeof(records.knockouts10388[rank]));std::memcpy(&value,&records.knockouts10388[rank],sizeof(value));
   const auto result=std::to_chars(text[base+2].data(),text[base+2].data()+20,value);if(result.ec!=std::errc{})return fail(d,Error::geometry,0x1e2b8c);lengths[base+2]=static_cast<std::size_t>(result.ptr-text[base+2].data());}}
 // PORT EXTENSION (DLC add-on course, 148r): the add-on planet name in the
 // header while the Hall shows an add-on course (original_ui_hall_addon50dl.h);
 // a name over the header's 14-character capacity keeps the stock planet name.
 auto* addon=originalUiHallAddonFrame50DL();
 const bool addonPlanet=addon&&addon->planetName&&!addon->planetName->empty()&&addon->planetName->size()<=14;
 const auto& name=addonPlanet?*addon->planetName:s.planetNames[in.planetIndex];lengths[27]=name.size();std::copy(name.begin(),name.end(),text[27].begin());
 if(addonPlanet)++addon->planetDrawn;
 for(std::size_t category=0;category<3;++category)for(std::size_t rank=0;rank<3;++rank)for(std::size_t column=0;column<3;++column){
  const auto n=(category*3+rank)*3+column;constexpr float x[]={225,350,280};constexpr std::size_t capacity[]={3,21,20};constexpr unsigned justify[]={4,0,1};
  if(!embedded(text[n].data(),lengths[n],capacity[column],x[column],float(99+86*category+21*rank),justify[column],*font,atlas->texture,s.scratchGeometry[n],d))return false;}
 if(!embedded(text[27].data(),lengths[27],14,320,43,3,*font,atlas->texture,s.scratchGeometry[27],d))return false;
 //1E33F0 is called after ordinary widgets. Its setters intentionally affect
 // those widgets on the next draw; OnTop widgets see the current theme now.
 const auto p=in.primaryColor79C,q=in.secondaryColor7A0;
 for(auto id:{27u,28u,29u,30u,6u,7u,8u})if(!s.widgets.setWidgetColor(static_cast<std::uint16_t>(id),p,d.widgets))return fail(d,Error::widgets,0x1e33f0);
 for(auto id:{1u,2u})if(!s.widgets.setWidgetColor(static_cast<std::uint16_t>(id),q,d.widgets))return fail(d,Error::widgets,0x1e38d8);
 // 1E390C compares retained +1464; the mapped +8C raw-texture replacement
 // affects the next +44 draw, not the raw packet already emitted this frame.
 s.currentBackgroundImage=s.backgroundImages[in.planetIndex];
 s.rowGeometry.swap(s.scratchGeometry);s.rowText=text;s.pageCount=0;
 const auto color=game::originalUiTextPacketColor50KG(p,255);
 for(std::size_t i=0;i<9;++i){auto& overlay=s.overlays[i];overlay.sourcePacketColor=color;
  auto& packet=s.pagePackets[s.pageCount++];packet={};packet.quad=&overlay;packet.packetColor=color;packet.materialIndex=overlayMaterial;packet.sourcePC=overlay.sourcePC;
  packet.objectOffset=static_cast<std::uint16_t>(i<6?0x4c0+i*0x60:0x700+(i-6)*0x60);}
 if(in.showPlanetHeader){auto& packet=s.pagePackets[s.pageCount++];packet={};packet.text=&s.rowGeometry[27];packet.packetColor=game::originalUiTextPacketColor50KG(q,255);
  packet.materialIndex=headerMaterial;packet.sourcePC=0x1e184c;packet.objectOffset=0x820;}
 constexpr std::uint32_t pcs[3][3]={{0x1e22e0,0x1e22f8,0x1e2310},{0x1e2b34,0x1e2b50,0x1e2b6c},{0x1e337c,0x1e3398,0x1e33b4}};
 constexpr std::uint16_t offsets[]={0x890,0xc80,0x1070};
 //1E2130/1E21F8/1E22C0 (and the race/knockout copies): 133F20(...,0xCCCCCC).
 const auto highlight=game::originalUiTextPacketColor50KG(0xCCCCCCu,255);
 for(std::size_t category=0;category<3;++category)for(std::size_t rank=0;rank<3;++rank)for(std::size_t column=0;column<3;++column){auto& packet=s.pagePackets[s.pageCount++];packet={};
  packet.text=&s.rowGeometry[(category*3+rank)*3+column];packet.packetColor=in.newRecordRow[category*3+rank]?highlight:color;packet.materialIndex=textMaterial;packet.sourcePC=pcs[category][column];
  packet.objectOffset=static_cast<std::uint16_t>(offsets[column]+(category*3+rank)*0x70);}
 return true;
}

}
