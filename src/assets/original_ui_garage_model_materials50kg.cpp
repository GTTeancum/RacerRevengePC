#include "original_ui_garage_model_materials50kg.h"
#include <utility>
namespace rr::assets {
// Defined in original_ui_material50kg.cpp (2475C0 model-row recipe).
bool readOriginalUiGarageSelectedPodFallbackMaterial50KG(const OriginalUiArchive50KG&,const std::string&,
    const OriginalUiMaterialTemplates50KG&,const OriginalUiMaterialRuntime50KG&,OriginalUiMaterial50KG&,
    OriginalUiMaterialDiagnostic50KG&) noexcept;
namespace {
using D=OriginalUiGarageModelMaterialDiagnostic50KG;using E=OriginalUiGarageModelMaterialError50KG;
using K=OriginalUiGarageMaterialKind50KG;
bool fail(D& d,E e)noexcept{d.error=e;return false;}
bool basename(const std::string& n)noexcept{
    if(n.empty()||n.size()>127||n=="."||n=="..")return false;
    for(unsigned char c:n)if(c<32||c>=127||c=='/'||c=='\\'||c==':')return false;
    return true;
}
// PORT EXTENSION (DLC career): a selected-model owner that is an add-on pod's own
// package archive (identity addonUi, never a catalog row).
bool addonSelected50DL(const std::shared_ptr<const OriginalUiArchive50KG>& a)noexcept{
    return a&&a->loaded()&&a->addon()&&a->identity()&&a->identity()->id==OriginalUiArchiveId50KG::addonUi;
}
bool authentic(const std::shared_ptr<const OriginalUiArchive50KG>& a,OriginalUiArchiveId50KG id)noexcept{
    return a&&a->loaded()&&a->identity()==originalUiArchiveIdentity50KG(id);
}
}
struct OriginalUiGarageModelMaterials50KG::Storage {
    std::shared_ptr<const void> modelPin;
    std::shared_ptr<const OriginalUiArchive50KG> archive,appStart,residentPodTextures;
    const OriginalUiMemberIdentity50KG* modelIdentity=nullptr;
    K kind=K::absent;std::size_t ordinal=0;
    std::vector<std::vector<OriginalUiGarageModelMaterialPass50KG>> materials;
    std::shared_ptr<OriginalUiScreenAssets50KG> images;
};
OriginalUiGarageModelMaterials50KG::OriginalUiGarageModelMaterials50KG()noexcept=default;
OriginalUiGarageModelMaterials50KG::~OriginalUiGarageModelMaterials50KG()=default;
bool OriginalUiGarageModelMaterials50KG::loadStatic(std::shared_ptr<const OriginalUiGarageSceneAssets50KG> a,
    std::size_t i,std::shared_ptr<const OriginalUiArchive50KG> app,const OriginalUiMaterialRuntime50KG& runtime,
    const OriginalUiIpumTextureCache50KG* cache,D& d)noexcept{
    d={};const auto* row=a?a->staticModel(i):nullptr;
    if(!a||!a->loaded()||!row||!row->model||row->ordinal!=i+1)return fail(d,E::input);
    const auto m=row->model;return load(std::move(a),m->scene(),m->source.identity,K::staticModel,i+1,m->archive,std::move(app),runtime,cache,d);
}
bool OriginalUiGarageModelMaterials50KG::loadActor(std::shared_ptr<const OriginalUiGarageSceneAssets50KG> a,
    std::size_t i,std::shared_ptr<const OriginalUiArchive50KG> app,const OriginalUiMaterialRuntime50KG& runtime,
    const OriginalUiIpumTextureCache50KG* cache,D& d)noexcept{
    d={};const auto* row=a?a->actor(i):nullptr;
    if(!a||!a->loaded()||!row||!row->model||row->ordinal!=i+1)return fail(d,E::input);
    const auto m=row->model;return load(std::move(a),m->scene(),m->source.identity,K::actor,i+1,m->archive,std::move(app),runtime,cache,d);
}
bool OriginalUiGarageModelMaterials50KG::loadSelected(std::shared_ptr<const OriginalUiGarageSceneAssets50KG> a,
    std::shared_ptr<const OriginalUiArchive50KG> app,const OriginalUiMaterialRuntime50KG& runtime,
    const OriginalUiIpumTextureCache50KG* cache,D& d,std::shared_ptr<const OriginalUiArchive50KG> residentPodTextures)noexcept{
    d={};const auto* row=a?a->selectedPod():nullptr;
    if(!a||!a->loaded()||!row||!row->model)return fail(d,E::input);
    const auto m=row->model;return load(std::move(a),m->scene(),m->source.identity,K::selected19E,0,m->archive,std::move(app),runtime,cache,d,
        std::move(residentPodTextures));
}
bool OriginalUiGarageModelMaterials50KG::loadWatto(std::shared_ptr<const OriginalUiGarageWattoAssets50KG> a,
    std::shared_ptr<const OriginalUiArchive50KG> garage,std::shared_ptr<const OriginalUiArchive50KG> app,
    const OriginalUiMaterialRuntime50KG& runtime,const OriginalUiIpumTextureCache50KG* cache,D& d)noexcept{
    d={};if(!a||!a->loaded()||!authentic(garage,OriginalUiArchiveId50KG::garage))return fail(d,E::input);
    const auto* scene=a->scene();const auto* identity=a->geometrySource();
    bool found=false;for(std::size_t i=0;i<garage->memberCount();++i)if(garage->member(i).identity==identity)found=true;
    if(!found)return fail(d,E::input);
    return load(std::move(a),scene,identity,K::watto,0,std::move(garage),std::move(app),runtime,cache,d);
}
bool OriginalUiGarageModelMaterials50KG::load(std::shared_ptr<const void> pin,const Scene48D* scene,
    const OriginalUiMemberIdentity50KG* identity,K kind,std::size_t ordinal,
    std::shared_ptr<const OriginalUiArchive50KG> archive,std::shared_ptr<const OriginalUiArchive50KG> app,
    const OriginalUiMaterialRuntime50KG& runtime,const OriginalUiIpumTextureCache50KG* cache,D& d,
    std::shared_ptr<const OriginalUiArchive50KG> residentPodTextures)noexcept{
    d={};if(storage_)return fail(d,E::alreadyLoaded);
    if(residentPodTextures&&(kind!=K::selected19E||!authentic(residentPodTextures,OriginalUiArchiveId50KG::podTextures)))
        return fail(d,E::input);
    if(!pin||!scene||!identity||scene->materials.empty()||scene->materials.size()>256||kind==K::absent||
       (kind==K::selected19E?(!archive||!archive->identity()||(!addonSelected50DL(archive)&&
        (originalUiGaragePodCareer50KG(archive->identity()->id)<0||!authentic(archive,archive->identity()->id)))):
        !authentic(archive,OriginalUiArchiveId50KG::garage))||
       !authentic(app,OriginalUiArchiveId50KG::appStart))return fail(d,E::input);
    try {
        auto n=std::make_unique<Storage>();n->modelPin=std::move(pin);n->archive=std::move(archive);n->appStart=std::move(app);
        n->modelIdentity=identity;n->kind=kind;n->ordinal=ordinal;n->residentPodTextures=std::move(residentPodTextures);
        OriginalUiMaterialTemplates50KG templates;
        if(!templates.load(*n->appStart,d.materialDetail))return fail(d,E::material);
        n->materials.resize(scene->materials.size());std::vector<OriginalUiImageSource50KG> sources;
        for(std::size_t m=0;m<scene->materials.size();++m){
            d.material=static_cast<std::uint32_t>(m);const auto& name=scene->materials[m];
            if(!basename(name))return fail(d,E::material);
            OriginalUiMaterial50KG material;
            const auto base="ui/textures/"+name;
            // 29B630 handle -1 in the selected namespace: only a boot-resident
            // UIPODTEX image (1B7698..1B7790) has the 2475C0 DefaultMT owner.
            const bool resident=n->residentPodTextures&&!n->archive->find(base+".psm").identity&&
                !n->archive->find(base+".pst").identity;
            if(resident?!readOriginalUiGarageSelectedPodFallbackMaterial50KG(*n->residentPodTextures,name,templates,
                    runtime,material,d.materialDetail):
               !readOriginalUiMaterial50KG(*n->archive,base+".psm",templates,
                    OriginalUiTemplate50KG::defaultMt,runtime,material,d.materialDetail))return fail(d,E::material);
            const auto& images=resident?*n->residentPodTextures:*n->archive;
            auto& passes=n->materials[m];passes.reserve(material.passes.size());
            for(std::size_t p=0;p<material.passes.size();++p){
                d.pass=static_cast<std::uint32_t>(p);OriginalUiGarageModelMaterialPass50KG pass;pass.source=std::move(material.passes[p]);
                if(pass.source.textureCount==1){
                    if(!basename(pass.source.textureName))return fail(d,E::texture);
                    const auto prefix="ui/textures/"+pass.source.textureName;
                    if(images.find(prefix+".atm").identity)return fail(d,E::animatedTexture);
                    const auto member=images.find(prefix+".pst");
                    if(!member.identity||!member.data)return fail(d,E::texture);
                    std::size_t i=0;for(;i<sources.size();++i)if(sources[i].member.identity==member.identity)break;
                    if(i==sources.size()){
                        if(sources.size()==256)return fail(d,E::texture);
                        OriginalUiImageSource50KG source;source.member=member;source.pstBytes=member.size;
                        // Local namespace only. A real image bank must canonicalize
                        // retained member/provenance identity before renderer use.
                        source.identity=0x474d000000000000ull|std::uint64_t(i+1);sources.push_back(source);
                    }
                    pass.imageIndex=static_cast<std::uint32_t>(i);
                }else if(pass.source.textureCount!=0)return fail(d,E::material);
                passes.push_back(std::move(pass));
            }
        }
        if(!sources.empty()){
            n->images=std::make_shared<OriginalUiScreenAssets50KG>();
            if(!n->images->load(sources,d.images,cache))return fail(d,E::images);
        }
        storage_=std::move(n);d={};return true;
    }catch(...){return fail(d,E::allocation);}
}
bool OriginalUiGarageModelMaterials50KG::loaded()const noexcept{return bool(storage_);}
K OriginalUiGarageModelMaterials50KG::kind()const noexcept{return storage_?storage_->kind:K::absent;}
std::size_t OriginalUiGarageModelMaterials50KG::sourceOrdinal()const noexcept{return storage_?storage_->ordinal:0;}
const OriginalUiMemberIdentity50KG* OriginalUiGarageModelMaterials50KG::modelIdentity()const noexcept{return storage_?storage_->modelIdentity:nullptr;}
std::uint32_t OriginalUiGarageModelMaterials50KG::materialCount()const noexcept{return storage_?static_cast<std::uint32_t>(storage_->materials.size()):0;}
std::uint32_t OriginalUiGarageModelMaterials50KG::passCount(std::uint32_t m)const noexcept{return storage_&&m<storage_->materials.size()?static_cast<std::uint32_t>(storage_->materials[m].size()):0;}
const OriginalUiGarageModelMaterialPass50KG* OriginalUiGarageModelMaterials50KG::pass(std::uint32_t m,std::uint32_t p)const noexcept{return storage_&&m<storage_->materials.size()&&p<storage_->materials[m].size()?&storage_->materials[m][p]:nullptr;}
std::shared_ptr<const OriginalUiScreenAssets50KG> OriginalUiGarageModelMaterials50KG::images()const noexcept{return storage_?storage_->images:nullptr;}
std::shared_ptr<const OriginalUiArchive50KG> OriginalUiGarageModelMaterials50KG::archive()const noexcept{return storage_?storage_->archive:nullptr;}
bool prepareOriginalUiGaragePrelitMaterial50KG(const OriginalUiGarageModelMaterials50KG& owner,
    std::uint32_t materialSlot,std::uint32_t passIndex,OriginalUiGaragePrelitMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};try {
        using Error=OriginalUiMaterialError50KG;
        const auto fail=[](OriginalUiMaterialDiagnostic50KG& diagnostic,Error error,
            const char* field,std::uint32_t pc) {
            diagnostic.error=error;diagnostic.field=field;diagnostic.sourcePC=pc;return false;
        };
        const auto same=[](const std::string& a,const char* b) noexcept {
            const auto fold=[](char c) noexcept {return c>='A'&&c<='Z'?char(c+32):c;};
            std::size_t i=0;
            for(;i<a.size()&&b[i];++i)if(fold(a[i])!=fold(b[i]))return false;
            return i==a.size()&&!b[i];
        };
        const auto* row=owner.pass(materialSlot,passIndex);
        const auto archive=owner.archive();const auto images=owner.images();
        const bool selected=owner.kind()==OriginalUiGarageMaterialKind50KG::selected19E;
        const auto archiveId=selected&&archive&&archive->identity()?archive->identity()->id:OriginalUiArchiveId50KG::garage;
        if(!owner.loaded() || !owner.modelIdentity() ||
           (!selected && owner.kind()!=OriginalUiGarageMaterialKind50KG::staticModel) ||
           !archive || !archive->loaded() ||
           (selected&&addonSelected50DL(archive)?false: // DLC career: package archive as selected owner
            ((selected&&originalUiGaragePodCareer50KG(archiveId)<0) ||
             archive->identity()!=originalUiArchiveIdentity50KG(archiveId))) ||
           !row || !images || !images->loaded())
            return fail(d,Error::profile,"actual Garage static/selected material and image owner",0x26EA0Cu);
        const auto& pass=row->source;d.pass=pass.pass;
        if(!pass.identity || pass.identity!=archive->find(pass.identity->name).identity ||
           pass.pass!=1 || !same(pass.microprogram,"PreLitCompressed") || pass.textureCount!=1)
            return fail(d,Error::profile,"authored Garage PreLitCompressed first pass",0x2476A0u);
        const auto memberName=std::string("ui\\textures\\")+pass.textureName+".pst";
        const auto* member=archive->find(memberName).identity;
        const auto* source=images->sourceProvenance(row->imageIndex);
        const auto* texture=images->decodedTexture(row->imageIndex);
        const auto* ipum=images->ipumProvenance(row->imageIndex);
        const auto* bank=images->textures();const auto* image=bank?bank->texture(row->imageIndex):nullptr;
        if(!member || !source || !texture || !image || source->memberIdentity!=member ||
           source->celIdentity || source->pstOffset || source->pstBytes!=member->bytes ||
           texture->mips.empty() || texture->mips.size()>originalUiIpumMipCount50KG || texture->kind>2)
            return fail(d,Error::texture,"exact Garage source PST and retained mip owner",0x24E880u);
        if(texture->kind==2){
            if(!ipum || ipum->sourcePstSha256!=member->sha256 || !same(ipum->sourceMember,member->name) ||
               ipum->sourceArchiveSha256!=archive->identity()->sha256 ||
               ipum->colorPolicyKind!=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1 ||
               ipum->mipCount!=texture->mips.size())
                return fail(d,Error::texture,"authenticated Garage IPUM color/mip provenance",0x24E880u);
        }else if(ipum)return fail(d,Error::texture,"indexed Garage PST cannot alias IPUM",0x24E880u);
        const auto& base=texture->mips.back();
        if(image->texture.width!=base.width || image->texture.height!=base.height || image->texture.rgba!=base.rgba)
            return fail(d,Error::texture,"published image equals original largest Garage mip",0x24EB90u);
        OriginalUiGaragePrelitMaterial50KG next;
        if(!OriginalUiGaragePrelitRasterAdapter50KG::convert(owner,pass,*texture,next,d))return false;
        next.owner=&owner;next.sourcePass=row;next.images=images.get();next.imageIndex=row->imageIndex;
        next.imageIdentity=image->identity;next.sourceMipCount=static_cast<std::uint32_t>(texture->mips.size());
        next.sourceStoredMipIndex=next.sourceMipCount-1;
        output=next;return true;
    }catch(...){d.error=OriginalUiMaterialError50KG::allocation;return false;}
}

