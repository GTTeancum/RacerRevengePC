#include "original_ui_rider_binder_assets50kg.h"
#include <utility>

namespace rr::assets {
struct OriginalUiRiderBinderAssets50KG::Storage {
    std::shared_ptr<const OriginalUiArchive50KG> rider,appStart;
    OriginalUiMaterialTemplates50KG templates;
    OriginalUiMaterial50KG material;
    OriginalUiAtm50KG animation;
    OriginalUiScreenAssets50KG images;
    OriginalUiMemberView50KG atm;
};
namespace {
using E=OriginalUiRiderBinderAssetError50KG;
bool fail(OriginalUiRiderBinderAssetDiagnostic50KG& d,E e) noexcept {d.error=e;return false;}
bool text(const OriginalUiDatSection50KG& s,const char* key,const char* expected) noexcept {
    const auto* v=s.find(key);return v&&v->sourceValuePresent&&v->text==expected;
}
bool absent(const OriginalUiDatSection50KG& s,const char* key) noexcept {
    const auto* v=s.find(key);return !v||!v->sourceValuePresent;
}
}
OriginalUiRiderBinderAssets50KG::OriginalUiRiderBinderAssets50KG() noexcept=default;
OriginalUiRiderBinderAssets50KG::~OriginalUiRiderBinderAssets50KG()=default;
bool OriginalUiRiderBinderAssets50KG::loaded()const noexcept{return bool(storage_);}
const OriginalUiMaterialPass50KG* OriginalUiRiderBinderAssets50KG::pass()const noexcept {
    return storage_?&storage_->material.passes[0]:nullptr;
}
const OriginalUiAtm50KG* OriginalUiRiderBinderAssets50KG::animation()const noexcept {
    return storage_?&storage_->animation:nullptr;
}
const OriginalUiScreenAssets50KG* OriginalUiRiderBinderAssets50KG::images()const noexcept {
    return storage_?&storage_->images:nullptr;
}
OriginalUiMemberView50KG OriginalUiRiderBinderAssets50KG::atmMember()const noexcept {
    return storage_?storage_->atm:OriginalUiMemberView50KG{};
}
bool OriginalUiRiderBinderAssets50KG::load(std::shared_ptr<const OriginalUiArchive50KG> rider,
    std::shared_ptr<const OriginalUiArchive50KG> appStart,const OriginalUiMaterialRuntime50KG& runtime,
    OriginalUiRiderBinderAssetDiagnostic50KG& d) noexcept {
    return loadImpl(std::move(rider),std::move(appStart),runtime,d,false);
}
bool OriginalUiRiderBinderAssets50KG::loadTournament(std::shared_ptr<const OriginalUiArchive50KG> tournament,
    std::shared_ptr<const OriginalUiArchive50KG> appStart,const OriginalUiMaterialRuntime50KG& runtime,
    OriginalUiRiderBinderAssetDiagnostic50KG& d) noexcept {
    return loadImpl(std::move(tournament),std::move(appStart),runtime,d,true);
}
bool OriginalUiRiderBinderAssets50KG::loadUnlock(std::shared_ptr<const OriginalUiArchive50KG> unlock,
    std::shared_ptr<const OriginalUiArchive50KG> appStart,const OriginalUiMaterialRuntime50KG& runtime,
    OriginalUiRiderBinderAssetDiagnostic50KG& d) noexcept {
    return loadImpl(std::move(unlock),std::move(appStart),runtime,d,false,true);
}
bool OriginalUiRiderBinderAssets50KG::loadImpl(std::shared_ptr<const OriginalUiArchive50KG> rider,
    std::shared_ptr<const OriginalUiArchive50KG> appStart,const OriginalUiMaterialRuntime50KG& runtime,
    OriginalUiRiderBinderAssetDiagnostic50KG& d,bool tournament,bool unlock) noexcept {
    d={};if(storage_)return fail(d,E::alreadyLoaded);
    if(!rider||!appStart||!rider->loaded()||!appStart->loaded()||
       rider->identity()!=originalUiArchiveIdentity50KG(unlock?OriginalUiArchiveId50KG::unlock:tournament?OriginalUiArchiveId50KG::tournament:OriginalUiArchiveId50KG::rider)||
       !appStart->identity()||appStart->identity()->id!=OriginalUiArchiveId50KG::appStart)
        return fail(d,E::archive);
    try {
        auto s=std::make_unique<Storage>();s->rider=std::move(rider);s->appStart=std::move(appStart);
        s->atm=s->rider->find("ui/textures/binder.atm");
        if(!s->atm.identity||!s->atm.data||s->rider->find("ui/textures/binder.pst").identity)
            return fail(d,E::archive);
        // 1D3E10 ->23BEB0 ->150450: 150514 explicitly reads DefaultMT3833E8.
        // Do not substitute ScreenMT or the main UI material fallback.
        if(!s->templates.load(*s->appStart,d.material)||
           !readOriginalUiMaterial50KG(*s->rider,"ui/textures/binder.psm",s->templates,
                OriginalUiTemplate50KG::defaultMt,runtime,s->material,d.material))return fail(d,E::material);
        if(s->material.passes.size()!=1)return fail(d,E::materialProfile);
        const auto& p=s->material.passes[0];using F=OriginalUiGsField50KG;
        if(!p.identity||p.microprogram!="Diffuse"||p.enabled!=1||p.textureCount!=1||
           p.textureCoordinateIndex!=0||p.textureName!="binder"||
           p.gs[F::inputColorA].value!=0||p.gs[F::inputColorB].value!=2||
           p.gs[F::inputAlphaC].value!=0||p.gs[F::inputColorD].value!=1||
           p.gs[F::alphaBlendingOnOff].value!=1||p.gs[F::textureMappingOnOff].value!=1||
           p.gs[F::alphaTestOnOff].value!=0||p.gs[F::zWriteDisabled].value!=1)
            return fail(d,E::materialProfile);
        if(!text(p.authored,"TextureAnimation","unique")||
           !text(p.authored,"animatedMapFramesPerSecond","16")||
           !text(p.authored,"animatedMapReverseLoopOnEnd","0")||
           !text(p.authored,"animatedMapLoopOnEndToFrame","0")||
           !absent(p.authored,"animatedMapInitialFrame")||!absent(p.authored,"animatedMapLooping"))
            return fail(d,E::animationProfile);
        if(!splitOriginalUiAtm50KG(s->atm.data,s->atm.size,s->animation,d.atm))return fail(d,E::atm);
        if(s->animation.revision!=1||s->animation.storage!=0||s->animation.frames.size()!=10)
            return fail(d,E::animationProfile);
        std::vector<OriginalUiImageSource50KG> sources;sources.reserve(10);
        for(std::size_t i=0;i<s->animation.frames.size();++i) {
            const auto& frame=s->animation.frames[i];OriginalUiImageSource50KG source;
            // Owner-local stable identity; image ranges and member identity stay exact.
            source.identity=(std::uint64_t(s->atm.identity->logicalOffset)<<32)|std::uint64_t(i+1);
            source.member=s->atm;source.pstOffset=frame.pstOffset;source.pstBytes=frame.pstBytes;
            sources.push_back(source);
        }
        if(!s->images.load(sources,d.images))return fail(d,E::images);
        storage_=std::move(s);return true;
    }catch(...){return fail(d,E::allocation);}
}
}
