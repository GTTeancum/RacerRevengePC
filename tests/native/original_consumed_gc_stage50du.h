#pragma once
#include "original_gc_world_cold_prefix50la.h"
#include "original_race_native_retirement50la.h"
#include "live_audio50hf_fixture.h"

namespace original_consumed_gc_stage50DU {
// Declare before the actual runtime/GPU borrowers, and after its audio owner.
// The last fallback runs only after those later owners have released their borrows.
class Owner final {
    const original_main_entry50KG::Application& application_;
    live_audio50HF::Owner& audio_;
    std::shared_ptr<rr::game::OriginalRaceConstructor50LA> race_;
    bool attempted_=false;
public:
    rr::platform::AudioOutputDiagnostic50HD closeAudioDiagnostic;
    rr::assets::OriginalStandardMaterialDiagnostic50LA closeMaterialDiagnostic;
    Owner(const original_main_entry50KG::Application& app,live_audio50HF::Owner& audio)noexcept
        :application_(app),audio_(audio){}
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    ~Owner()noexcept {if(!close())std::terminate();}
    bool begin(original_course_targets50KX::ConsumedLaunch& launch,std::uint64_t session,
        std::uint32_t racer,rr::game::OriginalRacerModelRenderGlobals50LA& modelGlobals,
        const std::shared_ptr<gc_original_world50LM::Owner>& world,
        rr::assets::OriginalStandardMaterialDiagnostic50LA& d,std::uint32_t& pc)noexcept {
        d={};pc=0;
        if(attempted_||!world||!world->loaded()||!world->mesh()||!world->bank()||!application_.riderProfile||
           !launch.matches(*application_.riderProfile,session,rr::platform::HostTrack50IM::GC,racer))return false;
        attempted_=true;
        if(!audio_.beginCountedRaceAudioStorage())return false;
        auto* mixer=audio_.mixerForCountedConstruction();
        auto* groups=audio_.groupsForCountedConstruction();
        if(!mixer||!groups)return false;
        race_=application_.constructConsumedRaceCold1A65C0(launch,session,pc);
        if(!race_||!application_.publishConsumedRace28C(race_))return false;
        if(!application_.beginConsumedRaceLoad1A6AA0(*race_,modelGlobals,d)||
           !application_.populateConsumedRaceResources1AA5A0(*race_,d)||
           !race_->constructRaceGroups1A6B5C(*groups,*mixer,pc)||
           !application_.constructConsumedGcRaceWorld1A6BC0(*race_,world,pc))return false;
        // Only this reached prefix. No complete slot30 load or App10 attachment.
        return true;
    }
    const std::shared_ptr<rr::game::OriginalRaceConstructor50LA>& race()const noexcept {return race_;}
    bool close()noexcept {
        if(!attempted_)return true;
        closeAudioDiagnostic={};closeMaterialDiagnostic={};
        if(!audio_.close(closeAudioDiagnostic))return false;
        if(race_){
            if(!application_.closeNativeConsumedRace(*race_,closeMaterialDiagnostic))return false;
            race_.reset();
        }
        return true;
    }
};
}