bool prepareOriginalUiGarageLitMaterial50KG(const OriginalUiGarageModelMaterials50KG& owner,
    std::uint32_t materialSlot,std::uint32_t passIndex,OriginalUiGarageLitMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};try {
        using Error=OriginalUiMaterialError50KG;
        const auto fail=[](OriginalUiMaterialDiagnostic50KG& diagnostic,Error error,
            const char* field,std::uint32_t pc) {
            diagnostic.error=error;diagnostic.field=field;diagnostic.sourcePC=pc;return false;
        };
        const auto same=[](const std::string& a,const char* b) noexcept {
            const auto fold=[](char c) noexcept {return c>='A'&&c<='Z'?char(c+32):c;};
            std::size_t i=0;
            for(;i<a.size()&&b[i];++i)if(fold(a[i])!=fold(b[i]))return false;
            return i==a.size()&&!b[i];
        };
        const auto* row=owner.pass(materialSlot,passIndex);
        const auto archive=owner.archive();const auto images=owner.images();
        const bool selected=owner.kind()==OriginalUiGarageMaterialKind50KG::selected19E;
        const auto archiveId=selected&&archive&&archive->identity()?archive->identity()->id:OriginalUiArchiveId50KG::garage;
        if(!owner.loaded() || !owner.modelIdentity() ||
           (!selected && owner.kind()!=OriginalUiGarageMaterialKind50KG::staticModel &&
            owner.kind()!=OriginalUiGarageMaterialKind50KG::actor && owner.kind()!=OriginalUiGarageMaterialKind50KG::watto) ||
           !archive || !archive->loaded() ||
           (selected&&addonSelected50DL(archive)?false: // DLC career: package archive as selected owner
            ((selected&&originalUiGaragePodCareer50KG(archiveId)<0) ||
             archive->identity()!=originalUiArchiveIdentity50KG(archiveId))) ||
           !row || !images || !images->loaded())
            return fail(d,Error::profile,"actual Garage model material and image owner",0x26EA0Cu);
        const auto& pass=row->source;d.pass=pass.pass;
        // 2475C0 selected-model owner: no PSM identity, one DefaultMT map whose
        // image is the boot-resident UIPODTEX member recorded at construction.
        const auto* resident=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::podTextures);
        const bool fallback=selected&&!pass.identity&&resident&&pass.provenance.creationPC==0x2475C0u&&
            pass.provenance.archive==resident&&pass.provenance.fallbackTexture&&
            pass.provenance.origin==OriginalUiMaterialOrigin50KG::authoredPsm&&
            pass.pass==1&&passIndex==0&&same(pass.microprogram,"DiffuseCompressed")&&pass.textureCount==1;
        if(!fallback&&(!pass.identity || pass.identity!=archive->find(pass.identity->name).identity ||
           pass.pass!=passIndex+1 ||
           (!same(pass.microprogram,"Diffuse") && !same(pass.microprogram,"DiffuseCompressed") &&
            !same(pass.microprogram,"SpecularMapCompressed")) || pass.textureCount!=1))
            return fail(d,Error::profile,"authored Garage diffuse/specular pass",0x2476A0u);
        const auto memberName=std::string("ui\\textures\\")+pass.textureName+".pst";
        const auto* member=fallback?pass.provenance.fallbackTexture:archive->find(memberName).identity;
        if(fallback&&(!member->name||!same(std::string(member->name),memberName.c_str())))
            return fail(d,Error::texture,"boot-resident UIPODTEX member of the 2475C0 owner",0x24E880u);
        const auto* imageArchive=fallback?resident:archive->identity();
        const auto* source=images->sourceProvenance(row->imageIndex);
        const auto* texture=images->decodedTexture(row->imageIndex);
        const auto* ipum=images->ipumProvenance(row->imageIndex);
        const auto* bank=images->textures();const auto* image=bank?bank->texture(row->imageIndex):nullptr;
        if(!member || !source || !texture || !image || source->memberIdentity!=member ||
           source->celIdentity || source->pstOffset || source->pstBytes!=member->bytes ||
           texture->mips.empty() || texture->mips.size()>originalUiIpumMipCount50KG || texture->kind>2)
            return fail(d,Error::texture,"exact Garage source PST and retained mip owner",0x24E880u);
        if(texture->kind==2){
            if(!ipum || ipum->sourcePstSha256!=member->sha256 || !same(ipum->sourceMember,member->name) ||
               ipum->sourceArchiveSha256!=imageArchive->sha256 ||
               ipum->colorPolicyKind!=OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1 ||
               ipum->mipCount!=texture->mips.size())
                return fail(d,Error::texture,"authenticated Garage IPUM color/mip provenance",0x24E880u);
        }else if(ipum)return fail(d,Error::texture,"indexed Garage PST cannot alias IPUM",0x24E880u);
        const auto& base=texture->mips.back();
        if(image->texture.width!=base.width || image->texture.height!=base.height || image->texture.rgba!=base.rgba)
            return fail(d,Error::texture,"published image equals original largest Garage mip",0x24EB90u);
        using F=OriginalUiGsField50KG;
        const bool aa1=pass.gs[F::onePassAntiAliasingOnOff].value!=0;
        if(aa1 && (owner.kind()!=OriginalUiGarageMaterialKind50KG::watto ||
           pass.identity!=archive->find("ui\\textures\\25c_7.psm").identity || pass.pass!=1 ||
           !same(pass.microprogram,"DiffuseCompressed") || pass.gs[F::alphaBlendingOnOff].value!=1 ||
           pass.gs[F::inputColorA].value!=0 || pass.gs[F::inputColorB].value!=1 ||
           pass.gs[F::inputAlphaC].value!=0 || pass.gs[F::inputColorD].value!=1 ||
           pass.gs[F::alphaTestOnOff].value!=0))
            return fail(d,Error::profile,"actual Watto25c_7 source-alpha AA1 pass",0x2476A0);
        OriginalUiGarageLitMaterial50KG next;
        if(!OriginalUiGarageLitRasterAdapter50KG::convert(owner,pass,*texture,aa1,next,d))return false;
        next.owner=&owner;next.sourcePass=row;next.images=images.get();next.imageIndex=row->imageIndex;
        next.imageIdentity=image->identity;next.sourceMipCount=static_cast<std::uint32_t>(texture->mips.size());
        next.sourceStoredMipIndex=next.sourceMipCount-1;
        output=next;return true;
    }catch(...){d.error=OriginalUiMaterialError50KG::allocation;return false;}
}

}
