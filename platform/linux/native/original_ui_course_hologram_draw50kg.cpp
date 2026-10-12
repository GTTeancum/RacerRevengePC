// Reconstructed native CPU owner. Source-only, untested. No guest execution.
#include "original_ui_course_hologram_draw50kg.h"
#include "../../../src/assets/original_ui_atm50kg.h"
#include <array>
namespace rr::platform {
namespace {
constexpr std::array<std::uint16_t,13> ids{{9,10,12,13,14,15,16,20,21,22,23,25,26}};
std::size_t ordinal(std::uint16_t id)noexcept{for(std::size_t i=0;i<ids.size();++i)if(ids[i]==id)return i;return ids.size();}
}
struct OriginalUiCourseHologramDraw50KG::Storage {
 struct Model {
  std::shared_ptr<assets::OriginalUiCourseHologramAssets50KG> assets;
  game::OriginalUiCourseHologramScene50KG scene;
  assets::OriginalUiMaterial50KG material;
  std::array<std::vector<std::uint32_t>,2> images;
  std::array<std::vector<assets::OriginalUiLitMaterial50KM>,2> policies;
  game::OriginalUiTextureAnimation50KG animation{};
  std::uint32_t frame=0;
 };
 std::array<Model,13> models;
 std::shared_ptr<assets::OriginalUiScreenAssets50KG> images;
 assets::OriginalUiImageBankInput50KG bank;
};
OriginalUiCourseHologramDraw50KG::OriginalUiCourseHologramDraw50KG()noexcept=default;
OriginalUiCourseHologramDraw50KG::~OriginalUiCourseHologramDraw50KG()=default;
bool OriginalUiCourseHologramDraw50KG::load(std::shared_ptr<const assets::OriginalUiArchive50KG> app,std::shared_ptr<const assets::OriginalUiArchive50KG> course,const assets::OriginalUiMaterialRuntime50KG& runtime,OriginalUiCourseHologramDrawDiagnostic50KG& d)noexcept{
 d={};auto fail=[&](std::uint32_t pc){d.sourcePC=pc;return false;};
 if(storage_||!app||!course||!app->loaded()||!course->loaded()||app->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::appStart)||course->identity()!=assets::originalUiArchiveIdentity50KG(assets::OriginalUiArchiveId50KG::course))return fail(0x1CD304);
 try{
  auto n=std::make_unique<Storage>();assets::OriginalUiMaterialTemplates50KG templates;
  if(!templates.load(*app,d.material))return fail(0x2476A0);
  std::vector<assets::OriginalUiImageSource50KG> sources;
  auto image=[&](assets::OriginalUiMemberView50KG m,std::size_t offset,std::size_t bytes){
   for(std::uint32_t i=0;i<sources.size();++i)if(sources[i].member.identity==m.identity&&sources[i].pstOffset==offset&&sources[i].pstBytes==bytes)return i;
   const auto id=static_cast<std::uint32_t>(sources.size());sources.push_back({std::uint64_t(id)+1,m,offset,bytes,{}});return id;
  };
  for(std::size_t i=0;i<ids.size();++i){
   auto& m=n->models[i];d.widget=ids[i];m.assets=std::make_shared<assets::OriginalUiCourseHologramAssets50KG>();
   if(!m.assets->load(course,ids[i],d.asset)||!m.scene.load(m.assets,d.scene))return fail(0x2A8770);
   const auto* scene=m.assets->scene();if(scene->materials.size()!=1||scene->lods.size()!=1)return fail(0x240C00);
   std::string name=scene->materials[0];
   if(!assets::readOriginalUiMaterial50KG(*course,"ui/textures/"+name+".psm",templates,assets::OriginalUiTemplate50KG::defaultMt,runtime,m.material,d.material)||m.material.passes.size()!=2)return fail(0x2476A0);
   for(std::uint32_t pass=0;pass<2;++pass){const auto& p=m.material.passes[pass];
    if(p.microprogram!="DiffuseCompressed"||p.textureCount!=1||p.textureCoordinateIndex!=0)return fail(0x2476A0);
    if(pass==0){auto pst=course->find("UI/Textures/"+p.textureName+".pst");if(!pst.identity)return fail(0x24DA30);m.images[pass].push_back(image(pst,0,pst.size));}
    else{
     if(p.textureName!="holobeam")return fail(0x29B950);
     const auto* unique=p.authored.find("TextureAnimation");float fps=0;std::int32_t reverse=-1,loopTo=-1;
     assets::OriginalUiPageDiagnostic50KG parse;
     if(!unique||unique->text!="unique"||!assets::originalUiNumber50KG(p.authored,"animatedMapFramesPerSecond",1,fps,parse,0x29B950)||fps!=16||!assets::originalUiInteger50KG(p.authored,"animatedMapReverseLoopOnEnd",0,reverse,parse,0x29B950)||reverse!=0||!assets::originalUiInteger50KG(p.authored,"animatedMapLoopOnEndToFrame",-1,loopTo,parse,0x29B950)||loopTo!=0)return fail(0x29B950);
     auto atm=course->find("UI/Textures/holobeam.atm");assets::OriginalUiAtm50KG frames;
     if(!atm.identity||!assets::splitOriginalUiAtm50KG(atm.data,atm.size,frames,d.texture)||frames.frames.size()!=16)return fail(0x102230);
     for(const auto& f:frames.frames)m.images[pass].push_back(image(atm,f.pstOffset,f.pstBytes));
     game::OriginalUiTextureAnimationConfig50KG config;config.framesPerSecond=fps;config.loopOnEndToFrame=loopTo;
     if(!game::initializeOriginalUiTextureAnimation50KG(m.animation,config,16))return fail(0x29B950);
    }
   }
  }
  n->images=std::make_shared<assets::OriginalUiScreenAssets50KG>();
  if(!n->images->load(sources,d.images))return fail(0x24DA30);
  for(auto& m:n->models)for(std::uint32_t p=0;p<2;++p)for(auto image:m.images[p]){
   assets::OriginalUiLitMaterial50KM policy;
   if(!assets::prepareOriginalUiLitMaterial50KM(m.material.passes[p],*n->images->decodedTexture(image),policy,d.material))return fail(0x2476A0);
   m.policies[p].push_back(policy);
  }
  n->bank.archives={app,course};n->bank.owners={n->images};storage_=std::move(n);d={};return true;
 }catch(...){return fail(0x1CD304);}
}
bool OriginalUiCourseHologramDraw50KG::loaded()const noexcept{return !!storage_;}
std::shared_ptr<const assets::OriginalUiScreenAssets50KG> OriginalUiCourseHologramDraw50KG::images()const noexcept{return storage_?storage_->images:nullptr;}
const assets::OriginalUiImageBankInput50KG* OriginalUiCourseHologramDraw50KG::bankInput()const noexcept{return storage_?&storage_->bank:nullptr;}
game::OriginalUiCourseHologramScene50KG* OriginalUiCourseHologramDraw50KG::model(std::uint16_t id)noexcept{auto i=ordinal(id);return storage_&&i<ids.size()?&storage_->models[i].scene:nullptr;}
const assets::OriginalUiCourseHologramAssets50KG* OriginalUiCourseHologramDraw50KG::modelAssets(std::uint16_t id)const noexcept{auto i=ordinal(id);return storage_&&i<ids.size()?storage_->models[i].assets.get():nullptr;}
const assets::OriginalUiMaterial50KG* OriginalUiCourseHologramDraw50KG::material(std::uint16_t id)const noexcept{auto i=ordinal(id);return storage_&&i<ids.size()?&storage_->models[i].material:nullptr;}
const std::vector<std::uint32_t>* OriginalUiCourseHologramDraw50KG::images(std::uint16_t id,std::uint32_t p)const noexcept{auto i=ordinal(id);return storage_&&i<ids.size()&&p<2?&storage_->models[i].images[p]:nullptr;}
const std::vector<assets::OriginalUiLitMaterial50KM>* OriginalUiCourseHologramDraw50KG::policies(std::uint16_t id,std::uint32_t p)const noexcept{auto i=ordinal(id);return storage_&&i<ids.size()&&p<2?&storage_->models[i].policies[p]:nullptr;}
std::uint32_t OriginalUiCourseHologramDraw50KG::currentImage(std::uint16_t id,std::uint32_t p)const noexcept{auto i=ordinal(id);return storage_&&i<ids.size()&&p<2?storage_->models[i].images[p][p?storage_->models[i].frame:0]:assets::unresolvedMaterialTexture50EA;}
bool OriginalUiCourseHologramDraw50KG::advanceTextureAnimations(std::uint32_t ms,OriginalUiCourseHologramDrawDiagnostic50KG& d)noexcept{
 d={};if(!storage_)return false;
 // Only actual visible model submissions reach the unique texture controller.
 for(std::size_t i=0;i<ids.size();++i){auto& m=storage_->models[i];if(m.scene.shown()&&!game::advanceOriginalUiTextureAnimation50KG(m.animation,ms,m.frame)){d.widget=ids[i];d.sourcePC=0x29AC9C;return false;}}
 return true;
}
bool OriginalUiCourseHologramDraw50KG::project(std::uint16_t id,std::uint32_t surface,const game::ModelPalette49S& palette,const game::OriginalUiLitContext50KM& context,bool fog,game::OriginalUiLitSurfaceOutput50KM& out,game::OriginalUiLitGeometryWorkspace50KM& workspace,OriginalUiCourseHologramDrawDiagnostic50KG& d)const noexcept{
 d={};auto i=ordinal(id);if(!storage_||i>=ids.size())return false;
 if(game::projectOriginalUiLitSceneSurface50KM(*storage_->models[i].assets->scene(),0,surface,palette,context,fog,out,workspace)!=game::OriginalUiLitGeometryStatus50KM::complete){d.widget=id;d.sourcePC=0x240C00;return false;}return true;
}
}
