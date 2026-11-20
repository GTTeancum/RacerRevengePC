#pragma once
#include "ra_cold_roster_arena50la.h"
#include "ra_cold_roster_resources50la.h"
#include "ra_source_camera_ownership50la.h"
#include "original_ui_main_entry50kg.h"
#include "native_racer_bank50hn_fixture.h"
#include "terrain_probe_authoring50fq_fixture.h"
#include "undertrack_owners50ew_fixture.h"
#include "live_terrain_probe50fr_fixture.h"
#include "../../src/game/original_shared_rng50iu.h"
#include <filesystem>
#include <memory>
#include <thread>
namespace interactive_cold50DU { struct OwnersContext; }
namespace ra_native_session50LA { class Join; }
namespace original_camera_session50KX { class Session; }
namespace ra_cold_roster50LA {
// A distinct native execution allocation, never a warm arena cleared in place.
// Only reached source owners may turn any numeric backing into guest knowledge.
// The legacy Fixture constructor and World::run remain byte-for-byte unchanged.
class FreshExecutionArena final {
    friend struct ::interactive_cold50DU::OwnersContext;
    friend class Operation;
    friend class ::ra_native_session50LA::Join;
    friend class ::original_camera_session50KX::Session;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    const std::thread::id thread_=std::this_thread::get_id();
    const std::shared_ptr<const Resources> resources_;
    runtime_startup50fl::Fixture live_;
    std::shared_ptr<g::CoupledNoiseContext49M> sharedNoise_;
    const g::AuthoredSelectionContext49N selection_;
    const std::uint32_t launchKind11258_,humanCount_;
    // Same application collision namespace, never a second normal-mode owner.
    g::CollisionNormalModeOwner50JA& normalMode_;
    g::BoostParameters50FK boost_{};
    float coolingCF8_=0;
    g::SpringGeneratedParameters48K mainSpring_{},secondarySpring_{};
    g::SpringResponseParameters48L mainResponse_{},secondaryResponse_{};
    g::AiRuntimeState50IV ai_{};
    native_ai50IV::Session aiSession_{};
    g::TrackNavigationProgress50EU navigation_{};
    lap_drive50GJ::Continuity lap_{};
    g::BasePoseHistoryState50Y history_{};
    g::PodSceneRebaseModel50FB rebaseModels_[5]{};
    unsigned rebaseCount_=0;
    g::PodSceneRebaseWorkspace50FB rebaseWorkspace_{};
    g::ModelBoundsResult49O rebaseBounds_[5]{};
    g::PodSceneRebaseDiagnostic50FB rebaseDiagnostic_{};
    a::EffectNames50BL effectNames_{};
    g::EffectOwner50BE effects_[14]{};
    std::size_t effectCount_=0;
    g::ResetEffectBindings50BK resetBindings_{};
    g::ResetEffectState50BK resetState_{};
    g::ResetEffectWorkspace50BK resetWorkspace_{};
    native_original_ai_recovery::Models recoveryModels_{};
    terrain_probe_authoring50fq_fixture::SourceBindings terrainBindings_{};
    a::PodFlameOwners50W flame_{};
    g::AuthoredBinding49N authored_{};
    float authoredSteering10A4_=0;
    g::Type2StaticQuery49X main_{},secondary_{};
    g::CollisionFilter48J mainFilter_{},secondaryFilter_{};
    terrain_probe_authoring50fq_fixture::Owner terrain_{};
    live_terrain_probe50fr::Owner terrainLive_{};
    undertrack_owners50ew_fixture::Owners recovery_{};
    g::Type3QueryInput48P movement_{};
    g::SpringVector4_48N endpoints_[2]{};
    std::uint32_t endpointMetadata_[2]{};
    g::Type3QuerySource48P movementSource_{};
    std::weak_ptr<ra_source_camera50LA::OwnershipBridge> borrowedBridge_;
    std::weak_ptr<Operation> operation_;
    const Operation* operationIdentity_=nullptr;
    bool lanesRegistered_=false,lanesRegistrationComplete_=false,sealed_=false,bankRetired_=false,nativeClosed_=false;
    // Last declared: snapshots/pins are retired BEFORE referred-to backing dies.
    native_racer50HN::Bank bank_;
    FreshExecutionArena(const std::filesystem::path& root,std::shared_ptr<const Resources> resources,
        const g::AuthoredSelectionContext49N& selection,std::uint32_t launchKind,
        g::CollisionNormalModeOwner50JA& normalMode,const original_main_entry50KG::Application& application,
        const terrain_probe_authoring50fq_fixture::SourceBindings& actualNamespace,
        const undertrack_owners50ew_fixture::SourceGlobals& recoveryNamespace)
        :resources_(std::move(resources)),live_(runtime_startup50fl::Fixture::SourceFreshStorageOnly{},root),
         sharedNoise_(application.state,&application.state->sharedNoise308030),
         selection_(selection),launchKind11258_(launchKind),humanCount_(resources_->humanCount_),normalMode_(normalMode) {
        live_.state.borrowedNoise50KG=sharedNoise_.get(); // Native alias to actual Application, no seed/copy.
        terrainBindings_=actualNamespace;recovery_.globals=recoveryNamespace;
        movement_.filter.identity=recoveryNamespace.identity[1]; // Native namespace alias only.
    }
    static std::shared_ptr<FreshExecutionArena> admitNative(const std::filesystem::path& root,
        std::shared_ptr<const Resources> resources,const g::AuthoredSelectionContext49N& selection,
        std::uint32_t actualLaunchKind,g::CollisionNormalModeOwner50JA& actualNormalMode,
        const original_main_entry50KG::Application& application,
        const terrain_probe_authoring50fq_fixture::SourceBindings& actualNamespace,
        const undertrack_owners50ew_fixture::SourceGlobals& recoveryNamespace)noexcept try {
        if(!application.state||!application.state->known||!resources||!resources->racerCount_||resources->racerCount_>8||resources->humanCount_>1)return {};
        const auto bit=[](std::uint32_t value)noexcept{return value&&!(value&(value-1));};
        const std::uint32_t categories[]={actualNamespace.physicsBodyCategory,actualNamespace.staticObjectCategory,
            actualNamespace.floorCategory,actualNamespace.tubeCategory};
        for(std::size_t i=0;i<4;++i){if(!bit(categories[i]))return {};
            for(std::size_t j=0;j<i;++j)if(categories[i]==categories[j])return {};}
        if(!actualNamespace.identity||actualNamespace.collisionTypeTag==actualNamespace.groupTypeTag||
           recoveryNamespace.terrainIdentity348!=actualNamespace.identity||
           recoveryNamespace.podCategory3E0718!=actualNamespace.physicsBodyCategory||
           recoveryNamespace.collisionTypeTag!=actualNamespace.collisionTypeTag||
           !bit(recoveryNamespace.movementCategory3E0720)||
           recoveryNamespace.movementCategory3E0720==recoveryNamespace.podCategory3E0718||
           !recoveryNamespace.identity[0]||!recoveryNamespace.identity[1]||
           recoveryNamespace.identity[0]==recoveryNamespace.identity[1]||
           recoveryNamespace.identity[0]==actualNamespace.identity||recoveryNamespace.identity[1]==actualNamespace.identity)return {};
        auto out=std::shared_ptr<FreshExecutionArena>(new FreshExecutionArena(root,std::move(resources),selection,actualLaunchKind,
            actualNormalMode,application,actualNamespace,recoveryNamespace));
        // Native fixed backing only, no guest filter/count/identity production.
        // True query reservation must establish schema before registration.
        out->mainFilter_.identityLists[0].identities.resize(4);
        out->secondaryFilter_.identityLists[0].identities.resize(3);
        return out;
    }catch(...){return {};}
    // Common rewrite (area C): the RA aggregate composition createConstructorArenaNative
    // (aggregate prepareConstructorRosterStorage / bindConstructorQueryOwners /
    // registerConstructorQueryWorldOwners) is retired with the RA world. PORT STAND-IN: no
    // common caller admits this fresh constructor arena yet; the common roster
    // (common_roster50CR::loadAll) loads every slot. The arena type stays for the bridge's
    // fresh constructor lanes (2123C8/212278/2127C8) and the Session constructor seam.
    Arena arena()noexcept {
        return {live_,{boost_,coolingCF8_,mainSpring_,mainResponse_,secondarySpring_,secondaryResponse_},
            selection_,ai_,aiSession_,navigation_,lap_,history_,rebaseModels_,rebaseCount_,rebaseWorkspace_,
            rebaseBounds_,rebaseDiagnostic_,effectNames_,effects_,effectCount_,resetBindings_,resetState_,resetWorkspace_,
            recoveryModels_,terrainBindings_,flame_,launchKind11258_,humanCount_,&authored_,&authoredSteering10A4_};
    }
    bool currentNative()const noexcept {
        return thread_==std::this_thread::get_id()&&!nativeClosed_&&!bankRetired_&&resources_&&sharedNoise_&&
            live_.state.borrowedNoise50KG==sharedNoise_.get()&&
            lanesRegistered_&&lanesRegistrationComplete_&&sealed_&&bank_.count()==resources_->racerCount_;
    }
    bool matches(const native_racer50HN::Bank& bank,const g::VehicleFrameState49W& frame,
        const g::VehicleFrameBindings49W& bindings,const g::AiRuntimeState50IV& ai,
        const g::BasePoseHistoryState50Y& history,
        const std::array<std::unique_ptr<native_racer50HW::Rig>,8>& rigs)const noexcept {
        return currentNative()&&&bank==&bank_&&&frame==&live_.state&&&bindings==&live_.bindings&&
            &ai==&ai_&&&history==&history_&&&rigs==&resources_->rigs_;
    }
    bool registerNativeLanes()noexcept try {
        if(thread_!=std::this_thread::get_id()||nativeClosed_||bankRetired_||lanesRegistered_||sealed_||operationIdentity_||
           terrain_.prepared||terrain_.receipt.archiveBytes||terrainLive_.initialized||recovery_.sourcePrepared||
           mainFilter_.identityLists[0].identities.size()!=4||secondaryFilter_.identityLists[0].identities.size()!=3||
           recovery_.podFilter.identityLists[0].identities.empty())return false;
        // Set before first enrollment: a partial allocation failure is terminal,
        // never retried by replaying earlier overlapping Bank spans.
        lanesRegistered_=true;
        g::registerRacerFrameExceptSharedNoise50IU(bank_,live_.state);bank_.owner(live_.configurationEntryNonzero);bank_.owner(live_.actionCodes);
        bank_.owner(live_.parameters);bank_.owner(live_.bindings);bank_.owner(live_.secondaryInputs);
        bank_.owner(live_.handRecords);bank_.owner(live_.selectors);bank_.owner(live_.frame);bank_.owner(live_.diagnostic);
        bank_.owner(boost_);bank_.owner(coolingCF8_);bank_.owner(mainSpring_);bank_.owner(mainResponse_);
        bank_.owner(secondarySpring_);bank_.owner(secondaryResponse_);bank_.owner(ai_);bank_.owner(navigation_);
        bank_.owner(lap_);bank_.owner(history_);bank_.owner(rebaseModels_);bank_.owner(rebaseCount_);
        bank_.owner(effectNames_);bank_.owner(effects_);bank_.owner(effectCount_);bank_.owner(resetBindings_);
        bank_.owner(recoveryModels_);bank_.owner(flame_);bank_.owner(authored_);bank_.owner(authoredSteering10A4_);
        bank_.owner(main_);bank_.owner(secondary_);bank_.owner(mainFilter_.identity);bank_.owner(secondaryFilter_.identity);
        bank_.owner(mainFilter_.flagsF8);bank_.owner(secondaryFilter_.flagsF8);
        bank_.contents(mainFilter_.identityLists[0].identities);bank_.contents(secondaryFilter_.identityLists[0].identities);
        recovery_.registerMutableOwners(bank_);
        bank_.owner(recovery_.podFilter.identity);bank_.owner(recovery_.globals.identity);
        bank_.owner(recovery_.globals.terrainIdentity348);
        bank_.owner(terrain_.initial);bank_.owner(terrain_.follow);bank_.owner(terrain_.lowerEndpoint140);
        bank_.owner(terrain_.lift1F0);bank_.owner(terrain_.receipt);bank_.owner(terrain_.prepared);
        bank_.owner(terrain_.spring1F4);bank_.owner(terrain_.damping1F8);bank_.owner(terrain_.gravity1FC);bank_.owner(terrain_.sideForce200);
        bank_.owner(terrain_.metadata.normalMode54);bank_.owner(terrain_.metadata.normalMode54Bound);
        bank_.owner(terrain_.metadata.terrainCallbackEnabled);
        bank_.owner(terrain_.metadata.filter.identity);bank_.owner(terrain_.metadata.filter.flagsF8);bank_.owner(terrainLive_);
        bank_.owner(movement_.filter.identity);bank_.owner(movement_.filter.flagsF8);bank_.owner(movement_.initial);bank_.owner(movement_.initialQueryCurrent);
        bank_.owner(movement_.copied40);bank_.owner(movement_.copied50);bank_.owner(endpoints_);bank_.owner(endpointMetadata_);
        lanesRegistrationComplete_=true;return true;
    }catch(...){return false;}
    bool sealNative()noexcept try {
        if(thread_!=std::this_thread::get_id()||nativeClosed_||bankRetired_||!lanesRegistered_||!lanesRegistrationComplete_||sealed_||operationIdentity_)return false;
        // The private composition must first enroll the exact aggregate world
        // in THIS Bank. Operation/Session independently verify its private pin.
        bank_.seal(resources_->racerCount_);sealed_=true;return true;
    }catch(...){return false;}
    bool bindOperationNative(const std::shared_ptr<Operation>& operation)noexcept {
        if(!currentNative()||!operation||operationIdentity_)return false;
        operation_=operation;operationIdentity_=operation.get();return true;
    }
    bool releaseOperationNative(const Operation& operation)noexcept {
        if(thread_!=std::this_thread::get_id()||operationIdentity_!=&operation)return false;
        operation_.reset();operationIdentity_=nullptr;return true;
    }
    bool closeNative()noexcept {
        if(thread_!=std::this_thread::get_id()||operationIdentity_)return false;
        if(nativeClosed_)return true;
        // Drop eligible Bank pins first: Bridge::canClose requires their
        // withdrawal. False below retains this arena for aggregate close and
        // retry; a retired Bank alone never authorizes freeing its backing.
        if(!bankRetired_){
            if(!bank_.retireNativeBacking())return false;
            bankRetired_=true; // One-way withdrawal, including partial close.
        }
        const auto bridge=borrowedBridge_.lock();
        if(bridge&&!bridge->closed_)return false;
        nativeClosed_=true;return true;
    }
public:
    ~FreshExecutionArena()noexcept {
        // Existing Bank pin checks camera/world borrowers before any pointer
        // backing dies; refused dependent retirement is never silently dropped.
        if(!closeNative())std::terminate();
    }
    FreshExecutionArena(const FreshExecutionArena&)=delete;
    FreshExecutionArena& operator=(const FreshExecutionArena&)=delete;
};
} // namespace ra_cold_roster50LA
