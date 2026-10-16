#pragma once
#include "original_race_pause_parent50la.h"
#include "../../src/game/original_pause_native_draw50la.h"

namespace original_pause_cold50LA {
class Owner final {
    friend struct original_main_entry50KG::Application;
    const original_main_entry50KG::Application* application_=nullptr;
    std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA> domain_;
    std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context_;
    std::shared_ptr<const rr::assets::OriginalUiArchive50KG> overlays_;
    std::shared_ptr<rr::assets::OriginalPauseSourceData50LA> data_;
    rr::assets::OriginalPausePrimitiveReferences50LA prewarm_;
    std::shared_ptr<rr::game::OriginalPauseStorage50LA> storage_;
    std::shared_ptr<rr::game::OriginalPauseNativeDraw50LA> drawing_;
    bool attempted_=false,prewarmed_=false,loaded_=false,drawAttempted_=false,closed_=false;
public:
    rr::assets::OriginalPauseSourceDiagnostic50LA source;
    rr::assets::OriginalStandardArchiveProvider50LA::Diagnostic provider;
    rr::assets::OriginalUiPageDiagnostic50KG parse;
    rr::assets::OriginalStandardMaterialDiagnostic50LA material;
    rr::assets::OriginalApplicationFontDiagnostic50LA font;
    rr::game::OriginalPauseStorage50LA::Diagnostic storageDiagnostic;
    rr::game::OriginalPauseNativeResources50LA::Diagnostic nativeDiagnostic;
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner(){if(!close())std::terminate();}
    const std::shared_ptr<rr::game::OriginalPauseStorage50LA>& storage()const noexcept{return storage_;}
    const std::shared_ptr<rr::game::OriginalPauseNativeDraw50LA>& drawing()const noexcept{return drawing_;}
    // Renderer owns drawing_ strongly until its real GPU retirement completes.
    bool retireDrawing()noexcept {
        if(drawing_&&drawing_.use_count()!=1)return false;
        drawing_.reset();return true;
    }
    bool close()noexcept {
        if(closed_)return true;
        if(!retireDrawing())return false;
        storage_.reset(); // Actual counted race retains its Pause until graph cleanup.
        for(std::size_t i=rr::assets::OriginalPausePrimitiveReferences50LA::count;i;--i)
            prewarm_.release299AC0(i-1);
        data_.reset();overlays_.reset();context_.reset();domain_.reset();
        closed_=true;return true;
    }
};
}
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
    if(out.application_!=this||!out.prewarmed_||out.closed_||out.storage_||out.loaded_||
       out.domain_!=standardMaterialDomainOwner_||out.context_!=materialContextOwner_||
       !out.context_||!out.context_->live()||!videoOwner_)return false;
    try {
        const auto table=out.domain_->materials382B38();
        const auto seal=out.domain_->coldCollector14D440();
        const auto provider=out.domain_->archiveProvider3E1A70();
        if(!table||!provider||!provider->sealed()||!table->coldCollectorCurrent(seal))return false;
        const auto revision=provider->revision();
        out.storage_=constructConsumedRacePause1A8AB0(race);
        if(!out.storage_)return false;
        struct DirectoryScope {
            std::string& actual;std::string saved;
            explicit DirectoryScope(std::string& value):actual(value),saved(value){actual="ui\\textures\\";}
            ~DirectoryScope(){actual.swap(saved);}
        } directory(materialDirectory36B970_); //Actual1A8A6C/1A8AA0, restore1A90CC.
        const auto maximum=static_cast<std::uint32_t>(videoOwner_->packetZMaximum);
        if(!out.storage_->loadBaseBackdrops1B8810(out.domain_,out.context_,out.data_,
               materialDirectory36B970_,maximum,out.storageDiagnostic)||
           !out.storage_->loadBasePolygons1B8DB0(materialDirectory36B970_,out.storageDiagnostic)||
           !out.storage_->loadBaseText1B8B9C(fonts328(),materialDirectory36B970_,fontDirectory36B420_,out.storageDiagnostic)||
           !out.storage_->loadDerived1BA390(materialDirectory36B970_,fontDirectory36B420_,
               static_cast<std::int32_t>(race.launchSelection_.subtype),maximum,out.storageDiagnostic)||
           !finishPauseEntityCold1A8ADC(*out.storage_))return false;
        if(provider->revision()!=revision||!table->coldCollectorCurrent(seal))return false;
        out.loaded_=true;return true;
    }catch(...){return false;}
}
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
