#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "original_pause_cold_owner50la.h"
#include "original_pause_entity50la.h"
#include <iostream>

namespace original_main_entry50KG {
std::shared_ptr<rr::game::OriginalPauseStorage50LA> Application::constructConsumedRacePause1A8AB0(
    rr::game::OriginalRaceConstructor50LA& owner)const noexcept {
    if(sourceRace28COwner_.get()!=&owner||owner.application_!=entitiesOwner_||owner.profile_!=riderProfile||
       !owner.canRetainPause100())return {};
    auto pause=constructPauseCold1A8AB0(owner.entity_.sessionGeneration);
    if(!pause)return {};
    owner.pause100_=pause; //Actual1A8AB8 publication before caller invokes Pause load.
    return pause;
}
bool Application::loadConsumedPauseSourceCold50LA(rr::game::OriginalRaceConstructor50LA& race,
    original_pause_cold50LA::Owner& out,bool beforeCollector)const noexcept {
    if(out.application_!=this||!out.prewarmed_||out.closed_||out.storage_||out.loaded_||
       out.domain_!=standardMaterialDomainOwner_||out.context_!=materialContextOwner_||
       !out.context_||!out.context_->live()||!videoOwner_){
        std::cerr<<"consumed_pause_cold_rejected step=owner prewarmed="<<out.prewarmed_<<" closed="<<out.closed_
            <<" storage="<<bool(out.storage_)<<" loaded="<<out.loaded_<<" domain="<<(out.domain_==standardMaterialDomainOwner_)
            <<" context="<<(out.context_==materialContextOwner_)<<std::endl;
        return false;}
    try {
        const auto table=out.domain_->materials382B38();
        const auto seal=out.domain_->coldCollector14D440();
        const auto provider=out.domain_->archiveProvider3E1A70();
        if(!table||!provider||!provider->sealed()||
           table->coldCollectorCurrent(seal)==beforeCollector){
            std::cerr<<"consumed_pause_cold_rejected step=collector"<<std::endl;return false;}
        const auto revision=provider->revision();
        out.storage_=constructConsumedRacePause1A8AB0(race);
        if(!out.storage_){std::cerr<<"consumed_pause_cold_rejected step=construct1A8AB0 race28C="
            <<(sourceRace28COwner_.get()==&race)<<" app="<<(race.application_==entitiesOwner_)
            <<" profile="<<(race.profile_==riderProfile)<<" retain="<<race.canRetainPause100()<<std::endl;return false;}
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
           !finishPauseEntityCold1A8ADC(*out.storage_)){
            std::cerr<<"consumed_pause_cold_rejected step=load pc="<<std::hex<<out.storageDiagnostic.sourcePC
                <<" material_pc="<<out.storageDiagnostic.material.sourcePC<<std::dec
                <<" material_status="<<static_cast<unsigned>(out.storageDiagnostic.material.status)
                <<" object="<<out.storageDiagnostic.object;
            {const auto& t=out.storageDiagnostic.material.texture;
             std::cerr<<" texture_status="<<static_cast<unsigned>(t.status)<<" texture_pc="<<std::hex<<t.sourcePC<<std::dec
                 <<" texture_row="<<t.row<<" texture_source="<<t.source;
             rr::assets::OriginalStandardTextureRowView50LA view;
             if(t.row>=0&&t.row<=32767&&out.domain_->textures383400()->row(static_cast<std::int16_t>(t.row),view)&&view.name00)
                 std::cerr<<" texture_name="<<*view.name00;}
            std::cerr<<std::endl;return false;}
        if(provider->revision()!=revision||table->coldCollectorCurrent(seal)==beforeCollector){
            std::cerr<<"consumed_pause_cold_rejected step=after_load"<<std::endl;return false;}
        out.loaded_=true;return true;
    }catch(...){return false;}
}
bool Application::loadHostRacePauseCold50LA(original_pause_cold50LA::Owner& out,
    std::uint64_t sessionGeneration,std::int32_t launchSubtype)const noexcept {
    // Same order and checks as loadConsumedPauseSourceCold50LA (collector open,
    // sealed provider revision unchanged); storage is not published to a race.
    if(out.application_!=this||!out.prewarmed_||out.closed_||out.storage_||out.loaded_||!sessionGeneration||
       launchSubtype<0||out.domain_!=standardMaterialDomainOwner_||out.context_!=materialContextOwner_||
       !out.context_||!out.context_->live()||!videoOwner_)return false;
    try {
        const auto table=out.domain_->materials382B38();
        const auto seal=out.domain_->coldCollector14D440();
        const auto provider=out.domain_->archiveProvider3E1A70();
        if(!table||!provider||!provider->sealed()||!table->coldCollectorCurrent(seal))return false;
        const auto revision=provider->revision();
        out.storage_=constructPauseCold1A8AB0(sessionGeneration);
        if(!out.storage_)return false;
        struct DirectoryScope {
            std::string& actual;std::string saved;
            explicit DirectoryScope(std::string& value):actual(value),saved(value){actual="ui\\textures\\";}
            ~DirectoryScope(){actual.swap(saved);}
        } directory(materialDirectory36B970_);
        const auto maximum=static_cast<std::uint32_t>(videoOwner_->packetZMaximum);
        if(!out.storage_->loadBaseBackdrops1B8810(out.domain_,out.context_,out.data_,
               materialDirectory36B970_,maximum,out.storageDiagnostic)||
           !out.storage_->loadBasePolygons1B8DB0(materialDirectory36B970_,out.storageDiagnostic)||
           !out.storage_->loadBaseText1B8B9C(fonts328(),materialDirectory36B970_,fontDirectory36B420_,out.storageDiagnostic)||
           !out.storage_->loadDerived1BA390(materialDirectory36B970_,fontDirectory36B420_,
               launchSubtype,maximum,out.storageDiagnostic)||
           !finishPauseEntityCold1A8ADC(*out.storage_))return false;
        if(provider->revision()!=revision||!table->coldCollectorCurrent(seal))return false;
        out.loaded_=true;return true;
    }catch(...){return false;}
}
const std::shared_ptr<rr::assets::OriginalPauseSourceData50LA>&
Application::pauseSourceDataForResults50LA(const original_pause_cold50LA::Owner& pause)const noexcept {
    return pause.data_;
}
}
