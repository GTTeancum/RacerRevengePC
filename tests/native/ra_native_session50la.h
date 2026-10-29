#pragma once
namespace interactive_cold50DU { struct OwnersContext; }
#include "ra_cold_roster_operation50la.h"
#include "ra_track_particle_manager50la.h"
#include "ra_static_model_factory50la.h"
#include "track_session_assets50ig_fixture.h"
#include "original_pause_cold_driver50la.h"
#include "../../src/game/original_results104_50la.h"

// Concrete synchronous composition of the existing native session owners.
// The caller keeps Application, the execution arena and Bank alive; this class
// neither creates a second arena nor owns a second clock/physics driver.
namespace ra_native_session50LA {
namespace g=rr::game;
namespace a=rr::assets;
struct Diagnostic {
    const char* operation=nullptr;
    const char* failure=nullptr;
    ra_cold_aggregate50LA::Diagnostic aggregate{};
    ra_cold_roster50LA::Diagnostic roster{};
    g::OriginalResults104Diagnostic50LA results{};
};
class Join final {
    friend struct ::interactive_cold50DU::OwnersContext;
    std::shared_ptr<ra_cold_aggregate50LA::Owner> aggregate_;
    const original_main_entry50KG::Application& application_;
    native_racer50HN::Bank& bank_;
    const std::uint64_t generation_;
    std::shared_ptr<ra_cold_roster50LA::Operation> roster_;
    bool attempted_=false,retired_=false,cameraAttempted_=false;
    std::array<bool,8> gridAttempted_{},freshAttempted_{};
    bool fail(Diagnostic& d,const char* why)noexcept{d.failure=why;return false;}
public:
    Join(std::shared_ptr<ra_cold_aggregate50LA::Owner> aggregate,
        const original_main_entry50KG::Application& application,
        native_racer50HN::Bank& bank,std::uint64_t generation)
        :aggregate_(std::move(aggregate)),application_(application),bank_(bank),generation_(generation){}
    Join(const Join&)=delete;Join& operator=(const Join&)=delete;
    ~Join(){
        if(roster_&&!retired_){Diagnostic diagnostic;if(!retireBeforeBankDestruction(diagnostic))std::terminate();}
    }

