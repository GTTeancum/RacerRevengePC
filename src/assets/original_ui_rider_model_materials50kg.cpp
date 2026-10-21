#include "original_ui_rider_model_materials50kg.h"
#include <utility>
namespace rr::assets {
namespace {
using E=OriginalUiRiderModelMaterialError50KG;using D=OriginalUiRiderModelMaterialDiagnostic50KG;
bool fail(D& d,E e) noexcept {d.error=e;return false;}
bool basename(const std::string& name) noexcept {
    if(name.empty()||name.size()>127)return false;
    for(unsigned char c:name)if(c<32||c>=127||c=='/'||c=='\\'||c==':')return false;
    return name!="."&&name!="..";
}
}
struct OriginalUiRiderModelMaterials50KG::Storage {
    std::shared_ptr<const void> model;
    std::shared_ptr<const OriginalUiArchive50KG> rider,appStart,podTextures;
    std::vector<std::vector<OriginalUiRiderModelMaterialPass50KG>> materials;
    std::shared_ptr<OriginalUiScreenAssets50KG> images;
};
OriginalUiRiderModelMaterials50KG::OriginalUiRiderModelMaterials50KG() noexcept=default;
OriginalUiRiderModelMaterials50KG::~OriginalUiRiderModelMaterials50KG()=default;
bool OriginalUiRiderModelMaterials50KG::loadCharacter(std::shared_ptr<const OriginalUiRiderCharacterAssets50KG> model,
    std::shared_ptr<const OriginalUiArchive50KG> rider,std::shared_ptr<const OriginalUiArchive50KG> app,
    const OriginalUiMaterialRuntime50KG& runtime,const OriginalUiIpumTextureCache50KG* cache,D& d) noexcept {
    d={};if(!model||!model->loaded()||!rider||!rider->loaded()||
        model->widgetSource()!=rider->find("UI/UIRider/Widgets.Dat").identity)return fail(d,E::input);
    const auto* scene=model->scene();return load(std::move(model),scene,std::move(rider),std::move(app),nullptr,runtime,cache,d);
}
bool OriginalUiRiderModelMaterials50KG::loadPod(std::shared_ptr<const OriginalUiRiderPodAssets50KG> model,
    std::shared_ptr<const OriginalUiArchive50KG> app,
    std::shared_ptr<const OriginalUiArchive50KG> podTextures,const OriginalUiMaterialRuntime50KG& runtime,
    const OriginalUiIpumTextureCache50KG* cache,D& d) noexcept {
    d={};if(!model||!model->loaded()||!podTextures||!podTextures->loaded()||
        podTextures->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::podTextures))return fail(d,E::input);
    const auto* scene=model->scene();auto rider=model->archive();
    return load(std::move(model),scene,std::move(rider),std::move(app),std::move(podTextures),runtime,cache,d);
}
bool OriginalUiRiderModelMaterials50KG::loadTournamentPod(
    std::shared_ptr<const OriginalUiRiderPodAssets50KG> model,
    std::shared_ptr<const OriginalUiArchive50KG> app,
    std::shared_ptr<const OriginalUiArchive50KG> podTextures,const OriginalUiMaterialRuntime50KG& runtime,
    const OriginalUiIpumTextureCache50KG* cache,D& d) noexcept {
    d={};const auto archive=model?model->archive():nullptr;
    if(!model||!model->loaded()||!archive||!archive->loaded()||
       archive->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament)||
       model->widgetSource()!=archive->find("UI/UITourn/Widgets.Dat").identity||
       model->widgetId()<39||model->widgetId()>61||!podTextures||!podTextures->loaded()||
       podTextures->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::podTextures))return fail(d,E::input);
    const auto* scene=model->scene();
    return load(std::move(model),scene,archive,std::move(app),std::move(podTextures),runtime,cache,d,true);
}
bool OriginalUiRiderModelMaterials50KG::load(std::shared_ptr<const void> model,const Scene48D* scene,
    std::shared_ptr<const OriginalUiArchive50KG> rider,std::shared_ptr<const OriginalUiArchive50KG> app,
    std::shared_ptr<const OriginalUiArchive50KG> podTextures,
    const OriginalUiMaterialRuntime50KG& runtime,const OriginalUiIpumTextureCache50KG* cache,D& d,bool tournament) noexcept {
    d={};if(storage_)return fail(d,E::alreadyLoaded);
    if(!model||!scene||scene->materials.empty()||scene->materials.size()>256||!rider||!app||
        !rider->loaded()||!app->loaded()||
        rider->identity()!=originalUiArchiveIdentity50KG(tournament?OriginalUiArchiveId50KG::tournament:OriginalUiArchiveId50KG::rider)||
        app->identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::appStart))return fail(d,E::input);
    try {
        auto next=std::make_unique<Storage>();next->model=std::move(model);
        next->rider=std::move(rider);next->appStart=std::move(app);
        next->podTextures=std::move(podTextures);
        OriginalUiMaterialTemplates50KG templates;
        if(!templates.load(*next->appStart,d.materialDetail))return fail(d,E::material);
        next->materials.resize(scene->materials.size());std::vector<OriginalUiImageSource50KG> sources;
        for(std::size_t m=0;m<scene->materials.size();++m){
            d.material=static_cast<std::uint32_t>(m);const auto& name=scene->materials[m];
            if(!basename(name))return fail(d,E::material);
            OriginalUiMaterial50KG material;
            if(!readOriginalUiMaterial50KG(*next->rider,"ui/textures/"+name+".psm",templates,
                OriginalUiTemplate50KG::defaultMt,runtime,material,d.materialDetail))return fail(d,E::material);
            auto& passes=next->materials[m];passes.reserve(material.passes.size());
            for(std::size_t p=0;p<material.passes.size();++p){
                d.pass=static_cast<std::uint32_t>(p);OriginalUiRiderModelMaterialPass50KG pass;
                pass.source=std::move(material.passes[p]);
                if(pass.source.textureCount==1){
                    if(!basename(pass.source.textureName))return fail(d,E::texture);
                    const auto prefix="ui/textures/"+pass.source.textureName;
                    // Do not replace an original animated source with a PST.
                    if(next->rider->find(prefix+".atm").identity||
                        (next->podTextures&&next->podTextures->find(prefix+".atm").identity))return fail(d,E::animatedTexture);
                    auto member=next->rider->find(prefix+".pst");
                    const auto shared=next->podTextures?next->podTextures->find(prefix+".pst"):OriginalUiMemberView50KG{};
                    // This is the concrete UI resource population, not an arbitrary
                    // filename fallback. Both owners are exact authenticated originals.
                    // An unproven registry collision is rejected rather than reordered.
                    if(member.identity&&shared.identity)return fail(d,E::texture);
                    if(shared.identity)member=shared;
                    if(!member.identity||!member.data)return fail(d,E::texture);
                    std::size_t i=0;for(;i<sources.size();++i)if(sources[i].member.identity==member.identity)break;
                    if(i==sources.size()){
                        if(sources.size()==256)return fail(d,E::texture);
                        OriginalUiImageSource50KG source{};source.member=member;
                        source.pstBytes=member.size;
                        // Owner-local ID only. Combined image bank must remap
                        // actual member/index identity before publishing spans.
                        source.identity=0x524d000000000000ull|std::uint64_t(i+1);
                        sources.push_back(source);
                    }
                    pass.imageIndex=static_cast<std::uint32_t>(i);
                }else if(pass.source.textureCount!=0)return fail(d,E::material);
                passes.push_back(std::move(pass));
            }
        }
        if(!sources.empty()){
            next->images=std::make_shared<OriginalUiScreenAssets50KG>();
            if(!next->images->load(sources,d.images,cache))return fail(d,E::images);
        }
        storage_=std::move(next);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiRiderModelMaterials50KG::loaded()const noexcept{return bool(storage_);}
std::uint32_t OriginalUiRiderModelMaterials50KG::materialCount()const noexcept {
    return storage_?static_cast<std::uint32_t>(storage_->materials.size()):0;
}
std::uint32_t OriginalUiRiderModelMaterials50KG::passCount(std::uint32_t m)const noexcept {
    return storage_&&m<storage_->materials.size()?static_cast<std::uint32_t>(storage_->materials[m].size()):0;
}
const OriginalUiRiderModelMaterialPass50KG* OriginalUiRiderModelMaterials50KG::pass(std::uint32_t m,std::uint32_t p)const noexcept {
    return storage_&&m<storage_->materials.size()&&p<storage_->materials[m].size()?&storage_->materials[m][p]:nullptr;
}
std::shared_ptr<const OriginalUiScreenAssets50KG> OriginalUiRiderModelMaterials50KG::images()const noexcept {
    return storage_?storage_->images:nullptr;
}
}
