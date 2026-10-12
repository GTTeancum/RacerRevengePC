// Reconstructed 2026-10-12. Untested source-only native presentation candidate.
#include "original_ui_course_draw_contribution50kg.h"
#include "original_ui_course_background_adapter50kg.h"
#include "original_ui_lit_material_adapter50km.h"
#include "../../../src/game/original_ui_course_clip_camera50kg.h"
#include "../../../src/game/original_ui_rider_model_fog50kg.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <numeric>
namespace rr::platform {
namespace {
using D=OriginalUiCourseContributionDiagnostic50KG;using E=OriginalUiCourseContributionError50KG;
constexpr auto missing=assets::unresolvedMaterialTexture50EA;
constexpr std::array<std::uint16_t,13> holoIds{{9,10,12,13,14,15,16,20,21,22,23,25,26}};
bool fail(D& d,E e,std::uint32_t pc=0)noexcept{d.error=e;d.sourcePC=pc;return false;}
std::string normalized(std::string s){for(auto& c:s){if(c=='\\')c='/';else if(c>='A'&&c<='Z')c+=32;}return s;}
bool sameSampler(const MaterialTextureSampler50EB& a,const MaterialTextureSampler50EB& b)noexcept{return a.minification==b.minification&&a.magnification==b.magnification&&a.s==b.s&&a.t==b.t;}
const assets::OriginalUiMemberIdentity50KG* member(const char* name)noexcept{
 const auto* a=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course);if(!a)return nullptr;
 for(std::size_t i=0;i<a->memberCount;++i){auto* m=assets::originalUiMemberIdentity50KG(a->id,i);if(m&&!std::strcmp(m->name,name))return m;}return nullptr;
}
}
struct OriginalUiCourseDrawContribution50KG::Storage {
 std::shared_ptr<OriginalUiCourseDrawResources50KG> resources;
 std::shared_ptr<OriginalUiCourseHologramDraw50KG> holo;
 std::shared_ptr<assets::OriginalUiImageBank50KG> bank;
 std::vector<MaterialTextureSampler50EB> samplers;
 std::vector<std::uint8_t> maxAlpha;
 std::shared_ptr<const MaterialTextureGl50EB> gpu;std::uint64_t publication=0;
 std::vector<game::OriginalUiTextureAnimation50KG> animation;std::vector<std::uint32_t> frames;
 std::map<const assets::OriginalUiMaterialPass50KG*,std::pair<std::uint32_t,std::uint32_t>> registrations;
 std::vector<std::pair<std::uint32_t,std::uint32_t>> orders;
 std::vector<assets::Vertex48B> vertices;std::vector<float> w,fog;
 std::vector<OriginalUiSpan50KG> spans;std::vector<OriginalUiNativeSpan50KG> states;
 game::OriginalUiLitContext50KM lit;game::OriginalUiCourseRenderContext50KG render;
 std::unique_ptr<game::OriginalUiLitGeometryWorkspace50KM> workspace;
 OriginalUiCourseDrawClock50KG clock;OriginalUiNativeDraw50KG draw;
 bool bankPrepared=false;
 bool active=false,ordinary=false,page=false,exit=false,prompts=false,top=false,finished=false,failed=false;
 std::uint8_t aperture=0;
 std::uint32_t mapped(std::uint32_t owner,std::uint32_t image)const noexcept{auto* m=bank?bank->mapping(owner,image):nullptr;return m?m->combinedIndex:missing;}
 bool add(const assets::Vertex48B* v,std::size_t count,const float* ws,const float* fs,OriginalUiMaterialSpan50KG material,const assets::OriginalUiMaterialPass50KG* source,D& d){
  if(!count)return true;
  if(count%4||vertices.size()+count>originalUiMaximumQuads50KG*4u||spans.size()>=originalUiMaximumSpans50KG)return fail(d,E::capacity);
  if(material.span.blend!=OriginalUiBlend50KG::opaque&&material.span.blend!=OriginalUiBlend50KG::sourceAlpha&&material.span.blend!=OriginalUiBlend50KG::additive)return fail(d,E::material);
  const bool textured=material.span.materialMode!=OriginalUiMaterialMode50KG::untexturedVertexColor;
  if(textured&&material.span.textureIndex>=maxAlpha.size())return fail(d,E::material);
  for(std::size_t i=0;i<count;++i){
   for(float x:v[i].position)if(!std::isfinite(x))return fail(d,E::geometry);
   for(float x:v[i].uv)if(!std::isfinite(x))return fail(d,E::geometry);
   if(v[i].position[2]<0||v[i].position[2]>16777215.f||std::floor(v[i].position[2])!=v[i].position[2]||v[i].color[3]>128)return fail(d,E::geometry);
   if(textured&&material.span.blend!=OriginalUiBlend50KG::opaque&&unsigned(v[i].color[3])*maxAlpha[material.span.textureIndex]>16384u)return fail(d,E::material);
  }
  std::pair<std::uint32_t,std::uint32_t> order{0,0};
  if(source){auto r=registrations.find(source);if(r==registrations.end())return fail(d,E::schedule);order=r->second;}
  material.span.firstQuad=static_cast<std::uint32_t>(vertices.size()/4);material.span.quadCount=static_cast<std::uint32_t>(count/4);
  vertices.insert(vertices.end(),v,v+count);for(std::size_t i=0;i<count;++i){w.push_back(ws?ws[i]:1.f);fog.push_back(fs?fs[i]:0.f);}
  spans.push_back(material.span);states.push_back(material.native);orders.push_back(order);return true;
 }
};
OriginalUiCourseDrawContribution50KG::OriginalUiCourseDrawContribution50KG()noexcept=default;
OriginalUiCourseDrawContribution50KG::~OriginalUiCourseDrawContribution50KG()=default;
bool OriginalUiCourseDrawContribution50KG::bind(std::shared_ptr<OriginalUiCourseDrawResources50KG> r,std::shared_ptr<OriginalUiCourseHologramDraw50KG> h,D& d)noexcept{
 d={};if(storage_)return fail(d,E::alreadyBound);if(!r||!r->loaded()||!h||!h->loaded())return fail(d,E::resource);
 try{auto s=std::make_unique<Storage>();s->resources=std::move(r);s->holo=std::move(h);s->workspace=std::make_unique<game::OriginalUiLitGeometryWorkspace50KM>();
  const auto& materials=*s->resources->materials();s->animation.resize(materials.size());s->frames.resize(materials.size());
  for(std::size_t i=0;i<materials.size();++i)if(materials[i].animated&&!game::initializeOriginalUiTextureAnimation50KG(s->animation[i],materials[i].animation,static_cast<std::uint32_t>(materials[i].images.size())))return fail(d,E::clock);
  storage_=std::move(s);return true;
 }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiCourseDrawContribution50KG::prepareBank(D& d)noexcept{
 d={};if(!storage_||storage_->bank)return fail(d,E::phase);auto& s=*storage_;
 try{
  assets::OriginalUiImageBankInput50KG input=*s.resources->bankInput();if(input.owners.size()!=1||s.holo->bankInput()->owners.size()!=1)return fail(d,E::bank);
  for(const auto& a:s.holo->bankInput()->archives){bool found=false;for(const auto& b:input.archives)found|=a->identity()==b->identity();if(!found)input.archives.push_back(a);}
  input.owners.push_back(s.holo->images());auto bank=std::make_shared<assets::OriginalUiImageBank50KG>();assets::OriginalUiImageBankDiagnostic50KG bd;
  if(!bank->load(input,bd))return fail(d,E::bank);s.bank=bank;
  const auto images=bank->images();const auto* textures=images->textures();s.samplers.resize(textures->textureCount());s.maxAlpha.resize(textures->textureCount());
  for(std::uint32_t i=0;i<textures->textureCount();++i){const auto& rgba=textures->texture(i)->texture.rgba;for(std::size_t j=3;j<rgba.size();j+=4)s.maxAlpha[i]=std::max(s.maxAlpha[i],rgba[j]);}
  std::map<std::string,std::uint32_t> ranks;assets::OriginalUiMemberView50KG table;
  for(const auto& a:input.archives)if(a->identity()->id==assets::OriginalUiArchiveId50KG::appStart)table=a->find("AppStart/Mat_Stbl.txt");
  if(!table.identity)return fail(d,E::schedule);std::size_t at=0;std::uint32_t token=0;
  while(at<table.size){while(at<table.size&&(table.data[at]==' '||table.data[at]=='\r'||table.data[at]=='\n'))++at;if(at==table.size)break;auto first=at;while(at<table.size&&table.data[at]!=' '&&table.data[at]!='\r'&&table.data[at]!='\n')++at;auto name=normalized(std::string(reinterpret_cast<const char*>(table.data+first),at-first));auto inserted=ranks.emplace(name,++token);if(!inserted.second)inserted.first->second=0;}
  if(token!=3235)return fail(d,E::schedule);
  auto registerPass=[&](const std::string& name,const assets::OriginalUiMaterialPass50KG& p){auto it=ranks.find(normalized(name));if(it==ranks.end()||!it->second)return false;s.registrations.emplace(&p,std::make_pair(it->second,p.pass));return true;};
  auto assign=[&](std::uint32_t image,const MaterialTextureSampler50EB& sampler){if(image>=s.samplers.size())return false;auto& current=s.samplers[image];if(current.minification!=MaterialTextureFilter50EB::unspecified&&!sameSampler(current,sampler))return false;current=sampler;return true;};
  for(const auto& b:*s.resources->materials()){
   if(b.material.passes.size()!=1||!registerPass(b.registeredName,b.material.passes[0]))return fail(d,E::schedule);const auto& p=b.material.passes[0];
   if(!p.textureCount){assets::OriginalUiNativeUntexturedMaterial50KG policy;if(!assets::prepareOriginalUiNativeUntexturedMaterial50KG(p,policy,d.materialDetail))return fail(d,E::material);continue;}
   for(auto image:b.images){auto index=s.mapped(0,image);assets::OriginalUiNativeMaterial50KG policy;OriginalUiMaterialSpan50KG converted;
    if(index==missing||!assets::prepareOriginalUiNativeMaterial50KG(p,*images->decodedTexture(index),policy,d.materialDetail)||!convertOriginalUiMaterialSpan50KG(policy,{0,1,1,index,textures->textureCount()},converted)||!assign(index,converted.native.sampler))return fail(d,E::material);}
  }
  for(auto id:holoIds){const auto* m=s.holo->material(id);const auto* a=s.holo->modelAssets(id);if(!m||!a||a->scene()->materials.size()!=1)return fail(d,E::model);
   for(std::uint32_t p=0;p<m->passes.size();++p){if(!registerPass("ui/textures/"+a->scene()->materials[0],m->passes[p]))return fail(d,E::schedule);
    for(auto image:*s.holo->images(id,p)){auto index=s.mapped(1,image);assets::OriginalUiLitMaterial50KM policy;OriginalUiMaterialSpan50KG converted;
     if(index==missing||!assets::prepareOriginalUiLitMaterial50KM(m->passes[p],*images->decodedTexture(index),policy,d.materialDetail)||!convertOriginalUiLitMaterialSpan50KM(policy,{0,1,1,index,textures->textureCount()},converted)||!assign(index,converted.native.sampler))return fail(d,E::material);}
   }
  }
  OriginalUiSpan50KG bg;OriginalUiNativeSpan50KG bgState;auto bgImage=s.mapped(0,s.resources->backgroundImage());
  if(!prepareOriginalUiCourseBackgroundSpan50KG(*images,bgImage,bg,bgState)||!assign(bgImage,bgState.sampler))return fail(d,E::material);
  for(const auto& p:s.samplers)if(p.minification==MaterialTextureFilter50EB::unspecified)return fail(d,E::material);
  s.bankPrepared=true;return true;
 }catch(...){return fail(d,E::allocation);}
}
std::shared_ptr<const assets::OriginalUiImageBank50KG> OriginalUiCourseDrawContribution50KG::bank()const noexcept{return storage_&&storage_->bankPrepared?storage_->bank:nullptr;}
const std::vector<MaterialTextureSampler50EB>* OriginalUiCourseDrawContribution50KG::samplers()const noexcept{return storage_&&storage_->bankPrepared?&storage_->samplers:nullptr;}
bool OriginalUiCourseDrawContribution50KG::bindPublishedBank(std::shared_ptr<const MaterialTextureGl50EB> gpu,D& d)noexcept{
 d={};if(!storage_||!storage_->bankPrepared||storage_->gpu||!gpu||!gpu->initialized()||!gpu->publicationToken())return fail(d,E::publication);auto& s=*storage_;const auto* images=s.bank->images()->textures();
 if(gpu->textureCount()!=images->textureCount())return fail(d,E::publication);for(std::uint32_t i=0;i<gpu->textureCount();++i)if(!gpu->asset(i)||gpu->asset(i)->identity!=images->texture(i)->identity)return fail(d,E::publication);
 s.publication=gpu->publicationToken();s.gpu=std::move(gpu);return true;
}
bool OriginalUiCourseDrawContribution50KG::begin(const OriginalUiCourseDrawClock50KG& clock,const game::OriginalUiCourseRenderContext50KG& render,const game::OriginalUiCourseLighting50KG& lights,D& d)noexcept{
 d={};if(!storage_||!storage_->bankPrepared||!storage_->gpu||!clock.known)return fail(d,E::phase);auto& s=*storage_;drain();
 if(!s.gpu->initialized()||s.gpu->publicationToken()!=s.publication)return fail(d,E::publication);
 if(!s.resources->widgets()->popupTemplatesHidden438()||s.resources->widgets()->popupEnabled438())return fail(d,E::widget,0x1DA714);
 if(render.cameraAfterFog.pageIdentity!=member("UI\\UICourse\\track.dat")||render.fogIdentity!=member("UI\\UICourse\\ui.dat")||lights.uiIdentity!=render.fogIdentity)return fail(d,E::input);
 game::OriginalUiCourseClipCamera50KG clip;if(!game::prepareOriginalUiCourseClipCamera50KG(render.cameraAfterFog,clip))return fail(d,E::geometry);
 // Explicit native two-light adaptation, not original per-model14A0E8 cache.
 if(game::projectOriginalUiGlobalLightRows50KM(true,lights.directionalLook78,lights.directional3C,lights.ambient50,s.lit.lights)!=game::original_racer_lighting50KM::Status::complete)return fail(d,E::model);
 s.lit.viewC0=render.cameraAfterFog.core.view;s.lit.projection80=render.cameraAfterFog.core.projection;s.lit.clipProjection290=clip.clipProjection290;s.lit.screen140=render.cameraAfterFog.sourceScreen.sourceScreen140;s.lit.cameraPosition180=render.cameraAfterFog.position;s.lit.fogEnd33Z=render.packetFogStart33Z;s.lit.fogSlope33W=render.packetFogScale33W;s.lit.canvasOffset=render.sourceCanvasOffset;s.render=render;s.clock=clock;
 try{if(clock.advanceAnimations&&!s.holo->advanceTextureAnimations(clock.milliseconds,d.hologram))return fail(d,E::clock);
  s.active=true;s.ordinary=s.page=s.exit=s.prompts=s.top=s.finished=s.failed=false;s.aperture=0;s.vertices.clear();s.w.clear();s.fog.clear();s.spans.clear();s.states.clear();s.orders.clear();
  OriginalUiMaterialSpan50KG material;if(!prepareOriginalUiCourseBackgroundSpan50KG(*s.bank->images(),s.mapped(0,s.resources->backgroundImage()),material.span,material.native))return fail(d,E::material);
  std::array<assets::Vertex48B,4> v{};constexpr float xy[4][2]={{0,0},{640,0},{640,448},{0,448}};
  for(unsigned i=0;i<4;++i){v[i].position[0]=xy[i][0];v[i].position[1]=xy[i][1];v[i].position[2]=0;v[i].uv[0]=(i==1||i==2)?1.f:1.f/640;v[i].uv[1]=i>=2?1.f:1.f/448;v[i].color[0]=v[i].color[1]=v[i].color[2]=128;v[i].color[3]=64;}
  if(!s.add(v.data(),4,nullptr,nullptr,material,nullptr,d)){s.failed=true;return false;}return true;
 }catch(...){s.failed=true;return fail(d,E::allocation);}
}
bool OriginalUiCourseDrawContribution50KG::appendVertices(const std::vector<assets::Vertex48B>& v,std::uint32_t index,D& d)noexcept{
 auto& s=*storage_;if(v.empty())return true;if(index>=s.resources->materials()->size())return fail(d,E::material);
 try{const auto& b=(*s.resources->materials())[index];if(b.material.passes.size()!=1)return fail(d,E::material);const auto& p=b.material.passes[0];
  if(b.animated&&s.clock.advanceAnimations&&!game::advanceOriginalUiTextureAnimation50KG(s.animation[index],s.clock.milliseconds,s.frames[index]))return fail(d,E::clock);
  const auto first=static_cast<std::uint32_t>(s.vertices.size()/4),count=static_cast<std::uint32_t>(v.size()/4);OriginalUiMaterialSpan50KG m;
  if(!p.textureCount){assets::OriginalUiNativeUntexturedMaterial50KG policy;if(!assets::prepareOriginalUiNativeUntexturedMaterial50KG(p,policy,d.materialDetail)||!convertOriginalUiUntexturedMaterialSpan50KG(policy,{first,count,first+count},m))return fail(d,E::material);}
  else{auto frame=s.frames[index];if(frame>=b.images.size())return fail(d,E::clock);auto image=s.mapped(0,b.images[frame]);assets::OriginalUiNativeMaterial50KG policy;
   if(image==missing||!assets::prepareOriginalUiNativeMaterial50KG(p,*s.bank->images()->decodedTexture(image),policy,d.materialDetail)||!convertOriginalUiMaterialSpan50KG(policy,{first,count,first+count,image,s.bank->images()->textures()->textureCount()},m))return fail(d,E::material);}
  return s.add(v.data(),v.size(),nullptr,nullptr,m,&p,d);
 }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiCourseDrawContribution50KG::append(const game::OriginalUiOverlayQuad50KG& q,std::uint32_t material,float z,D& d)noexcept{
 try{std::vector<assets::Vertex48B> v(4);constexpr unsigned order[]={0,1,3,2};for(unsigned i=0;i<4;++i){const auto& a=q.vertices[order[i]];v[i].position[0]=a.x;v[i].position[1]=a.y;v[i].position[2]=z;v[i].uv[0]=a.u;v[i].uv[1]=a.v;for(unsigned c=0;c<4;++c)v[i].color[c]=q.sourcePacketColor[c];}return appendVertices(v,material,d);}catch(...){return fail(d,E::allocation);}
}
bool OriginalUiCourseDrawContribution50KG::model(std::uint16_t id,D& d)noexcept{
 auto& s=*storage_;auto* model=s.holo->model(id);auto* source=s.holo->modelAssets(id);if(!model||!source)return fail(d,E::model);if(!model->shown())return true;
 try{game::ModelPalette49S palette;game::OriginalUiCourseHologramSceneDiagnostic50KG md;if(!model->projectPalette(1,palette,md))return fail(d,E::model);
  game::OriginalUiRiderModelFog50KG fog;if(!game::prepareOriginalUiRiderModelFog50KG(*source->hierarchy(),model->graph(),s.render.cameraAfterFog.position,s.render.fogStart,fog))return fail(d,E::model);
  const auto* scene=source->scene();const auto* material=s.holo->material(id);if(!scene||scene->lods.size()!=1||!material)return fail(d,E::model);
  for(std::uint32_t surface=0;surface<scene->lods[0].surfaces.size();++surface){
   const auto& shape=scene->lods[0].surfaces[surface];if(shape.material_index!=0||shape.indices.size()%3)return fail(d,E::geometry);
   const auto count=static_cast<std::uint32_t>(shape.indices.size()/3);if(!count)continue;
   std::vector<assets::Vertex48B> vertices(count*4u);std::vector<float> w(count*4u),f(count*4u);
   game::OriginalUiLitSurfaceOutput50KM output{vertices.data(),w.data(),f.data(),count,0,0};
   if(!s.holo->project(id,surface,palette,s.lit,fog.computeRadialFog1B2,output,*s.workspace,d.hologram))return fail(d,E::model);
   for(std::uint32_t pass=0;pass<material->passes.size();++pass){const auto& p=material->passes[pass];auto image=s.mapped(1,s.holo->currentImage(id,pass));assets::OriginalUiLitMaterial50KM policy;OriginalUiMaterialSpan50KG m;
    const auto first=static_cast<std::uint32_t>(s.vertices.size()/4);
    if(image==missing||!assets::prepareOriginalUiLitMaterial50KM(p,*s.bank->images()->decodedTexture(image),policy,d.materialDetail)||!convertOriginalUiLitMaterialSpan50KM(policy,{first,output.slotCount,first+output.slotCount,image,s.bank->images()->textures()->textureCount()},m)||!s.add(vertices.data(),output.slotCount*4u,w.data(),f.data(),m,&p,d))return fail(d,E::material);
   }
  }return true;
 }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiCourseDrawContribution50KG::widgets(bool top,bool ancestors,D& d)noexcept{
 auto& s=*storage_;auto* widgets=s.resources->widgets();const auto* order=widgets->order(top);if(!order)return fail(d,E::widget);
 try{for(auto id:*order){d.widget=id;const auto state=widgets->state(id);if(!ancestors||!state.shown)continue;
   if(state.kind==game::OriginalUiCourseWidgetKind50KG::character){if(!model(id,d))return false;continue;}
   const bool selectedVariant=state.kind==game::OriginalUiCourseWidgetKind50KG::selectableIcon&&state.selected;
   const auto material=s.resources->widgetMaterial(id,selectedVariant);
   if(const auto* desc=widgets->textDescriptor(id)){
    auto* lines=widgets->textLines(id);auto* font=s.resources->textFont(id);auto* atlas=s.resources->textAtlas(id);if(!lines||!font||!atlas)return fail(d,E::geometry);
    float z=0;if(!s.resources->textPacketDepth(id,z))return fail(d,E::geometry);
    for(std::size_t line=0;line<lines->size();++line){const auto& text=(*lines)[line];game::OriginalUiTextInput50KG input{font,atlas,reinterpret_cast<const std::uint8_t*>(text.data()),text.size(),0,0};game::OriginalUiTextLayout50KG glyphs;
     if(!game::layoutOriginalUiText50KG(input,glyphs,d.text))return fail(d,E::geometry);
     std::int32_t width=0,height=0;if(!game::roundOriginalUiTextMetric50KG(glyphs.widthBeforeScaleAndRounding,1,width)||!game::roundOriginalUiTextMetric50KG(glyphs.firstGlyphHeightBeforeScaleAndRounding,1,height))return fail(d,E::geometry);
     game::OriginalUiCourseTextProjection50KG live;if(!widgets->projectText(id,line,glyphs.fontLineHeight,width,height,ancestors,live,d.widgetDetail)){if(d.widgetDetail.error==game::OriginalUiCourseWidgetError50KG::hidden)continue;return fail(d,E::widget);}
     if(live.line.scale!=1||live.line.rotation!=0||live.line.packetColor[3]>128)return fail(d,E::geometry);
     input.localX=live.position.positionedX;input.localY=live.position.positionedY;
     if(!game::layoutOriginalUiText50KG(input,glyphs,d.text))return fail(d,E::geometry);
     std::vector<assets::Vertex48B> vertices;vertices.reserve(glyphs.count*4u);
     for(std::size_t q=0;q<glyphs.count;++q)for(const auto& g:glyphs.quads[q].vertices){assets::Vertex48B v{};v.position[0]=g.x;v.position[1]=g.y;v.position[2]=z;v.uv[0]=g.u;v.uv[1]=g.v;for(unsigned c=0;c<4;++c)v.color[c]=live.line.packetColor[c];vertices.push_back(v);}
     if(!appendVertices(vertices,material,d))return false;
    }
   }else if(widgets->iconDescriptor(id,selectedVariant)){
    game::OriginalUiCourseIconProjection50KG live;if(!widgets->projectIcon(id,ancestors,live,d.widgetDetail)||live.images!=s.resources->images().get())return fail(d,E::widget);
    if(material>=s.resources->materials()->size())return fail(d,E::material);const auto& binding=(*s.resources->materials())[material];if(binding.images.empty()||live.imageIndex!=binding.images[0])return fail(d,E::material);
    if(!append(live.layout.quad,material,live.layout.packetDepth,d))return false;
   }
  }return true;
 }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiCourseDrawContribution50KG::projectOrdinaryWidgets(bool shown,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::phase);auto& s=*storage_;if(!s.active||s.ordinary||s.failed||s.finished)return fail(d,E::phase);if(!widgets(false,shown,d)){s.failed=true;return false;}s.ordinary=true;return true;
}
bool OriginalUiCourseDrawContribution50KG::projectPage(const game::OriginalUiCoursePageDraw50KG& page,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::phase);auto& s=*storage_;if(!s.active||!s.ordinary||s.page||s.failed||s.finished||!page.prepared()||page.images()!=s.resources->images()||page.pageIdentity()!=member("UI\\UICourse\\track.dat"))return fail(d,E::phase);
 const auto* packets=page.packets();if(!packets)return fail(d,E::geometry);
 for(const auto& p:*packets){if(!p.materialName){s.failed=true;return fail(d,E::material);}auto m=s.resources->materialIndex(p.materialName);if(m>=s.resources->materials()->size()||(*s.resources->materials())[m].material.identity!=p.materialIdentity||!appendVertices(p.vertices,m,d)){s.failed=true;return false;}}
 s.page=true;return true;
}
bool OriginalUiCourseDrawContribution50KG::projectExitText(const game::OriginalUiApertureHook50KG& hook,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::phase);auto& s=*storage_;if(!s.active||!s.page||s.exit||s.aperture||s.failed||s.finished)return fail(d,E::phase);
 try{const auto m=s.resources->materialIndex("font/denmrk16top");if(m>=s.resources->materials()->size())return fail(d,E::material);const auto& b=(*s.resources->materials())[m];game::OriginalUiExitTextProjection50KG live;
  auto* owner=s.resources->exitText();if(!owner||owner->projectRender2E0170(hook,live)!=game::OriginalUiExitTextStatus50KG::ok||!live.glyphs||live.images!=s.resources->images().get()||b.images.size()!=1||live.imageIndex!=b.images[0]||live.materialIdentity!=b.material.identity||live.assignedDepth!=1||live.glyphs->count>10)return fail(d,E::geometry);
  std::vector<assets::Vertex48B> vertices;for(std::size_t q=0;q<live.glyphs->count;++q)for(const auto& g:live.glyphs->quads[q].vertices){assets::Vertex48B v{};v.position[0]=g.x;v.position[1]=g.y;v.position[2]=16777214.f;v.uv[0]=g.u;v.uv[1]=g.v;for(unsigned c=0;c<4;++c)v.color[c]=live.packetColor[c];vertices.push_back(v);}
  if(!appendVertices(vertices,m,d)){s.failed=true;return false;}s.exit=true;return true;
 }catch(...){s.failed=true;return fail(d,E::allocation);}
}
bool OriginalUiCourseDrawContribution50KG::projectAperture(std::uint8_t ordinal,const game::OriginalUiApertureRectangle50KG& rect,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::phase);auto& s=*storage_;if(!s.active||!s.page||s.failed||s.finished||s.prompts||ordinal!=s.aperture||ordinal>1||rect.sourceObjectOffset!=(ordinal?0x428u:0x424u)||!rect.dirty3C)return fail(d,E::phase);
 game::OriginalUiOverlayQuad50KG q;q.sourcePacketColor={{0,0,0,63}};constexpr unsigned order[]={0,2,1,3};for(unsigned i=0;i<4;++i){const auto& v=rect.vertices[order[i]];q.vertices[i]={v.x,v.y,0,0};}
 if(!append(q,s.resources->materialIndex("ui/textures/notexture"),16777214.f,d)){s.failed=true;return false;}++s.aperture;return true;
}
bool OriginalUiCourseDrawContribution50KG::projectPrompts(std::uint32_t mask,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::phase);auto& s=*storage_;if(!s.active||!s.page||s.aperture!=2||s.prompts||s.top||s.failed||s.finished||(mask&~15u))return fail(d,E::phase);
 auto* prompts=s.resources->prompts();if(!prompts)return fail(d,E::resource);for(const auto& p:*prompts)if(mask&p.mask)if(!append(p.quad,p.material,16777214.f,d)){s.failed=true;return false;}s.prompts=true;return true;
}
bool OriginalUiCourseDrawContribution50KG::projectOnTopWidgets(bool shown,D& d)noexcept{
 d={};if(!storage_)return fail(d,E::phase);auto& s=*storage_;if(!s.active||!s.prompts||s.top||s.failed||s.finished)return fail(d,E::phase);if(!widgets(true,shown,d)){s.failed=true;return false;}s.top=true;return true;
}
bool OriginalUiCourseDrawContribution50KG::finish(D& d)noexcept{
 d={};if(!storage_)return fail(d,E::phase);auto& s=*storage_;if(!s.active||!s.ordinary||!s.page||s.aperture!=2||!s.prompts||!s.top||s.failed||s.finished||!s.gpu||s.gpu->publicationToken()!=s.publication)return fail(d,E::unfinished);
 try{std::vector<std::size_t> order(s.spans.size());std::iota(order.begin(),order.end(),0);std::stable_sort(order.begin(),order.end(),[&](std::size_t a,std::size_t b){return s.orders[a]<s.orders[b];});
  std::vector<assets::Vertex48B> vertices;std::vector<float> w,fog;std::vector<OriginalUiSpan50KG> spans;std::vector<OriginalUiNativeSpan50KG> states;
  vertices.reserve(s.vertices.size());w.reserve(s.w.size());fog.reserve(s.fog.size());
  for(auto i:order){auto span=s.spans[i];auto first=span.firstQuad*4u,count=span.quadCount*4u;span.firstQuad=static_cast<std::uint32_t>(vertices.size()/4);vertices.insert(vertices.end(),s.vertices.begin()+first,s.vertices.begin()+first+count);w.insert(w.end(),s.w.begin()+first,s.w.begin()+first+count);fog.insert(fog.end(),s.fog.begin()+first,s.fog.begin()+first+count);spans.push_back(span);states.push_back(s.states[i]);}
  s.vertices.swap(vertices);s.w.swap(w);s.fog.swap(fog);s.spans.swap(spans);s.states.swap(states);s.draw={};
  auto& draw=s.draw.draw;draw.textures=s.gpu.get();draw.texturePublication=s.publication;draw.vertices=s.vertices.data();draw.quadCount=static_cast<std::uint32_t>(s.vertices.size()/4);draw.spans=s.spans.data();draw.spanCount=static_cast<std::uint32_t>(s.spans.size());draw.canvasWidth=640;draw.canvasHeight=448;
  s.draw.states=s.states.data();s.draw.stateCount=static_cast<std::uint32_t>(s.states.size());s.draw.opaquePresentationAlphaOnly=true;s.draw.sourceRasterW=s.w.data();s.draw.sourceRasterWCount=static_cast<std::uint32_t>(s.w.size());s.draw.particleFog=s.fog.data();s.draw.particleFogCount=static_cast<std::uint32_t>(s.fog.size());for(unsigned c=0;c<3;++c)s.draw.particleFogColor[c]=s.render.sourceFogRgb[c]/255.f;
  // Complete fresh-cleared RGB presentation only: no destination-alpha factor,
  // retained race target or downstream source-alpha consumer is admitted.
  s.finished=true;return true;
 }catch(...){s.failed=true;return fail(d,E::allocation);}
}
const OriginalUiNativeDraw50KG* OriginalUiCourseDrawContribution50KG::draw()const noexcept{return storage_&&storage_->finished&&!storage_->failed?&storage_->draw:nullptr;}
void OriginalUiCourseDrawContribution50KG::drain()noexcept{if(storage_){storage_->active=false;storage_->finished=false;storage_->draw={};}}
}