    // Call after the real source/trigger/query reservation, arena registration
    // and Bank::seal. The aggregate owns the adopted original rigs/COLs. The
    // existing driver registers its actual persistent arena fields once, before
    // sealing, including the trigger adapter's per-racer history.
    bool construct(const ra_cold_roster50LA::Arena& arena,
        std::shared_ptr<const ra_cold_roster50LA::FreshProbe> constructorProbe,
        const ra_source_camera50LA::FreshRecoveryArenaOwners& recovery,
        std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context,
        const std::array<std::vector<std::uint8_t>,6>& crowdImages,Diagnostic& d,
        std::shared_ptr<ra_cold_roster50LA::RuntimeOwners> owners={},
        g::OriginalRaceConstructor50LA* consumedRace=nullptr,
        original_pause_cold50LA::Owner* pause=nullptr,bool canonicalTournamentGrid=false)noexcept {
        return constructImpl(arena,std::move(constructorProbe),recovery,std::move(context),
            crowdImages,d,std::move(owners),consumedRace,pause,canonicalTournamentGrid,nullptr);
    }
private:
    // Distinct private native-constructor composition. The typed arena owns
    // this Bank; no legacy execution arena or prepared query is adopted.
    bool constructConstructorOnly(const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>& fresh,
        std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context,Diagnostic& d,
        std::shared_ptr<ra_cold_roster50LA::RuntimeOwners> owners={})noexcept {
        d={};
        if(attempted_||retired_||!fresh||!fresh->currentNative()||&fresh->bank_!=&bank_||
           !aggregate_||!generation_)return fail(d,"distinct constructor session identity refused");
        attempted_=true;
        try {
            const auto prefix=aggregate_->completedPrefixes();
            if(!prefix.sources||!prefix.staticCandidates||!prefix.particleAuthoring||
               !prefix.runtimeQueryWorldOwners||prefix.staticWorld||prefix.trackParticleManager)
                return fail(d,"source constructor world prerequisites differ");
            d.operation="actual constructor lighting/world/particles";
            if(!prefix.lighting&&!aggregate_->prepareLighting(static_cast<std::uint32_t>(bank_.count()),d.aggregate))return false;
            const a::OriginalStandardMaterialPath50LA path{&application_.materialDirectory36B970()};
            if(!aggregate_->prepareStaticWorld(application_.standardMaterialDomain(),std::move(context),path,d.aggregate)||
               !aggregate_->constructRuntimeTrackParticleManager(application_,generation_,d.aggregate)||
               !aggregate_->constructStaticModelPrefixes(application_,d.aggregate)||
               !aggregate_->completeStaticModelResourcesAndPlacement(d.aggregate)||
               !aggregate_->registerStaticModelObserversAndParents(d.aggregate))return false;
            d.operation="source constructor retained roster";
            roster_=ra_cold_roster50LA::Operation::bindConstructorOnly(aggregate_,application_,fresh,d.roster,std::move(owners));
            if(!roster_)return false;
            for(std::size_t slot=0;slot<bank_.count();++slot)
                if(!roster_->constructConstructorSlot(bank_,slot,d.roster))return false;
            d.operation="actual constructor model resources";
            if(!roster_->completeConstructorModelResources(d.roster))return false;
            // Native producer admission only: no grid/fresh membership, image
            // collector, complete Racer2146, source Race return or activation.
            return true;
        }catch(...){return fail(d,"constructor composition stopped; partial owners retained");}
    }
    bool constructConstructorCamera(const ra_source_camera50LA::CameraRuntimeOwners& runtime,
        const std::vector<std::uint8_t>& cam,original_camera_session50KX::DisplayInputs& display,
        const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>& fresh,Diagnostic& d)noexcept {
        d={};d.operation="same constructor-only Session";
        if(!fresh||!roster_||retired_||cameraAttempted_||&runtime.bank!=&bank_||
           !roster_->constructorSlotsCurrent(*fresh)||!roster_->constructorResourcesComplete_)
            return fail(d,"constructor-only camera owner differs");
        cameraAttempted_=true;
        return aggregate_->constructRetainedRuntimeSourceCamera(application_,runtime,cam,display,fresh,d.aggregate);
    }
    // Only the actual synchronous cold Owners caller can supply this existing
    // session identity borrow. This is not a public arbitrary-ID loader.
    bool constructHumanTournamentCold(const ra_cold_roster50LA::Arena& arena,
        std::shared_ptr<const ra_cold_roster50LA::FreshProbe> constructorProbe,
        const ra_source_camera50LA::FreshRecoveryArenaOwners& recovery,
        std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context,
        const std::array<std::vector<std::uint8_t>,6>& crowdImages,Diagnostic& d,
        std::shared_ptr<ra_cold_roster50LA::RuntimeOwners> owners,
        g::OriginalRaceConstructor50LA& consumedRace,original_pause_cold50LA::Owner& pause,
        const std::array<std::uint32_t,8>& actualRacer60)noexcept {
        d={};
        if(arena.humanCount!=1u||bank_.count()!=8u)
            return fail(d,"human Results cold entry requires the actual one-human/eight-slot roster");
        return constructImpl(arena,std::move(constructorProbe),recovery,std::move(context),
            crowdImages,d,std::move(owners),&consumedRace,&pause,true,&actualRacer60);
    }
    bool constructImpl(const ra_cold_roster50LA::Arena& arena,
        std::shared_ptr<const ra_cold_roster50LA::FreshProbe> constructorProbe,
        const ra_source_camera50LA::FreshRecoveryArenaOwners& recovery,
        std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context,
        const std::array<std::vector<std::uint8_t>,6>& crowdImages,Diagnostic& d,
        std::shared_ptr<ra_cold_roster50LA::RuntimeOwners> owners,
        g::OriginalRaceConstructor50LA* consumedRace,original_pause_cold50LA::Owner* pause,
        bool canonicalTournamentGrid,const std::array<std::uint32_t,8>* actualHumanRacer60)noexcept {
        d={};
        if(attempted_||retired_||!aggregate_||!generation_||(!bank_.count()||bank_.count()>8u)||
           (pause&&!consumedRace))
            return fail(d,"RA construction requires the same fresh sealed active-roster session");
        attempted_=true;
        try {
            const auto prefix=aggregate_->completedPrefixes();
            if(!prefix.sources||!prefix.staticCandidates||!prefix.particleAuthoring||
               !prefix.runtimeQueryWorldOwners||prefix.staticWorld||prefix.trackParticleManager)
                return fail(d,"RA registered source/query prerequisites are incomplete or construction already began");
            d.operation="actual roster lighting";
            if(!prefix.lighting&&!aggregate_->prepareLighting(static_cast<std::uint32_t>(bank_.count()),d.aggregate))return false;
            d.operation="same Application world";
            const a::OriginalStandardMaterialPath50LA path{&application_.materialDirectory36B970()};
            if(!aggregate_->prepareStaticWorld(application_.standardMaterialDomain(),std::move(context),path,d.aggregate))return false;
            d.operation="actual scene particle manager";
            if(!aggregate_->constructRuntimeTrackParticleManager(application_,generation_,d.aggregate))return false;
            d.operation="static model constructors";
            if(!aggregate_->constructStaticModelPrefixes(application_,d.aggregate)||
               !aggregate_->completeStaticModelResourcesAndPlacement(d.aggregate)||
               !aggregate_->registerStaticModelObserversAndParents(d.aggregate))return false;
            d.operation="retained original roster binding";
            roster_=ra_cold_roster50LA::Operation::bind(aggregate_,application_,bank_,arena,
                std::move(constructorProbe),d.roster,std::move(owners));
            if(!roster_)return false;
            d.operation="active original racer constructors";
            for(std::size_t slot=0;slot<bank_.count();++slot)
                if(!roster_->constructSlot(bank_,slot,d.roster))return false;
            // Every constructor precedes every grid; every grid precedes every
            // fresh membership. These methods own their actual Bank visits.
            d.operation="original grid and terrain placement";
            for(std::size_t slot=0;slot<bank_.count();++slot){
                gridAttempted_[slot]=true;
                if(!aggregate_->placeRetainedRuntimeGridSlot(bank_,slot,d.aggregate,canonicalTournamentGrid))return false;
            }
            d.operation="actual visibility and recovery memberships";
            for(std::size_t slot=0;slot<bank_.count();++slot){
                freshAttempted_[slot]=true;
                if(!aggregate_->attachRetainedRuntimeFreshSlot(bank_,slot,recovery,d.aggregate))return false;
            }
            d.operation="retained racer material resources";
            if(!aggregate_->prepareRetainedRuntimeModelResources(bank_,d.aggregate))return false;
            // HOST staging adaptation: source Pause100 then Results104 acquire
            // before the one combined image/collector plan; no second collector.
            if(pause){
                d.operation="same counted race Pause100 then Results104 loading";
                const bool loaded=actualHumanRacer60?
                    application_.loadHumanTournamentPauseResultsBeforeCollector50LA(
                        *consumedRace,*pause,roster_,*actualHumanRacer60,d.results):
                    application_.loadConsumedPauseResultsBeforeCollector50LA(
                        *consumedRace,*pause,roster_,d.results);
                if(!loaded)
                    return fail(d,"same counted race Pause/Results load rejected; actual partial owners retained");
            }
            d.operation="single combined world image population";
            if(!aggregate_->prepareStaticWorldImages(crowdImages,d.aggregate)||
               !aggregate_->prepareStaticWorldDrawPlan(d.aggregate))return false;
            // This is a completed cold join, not a running-race receipt. The
            // existing frame/manager/start-gate path still owns activation.
            return true;
        }catch(...){return fail(d,"RA native composition failed; actual partial owners retained for retirement");}
    }
public:
    bool constructCamera(const ra_source_camera50LA::CameraRuntimeOwners& runtime,
        const std::vector<std::uint8_t>& originalCam,
        original_camera_session50KX::DisplayInputs& display,Diagnostic& d)noexcept {
        d={};d.operation="same roster source camera";
        if(!roster_||retired_||cameraAttempted_||&runtime.bank!=&bank_)
            return fail(d,"RA camera requires the actual retained cold roster and Bank");
        cameraAttempted_=true;
        return aggregate_->constructRetainedRuntimeSourceCamera(application_,runtime,originalCam,display,d.aggregate);
    }
    rr::game::OriginalWreckComponent50LA* introController174A30(Diagnostic& d)noexcept {
        d={};d.operation="actual RA intro controller48";
        return cameraAttempted_&&!retired_&&aggregate_?aggregate_->introController174A30(d.aggregate):nullptr;
    }
    bool constructIntroAudio177750(std::shared_ptr<const rr::assets::AudioBank50HC> bank,
        std::size_t firstVoice,std::size_t voiceCount,Diagnostic& d)noexcept {
        d={};d.operation="actual RA intro audio80";
        return cameraAttempted_&&!retired_&&aggregate_&&
            aggregate_->constructIntroAudio177750(std::move(bank),firstVoice,voiceCount,d.aggregate);
    }
    std::shared_ptr<original_camera_session50KX::Session> prepareAndBorrowCamera(Diagnostic& d)noexcept {
        d={};d.operation="actual source camera preparation";
        if(!cameraAttempted_||retired_||!aggregate_)return {};
        if(aggregate_->prepareRuntimeSourceCamera(generation_,d.aggregate).status!=
            original_camera_session50KX::Status::complete)return {};
        return aggregate_->borrowRuntimeSourceCamera(aggregate_,d.aggregate);
    }
    // Call at the existing renderer publication point, before converting the
    // view matrix. Fog preparation rebuilds the same camera; lighting and
    // diffuse publication must therefore follow it, never precede it.
    bool prepareRenderPublication(ra_static_lighting50LA::StaticDiffuseDiagnostic& diffuse,
        Diagnostic& d)noexcept {
        d={};d.operation="actual camera world publication";
        if(!cameraAttempted_||retired_||!aggregate_)return fail(d,"actual RA camera is unavailable");
        return aggregate_->prepareNativeStaticFogObservations(d.aggregate)&&
            aggregate_->prepareNativeStaticLighting(d.aggregate)&&
            aggregate_->prepareNativeStaticDiffuseStream(diffuse);
    }
    // Root cancels/drains pending real display work before retirement. No fake
    // countdown completion, no reset of partial physics, and no guessed query W.
    bool retireBeforeBankDestruction(Diagnostic& d)noexcept {
        d={};d.operation="native session retirement";
        if(retired_)return true;
        if(!aggregate_)return fail(d,"RA aggregate was not retained");
        if(cameraAttempted_&&aggregate_->closeRuntimeSourceCamera(d.aggregate).status!=
            original_camera_session50KX::Status::complete)return false;
        for(std::size_t slot=bank_.count();slot-->0;){
            if(freshAttempted_[slot]&&!aggregate_->retireRuntimeFreshSlot(bank_,slot,d.aggregate))return false;
            freshAttempted_[slot]=false;
            if(gridAttempted_[slot]&&!aggregate_->retireRuntimeGridSlot(bank_,slot,d.aggregate))return false;
            gridAttempted_[slot]=false;
        }
        if(roster_&&!roster_->close())return fail(d,"RA original model retirement remains incomplete");
        roster_.reset();retired_=true;return true;
    }
    // Aggregate::close is deliberately NOT called here: the actual Bank must
    // first retire every saved pointer and its lifetime pin, either through
    // explicit terminal retireNativeBacking() or actual Bank destruction.
};
} // namespace ra_native_session50LA
