#pragma once
#include "original_race_pause_parent50la.h"
#include "../../src/game/original_pause_native_draw50la.h"
#include "original_pause_cold_owner50la.h"

namespace original_main_entry50KG {
inline bool Application::prewarmPauseCold50LA(original_pause_cold50LA::Owner& out)const noexcept {
    namespace a=rr::assets;
    if(out.attempted_||out.closed_)return false;
    out.attempted_=true;out.application_=this;
    try {
        if(!enabled||!videoOwner_||!Video::materialConstructionPinLive(*videoOwner_)||
           !materialContextOwner_||!materialContextOwner_->live()||
           materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())return false;
        out.domain_=standardMaterialDomainOwner_;out.context_=materialContextOwner_;
        out.overlays_=readArchive(assetRoot,a::OriginalUiArchiveId50KG::overlays);
        const auto localization=readArchive(assetRoot,a::OriginalUiArchiveId50KG::localization);
        out.data_=std::make_shared<a::OriginalPauseSourceData50LA>();
        if(!out.data_->load(out.overlays_,*localization,out.source))return false;
        const auto provider=out.domain_->archiveProvider3E1A70();
        if(!provider||!provider->mountAuthenticatedUi(out.overlays_,out.provider)||
           !provider->sealNativeMounts(out.provider))return false;
        std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
        for(auto id:{a::OriginalPauseSourceMember50LA::letterBoxMaterial,
            a::OriginalPauseSourceMember50LA::iconMaterial,a::OriginalPauseSourceMember50LA::fontMaterial,
            a::OriginalPauseSourceMember50LA::leftArrowMaterial,a::OriginalPauseSourceMember50LA::rightArrowMaterial}){
            const auto member=out.data_->member(id);
            auto source=std::make_shared<a::OriginalStandardMaterialSource50LA>();
            if(!source->prepare(member.identity->name,
                std::vector<std::uint8_t>(member.data,member.data+member.size),out.parse))return false;
            materials.push_back(std::move(source));
        }
        std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
        for(auto id:{a::OriginalPauseSourceMember50LA::results1Pixels,
            a::OriginalPauseSourceMember50LA::results2Pixels,a::OriginalPauseSourceMember50LA::ingameFontPixels,
            a::OriginalPauseSourceMember50LA::iconPixels,a::OriginalPauseSourceMember50LA::leftArrowFrames,
            a::OriginalPauseSourceMember50LA::rightArrowFrames}){
            const auto member=out.data_->member(id);
            textures.push_back({member.identity->name,
                std::vector<std::uint8_t>(member.data,member.data+member.size)});
        }
        if(!out.domain_->appendOriginalSources(materials,textures,out.material))return false;
        struct DirectoryScope {
            std::string& actual;std::string saved;
            explicit DirectoryScope(std::string& value):actual(value),saved(value){}
            ~DirectoryScope(){actual.swap(saved);}
        } directory(materialDirectory36B970_);
        // Native cold references keep precisely the later original rows live;
        // no Entity is counted and no collector is built during this prewarm.
        for(std::size_t i=0;i<a::OriginalPausePrimitiveReferences50LA::count;++i){
            materialDirectory36B970_=a::OriginalPausePrimitiveReferences50LA::source(i).directory;
            materialDirectory36B970_+='\\';
            if(!out.prewarm_.acquireNext(out.domain_,materialApplication,out.data_,
                materialDirectory36B970_,out.material))return false;
        }
        if(!fonts328()->loadPauseBase1B8BC4(out.overlays_,materialDirectory36B970_,
            fontDirectory36B420_,out.font))return false;
        out.prewarmed_=true;return true;
    }catch(...){return false;}
}
inline bool Application::loadConsumedPauseCold50LA(rr::game::OriginalRaceConstructor50LA& race,
    original_pause_cold50LA::Owner& out)const noexcept {
    return loadConsumedPauseSourceCold50LA(race,out,false);
}
// Only the private connected RA Results caller can select pre-collector staging.
// Legacy callers still require the same current collector before and after load.
// The existing pre-/post-collector loader has one out-of-line definition.
inline bool Application::finishPauseDrawCold50LA(original_pause_cold50LA::Owner& out)const noexcept {
    if(out.application_!=this||!out.loaded_||out.closed_||out.drawAttempted_||!out.storage_||
       out.domain_!=standardMaterialDomainOwner_||out.context_!=materialContextOwner_)return false;
    out.drawAttempted_=true;
    try {
        out.drawing_=std::make_shared<rr::game::OriginalPauseNativeDraw50LA>();
        return out.drawing_->initialize(out.storage_,out.nativeDiagnostic);
    }catch(...){return false;}
}
}
