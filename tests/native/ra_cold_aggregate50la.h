#pragma once
#include "ra_cold_aggregate_prefix50la.h"
#include "../../src/assets/original_particle_source_config50la.h"
#include "../../src/assets/original_source_sky_file50la.h"
#include "../../src/game/original_world_particle_invocation50la.h"
#include "../../src/game/original_world_fog_invocation50la.h"
#include "../../src/game/original_particle_fog_placement50la.h"
#include "../../src/game/original_particle_fog_velocity50la.h"
#include "../../src/game/original_particle_fog_lifetime50la.h"
#include "../../src/game/original_particle_fog_frame50la.h"
#include "../../platform/linux/native/original_ui_garage_fog_color_lease50kg.h"
#include "../../platform/linux/native/original_world_source_config50la.h"
namespace original_results_roster50LA { class Read; }
namespace ra_native_session50LA { class Join; }
namespace rr::platform {class OriginalUiLightManagerOwner50KG;struct OriginalUiMainApplication50KG;}
namespace rr::game { class OriginalRacePauseWorldAuthority50LA; }
#include "ra_static_registry50la.h"
#include "ra_cold_navigation_lease50la.h"
#include "original_racer_material_sources50la_fixture.h"
#include "ra_source_camera_ownership50la.h"
#include "ra_pod_scene50la.h"
#include "../../src/game/original_racer_world_borrow50la.h"
#include "ra_static_world50la.h"
#include "ra_static_lighting50la.h"
#include "ra_static_fog_binding50la.h"
#include "ra_trigger_authoring50la.h"
#include "original_track_sky50kx_fixture.h"
#include "sky50gw_fixture.h"
#include "ra_crowd_image50la.h"
#include "runtime_asset_paths50lb.h"
#include "ra_atm_authoring50la.h"
#include "ra_audio_authoring50la.h"
#include "ra_particle_authoring50la.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include <exception>
#include <algorithm>
#include <memory>
#include <optional>
#include <thread>
#include <string>
#include <utility>

namespace rr::assets {class OriginalTaCutAuthoringCandidate;}
namespace original_main_entry50KG { struct Application; class RaceDisplayLifetime50LA; }
namespace rr::platform {class OriginalUiRiderProfileOwner50KG;}

namespace ra_cold_aggregate50LA {
namespace a=rr::assets;namespace g=rr::game;
namespace light=g::original_gc_gameplay_lighting_candidate;
// These are accounting stages, not original source constructor/scene receipts.
// Dependency order is explicit; no unproved full retail loader order is implied.
enum class Stage : std::uint8_t {none,sources,staticCandidates,staticRegistry,
    authoring,lighting,detachedParticles,closed,staticWorld,staticWorldImages,staticWorldDrawPlan,runtimeQueryStorage,runtimeQueryOwners,runtimeQueryWorldOwners,trackParticleManager,sourceCamera};
struct MissingLiveJoins {
    // Permanent scope descriptions, never writable readiness/activation flags.
    static constexpr const char* collision=
        "additive trigger/history kernel, dynamic/query owners and real live registry membership";
    static constexpr const char* scenery=
        "actual model/SoulTree/material/GPU owners, sky images and source camera";
    static constexpr const char* lighting=
        "actual racer/component visibility and current camera/draw leases";
    static constexpr const char* animation=
        "canonical model material/map/service ownership and verified IPUM images";
    static constexpr const char* audio=
        "actual ready PCM bank, groups/mixer, D44 sounds and ambience/intro owners";
    static constexpr const char* particles=
        "actual scene particle manager, live matched SoulTrees, parent and runtime/render ownership";
};
struct Diagnostic {
    std::size_t rosterSlot=0,rosterRole=0;
    original_racer_material_sources50LA::Diagnostic rosterSources{};
    a::OriginalStandardArchiveProvider50LA::Diagnostic rosterMount{};
    Stage stage=Stage::none;const char* failure=nullptr;
    ra_cold_sources50LA::Diagnostic source{};
    ra_static_candidates50LA::Owner::Diagnostic candidates{};
    ra_static_registry50LA::ColdStage::Diagnostic registry{};
    ra_source_camera50LA::Diagnostic runtimeQueries{};
    ra_trigger_authoring50LA::Owner::Diagnostic triggers{};
    ra_atm_authoring50LA::Owner::Diagnostic atm{};
    ra_audio_authoring50LA::Owner::Diagnostic audio{};
    ra_particle_authoring50LA::Owner::Diagnostic particles{};
    light::Diagnostic lighting{};
    g::OriginalTrackParticleColdDiagnostic50LA trackParticles{};
    ra_static_world50LA::Owner::Diagnostic world{};
    ra_static_model_cold50LA::Owner::Diagnostic staticModels{};
    std::string sky;
};
class Owner final : private ra_pod_scene50LA::Owner {
    std::shared_ptr<const g::OriginalRacerWorldBorrow50LA> mintConstructorRacerWorldBorrow(
        const std::shared_ptr<Owner>& self)noexcept;
    bool constructRuntimeSourceCamera(const original_main_entry50KG::Application&,
        const ra_source_camera50LA::CameraCallerOwners&,
        const std::vector<std::uint8_t>&,
        original_camera_session50KX::DisplayInputs&,
        const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,
        const std::shared_ptr<const g::OriginalRacerWorldBorrow50LA>&,Diagnostic&)noexcept;
    bool constructRuntimeSourceCameraImpl(const original_main_entry50KG::Application&,
        const ra_source_camera50LA::CameraCallerOwners&,
        const std::vector<std::uint8_t>&,
        original_camera_session50KX::DisplayInputs&,
        const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,
        const std::shared_ptr<const g::OriginalRacerWorldBorrow50LA>&,Diagnostic&)noexcept;
    bool constructRetainedRuntimeSourceCamera(const original_main_entry50KG::Application&,
        const ra_source_camera50LA::CameraRuntimeOwners&,
        const std::vector<std::uint8_t>&,
        original_camera_session50KX::DisplayInputs&,
        const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,Diagnostic&)noexcept;

    friend class ra_cold_roster50LA::FreshExecutionArena;
    friend class ::ra_source_camera50LA::OwnershipBridge; //Exact incoming World zone borrow; private base conversion.
    friend class ::ra_native_session50LA::Join;
    bool registerConstructorQueryWorldOwners(
        const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,Diagnostic&)noexcept;
    bool constructorMaterialNamespaceCurrent(const a::OriginalStandardMaterialDomain50LA& domain)const noexcept {
        const auto& provider=domain.archiveProvider3E1A70();
        return prefix_.staticWorld&&world_&&world_->domain_.get()==&domain&&provider&&
            provider->sealed()&&provider->revision()==world_->providerRevision_&&!worldImagesAttempted_;
    }
    bool prepareConstructorRosterStorage(std::shared_ptr<g::OriginalApplicationEntities50LA>,
        const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,Diagnostic&)noexcept;
    bool bindConstructorQueryOwners(const std::shared_ptr<ra_cold_roster50LA::FreshExecutionArena>&,
        Diagnostic&)noexcept;


    // Stage B: exclusive fresh shared allocation factory, deliberately private
    // and UNCALLED. No loader/default route selects this incomplete mode.
    bool prepareSharedRosterStorage(
        std::shared_ptr<g::OriginalApplicationEntities50LA>,
        std::shared_ptr<const ra_cold_roster50LA::Resources>,
        undertrack_owners50ew_fixture::Owners*,
        const undertrack_owners50ew_fixture::SourceGlobals&,
        const terrain_probe_authoring50fq_fixture::Owner&,const g::Type3QueryInput48P&,
        const std::vector<ra_source_camera50LA::WreckReservation>&,Diagnostic&)noexcept;

    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    //Private same-World raw-borrow validation, never a public readiness token.
    bool nativePortalTailCurrent148D00(const g::OriginalWreckComponent50LA* parent,
        const g::OriginalWreckComponent50LA* portal)const noexcept {
        if(parent!=&container4BC_||!portal||!canCloseTopology())return false;
        for(const auto& owned:nativePortals_)
            if(owned&&owned->attached && &owned->entity==portal)return true;
        return false;
    }
    friend struct ::interactive_setup50DU::Context; //Private concrete projection/water producer lifetime borrows only.
    native_world_water50LA::Domain::Lease bindNativeWaterContext(const ::interactive_setup50DU::Context&)noexcept;
    bool nativeWaterDomainMatches(const native_world_water50LA::Domain&)const noexcept;
    friend class ::original_results_roster50LA::Read;
    friend class rr::game::OriginalRaceConstructor50LA;
    friend class ::ra_pod_scene50LA::Owner;
    bool resetFreshWorld270550(std::uint32_t&)noexcept;
    // Read-only row identity validation; deliberately not the mutation/count
    // preflight, which must reject live constructor borrowers. No issuer yet.
    bool readSourceWorldRows1AE0C8(const original_main_entry50KG::Application&,
        std::uint64_t,const void*&,const void*&)const noexcept;
    bool readSourceWorldFields2107FC(const original_main_entry50KG::Application&,
        std::uint64_t,const void*&,const void*&,const rr::game::OriginalRaceConstructor50LA*&)const noexcept;

// Insert inside ra_cold_aggregate50LA::Owner private area beside resetFreshWorld270550.
// The actual Race loading continuation alone arms invoked1AA040, after proving
// race.world74_.get()==this, same Application/Entity domain/profile selected RA.
// No public setter, constructor-default invocation, or general reset grant.
const rr::game::OriginalRaceConstructor50LA* sourceLoadRace_=nullptr;
const original_main_entry50KG::Application* sourceLoadApplication_=nullptr;
enum class SourceLoadReset270550Phase:std::uint8_t {
    unreached,invoked1AA040,entered,returned,failed
};
SourceLoadReset270550Phase sourceLoadReset270550Phase_=SourceLoadReset270550Phase::unreached;
std::uint32_t sourceLoadReset270550PC_=0x1aa040;
const std::string* sourceParticleDirectory36B530(const g::OriginalApplicationEntities50LA*)const noexcept;
// Private ordered1B4DC0 custody; only the completed same-owner source predecessor may issue it.
std::size_t sourceParticleInvocationBorrowers_=0;
std::unique_ptr<a::OriginalParticleSourceConfig50LA> sourceParticleConfig_;
const g::OriginalWorldParticleInvocation50LA* sourceParticleConfigInvocation_=nullptr;
bool beginSourceParticleConfig1617A0(const g::OriginalWorldParticleInvocation50LA&,std::uint32_t&)noexcept;
bool abortSourceParticleConfigNative(std::uint32_t&)noexcept;
static bool releaseSourceParticleInvocationNative(const void*,const g::OriginalWorldParticleInvocation50LA&)noexcept;
enum class SourcePostLightPhase:std::uint8_t {unreached,fogEntered,fogReturned,particleEntered,bounded,particleReturned};
SourcePostLightPhase sourcePostLightPhase_=SourcePostLightPhase::unreached;
std::uint32_t sourcePostLightPC_=0x1aa158;
std::optional<std::uint32_t> sourceFogCallReturnV0_,sourceParticleStageReturnV0_;
std::optional<bool> sourceParticleSelectorNone_,sourceParticleVisibilityWasNull_;
bool sourceParticleStageLogAttempted_=false,sourceParticleStageLogError_=false;
std::size_t sourceParticleStageLogWritten_=0;
bool sourceParticleEnvironment35CWritten_=false,sourceParticlePredecessorEntered_=false;
bool continueSourceFogAndParticle1AA158(std::uint32_t&)noexcept;
bool sourceParticlePredecessorReturned_=false;
std::unique_ptr<g::OriginalTrackParticleCold50LA> sourceParticlePendingAllocation_;
std::optional<g::OriginalTrackParticleCold50LA*> sourceParticle4A0_;
std::optional<bool> sourceParticleAllocationNonNull_;
std::optional<std::int32_t> sourceParticleDepthBefore_,sourceParticleDepthAfter_;
g::OriginalTrackParticleCold50LA* sourceParticleReturnedSelf_=nullptr; //Historical address after native cancellation.
bool sourceParticleAllocationAttempted_=false,sourceParticleAllocationReturned_=false,
     sourceParticleConstructorReturned_=false,sourceParticlePublished4A0_=false,
     sourceParticleLoadEntered_=false,sourceParticleLoadReturned_=false,
     sourceParticle4A0NativeWithdrawn_=false,sourceParticleStageNativeClosed_=false;
std::uint32_t sourceParticleStagePC_=0x1b4e34;
bool continueSourceParticleManager1B4E34(std::uint32_t&)noexcept;
bool abortSourceParticleStageNative(std::uint32_t&)noexcept;
bool sourceParticleStageCountedNative(std::size_t&)const noexcept;
bool sourceVisibilityEnvironmentReturned_=false;
std::optional<std::uint32_t> sourceVisibilityEnvironmentReturnV0_;
bool constructSourceVisibilityEnvironment272FC0(std::uint32_t&)noexcept;
bool preflightSourceVisibilityEnvironmentNative()const noexcept;
enum class SourcePostParticleModelsPhase:std::uint8_t {unreached,entered,bounded,nativeClosed};
SourcePostParticleModelsPhase sourcePostParticleModelsPhase_=SourcePostParticleModelsPhase::unreached;
std::uint32_t sourcePostParticleModelsPC_=0x1aa194;
std::optional<std::uint32_t> sourcePostParticleLoadingMode30_,sourceEmptyInstancesReturnV0_;
std::uint8_t sourceGeometryDirectoryStoreMask_=0;
std::optional<std::int32_t> sourceEmptyInstancesCounterS1_,sourceEmptyInstancesCountRead_;
std::optional<const void*> sourceEmptyInstancesPointerRead_;
std::optional<std::uint32_t> sourceStandardZoneS0_,sourceStandardZoneByteOffsetS1_,sourceStandardZonePointerOffsetS2_;
std::optional<std::uint32_t> sourceStandardSlotS0_,sourceStandardSlotByteOffsetS1_,sourceStandardWrapperSlot_;
std::optional<std::uint32_t> sourceStandardTypeSlotS0_,sourceStandardTypeByteOffsetS1_,sourceStandardTypeZoneRead_,sourceStandardTypeCountRead_;
const void* sourceStandardTypeParentRead_=nullptr;
std::array<std::uint32_t,10> sourceStandardCompletedCallbacks_{},sourceStandardTypeScanned_{};
std::array<bool,10> sourceStandardParentReturned_{},sourceStandardTypeReturned_{},sourceStandardDiagnosticAttempted_{};
std::array<int,10> sourceStandardDiagnosticWritten_{};
std::uint16_t sourceStandardVisibilityMask_=0;
std::optional<std::uint32_t> sourceStandardWorldReturnV0_;
//Actual source1AA208/1B3AE0 effect proof; distinct from legacy native receipt.
bool sourceBuoyAttachNoEffect1B3AE0_=false;
const void* sourceStandardParentS3_=nullptr;
const void* sourceStandardWrapperA0_=nullptr;
bool sourceStandardZoneStored_=false,sourceStandardVisibilityStored_=false,sourceStandardCallbackEntered_=false;
bool continueSourcePostParticleModels1AA194(std::uint32_t&)noexcept;
bool continueSourceStandardModel27ABC0(std::uint32_t&)noexcept;
bool continueSourceStandardSuccess27AC64(ra_static_model_cold50LA::Owner::StandardWrapper&,std::uint32_t&)noexcept; //Conditional; caller remains unissued.
bool finishSourceStandardRenderable24076C(ra_static_model_cold50LA::Owner::StandardWrapper&,std::uint32_t&)noexcept;
bool continueSourceStandardSmoker27AE68(ra_static_model_cold50LA::Owner::StandardWrapper&,std::uint32_t&)noexcept;
bool continueSourceStandardAttachment27AE14(ra_static_model_cold50LA::Owner::StandardWrapper&,std::uint32_t&)noexcept; //Requires genuine live observer return.
bool releaseSourceAbsentModel26E9D4(ra_static_model_cold50LA::Owner::StandardWrapper&,std::uint32_t&)noexcept;
bool continueSourceStandardObserver28E8A0(ra_static_model_cold50LA::Owner::StandardWrapper&,std::uint32_t&)noexcept;
bool continueSourceStandardObserverSecondary280BF0(SourceModelObserverFrame&,std::uint32_t&)noexcept;
bool preflightSourceStandardObserversNative()const noexcept;
bool preflightSourceStandardObserverSecondaryNative(const SourceModelObserverFrame&)const noexcept;
bool withdrawSourceStandardObserverSecondaryNative(SourceModelObserverFrame&)noexcept;
bool retireSourceStandardObserversNative(std::uint32_t&)noexcept;
bool sourceStandardObserverReturnCurrent(const ra_static_model_cold50LA::Owner::StandardWrapper&)const noexcept;
bool preflightSourceStandardModelsNative(std::size_t&)const noexcept;
bool retireSourceStandardModelsNative(std::uint32_t&)noexcept;
bool preflightSourcePostParticleModelsNative()const noexcept;
bool abortSourcePostParticleModelsNative(std::uint32_t&)noexcept;
bool sourceWorldLoadInvocationCurrent()const noexcept; //Parent owns cpp exact token predicate.
bool resetColdLoadWorld270550(std::uint32_t&)noexcept;

    friend class rr::game::OriginalRacePauseWorldAuthority50LA;
    friend class rr::platform::MaterialTextureGl50EB;
    friend class rr::platform::VehicleWindowGl50DS;
    friend class ra_cold_roster50LA::Operation;
    // Private renderer borrow; no vector or publication capability escapes
    // the actual synchronous selected-surface preparation callback.
    template<class Visitor>
    bool visitNativeStaticDiffuseSurface(std::size_t surface,Visitor&& visitor)const noexcept {
        return !terminal_&&!closed_&&staticLighting_&&
            staticLighting_->visitDiffuseSurface(surface,std::forward<Visitor>(visitor));
    }
    template<class Visitor>
    bool visitNativeStaticRawPositionSurface(std::size_t surface,Visitor&& visitor)const noexcept {
        return !terminal_&&!closed_&&staticFog_&&
            staticFog_->visitRawPositionSurface(surface,std::forward<Visitor>(visitor));
    }
    // One actual decoded source owner. All child borrows die before it. No
    // external source/registry injection or second source decode is possible.
    std::shared_ptr<ra_cold_sources50LA::Owner> source_;
    //Windows/Linux replacement for the parser role published at World34 by
    //1B399C. It aliases this SAME authenticated decoded source; no parser,
    //geometry, actors, Entity counts or mutable second World are manufactured.
    //Publication precedes fallible provider/member validation. A failed load
    //keeps this bounded prefix, but document stays null until validation ends.
    struct NativeScbReader34 {
        std::shared_ptr<const ra_cold_sources50LA::Owner> source;
        std::shared_ptr<a::OriginalStandardArchiveProvider50LA> provider;
        a::OriginalStandardArchiveProvider50LA::MemberView member{};
        const a::ParameterDocument48F* document=nullptr;
    };
    //Actual per-World immutable construction-thread identity. In particular,
    //a rejected cross-thread public close must not revoke this native view.
    const std::thread::id nativeScbThread_=std::this_thread::get_id();
    std::optional<NativeScbReader34> nativeScb34_;
    std::optional<const void*> worldParser34_; //Actual shared role, SCB or TOB, never both.
    bool nativeScbRevoked_=false;
    // Separate genuine FileManager/Config custody; never the native reader34.
    std::shared_ptr<a::OriginalRaTrackSourceFile50LA> sourceTrackFile_;
    std::unique_ptr<rr::platform::OriginalWorldSourceConfig50LA> sourceWorldConfig_;
    enum class SourceTrackStep:std::uint8_t {unreached,mount,archiveOnly,cache,
        savedDirectory,reset,filename,config,opened,filenameReturned,sceneInfo,sceneInfoReturned,sceneConfigClosed,objectsReturned,objectConfigClosed,audioReturned,audioDescriptorsReturned,audioConfigClosed,terminal,nativeClosed};
    SourceTrackStep sourceTrackStep_=SourceTrackStep::unreached;
    std::uint32_t sourceTrackPC_=0x1a6d48;
    bool sourceConfig34Published_=false,sourceTrackNativeAttempted_=false;
    bool loadSourceTrackSegment1A6D48(const rr::game::OriginalRaceConstructor50LA&,
        const original_main_entry50KG::Application&,const std::string&,std::string&,std::uint32_t&)noexcept;
    std::array<char,260> sourceSavedModelDirectory_{}; //Only reached prefix is source-known.
    std::size_t sourceSavedDirectoryBytes_=0;
    bool decodeSourceLoadingSources(const std::vector<std::uint8_t>& original,
        g::SourceLibmState49T& actualLibm,a::TrackCollisionMasks50AN actualMasks,Diagnostic& d)noexcept {
        if(!enter(d,Stage::sources,sourceAttempted_,true))return false;
        try {
            source_=std::make_shared<ra_cold_sources50LA::Owner>();
            if(!source_->retainTrackInputBeforeDecode(original))return fail(d,"RA encoded source admission failed");
            if(!source_->decode(original,actualLibm,actualMasks,d.source))
                return fail(d,"RA source decode failed; serial numeric prefix retained");
            prefix_.sources=true;return true;
        }catch(...){return fail(d,"RA source-input allocation failure");}
    }
// Inside ra_cold_aggregate50LA::Owner private area. Separate native-only Track
// namespace custody, never roster, actual FileManager registration or World-ready.
bool sourceTrackMountAttempted_=false,sourceTrackMountComplete_=false,sourceTrackMountClosed_=false;
const original_main_entry50KG::Application* sourceTrackMountApplication_=nullptr;
std::shared_ptr<a::OriginalStandardMaterialDomain50LA> sourceTrackMountDomain_;
std::shared_ptr<const ra_cold_sources50LA::Owner> sourceTrackMountSource_;
a::NativeRaceMountLease50LA sourceTrackMountLease_;
bool prepareSourceTrackNativeProvider(const original_main_entry50KG::Application&,
    a::OriginalStandardMaterialDiagnostic50LA&)noexcept;
bool closeSourceTrackNativeProvider(a::OriginalStandardMaterialDiagnostic50LA&)noexcept;
// Parent abortSourceTrackNative order: Config.abortNative->sourceWorldConfig_.reset;
// sourceTrackFile_->closeNative->sourceTrackFile_.reset; THEN this close method.
// Keep all three owners on failure; no foreign provider mount is removed.

    // Explicit native stdout adaptation of1336C0. Original return is ignored
    // by the real caller; partial native output/error are retained, never replayed.
    bool sourceWorldLogAttempted_=false,sourceWorldLogNativeError_=false;
    std::size_t sourceWorldLogRequested_=0,sourceWorldLogWritten_=0;
    bool continueSourceWorldLog1B3AAC(std::uint32_t&)noexcept;
    enum class SourceSceneInfoPhase:std::uint8_t {unreached,select,name,portals,zones,environment,boundary,returnedPrefix,returnedOne};
    SourceSceneInfoPhase sourceSceneInfoPhase_=SourceSceneInfoPhase::unreached;
    bool sourceSceneInfoAttempted_=false;
    struct SourceSceneWarning {bool attempted=false,error=false;std::size_t requested=0,written=0;};
    std::array<SourceSceneWarning,3> sourceSceneWarnings_{};
    bool continueSourceSceneInfo270900(std::uint32_t&)noexcept;
    // Same actual nullable Owner published at270A70, not legacy prepared metadata.
    std::unique_ptr<g::original_world_environment50KG::Owner> sourceEnvironment3A4_;
    enum class SourceEnvironmentPhase:std::uint8_t {unreached,entered,bounded,returnedZero,returnedOne,nativeClosed};
    SourceEnvironmentPhase sourceEnvironmentPhase_=SourceEnvironmentPhase::unreached;
    bool sourceEnvironmentPublished_=false;
    bool continueSourceEnvironment270A58(std::uint32_t&)noexcept;
    bool abortSourceEnvironmentNative(std::uint32_t&)noexcept;

    enum class SourceLightsPhase:std::uint8_t {unreached,entered,bounded,returnedZero,returnedOne,nativeClosed};
    SourceLightsPhase sourceLightsPhase_=SourceLightsPhase::unreached;
    std::uint32_t sourceLightsPC_=0x270aa8;
    bool sourceLightsNullPublished_=false;
    bool loadSourceLightDescriptor277390(std::size_t,std::uint32_t&)noexcept;
    bool sourceWorldLightsInvocationCurrent()const noexcept;
    bool loadSourceWorldLights270AA8(std::uint32_t&)noexcept;
    bool abortSourceWorldLightsNative(std::uint32_t&)noexcept;
    bool sourceWorldFogInvocationCurrent()const noexcept;
    bool loadSourceWorldFog270B14(std::uint32_t&)noexcept;
    bool preflightSourceWorldFogNative()const noexcept;
    bool abortSourceWorldFogNative(std::uint32_t&)noexcept;
struct SourceZone275C50 {
    std::int32_t index00;
    std::array<char,260> name04;std::size_t nameKnown=0;
    std::int32_t staticCount108;
    std::optional<std::int32_t> collisionCount10C;
    std::array<std::int32_t,4> search110; //110,114,118,11C
    std::unique_ptr<std::int32_t[]> exclusions120;
    std::int32_t exclusionCount124,reverb128;
    std::array<std::uint32_t,4> fogColor12C;
    std::uint32_t fogVisibility13C,fogHaziness140,fogVtable144;
    std::uint8_t flags148=0,knownFlags148=0;
    std::array<std::optional<std::uint32_t>,4> bounds14C;
    std::uint32_t vtable15C;
    std::size_t exclusionsProduced=0;
    bool constructed=false,loaded=false,exclusionAllocationReturned=false;
};
std::unique_ptr<SourceZone275C50[]> sourceZones350_;
std::size_t sourceZonesConstructed_=0,sourceZonesLoaded_=0;
bool sourceZonesArrayReturned_=false,sourceZonesPublished_=false;
const g::OriginalApplicationEntities50LA* sourceZonesApplication_=nullptr;
std::uint64_t sourceZonesGeneration_=0;
enum class SourceZonesPhase:std::uint8_t {unreached,entered,bounded,returned,nativeClosed};
SourceZonesPhase sourceZonesPhase_=SourceZonesPhase::unreached;
std::uint32_t sourceZonesPC_=0x270b58;

// Aggregate-private additions only; base role remains sole array owner.
enum class SourcePortalsPhase:std::uint8_t {unreached,entered,bounded,returned,nativeClosed};
SourcePortalsPhase sourcePortalsPhase_=SourcePortalsPhase::unreached;
std::uint32_t sourcePortalsPC_=0x270bf8;
std::size_t sourcePortalsConstructed_=0,sourcePortalsLoaded_=0;
bool sourcePortalsAllocationReturned_=false;
bool sourceWorldPortalsInvocationCurrent()const noexcept; //Parent actual caller.
bool loadSourceWorldPortals270BF8(std::uint32_t&)noexcept;
bool preflightSourceWorldPortalsNative()const noexcept;
bool abortSourceWorldPortalsNative(std::uint32_t&)noexcept;
//Parent owns exact270594 sourcePortal3A0_=nullptr in both true resets,
//fresh/repeat admission and actual caller/cleanup-before-Zones integration.

// Aggregate private; SAME existing base worldPhysics3D4_ backing.
std::optional<g::OriginalWorldPhysicsIndex50LA*> sourcePhysics3D4_;
// Initially UNKNOWN. Neither270550 reset writes3D4; no syntheticNULL producer.
enum class SourcePhysicsPhase:std::uint8_t {unreached,entered,bounded,returned,nativeClosed};
SourcePhysicsPhase sourcePhysicsPhase_=SourcePhysicsPhase::unreached;
std::uint32_t sourcePhysicsPC_=0x270cc4;
bool sourcePhysicsPrepareReturned_=false;
bool sourceWorldPhysicsInvocationCurrent()const noexcept; //Parent actual caller.
bool loadSourceWorldPhysics270CC4(std::uint32_t&)noexcept;
bool preflightSourceWorldPhysicsNative()const noexcept;
bool abortSourceWorldPhysicsNative(std::uint32_t&)noexcept;

//Aggregate private; existing base nativeEmptyInstances3DC_ is the SOLE role.
enum class SourceInstancePhase:std::uint8_t {unreached,entered,bounded,returned,nativeClosed};
SourceInstancePhase sourceInstancePhase_=SourceInstancePhase::unreached;
std::uint32_t sourceInstancePC_=0x270fd0;
bool sourceWorldInstanceMasterInvocationCurrent()const noexcept; //Parent exact caller predicate.
bool loadSourceWorldInstanceMaster270FD0(std::uint32_t&)noexcept;
bool preflightSourceWorldInstanceMasterNative()const noexcept;
bool abortSourceWorldInstanceMasterNative(std::uint32_t&)noexcept;
//Include original_source_world_instances50la.inl after complete aggregate declaration.
//Normal and rollback native close: Instance before Physics, Portal, Config/File.

//Aggregate private. Same actual Owner class and sole sourceModels3C4_ lane.
std::unique_ptr<ra_static_model_cold50LA::Owner> sourceStaticOwner_;
//Actual1B3C1C stack copies and1B3C30 scan. Owned scalar snapshots ONLY;
//no parent/wrapper/model/graph pointer survives the synchronous getter.
struct SourceFinishLineAnchors {
    bool entered=false,initialized=false,scanReturned=false;
    std::array<std::uint32_t,4> originWords{};
    std::array<std::array<std::uint32_t,4>,2> stackWords{};
    std::uint32_t ordinal=0;
    std::optional<std::uint32_t> liveCount;
    std::array<std::optional<std::uint32_t>,2> lastMatchingSlot;
} sourceFinishLineAnchors_;
bool continueSourceFinishLineAnchors1B3C1C(std::uint32_t&)noexcept;
//Executed scalar comparisons only: fresh process-origin words, never the scan's
//initial snapshot or a match-count policy. W is not read by this source branch.
struct SourceFinishLineComparison {
    bool entered=false,returned=false;
    std::array<std::array<std::uint32_t,3>,2> originWords{};
    std::array<std::uint8_t,2> readMask{};
    std::array<std::optional<bool>,2> equalsOrigin;
    std::optional<bool> missingAnchor;
} sourceFinishLineComparison_;
bool continueSourceFinishLineComparison1B3CD8(std::uint32_t&)noexcept;
//Scalar control-flow receipts, not a guest-register emulator. The genuine
//epilogue preserves whatever V0 the selected branch left; no success integer.
struct SourceFinishLineCallerReturn {
    std::uint8_t restoreMask=0; //RA/S2/S1/S0 LQ receipts, in source order.
    bool stackReleased=false,returned=false,v0Preserved=false;
    std::optional<std::uint32_t> loadingMode30;
    bool progressSkipped=false;
} sourceFinishLineCallerReturn_;
//Executed scalar frame/dispatch receipts. No retained native pointer borrow,
//guest register fabrication, collision load result, or World readiness flag.
struct SourceCollisionDispatch128 {
    std::uint8_t stage=0;
    std::optional<std::uint32_t> worldTable08,target128,groupCount30,wrapperTable0C,target0C;
    std::optional<std::uint32_t> previousModelZone354; //Historical model cursor, not a live World lane.
    std::uint32_t zoneS0=0,zoneOffsetS1=0,slotS0=0,slotOffsetS1=0;
    bool zone354Stored=false;
    std::optional<std::uint32_t> storedZone354,liveZoneCount34C;
    std::array<std::uint32_t,10> callbacksReturned{};
    std::array<bool,10> groupsReturned{},emptyGroupsSkipped{},groupLogAttempted{};
    std::array<std::size_t,10> groupLogWritten{};
    bool worldReturned=false;
    std::optional<std::uint32_t> callbackReturnV0; //Ignored by original caller, retained as history.
    bool callbackReturned=false,loopCountReloaded=false;
} sourceCollisionDispatch128_;
bool continueSourceCollisionDispatch1AA244(std::uint32_t&)noexcept;
struct SourcePostCollisionCaller {
    std::optional<std::uint32_t> loadingMode30;
    bool progressSkipped=false;
    std::uint8_t animationDirectoryStoreMask=0;
    std::optional<std::uint32_t> worldTable08,target120,target124;
    bool baseToyNullRead=false,derivedToyNullRead=false;
    std::optional<std::uint32_t> baseToyReturnV0,toyReturnV0;
    bool toyReturned=false,particleDispatched=false,particleReturned=false;
    std::optional<std::uint32_t> particleReturnV0,target144;
    //Bounded Fog-derived World epilogue: V0, restoreRA, JR, stack delay.
    std::uint8_t particleWrapperReturnMask=0;
} sourcePostCollisionCaller_;
bool continueSourcePostCollisionCaller1AA258(std::uint32_t&)noexcept;
struct SourcePostParticleCaller1AA2CC {
    std::uint8_t step=0; //Fresh World, table, target144, JALR, NOP delay.
    std::optional<std::uint32_t> worldTable08;
} sourcePostParticleCaller_;
bool preflightSourcePostParticleCallerNative()const noexcept;
bool continueSourcePostParticleCaller1AA2CC(std::uint32_t&)noexcept;
struct SourceTriggerServiceRow {
    std::optional<std::uint32_t> rowIndex,count30,descriptorTable6C,descriptorTarget0C;
    bool rowRead=false,rowNonNull=false,assertionLazyStored=false,assertionReturned=false,descriptorEntered=false;
    std::optional<std::uint8_t> guard3D686CRead;
    std::optional<std::uint32_t> assertion444618Read,worldZones34CRead;
    std::optional<std::int32_t> zone08,callbackZone08;
    bool collisionArrayRead=false,collisionNodeRead=false;
    bool trigger00Read=false,geometryReturned=false,triggerCallbackReturned=false;
    bool actionLoopEntered=false,actionRead=false,actionReturned=false;
    bool geometrySourceFreed=false,descriptorReturned=false,parentOperandsRead=false,registrationReturned=false;
    std::optional<std::uint32_t> descriptorReturnV0;
    std::optional<std::uint32_t> actionIndex,actionOffset,actionCount,actionTable108,actionTarget0C,actionReturnV0;
    bool parentAdvanced=false;
    std::optional<std::uint32_t> nextRow;
};
struct SourceTriggerService274E90 {
    bool entered=false,listRead=false,listNonNull=false,listEntered=false,nativeWithdrawn=false;
    std::uint8_t fogPrefixStep=0; //Own bounded prologue/list/first-row/BEQL history, through12.
    std::uint8_t fogBodyStep=0; //Own assertion/descriptor/callback operands, through16.
    std::uint8_t fogDescriptorStep=0; //Own single-action/geometry retirement/parent return stages, through18; next count remains pending.
    std::optional<std::uint32_t> fogAssertionPredicate,fogAssertionReturnV0;
    g::OriginalTriggerFogGeometryPrefix50LA fogGeometry;
    g::OriginalTriggerFogMembershipPrefix50LA fogMembership;
    g::OriginalTriggerFogBoundsPrefix50LA fogBounds;
    g::OriginalTriggerFogPrimaryPrefix50LA fogPrimary;
    std::optional<std::uint32_t> listTable08,listTarget7C,currentRow,freshCount30;
    std::array<SourceTriggerServiceRow,2> rows;
    bool listReturned=false,wrapperReturned=false;
    std::optional<std::uint32_t> listReturnV0,wrapperReturnV0;
} sourceTriggerService274E90_;
bool preflightSourceTriggerServiceRowNative(const SourceTriggerServiceRow&,std::size_t)const noexcept;
bool continueSourceTriggerService274E90(std::uint32_t&)noexcept;
bool sourceTriggerFogPortalGraphLiveNative(std::size_t)const noexcept;
bool continueSourceTriggerFogBody2DCB6C(std::uint32_t&)noexcept;
bool preflightSourceTriggerServiceNative()const noexcept;
//New synchronous caller occurrence after274E90; stored values are history,
//never a profile snapshot used to authorize later calls.
struct SourcePostTriggerCaller {
    bool entered=false,applicationRead=false,profileRead=false,worldRead=false,dispatched=false;
    std::optional<std::uint8_t> profile1125C,mode;
    std::optional<std::uint32_t> argumentA1,worldTable08,target13C;
} sourcePostTriggerCaller_;
bool continueSourcePostTriggerCaller1AA2E0(std::uint32_t&)noexcept;
struct SourceSkySetup274AE0 {
    bool entered=false,environmentRead=false,environmentNonNull=false,returned=false;
    std::optional<std::uint32_t> argumentS1,arrayBytes,returnV0,callerResumePC;
    std::optional<std::int32_t> count00;
    std::uint8_t restoreMask=0;
    bool stackReleased=false,callerSkipExecuted=false;
} sourceSkySetup274AE0_;
bool continueSourceSkySetup274AF4(std::uint32_t&)noexcept;
//Distinct source3F8 custody. No prepared NativeSkyArray3F8 or image fixture.
//Pointer slots are unknown until their actual source stores; only published slots may be read.
//Native typed facet of actual0xAF0 allocation; unreached guest lanes stay unknown.
struct SourceSkyOwner {g::OriginalWreckComponent50LA entity;
    std::uint32_t count7D8=0; //Constructor129320 writes zero; subsequent partial file writes preserve remaining bytes.
};
std::unique_ptr<SourceSkyOwner> sourceSkyOwner_;
struct SourceSkyConstructor288A50 {
    bool allocationAttempted=false,allocationReturned=false,allocationNonNull=false,entered=false;
    bool countAcquired=false,countRetiredNative=false,searchableReturned=false,baseReturned=false,returned=false;
    bool tableZeroed=false,slotPublished=false;
    std::optional<std::uint32_t> countBefore,vtable08;
    std::optional<std::uint16_t> query30;
    std::optional<std::uint8_t> type32;
    std::uint8_t flags33=0,knownFlags33=0;
    std::array<std::optional<std::uint32_t>,9> nullFields; //38/3C/40/44/4C/48/7D8/7D0/7D4
    std::uint32_t triplesWritten=0;
} sourceSkyConstructor288A50_;
bool continueSourceSkyConstructor274B60(std::uint32_t&)noexcept;
struct SourceSkyError129960 {
    bool entered=false,lockRead=false,unlinkedLeafRead=false,authorityRetired=false,referenceStored=false;
    bool derivedDestructorEntered=false,baseDestructorEntered=false;
    std::size_t failedItem=0;
} sourceSkyError129960_;
bool continueSourceSkyError129960(std::uint32_t&,std::size_t)noexcept;
struct SourceSkyLoad288A90 {
    bool entered=false,policy3E0990Stored=false,policy36C0F0Stored=false;
    bool environmentRead=false,fileRead=false,receiverRead=false,receiverNonNull=false;
    bool wrapperEntered=false,directoryRead=false,formatted=false,loaderEntered=false,noOp141C20Returned=false;
    std::optional<std::uint32_t> argumentA1,argumentF12;
    std::optional<std::size_t> fileLength,pathLength;
    std::array<char,260> fileName; //Known through the copied source NUL only.
    std::array<char,272> path30; //Native bound within source frame after stack30, not a guest bounds check.
} sourceSkyLoad288A90_;
bool continueSourceSkyLoad12A100(std::uint32_t&)noexcept;
std::unique_ptr<a::OriginalSourceSkyFile50LA> sourceSkyFile_;
struct SourceSkyFileCaller1295E0 {
    bool entered=false,resolverRead=false,openAttempted=false,openReturned=false,readAttempted=false,readReturned=false;
    std::optional<std::int64_t> handleS3;
    std::optional<std::uint32_t> firstReadItems;
} sourceSkyFileCaller1295E0_;
bool continueSourceSkyFile1295E0(std::uint32_t&)noexcept;
struct SourceSkyHeader129644 {
    bool entered=false,resolverRead=false,countReadAttempted=false,countReadReturned=false;
    bool loopEntered=false,offsetReadAttempted=false,offsetReadReturned=false,tellReturned=false,seekReturned=false;
    std::optional<std::uint32_t> countReadItems,count7D8,paletteCount,paletteOffset;
    std::optional<std::int32_t> indexS2;
    std::optional<std::int64_t> savedS4;
    std::size_t receiverBytesWritten=0;
    std::optional<std::size_t> resourceStart;
    bool field40Cleared=false;
} sourceSkyHeader129644_;
bool continueSourceSkyHeader129644(std::uint32_t&)noexcept;
struct SourceSkyPaletteOwner {
    std::unique_ptr<std::uint8_t[]> storage; //Fresh exact0x18B18 bytes; unknown unless a source store/read reached them.
};
std::unique_ptr<SourceSkyPaletteOwner> sourceSkyPaletteOwner_;
std::optional<SourceSkyPaletteOwner*> sourceSkyPalette7D0_;
struct SourceSkyPalette2E38C0 {
    bool entered=false,allocationAttempted=false,allocationReturned=false,allocationNonNull=false;
    bool constructorEntered=false,baseReturned=false,derivedVtableStored=false,slotPublished=false,nativeWithdrawn=false;
    bool nativeReferenceWithdrawn=false,constructorReturned=false;
    bool sourceReleaseEntered=false,sourceReferenceDecremented=false,sourceAssociationScanReturned=false;
    bool sourceBaseDestructed=false,sourceFreeAdmitted=false,sourceBackingFreed=false,sourceReleaseReturned=false;
    std::optional<std::uint32_t> vtable08;
    std::optional<std::uint64_t> reference00;
    std::array<bool,3> readAttempted{{false,false,false}},readReturned{{false,false,false}};
    std::array<std::size_t,2> fieldBytes{{0,0}};
    std::optional<std::uint32_t> compressedFlag;
    bool rawEntered=false,rawReturned=false,count14Read=false;
    std::array<bool,3> payloadAttempted{{false,false,false}},payloadReturned{{false,false,false}};
    std::array<std::size_t,3> payloadBytes{{0,0,0}};
    std::optional<std::uint32_t> count14;
    std::uint32_t convertedColors=0;
} sourceSkyPalette2E38C0_;
bool continueSourceSkyPalette1296F4(std::uint32_t&)noexcept;
bool continueSourceSkyRawPalette2E3B68(std::uint32_t&,std::size_t occurrence=0)noexcept;
bool continueSourceSkyPaletteHeaders2E38FC(std::uint32_t&,std::size_t)noexcept;
bool continueSourceSkyNextPaletteReturn129710(std::uint32_t&)noexcept;
struct SourceSkyPaletteReturn129710 {
    bool entered=false,restoreAttempted=false,restoreReturned=false,nextEntered=false,field40Cleared=false;
    std::optional<std::int32_t> indexS2;
    std::optional<std::uint32_t> nextOffset;
    std::optional<std::int64_t> nextSavedS4;
} sourceSkyPaletteReturn129710_,sourceSkyNextPaletteReturn129710_;
bool continueSourceSkyPaletteReturn129710(std::uint32_t&)noexcept;
bool continueSourceSkySecondPaletteRestore129714(std::uint32_t&,std::size_t occurrence=1)noexcept;
bool continueSourceSkyFollowingPaletteDirectory(std::uint32_t&,SourceSkyPaletteReturn129710&)noexcept;
std::unique_ptr<SourceSkyPaletteOwner> sourceSkyNextPaletteOwner_;
SourceSkyPalette2E38C0 sourceSkyNextPaletteHistory_;
struct SourceSkyLaterPalette {
    std::unique_ptr<SourceSkyPaletteOwner> owner;
    SourceSkyPalette2E38C0 history;
    SourceSkyPaletteReturn129710 returned;
};
std::vector<std::unique_ptr<SourceSkyLaterPalette>> sourceSkyLaterPalettes_;
std::optional<std::size_t> sourceSkyPublishedPaletteOccurrence_;
bool sourceSkyLaterDispatcherActive_=false;
SourceSkyPalette2E38C0& sourcePaletteHistory(std::size_t i)noexcept {
    return i==0?sourceSkyPalette2E38C0_:i==1?sourceSkyNextPaletteHistory_:sourceSkyLaterPalettes_[i-2]->history;
}
const SourceSkyPalette2E38C0& sourcePaletteHistory(std::size_t i)const noexcept {
    return i==0?sourceSkyPalette2E38C0_:i==1?sourceSkyNextPaletteHistory_:sourceSkyLaterPalettes_[i-2]->history;
}
std::unique_ptr<SourceSkyPaletteOwner>& sourcePaletteOwner(std::size_t i)noexcept {
    return i==0?sourceSkyPaletteOwner_:i==1?sourceSkyNextPaletteOwner_:sourceSkyLaterPalettes_[i-2]->owner;
}
const std::unique_ptr<SourceSkyPaletteOwner>& sourcePaletteOwner(std::size_t i)const noexcept {
    return i==0?sourceSkyPaletteOwner_:i==1?sourceSkyNextPaletteOwner_:sourceSkyLaterPalettes_[i-2]->owner;
}
SourceSkyPaletteReturn129710& sourcePaletteReturn(std::size_t i)noexcept {
    return i==0?sourceSkyPaletteReturn129710_:i==1?sourceSkyNextPaletteReturn129710_:sourceSkyLaterPalettes_[i-2]->returned;
}
bool finishSourceSkyPaletteConstructor(std::uint32_t&,std::size_t)noexcept;
bool continueSourceSkyPaletteReturnForOccurrence(std::uint32_t&,std::size_t)noexcept;
bool continueSourceSkyLaterPalettes(std::uint32_t&,std::size_t)noexcept;
bool preflightSourceSkyLaterPalettesNative()const noexcept;
struct SourceSkyItemOwner {
    std::unique_ptr<std::uint8_t[]> storage;
    std::array<bool,0x660> known{};
    // Typed views of actual group/entry +58 slots; no guest pointer bits invented.
    std::array<a::OriginalSourceTextureLifetimeFacet50LA*,96> resources58{};
};
std::unique_ptr<SourceSkyItemOwner> sourceSkyFirstItem_;
std::unique_ptr<SourceSkyItemOwner*[]> sourceSkyItemSlots3C_;
std::optional<SourceSkyItemOwner**> sourceSkyPublishedItems3C_;
struct SourceSkyEmbeddedFC {
    std::unique_ptr<std::uint8_t[]> object20,alignedBacking;
    std::array<bool,0x20> known{};
    std::uint8_t* data00=nullptr;
    std::uint8_t* cursor04=nullptr;
    bool allocationAttempted=false,allocationReturned=false,constructorEntered=false;
    bool alignedAttempted=false,alignedReturned=false,uncachedAliasSelected=false,zeroed=false,constructorReturned=false;
    bool published=false,nativeWithdrawn=false;
    std::uint32_t roundedCount=0,bytes=0;
};
struct SourceSkyCompressedBacking {
    std::unique_ptr<std::uint8_t[]> allocation;
    std::uint8_t* aligned=nullptr;
    std::size_t capacity=0;
    bool attempted=false,returned=false,published=false,nativeWithdrawn=false;
};
struct SourceSkyFollowingCompressedLevel {
    std::size_t ordinal=0;
    std::uint32_t dimension=0,index=0,size16C=0;
    bool selected=false,sizeReadAttempted=false,sizeReadReturned=false,sizeStored=false;
    SourceSkyCompressedBacking backing;
    bool counterRead=false,counterStored=false,transferShort=false,transferAccepted=false;
    std::uint32_t counterBefore=0,counterAfter=0;
    std::uint64_t counterSequence=0;
    std::size_t knownPrefix=0;
    bool dimensionsStored=false,mirrorDimensionsStored=false,headerSizeAdjusted=false,baseAliasStored=false,offsetAliasStored=false;
    std::array<bool,2> logReturned{};
    std::size_t liveOffset118=0;
    bool loopAdvanced=false,nextLoopTaken=false;
    std::uint32_t nextDimension=0;
    std::size_t nextOrdinal=0;
};
struct SourceSkyResourcePrefix {
    bool entered=false,compressed=false,sizeReadAttempted=false,sizeReadReturned=false;
    bool allocationAttempted=false,allocationReturned=false,allocationNonNull=false,constructorEntered=false,baseReferenceAcquired=false;
    bool nativeWithdrawn=false,nativeReferenceRetired=false;
    std::size_t fileOrdinal=0,group=0,entry=0,sizeCopied=0;
    std::optional<std::uint32_t> groupWord50,format48,sizeCode,side,bytesPerPixel,payloadBytes,sizeReadItems,vtable08;
    std::optional<std::uint64_t> reference00;
    std::unique_ptr<a::OriginalSourceTextureLifetimeFacet50LA> texture;
    bool managerRead=false,registrationReturned=false,base29C6E0Returned=false;
    bool derivedTableStored=false,embeddedFCConstructed=false,derivedConstructorReturned=false;
    // Actual byte RMW preserves unwritten upper bits. These are bit-level
    // known-zero receipts, not invented values for the whole EE byte.
    std::array<std::uint8_t,5> level130KnownZero{};
    std::uint8_t flagsF8KnownZero=0;
    std::size_t derivedLevels=0;
    bool compressedStackInitialized=false,compressedHeaderConverted=false,compressedB8Replaced=false,compressedLoaderEntered=false;
    std::array<std::optional<std::uint32_t>,5> compressedHeaderWords;
    std::array<std::size_t,5> compressedHeaderCopied{};
    std::array<bool,5> compressedHeaderAttempted{},compressedHeaderReturned{};
    std::array<std::optional<std::uint32_t>,5> compressedHeaderItems;
    std::optional<std::uint32_t> compressedPendingReturnPC,compressedInputFormat,compressedInputLayout;
    SourceSkyCompressedBacking compressedBacking;
    bool compressedCounterRead=false,compressedCounterStored=false;
    bool compressedTransferShort=false,compressedLevelDimensionsStored=false;
    std::size_t compressedKnownPrefix=0;
    std::array<bool,2> compressedLogReturned{};
    bool compressedMirrorDimensionsStored=false,compressedHeaderSizeAdjusted=false;
    bool compressedBaseAliasStored=false,compressedOffsetAliasStored=false;
    bool resourceTailEntered=false,resourceAlignmentReturned=false,resourceSourceReturned=false;
    std::optional<std::uint16_t> resourceTailFlags4C;
    std::optional<std::uint32_t> resourceSourceResult;
    bool postLoaderSlotReloaded=false,postLoaderVirtualEntered=false,postLoaderVirtualReturned=false;
    a::OriginalSourceTextureLifetimeFacet50LA* postLoaderReceiver=nullptr;
    std::optional<std::uint32_t> postLoaderTarget,postLoaderResult;
    std::array<std::uint32_t,3> postLoaderArguments{};
    bool compressedLoopAdvanced=false,compressedNextLoopTaken=false;
    bool compressedTailEntered=false,compressedTailCountStored=false,compressedTailFormatStored=false,compressedFinalTellReturned=false,compressedLoaderReturned=false;
    std::optional<std::uint32_t> compressedTailFormat,compressedLoaderResult;
    std::optional<std::uint32_t> compressedNextDimension,compressedNextIteration;
    std::vector<std::unique_ptr<SourceSkyFollowingCompressedLevel>> followingCompressedLevels;
    std::size_t compressedLiveOffset118=0; //Typed offset into same owned backing, never a fabricated host pointer.
    std::uint32_t compressedCounterBefore=0,compressedCounterAfter=0;
    std::uint64_t compressedCounterSequence=0;
    bool compressedCountAdjusted=false,compressedLevelSetup=false,compressedLevelSizeStored=false;
    std::optional<std::uint32_t> compressedLevelIndex,compressedPayloadSize16C;
    bool compressedDimensionsStored=false,compressedLoopInitialized=false,compressedOffsetsReturned=false,compressedOffsetShortExit=false;
    std::optional<std::uint32_t> loaderStack140,loaderEffectiveFormat,loaderEffectiveLayout,compressedLoopScale,compressedLoopIndex;
    bool slot58Stored=false,slot58WithdrawnNative=false,loaderClassified=false,rawLoaderEntered=false;
    std::optional<std::uint32_t> loaderFormat48,rawFormat29E640;
    std::optional<std::uint64_t> loaderHandle40;
    // Caller argument borrow only: virtual loader has not retained the palette.
    SourceSkyPaletteOwner* loaderPaletteBorrow=nullptr;
    a::OriginalSourceTextureLifetimeFacet50LA* loaderReceiver=nullptr;
    bool loaderGuardReturned=false,loaderFrameInitialized=false,loaderFlagsStored=false,loaderWorkspaceCleared=false;
    std::optional<std::uint32_t> loaderFormat3C,loaderE8,loaderMaximumBits;
    std::uint8_t loaderF8KnownZero=0,loaderF8KnownOne=0;
    std::array<std::uint8_t,0x30> loaderWorkspace170{};
    std::shared_ptr<a::OriginalStandardTextureLifetime50LA> loaderBufferManager;
    std::array<a::OriginalStandardTextureLifetime50LA::SourceBufferCall,2> loaderBuffers;
    bool loaderNullBufferExit=false;
    std::optional<std::uint32_t> loaderBufferBytes;
    bool payloadBranchSelected=false,payloadExtendedFormat=false,payloadReadAttempted=false,payloadReadReturned=false,payloadShortExit=false,payloadVirtualEntered=false;
    std::optional<std::uint32_t> payloadFileBits,payloadReadBytes,payloadReadItems;
    std::size_t payloadCopied=0,payloadKnownPrefixBefore=0;
    std::uint8_t* payloadVirtualBorrow=nullptr;
    std::optional<std::uint32_t> payloadPendingReturnPC; // Distinct call continuation, not a completed return.
    bool payloadFieldsStored=false,paletteB4Stored=false,formatNormalized=false,embeddedFCGuardReturned=false;
    SourceSkyPaletteOwner* resourcePaletteB4=nullptr; // Actual stored borrow, no source AddRef yet.
    std::optional<std::uint32_t> normalizedFormatS1;
    std::uint32_t embeddedFCCapacityArgument=0;
    std::unique_ptr<SourceSkyEmbeddedFC> embedded;
    SourceSkyEmbeddedFC* embeddedFCPublished=nullptr;
    bool embeddedCallReturned=false,resourceFormatFieldsStored=false,levelAllocationEntered=false;
    std::optional<std::uint32_t> resourceCurrentE8;
    std::array<std::uint64_t,6> rawStackScalars{}; // stack08..30; palette at00 is typed separately.
};
struct SourceSkyResourceCall {std::size_t index=0,cursor=0;bool entered=false,bodyEntered=false;};
struct SourceSkyItemRegistration {
    std::unique_ptr<SourceSkyResourcePrefix> pendingResource;
    std::vector<std::unique_ptr<SourceSkyResourcePrefix>> completedResources;
    std::vector<std::unique_ptr<SourceSkyResourceCall>> resourceCalls;
    bool resourceScanExhausted=false;
    bool entered=false,field38Stored=false,paletteRead=false,seekAttempted=false,seekReturned=false,returned=false,nativeWithdrawn=false;
    SourceSkyPaletteOwner* paletteBorrow=nullptr;
    std::optional<std::size_t> paletteOccurrence;
    std::array<std::optional<std::uint16_t>,96> masks;
    std::size_t completedEntries=0;
    std::optional<std::uint32_t> sourceResult;
};
struct SourceSkyItemState {
    SourceSkyItemRegistration registration;
    bool nativeWithdrawn=false,itemAllocationAttempted=false,itemAllocationReturned=false,itemAllocationNonNull=false;
    bool constructorEntered=false,baseReturned=false,constructorReturned=false,parserEntered=false,handleStored=false;
    bool headerAttempted=false,headerReturned=false,referenceWithdrawnNative=false;
    bool parserReturned=false,slotStored=false;
    bool failureReleaseEntered=false,failureReferenceStored=false,failureDestructorEntered=false,failureBaseReturned=false;
    std::uint32_t failureNullSlotsRead=0;
    bool failureSourceDeleted=false,failureNullReturned=false;
    std::optional<std::uint32_t> parserModeA2,dataPosition650,vtable08,headerItems;
    std::optional<std::uint64_t> reference00,handle40;
    std::optional<std::array<std::uint32_t,4>> origin3E0900;
    std::size_t headerCopied=0;
    std::uint32_t callbacks=0,defaultGroups=0;
};
struct SourceSkyItemLoopHistory {
    bool restoreAttempted=false,restoreReturned=false,nextDirectoryEntered=false;
    std::optional<std::uint32_t> advancedIndexS4,advancedOffsetS2,loopCount7D8,nextAbsoluteOffset;
    std::optional<std::int64_t> nextSavedS5;
};
struct SourceSkyItems129758 : SourceSkyItemState, SourceSkyItemLoopHistory {
    bool entered=false,allocationAttempted=false,allocationReturned=false,allocationNonNull=false,published=false;
    std::optional<std::uint32_t> count7D8,arrayBytes,indexS4,offsetS2,absoluteOffset;
    std::optional<std::int64_t> savedS5;
    bool directoryEntered=false;
} sourceSkyItems129758_;
struct SourceSkyLaterItem {SourceSkyItemState history;SourceSkyItemLoopHistory loop;std::unique_ptr<SourceSkyItemOwner> owner;};
std::vector<std::unique_ptr<SourceSkyLaterItem>> sourceSkyLaterItems_;
SourceSkyItemState& sourceItemHistory(std::size_t i)noexcept {return i?sourceSkyLaterItems_[i-1]->history:sourceSkyItems129758_;}
const SourceSkyItemState& sourceItemHistory(std::size_t i)const noexcept {return i?sourceSkyLaterItems_[i-1]->history:sourceSkyItems129758_;}
std::unique_ptr<SourceSkyItemOwner>& sourceItemOwner(std::size_t i)noexcept {return i?sourceSkyLaterItems_[i-1]->owner:sourceSkyFirstItem_;}
const std::unique_ptr<SourceSkyItemOwner>& sourceItemOwner(std::size_t i)const noexcept {return i?sourceSkyLaterItems_[i-1]->owner:sourceSkyFirstItem_;}
SourceSkyItemLoopHistory& sourceItemLoop(std::size_t i)noexcept {return i?sourceSkyLaterItems_[i-1]->loop:sourceSkyItems129758_;}
const SourceSkyItemLoopHistory& sourceItemLoop(std::size_t i)const noexcept {return i?sourceSkyLaterItems_[i-1]->loop:sourceSkyItems129758_;}
bool continueSourceSkyItemFailure128C8C(std::uint32_t&,std::size_t)noexcept;
bool sourceSkyItemDriverActive_=false;
struct SourceSkyPacketRow {
    std::unique_ptr<std::uint8_t[]> object30;
    std::array<bool,0x30> known{};
    bool allocationAttempted=false,allocationReturned=false,constructorReturned=false;
    bool slot7E8Stored=false,helper12A510Entered=false,nativeWithdrawn=false;
    const SourceSkyOwner* packetSkyThis=nullptr;
    std::unique_ptr<std::uint8_t[]> packetBacking;
    std::uint8_t* packetC0=nullptr;
    bool packetAllocationAttempted=false,packetAllocationReturned=false,packetZeroed=false,packetPrefixStored=false;
};
struct SourceSkyPostItems {
    bool entered=false,nativeWithdrawn=false;std::uint32_t indexS4=0,offsetS2=0;
    bool tailEntered=false,index40Stored=false,firstSlotRead=false,firstSlotSelected=false,packetCallEntered=false;
    bool packetAllocationAttempted=false,packetAllocationReturned=false,packetZeroed=false,packetHeaderStored=false,packetNativeWithdrawn=false;
    // Original LWC1/SWC1 copies: retain bits, including NaNs, without host FP conversion.
    std::array<std::uint32_t,3> packetItemWords{};
    std::uint8_t packetItemLanesStored=0;
    bool packetMatrixStored=false,packetTagsStored=false;
    bool packetPointerStored=false,packetReturned=false,packetPublished7E0=false;
    // Typed source addresses, never truncated host pointers encoded as EE words.
    std::uint8_t* packetData04=nullptr;
    std::uint8_t* packet7E0=nullptr;
    bool rowsEntered=false;
    std::uint32_t rowIndexS4=0,rowOffsetS2=0;
    std::array<std::unique_ptr<SourceSkyPacketRow>,96> rows;
    std::array<std::uint8_t*,96> rows7E8{};
    std::unique_ptr<std::uint8_t[]> packetBacking;
    std::uint8_t* packet340=nullptr;
} sourceSkyPostItems_;
bool continueSourceSkyPostRegistration129890(std::uint32_t&)noexcept;
std::optional<SourceSkyItemOwner*> sourceSkySelected38_;
std::optional<std::size_t> sourceSkySelectedItemOccurrence_;
bool continueSourceSkyPostItems(std::uint32_t&)noexcept;
bool continueSourceSkyResourcePrefix(std::uint32_t&,std::size_t,std::size_t,std::size_t)noexcept;
bool preflightSourceSkyResourceOccurrence(std::size_t,const SourceSkyResourcePrefix&,bool)const noexcept;
bool sourceSkyResourceDriverActive_=false;
bool continueSourceSkyResourceDriver(std::uint32_t&,std::size_t,std::size_t,std::size_t)noexcept;
bool continueSourceSkyRegistration129168(std::uint32_t&,std::size_t)noexcept;
bool preflightSourceSkyResourcePrefix(std::size_t)const noexcept;
bool continueSourceSkyResourceTail129054(std::uint32_t&,std::size_t)noexcept;
bool continueSourceSkyEmbedded257750(std::uint32_t&,std::size_t)noexcept;
bool continueSourceSkyPayload250EE0(std::uint32_t&,std::size_t)noexcept;
static bool sourceSkyFormat29E510(std::uint32_t)noexcept;
static std::uint32_t sourceSkyFormat29E480(std::uint32_t)noexcept;
static std::uint32_t sourceSkyFormat29E640(std::uint32_t)noexcept;
static std::uint32_t sourceSkyFormatBits29E910(std::uint32_t)noexcept;
static std::uint32_t sourceSkyFileBits29E5B0(std::uint32_t)noexcept;
bool continueSourceSkyItemLoop(std::uint32_t&,std::size_t)noexcept;
bool continueSourceSkyItemBody(std::uint32_t&,std::size_t)noexcept;
bool preflightSourceSkyItemOccurrence(std::size_t)const noexcept;
bool continueSourceSkyItems129758(std::uint32_t&)noexcept;
bool continueSourceSkyItemRestore(std::uint32_t&,std::size_t)noexcept;
bool preflightSourceSkyItemsNative()const noexcept;
bool withdrawSourceSkyItemsNative()noexcept;


bool continueSourceSkyOldPalette1296D0(std::uint32_t&,std::size_t occurrence=1)noexcept;
bool initializeSourceSkyPaletteBase2E38C0(SourceSkyPaletteOwner&,SourceSkyPalette2E38C0&,std::uint32_t&)noexcept;
bool preflightSourceSkyPaletteNative()const noexcept;
bool withdrawSourceSkyPaletteNative()noexcept;



std::unique_ptr<SourceSkyOwner*[]> sourceSkySlots3F8_;
std::optional<SourceSkyOwner**> sourceSkyPublished3F8_;
struct SourceSkyArray274B24 {
    bool entered=false,allocationAttempted=false,allocationReturned=false,allocationNonNull=false;
    bool published=false,nativeWithdrawn=false,fileRead=false,noneSkip=false;
    std::optional<std::uint32_t> indexS0,allocatedCount,fileIndex;
    std::optional<std::int32_t> freshCount;
    std::array<char,260> fileName; //Only fileLength+1 bytes become known.
    std::optional<std::size_t> fileLength;
    std::optional<bool> equalsNone;
} sourceSkyArray274B24_;
bool continueSourceSkyArray274B24(std::uint32_t&)noexcept;
bool preflightSourceSkyArrayNative()const noexcept;
bool withdrawSourceSkyArrayNative()noexcept;




//Historical source dispatch registers only: no new owner or borrowed raw pointer.
struct NativeParticleRow27DE00;
struct NativeParticleDescriptor27C430;
//Distinct continuation of parent27DFD0 and fresh next27C7B0 invocation.
//The first-row SourceParticleService124 below remains immutable history.
struct SourceParticleNextRow27E018 {
    bool entered=false,offsetAdvanced=false,rowAdvanced=false,countRead=false,listReturned=false;
    std::uint32_t offsetS1=0,rowS0=0;
    std::optional<std::int32_t> count30;
    NativeParticleRow27DE00* row=nullptr;
    NativeParticleDescriptor27C430* descriptor=nullptr;
    bool rowRead=false,dispatched=false,descriptorRead=false,descriptorSourceRetired=false,nativeWithdrawn=false;
    std::optional<std::uint32_t> table14,target0C,type04;
    std::optional<bool> emitterIsNone;
};
//One actual parent continuation after each authored row, including the final
//count comparison. Entry index3 is termination only, never a fabricated row4.
std::array<SourceParticleNextRow27E018,4> sourceParticleNextRows_;
bool continueSourceParticleNextRow27E018(std::size_t,std::uint32_t&)noexcept;
bool preflightSourceParticleNextRowNative()const noexcept;
bool preflightSourceParticleNextRowNative(std::size_t)const noexcept;
struct SourceParticleService124 {
    bool entered=false,listRead=false,listNonNull=false,listEntered=false;
    std::optional<std::uint32_t> listTable08,listTarget78,rowTable14,rowTarget0C;
    std::optional<std::int32_t> rowS0,count30;
    std::optional<std::uint32_t> offsetS1;
    bool rowRead=false,rowEntered=false,descriptorRead=false;
    std::optional<bool> emitterIsNone;
    std::optional<std::uint32_t> descriptorType04;
    bool rowType08Stored=false;
    std::optional<std::uint8_t> guard3D6528Read;
    bool assertionLazyStored=false;
    std::optional<std::uint32_t> assertion443068Read,assertionReturnV0;
    bool manager4A0Read=false,manager4A0NonNull=false;
    std::optional<std::int32_t> instances330Read;
    bool countPositive=false,emitterArrayAttempted=false,emitterArrayPublished=false;
    std::optional<std::uint32_t> emitterType,emitterAllocationBytes,emitterConstructor;
    std::size_t emitterAttempts=0,emitterPublishedSlots=0;
    bool rowCount0CStored=false,rowStart10Stored=false;
    std::optional<std::array<std::uint32_t,4>> attachmentOffsetE0;
    std::optional<bool> attachmentIsNone,childIsNone;
    std::optional<std::uint32_t> attachmentZone,attachmentSlot,attachmentLiveZones,attachmentLiveCount;
    std::uint32_t attachmentMatches=0;
    bool matchCountAdvanced=false,descriptorDeleteAdmitted=false,descriptorFreed=false,rowServiceReturned=false;
    std::optional<std::uint8_t> completionGuardRead;
    bool completionLazyStored=false,completionAssertionReturned=false;
    std::optional<std::uint32_t> completionAssertionRead;
    bool attachmentMatched=false,attachmentPhysics=false,childLookupPending=false,toyScanNullRead=false;
    std::optional<std::uint32_t> configTarget10,configLoaderTarget,configEmitterIndex;
    bool configEntered=false,configEmitterNonNull=false,configModelNonNull=false,configManagerRead=false;
    bool type0LoadEntered=false,type0NoOpReturned=false,type0ManagerStored=false,type0ModelStored=false,type0OffsetStored=false;
    bool type0DirectoryCopied=false,type0FilenameAppended=false;
    bool type1LoadEntered=false,type1NoOpReturned=false,type1ManagerStored=false,type1ModelStored=false,type1OffsetStored=false;
    bool type1DirectoryCopied=false,type1FilenameAppended=false;
    bool type2LoadEntered=false,type2NoOpReturned=false,type2ManagerStored=false,type2ModelStored=false,type2OffsetStored=false;
    bool type2DirectoryCopied=false,type2FilenameAppended=false;
    bool type9LoadEntered=false,type9NoOpReturned=false,type9ManagerStored=false,type9ModelStored=false,type9OffsetStored=false;
    bool type9DirectoryCopied=false,type9FilenameAppended=false;
    bool type11LoadEntered=false,type11NoOpReturned=false,type11ManagerStored=false,type11ModelStored=false,type11OffsetStored=false;
    bool type11DirectoryCopied=false,type11FilenameAppended=false;
    bool type12LoadEntered=false,type12NoOpReturned=false,type12ManagerStored=false,type12ModelStored=false,type12OffsetStored=false;
    bool type12DirectoryCopied=false,type12FilenameAppended=false;
    bool type8LoadEntered=false,type8NoOpReturned=false,type8ManagerStored=false,type8ModelStored=false,type8OffsetStored=false;
    bool type8DirectoryCopied=false,type8FilenameAppended=false;
    bool emitterConfigConstructAttempted=false,emitterConfigConstructReturned=false,emitterConfigOpenAttempted=false;
    std::optional<bool> emitterConfigOpenReturned;
    std::optional<std::uint32_t> emitterConfigTarget94;


} sourceParticleService124_;
//Distinct source call records for authored rows1..3. List-entry fields remain
//unset; completed row0 and each Config stay owned by their original occurrence.
std::array<SourceParticleService124,3> sourceParticleLaterRows_;
bool continueSourceParticleRowCreation27C81C(std::size_t,SourceParticleService124&,std::uint32_t&)noexcept;
bool continueSourceParticleRowAttachment27CC08(std::size_t,SourceParticleService124&,std::uint32_t&)noexcept;
bool continueSourceParticleService274370(std::uint32_t&)noexcept;
bool continueSourceParticleWorldReturn2743A0(std::uint32_t&)noexcept;
bool sourceCompletedParticleWorldCustodyCurrentNative()const noexcept;
bool continueSourceEmitterLoaderPrefixNative(std::size_t,SourceParticleService124&,std::uint32_t&)noexcept;
bool continueSourceEmitterConfig15AE00(std::size_t,SourceParticleService124&,std::uint32_t&)noexcept;
bool drawSourceParticleMode2Native(std::size_t,std::size_t,a::OriginalParticleSourceConfig50LA&,std::uint32_t&)noexcept;
std::optional<std::size_t> sourceNonFogPrecedingFogRowNative(std::size_t)const noexcept;
bool sourceNonFogAfterFogCustodyCurrentNative(std::size_t)const noexcept;
bool withdrawSourceParticleEmitterBorrowsNative()noexcept;






enum class SourceStaticPhase:std::uint8_t {unreached,nativeAdmitted,entered,bounded,prefixComplete,continuing,returned,nativeClosed};
SourceStaticPhase sourceStaticPhase_=SourceStaticPhase::unreached;
std::uint32_t sourceStaticPC_=0x2710f0;
//Actual retained first2710F0/27B740 frame lanes. No loop advancement yet.
std::uint32_t sourceStaticZoneIndex_=0,sourceStaticModelIndex_=0,sourceStaticPhysicsCount_=0;
std::uint32_t sourceStaticPhysicsCountZone_=0; //Native identity of the actual current/last S1 count frame.
std::uint32_t sourceStaticSucceededZones_=0,sourceStaticZonePointerOffset_=0,sourceStaticZoneByteOffset_=0,sourceStaticSlotByteOffset_=0;
g::OriginalWorldPhysicsIndex50LA* sourceStaticPhysicsArg_=nullptr;
rr::platform::OriginalWorldSourceConfig50LA* sourceStaticConfigArg_=nullptr;
const void* sourceStaticZoneArg_=nullptr;
std::optional<std::uint32_t> sourceStaticResolvedType_,sourceStaticFactoryVtable_;
bool sourceWorldStaticModelsInvocationCurrent()const noexcept; //Parent exact v88 gate.
bool loadSourceWorldStaticModels2710F0(std::uint32_t&)noexcept;
bool preflightSourceWorldStaticModelsNative()const noexcept;
bool abortSourceWorldStaticModelsNative(std::uint32_t&)noexcept;
bool constructSourcePhysicsWrapper27BA3C(std::size_t,std::size_t,std::uint32_t&)noexcept;
bool loadSourcePhysicsDescriptor27B370(std::size_t,std::size_t,std::uint32_t&)noexcept;
bool loadSourceFirstStaticDescriptor27AB60(std::uint32_t&)noexcept;
//Include original_source_world_static_models50la.inl after complete class.
//Native cleanup sourceStaticOwner before Instance/Physics/Zone/Config/File.

//Extend existing SourceStaticPhase enum with continuing and returned (keep existing prefixComplete).
std::uint32_t sourceStaticZoneArgIndex_=0; //Exact last271194 argument producer, may precede current outerindex on partial failure.
//Same sourceStaticOwner_ and retained actual frame fields; no duplicate owners/lanes.
bool sourceWorldStaticCohortInvocationCurrent()const noexcept; //Parent same current first-prefix caller.
bool continueSourceWorldStaticModels27B89C(std::uint32_t&)noexcept;
bool loadSourceStaticDescriptor27AB60(std::size_t,std::size_t,bool,std::uint32_t&)noexcept;
//Include original_source_world_static_cohort50la.inl after existing source static fragment.
//SourcePhase returned PC270D0C is actual v88 return, NOT legacy constructed_ or image/Scene readiness.

//Private Owner declarations. Include original_source_world_collision50la.inl
//after aggregate complete class, Config collision fragment, and cold prefix.
enum class SourceCollisionPhase {unreached,nativeAdmitted,entered,bounded,returned,nativeClosed};
SourceCollisionPhase sourceCollisionPhase_=SourceCollisionPhase::unreached;
std::uint32_t sourceCollisionPC_=0,sourceCollisionResolvedType_=0;
std::size_t sourceCollisionZoneIndex_=0,sourceCollisionObjectIndex_=0,sourceCollisionSucceeded_=0;
const rr::platform::OriginalWorldSourceConfig50LA* sourceCollisionConfigArg_=nullptr;
const SourceZone275C50* sourceCollisionZoneArg_=nullptr;
std::size_t sourceCollisionZoneArgIndex_=10;
bool sourceWorldCollisionInvocationCurrent()const noexcept; //Parent exact caller token predicate.
bool loadSourceWorldCollisionObjects1B4EC0(std::uint32_t&)noexcept;
bool loadSourceCollisionDescriptor1B5510(std::size_t,std::size_t,std::uint32_t&)noexcept;
bool preflightSourceWorldCollisionNative()const noexcept;
bool abortSourceWorldCollisionNative(std::uint32_t&)noexcept;

//Private aggregate Owner fields/methods. Existing worldRoles4DCTo4EC_ remains
//the sole actual source pointer lane; these are backing and reached-state only.
struct SourceController20 {
    std::array<std::uint32_t,5> words;
    std::uint8_t knownMask=0;
};
enum class SourceControllerPhase:std::uint8_t {unreached,entered,bounded,returnedControllers,nativeClosed};
SourceControllerPhase sourceControllerPhase_=SourceControllerPhase::unreached;
std::array<std::unique_ptr<SourceController20>,5> sourceControllers_;
std::array<bool,5> sourceControllerAllocationReturned_{};
std::uint8_t sourceControllerPublishedMask_=0;
bool sourceControllerBranchKnown_=false,sourceControllerBranchTaken_=false;
std::uint32_t sourceControllerPC_=0;
const g::OriginalApplicationEntities50LA* sourceControllerApplication_=nullptr;
std::uint64_t sourceControllerGeneration_=0;
bool sourceWorldTailInvocationCurrent()const noexcept; //Parent exact Race/World/Config/v8C-return gate.
bool loadSourceWorldControllers270D1C(std::uint32_t&)noexcept;
bool preflightSourceWorldControllersNative()const noexcept;
bool abortSourceWorldControllersNative(std::uint32_t&)noexcept;

//Sole actual World368 source lane, unknown until real158BDC output store.
std::optional<std::uint32_t> sourceAmbientFlashMinimum368_;
bool finishSourceWorld270E80(std::uint32_t&)noexcept;

//Actual distinct1B4330 Config occurrence. Closed SCB wrapper remains in
//sourceWorldConfig_ for historical descriptor argument identity, never reused.
std::unique_ptr<rr::platform::OriginalWorldSourceConfig50LA> sourceWorldObjectConfig_;
bool sourceSceneConfigCloseAttempted_=false,sourceSceneConfigCloseReturned_=false;
bool sourceSceneConfigNativeWithdrawn_=false,sourceObjectConfig34Published_=false;
enum class SourceObjectPhase:std::uint8_t {unreached,entered,allocated,opened,bounded,returnedZero,returnedOne,nativeClosed};
SourceObjectPhase sourceObjectPhase_=SourceObjectPhase::unreached;
std::uint32_t sourceObjectPC_=0;
std::array<char,260> sourceObjectToyPath_,sourceObjectBinaryPath_;
std::size_t sourceObjectToyPathKnown_=0,sourceObjectBinaryPathKnown_=0;
std::optional<std::uint8_t> sourceObjectProfileByte1125C_;
std::optional<std::uint32_t> sourceObjectReturnValue_;
bool sourceObjectLogAttempted_=false,sourceObjectLogMissing_=false,sourceObjectLogNativeError_=false;
std::size_t sourceObjectLogRequested_=0,sourceObjectLogWritten_=0;
bool closeSourceSceneConfig271C10(std::uint32_t&)noexcept;
bool sourceObjectConfigCloseAttempted_=false,sourceObjectConfigCloseReturned_=false,sourceObjectConfigNativeWithdrawn_=false;
bool closeSourceObjectConfig272800(std::uint32_t&)noexcept;
bool loadSourceWorldObjects1B4330(std::uint32_t&)noexcept;
bool writeSourceObjectDiagnostic(bool,std::uint32_t&)noexcept;
//Actual distinct AUB Config; neither closed TOB nor native authoring is adopted.
std::unique_ptr<rr::platform::OriginalWorldSourceConfig50LA> sourceWorldAudioConfig_;
std::optional<const void*> sourceAudioParser38_;
enum class SourceAudioPhase:std::uint8_t {unreached,entered,allocated,opened,bounded,returnedZero,returnedOne,nativeClosed};
SourceAudioPhase sourceAudioPhase_=SourceAudioPhase::unreached;
std::uint32_t sourceAudioPC_=0;
bool sourceAudioConfig38Published_=false;
std::array<char,260> sourceAudioTextPath_,sourceAudioBinaryPath_;
std::size_t sourceAudioTextPathKnown_=0,sourceAudioBinaryPathKnown_=0;
std::optional<std::uint32_t> sourceAudioReturnValue_;
bool sourceAudioLogAttempted_=false,sourceAudioLogMissing_=false,sourceAudioLogNativeError_=false;
std::size_t sourceAudioLogRequested_=0,sourceAudioLogWritten_=0;
bool loadSourceWorldAudio1B4720(std::uint32_t&)noexcept;
bool writeSourceAudioDiagnostic(bool,std::uint32_t&)noexcept;
bool preflightSourceWorldAudioNative()const noexcept;
bool abortSourceWorldAudioNative(std::uint32_t&)noexcept;


    bool sourceWorldZonesInvocationCurrent()const noexcept;
    bool loadSourceWorldZones270B58(std::uint32_t&)noexcept;
    bool loadSourceWorldZone275E30(std::size_t,std::uint32_t&)noexcept;
    bool preflightSourceWorldZonesNative()const noexcept;
    bool abortSourceWorldZonesNative(std::uint32_t&)noexcept;





// Inside ra_cold_aggregate50LA::Owner private area. Include fragment only after
// complete Application, Config, base Owner and aggregate definitions.
struct SourceWorldBoundary348 final {
    std::array<std::uint32_t,4> position00,direction10;
    std::uint32_t vtable20;
    // Source24..2F remain unknown; native struct size is not guest allocation size.
};
std::unique_ptr<SourceWorldBoundary348> sourceBoundary348_;
enum class SourceBoundaryPhase:std::uint8_t {unreached,entered,bounded,returnedZero,returnedOne,nativeClosed};
SourceBoundaryPhase sourceBoundaryPhase_=SourceBoundaryPhase::unreached;
std::uint32_t sourceBoundaryPC_=0x270a50;
bool sourceBoundaryPublished_=false;
// Parent implements exact reached270A50 after four first SceneInfo getters,
// same actual Race/World/Config and caller token. Not a public readiness setter.
bool sourceWorldBoundaryInvocationCurrent()const noexcept;
bool allocateLoadSourceWorldBoundary272970(std::uint32_t&)noexcept;
bool preflightSourceWorldBoundaryNative()const noexcept;
bool abortSourceWorldBoundaryNative(std::uint32_t&)noexcept;
// Parent must abort this payload BEFORE Config/File/namespace withdrawal and
// before generic base topology cleanup withdraws worldBoundary348_ knowledge.

    bool abortSourceTrackNative(std::uint32_t&)noexcept;
    bool loadSourceWorld1B3880(std::uint32_t&)noexcept;
    bool nativeTrackArchiveCurrent(const original_main_entry50KG::Application&)const noexcept;
    bool bindNativeTrackScb34(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    // Immutable native authoring aliases/read results on this SAME World.
    // Not source live-field shadows:35C may already be1 from actual1B4E20.
    // Native collision/visibility callbacks remain replacements. The private
    //270CC4/270CDC/27C110 physics-index producer now owns actual3D4 lanes.
    struct NativeSceneInfo270900 {
        enum class Read:std::uint8_t {sceneName,portals,zones,environmentNode,roles};
        const a::ParameterDocument48F* document=nullptr;
        const a::ParameterBlock48F* sceneInfo=nullptr;
        const a::ParameterField48F* name244=nullptr;
        std::int32_t authoredPortals358=0,authoredZones34C=1,authoredUseEnvironment35C=0;
        Read next=Read::sceneName;
        const track_runtime_storage50IG::Storage* physicsCollisionBackend=nullptr;
        const light::Owner* environmentVisibilityBackend=nullptr;
        bool nativeRolesAccepted=false;
    };
    std::optional<NativeSceneInfo270900> nativeSceneInfo270900_;
    bool publishNativeModelParents2710F0(std::uint32_t&)noexcept;
    bool continueNativeSceneInfo270900(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool nativeSceneInfo270900Current(const original_main_entry50KG::Application&,std::uint32_t&)const noexcept;
    //The same temporary World34 role is SCB first, then TOB. These optional
    //native views are mutually exclusive; immutable decoded backing outlives
    //either role. Neither wrapper is another parsed document or scene.
    struct NativeTobReader34 {
        enum class Next:std::uint8_t {load,podDust,podCache,rain,animations,complete};
        std::shared_ptr<const ra_cold_sources50LA::Owner> source;
        std::shared_ptr<a::OriginalStandardArchiveProvider50LA> provider;
        const ra_particle_authoring50LA::Owner* authoring=nullptr;
        a::OriginalStandardArchiveProvider50LA::MemberView member{};
        const a::ParameterDocument48F* document=nullptr;
        Next next=Next::load;
    };
    std::optional<NativeTobReader34> nativeTob34_;
    struct NativeAubReader38 {
        std::shared_ptr<const ra_cold_sources50LA::Owner> source;
        std::shared_ptr<a::OriginalStandardArchiveProvider50LA> provider;
        const ra_audio_authoring50LA::Owner* authoring=nullptr;
        std::string memberName;
        a::OriginalStandardArchiveProvider50LA::MemberView member{};
        const a::ParameterDocument48F* document=nullptr;
    };
    std::optional<NativeAubReader38> nativeAub38_;
    bool withdrawNativeTob34_272800(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool bindNativeAub38_1B4720(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    struct NativeEmptyAmbient3B4 {
    bool sourceMode=false,sourceCounted=false,sourceEnrolled=false;
    std::uint8_t sourceConstructorMask=0;
    std::optional<std::uint32_t> sourceVtableActual08;

        Entity entity;
        const std::uint32_t sourceVtable08=0x3d2980;
        std::int32_t count30=0;
        std::nullptr_t rows34=nullptr;
    };
    //Exclusive native first-entry metadata domain, NOT a270550 reset receipt.
    std::unique_ptr<NativeEmptyAmbient3B4> nativeEmptyAmbient3B4_;
    bool nativeEmptyAmbient271470Complete_=false;
    enum class SourceAmbientPhase {unreached,entered,bounded,returnedZero,nativeClosed};
    SourceAmbientPhase sourceAmbientPhase_=SourceAmbientPhase::unreached;
    std::optional<NativeEmptyAmbient3B4*> sourceAmbient3B4_;
    const rr::platform::OriginalWorldSourceConfig50LA* sourceAmbientConfigArg_=nullptr;
    bool sourceAmbientPublished_=false,sourceAmbientLoaderReturned_=false;
    bool sourceAmbientGetterReturned_=false,sourceAmbientDestructorReturned_=false;
    std::uint32_t sourceAmbientPC_=0;
    bool sourceWorldAmbientInvocationCurrent()const noexcept; //parent
    bool loadSourceWorldAmbient271470(std::uint32_t&)noexcept;
    bool preflightSourceWorldAmbientNative()const noexcept;
    bool abortSourceWorldAmbientNative(std::uint32_t&)noexcept;

    bool loadNativeEmptyAmbient271470(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    struct NativeSoulTreeSoundDescriptor278C00 {
    struct SourceTag final {};
    explicit NativeSoulTreeSoundDescriptor278C00(SourceTag)noexcept:resource00(),attach1D0(){}
    bool sourceMode=false,sourceConstructorReturned=false,sourceLoaded=false;
    std::array<char,260> sourceResource00,sourceAttach1D0;
    std::optional<std::size_t> sourceResourceLength,sourceAttachLength;
    std::array<std::uint32_t,184> sourceWords;
    std::array<std::uint8_t,184> sourceKnown{};

        const std::uint32_t sourceVtable1C4=0x3d2a60;
        std::string resource00="none",attach1D0="body";
        //278750 copies unknown current442F80.158C00 replaces only XYZ,
        //NOT W; no fabricated zero/one fourth component is introduced.
        std::optional<std::array<float,3>> position110;
        std::optional<std::uint32_t> positionW11C;
        float max120=0,min124=0,oneShotDistance13C=0,randomTriggerPercent140=0;
        std::int32_t looping128=0,oneShot12C=0,reset130=0,force2D134=0,stereo138=0;
        //Source memset150..1BF. Entries become unknown only where copied
        //from unproven global W; all other initialized bytes stay known.
        std::array<std::optional<std::uint32_t>,28> state150;
        std::int32_t priority1C0=10;
        const a::ParameterBlock48F* actualSource=nullptr;
        bool loaded=false;
        NativeSoulTreeSoundDescriptor278C00(){for(auto& word:state150)word=0u;}
    };
    struct NativeSoulTreeSoundRow278DD0 {
    bool sourceMode=false,sourceDescriptorAttempted=false,sourceDescriptorPublished=false;
    bool sourceCallReturned=false,sourceCallValue=false;
    std::uint8_t sourceConstructorMask=0;
    std::optional<std::uint32_t> sourceVtableActual00;
    std::optional<NativeSoulTreeSoundDescriptor278C00*> sourceDescriptor08;

        const std::uint32_t sourceVtable00=0x3d2a30;
        std::nullptr_t runtime04=nullptr; //278E80/103E20 has NOT run.
        std::unique_ptr<NativeSoulTreeSoundDescriptor278C00> descriptor08;
    };
    struct NativeSoulTreeSoundList3B8 {
    bool sourceMode=false,sourceEnrolled=false,sourceConstructorReturned=false;
    bool sourceArrayAttempted=false,sourceArrayPublished=false;
    std::uint8_t sourceConstructorMask=0;
    std::optional<std::uint32_t> sourceVtableActual08;
    std::optional<NativeSoulTreeSoundRow278DD0*> sourceRows34;
    std::size_t sourceRowsConstructed=0,sourceLoaded=0;
    std::optional<std::uint32_t> sourceArrayHeaderStride00,sourceArrayHeaderCount04;

        Entity entity;bool counted=false,complete=false;
        const std::uint32_t sourceVtable08=0x3d2880;
        std::int32_t count30=0;
        std::unique_ptr<NativeSoulTreeSoundRow278DD0[]> rows34;
        const ra_audio_authoring50LA::Owner* actualOwner=nullptr;
        std::size_t loaded=0;
    };
    std::unique_ptr<NativeSoulTreeSoundList3B8> nativeSoulTreeSounds3B8_;
    enum class SourceSoulTreeSoundPhase {unreached,entered,bounded,returned,nativeClosed};
    SourceSoulTreeSoundPhase sourceSoulTreeSoundPhase_=SourceSoulTreeSoundPhase::unreached;
    std::optional<NativeSoulTreeSoundList3B8*> sourceSoulTreeSounds3B8_;
    const rr::platform::OriginalWorldSourceConfig50LA* sourceSoulTreeSoundConfigArg_=nullptr;
    bool sourceSoulTreeSoundPublished_=false,sourceSoulTreeSoundCountReturned_=false;
    std::optional<std::uint32_t> sourceSoulTreeSoundIndexS0_,sourceSoulTreeSoundOffsetS1_;
    std::array<char,272> sourceSoulTreeSoundName50_;
    std::size_t sourceSoulTreeSoundNameProduced_=0;
    std::uint32_t sourceSoulTreeSoundPC_=0;
    bool sourceWorldSoulTreeSoundsInvocationCurrent()const noexcept; //parent
    bool loadSourceWorldSoulTreeSounds2714D8(std::uint32_t&)noexcept;
    bool preflightSourceWorldSoulTreeSoundsNative()const noexcept;
    bool abortSourceWorldSoulTreeSoundsNative(std::uint32_t&)noexcept;
    bool constructSourceSoulTreeDescriptor278C00(NativeSoulTreeSoundDescriptor278C00&,
        const std::array<std::uint32_t,4>&,std::uint32_t&)noexcept;
    bool loadSourceSoulTreeDescriptor278D00(NativeSoulTreeSoundDescriptor278C00&,
        const std::array<std::uint32_t,4>&,bool&,std::uint32_t&)noexcept;

    bool loadNativeSoulTreeSoundDescriptors2714D8(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool canRetireNativeSoulTreeSounds3B8()const noexcept;
    bool retireNativeSoulTreeSounds3B8()noexcept;
    struct NativeMaterialSoundDescriptor {
    struct SourceTag final {};
    explicit NativeMaterialSoundDescriptor(SourceTag)noexcept:resource00(){}
    bool sourceMode=false,sourceConstructorReturned=false,sourceCallReturned=false,sourceCallValue=false;
    std::array<char,260> sourceResource00;
    std::optional<std::size_t> sourceResourceLength;
    std::array<std::uint32_t,124> sourceWords;
    std::array<std::uint8_t,124> sourceKnown{};

        std::uint32_t sourceVtable1C4=0x3d2a90; //278650 common, drive278950 replaces3D2A80.
        std::string resource00="none";
        std::array<std::optional<std::uint32_t>,4> position110{}; //Unknown current442F80, allXYZW.
        float max120=0,min124=0,oneShotDistance13C=0,randomTriggerPercent140=0;
        std::int32_t looping128=0,oneShot12C=0,reset130=0,force2D134=0,stereo138=0;
        std::array<std::optional<std::uint32_t>,28> state150;
        std::int32_t priority1C0=10;
        //Reached only for drive rows; source constructor278950 initializes all.
        std::array<std::int32_t,5> drive1D0{};
        float slope1E4=0;
        const a::ParameterBlock48F* actualSource=nullptr;
        const std::string* actualCatalogName=nullptr; //Existing native .vag mapping, NOT source.wav.
        const g::MaterialDriveParameters50HI* actualDrive=nullptr;
        bool loaded=false;
        NativeMaterialSoundDescriptor(){for(auto& word:state150)word=0u;}
    };
    struct SourceMaterialSoundArray final {
        std::array<NativeMaterialSoundDescriptor,12> rows;
        SourceMaterialSoundArray()noexcept:rows{{
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{}),
            NativeMaterialSoundDescriptor(NativeMaterialSoundDescriptor::SourceTag{})}}{}
    };
    struct NativeMaterialSoundList {
    bool sourceMode=false,sourceEnrolled=false,sourceConstructorReturned=false;
    bool sourceArrayAttempted=false,sourceArrayPublished=false,sourceCountReturned=false;
    std::uint8_t sourceConstructorMask=0;
    std::optional<std::uint32_t> sourceVtableActual08;
    std::unique_ptr<SourceMaterialSoundArray> sourceArray;
    std::optional<NativeMaterialSoundDescriptor*> sourceRows34;
    std::size_t sourceRowsConstructed=0,sourceLoaded=0;
    std::optional<std::uint32_t> sourceArrayHeaderStride00,sourceArrayHeaderCount04;

        Entity entity;bool counted=false,complete=false,drive=false;
        std::uint32_t sourceVtable08=0x3d2800;
        std::int32_t count30=0;
        std::unique_ptr<NativeMaterialSoundDescriptor[]> rows34;
        const ra_audio_authoring50LA::Owner* actualOwner=nullptr;
        const a::MaterialAudioCatalog50HI* actualCatalog=nullptr;
        std::size_t loaded=0;
    };
    //Two distinct counted source lists, not a shared/deduplicated Sound owner.
    std::array<std::unique_ptr<NativeMaterialSoundList>,2> nativeMaterialSounds3BC_;
    enum class SourceMaterialSoundPhase {unreached,entered,bounded,returned,nativeClosed};
    SourceMaterialSoundPhase sourceMaterialSoundPhase_=SourceMaterialSoundPhase::unreached;
    std::array<std::optional<NativeMaterialSoundList*>,2> sourceMaterialSounds3BC_;
    std::array<bool,2> sourceMaterialSoundPublished_{},sourceMaterialSoundNativeWithdrawn_{};
    const rr::platform::OriginalWorldSourceConfig50LA* sourceMaterialSoundConfigArg_=nullptr;
    std::optional<std::uint32_t> sourceMaterialSoundKind_,sourceMaterialSoundIndexS0_,sourceMaterialSoundOffsetS1_;
    std::array<char,272> sourceMaterialSoundName50_;
    std::size_t sourceMaterialSoundNameProduced_=0;
    std::uint32_t sourceMaterialSoundPC_=0;
    bool sourceWorldMaterialSoundsInvocationCurrent()const noexcept; //parent
    bool loadSourceWorldMaterialSounds271530(std::uint32_t&)noexcept;
    bool preflightSourceWorldMaterialSoundsNative()const noexcept;
    bool abortSourceWorldMaterialSoundsNative(std::uint32_t&)noexcept;
    bool constructSourceMaterialSoundDescriptor278650(NativeMaterialSoundDescriptor&,bool,
        const std::array<std::uint32_t,4>&,std::uint32_t&)noexcept;
    bool loadSourceMaterialSoundDescriptor2787B0(NativeMaterialSoundDescriptor&,bool,
        const std::array<std::uint32_t,4>&,bool&,std::uint32_t&)noexcept;

    bool loadNativeMaterialSoundDescriptors271530(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool canRetireNativeMaterialSounds3BC()const noexcept;
    bool retireNativeMaterialSounds3BC()noexcept;
    enum class SourceMethodArraysPhase:std::uint8_t {unreached,entered,bounded,returned,nativeClosed};
SourceMethodArraysPhase sourceMethodArraysPhase_=SourceMethodArraysPhase::unreached;
std::optional<std::uint32_t> sourceMethodS0_;
std::array<std::optional<NativeListMethodCustody**>,4> sourceMethodArrays_;
std::uint8_t sourceMethodArrayAllocationMask_=0;
std::array<std::uint8_t,10> sourceMethodSlotNullMask_{};
std::array<std::uint8_t,10> sourceMethodReturnedMask_{};
std::array<std::array<std::optional<std::int32_t>,10>,4> sourceMethodSelectors_;
std::uint32_t sourceMethodArraysPC_=0x272c90;
bool sourceWorldMethodArraysInvocationCurrent()const noexcept;
bool preflightSourceWorldMethodArraysNative()const noexcept;
bool continueSourceWorldMethodArrays272C90(std::uint32_t&)noexcept;
bool abortSourceWorldMethodArraysNative(std::uint32_t&)noexcept;

enum class SourcePortalGraphPhase:std::uint8_t {unreached,entered,bounded,returned,nativeClosed};
    SourcePortalGraphPhase sourcePortalGraphPhase_=SourcePortalGraphPhase::unreached;
    std::optional<std::uint32_t> sourcePortalGraphS0_;
    std::array<std::uint8_t,10> sourcePortalCallerMask_{};
    std::array<std::array<std::array<std::optional<SearchNode*>,2>,4>,10> sourcePortalLinkArguments_;
    std::array<std::array<std::optional<SearchNode**>,4>,10> sourcePortalLinkArrays_;
    std::uint32_t sourcePortalGraphPC_=0x272b28;
    bool sourcePortalParameterFreeReached_=false,sourcePortalParameterNullStored_=false;
    bool sourceWorldPortalGraphInvocationCurrent()const noexcept;
    bool sourcePortalTailCurrent148D00(std::size_t,const g::OriginalWreckComponent50LA*,
        const g::OriginalWreckComponent50LA*,std::uint32_t,std::uint32_t)const noexcept;
    bool preflightSourceWorldPortalGraphNative()const noexcept;
    bool sourceWorldPortalGraphCounted(std::size_t&)const noexcept;
    bool abortSourceWorldPortalGraphNative(std::uint32_t&)noexcept;
    bool continueSourceWorldPortalGraph272B28(std::uint32_t&)noexcept;

    enum class SourceTopologyPhase {unreached,entered,bounded,lightZonesReturned,nodesReturned,portalsReturned,methodsReturned,observerReturned,nativeClosed};
    std::optional<std::uint32_t> sourceDefaultEnvironment35C_;
    std::optional<std::uint32_t> sourceTopologyReturnV0_;
    bool sourceTopologyRoutineReturned_=false;
    enum class SourceLightPopulationPhase {unreached,entered,bounded,wrapperPending,returned,nativeClosed};
    SourceLightPopulationPhase sourceLightPopulationPhase_=SourceLightPopulationPhase::unreached;
    std::uint32_t sourceLightPopulationPC_=0x272f94;
    std::optional<std::int32_t> sourceLightPopulationS0_,sourceLightPopulationCount_,sourceLightPopulationZone_;
    NativeLightWrapper277C10* sourceLightPopulationWrapper_=nullptr;
    SourceLightDescriptor277220* sourceLightPopulationDescriptor_=nullptr;
    std::uint8_t sourceLightAssertionGuard_=0,sourceLightAssertionEnabled_=0,sourceLightAssertionMask_=0;
    bool sourceLightAssertionPassed_=false;
    std::optional<std::int32_t> sourceLightPopulationItemsEnd_; //Historical185 before original S0 is reused for zones.
    std::uint16_t sourceLightZoneReadMask_=0,sourceLightZoneSkippedMask_=0,sourceLightZoneAppliedMask_=0;
    std::optional<std::uint32_t> sourceLightPopulationReturnV0_;
    bool preflightSourceTypedPopulationLoopNative()const noexcept;
    bool continueSourceTypedLightIteration273BA8(std::size_t,std::uint32_t&)noexcept;
    bool sourceFirstDescriptorDisposed_=false,sourceFirstWrapperReturned_=false;
    std::optional<std::uint32_t> sourceFirstWrapperV0_;

    enum class SourceSecondLightPhase {unreached,entered,bounded,published,nativeClosed};
    SourceSecondLightPhase sourceSecondLightPhase_=SourceSecondLightPhase::unreached;
    std::uint32_t sourceSecondLightPC_=0x273ba8;
    NativeLightWrapper277C10* sourceSecondLightWrapper_=nullptr;
    SourceLightDescriptor277220* sourceSecondLightDescriptor_=nullptr;
    std::optional<std::int32_t> sourceSecondLightZone_;
    const char* sourceSecondLensStack00_=nullptr;
    std::optional<std::int32_t> sourceSecondOrdinalStack08_;
    bool sourceSecondArgumentsBorrowed_=false;
    bool sourceSecondDescriptorDisposed_=false,sourceSecondWrapperReturned_=false;
    std::optional<std::uint32_t> sourceSecondWrapperV0_;
    std::uint8_t sourceSecondGuard_=0,sourceSecondEnabled_=0,sourceSecondGuardMask_=0;
    bool sourceSecondAssertionPassed_=false;
    bool continueSourceTypedLight273BA8(std::size_t,std::uint32_t&)noexcept;
    bool configureSourceSpotlight146F60(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool registerSourceSpotlight148F30(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool continueSourceLaterAmbient146F60(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool retireAndAttachSourceTypedWrapper2780AC(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool continueSourceSecondLight273BA8(std::uint32_t&)noexcept;
    bool preflightSourceSecondLightFrameNative()const noexcept;
    bool abortSourceSecondLightFrameNative()noexcept;
    bool continueSourceLightPopulation1AA144(std::uint32_t&)noexcept;
    bool preflightSourceLightPopulationNative()const noexcept;
    bool abortSourceLightPopulationNative(std::uint32_t&)noexcept;
    bool continueSourceDefaultObserver272F28(std::uint32_t&)noexcept;
    bool preflightSourceDefaultObserverNative()const noexcept;
    bool abortSourceDefaultObserverNative(std::uint32_t&)noexcept;
    SourceTopologyPhase sourceTopologyPhase_=SourceTopologyPhase::unreached;
    std::unique_ptr<g::OriginalWreckComponent50LA> sourceTopologyContainer_;
    bool sourceTopologyContainerCounted_=false,sourceTopologyContainerEnrolled_=false;
    bool sourceTopologyContainerPublished_=false,sourceTopologyVisibilityPublished_=false,sourceTopologyCollisionPublished_=false;
    std::shared_ptr<rr::platform::OriginalUiLightManagerOwner50KG> sourceTopologyLightManager_;
    std::uint32_t sourceTopologyPC_=0;
    bool sourceWorldLightZonesInvocationCurrent(const rr::platform::OriginalUiLightManagerOwner50KG&)const noexcept;
    bool preflightSourceWorldTopologyNative()const noexcept;
    bool preflightSourceWorldTopologyCountNative()const noexcept;
    bool abortSourceWorldTopologyNative(std::uint32_t&)noexcept;
    bool loadSourceWorldTopology2729E0(std::uint32_t&)noexcept;

// Aggregate-private additions. Index0 visibility,1 collision; no legacy inline owner adoption.
enum class SourceSearchNodesPhase:std::uint8_t {unreached,entered,bounded,returned,nativeClosed};
SourceSearchNodesPhase sourceSearchNodesPhase_=SourceSearchNodesPhase::unreached;
std::array<std::array<SourceSearchNode27F120,10>,2> sourceSearchNodes_;
std::optional<std::uint32_t> sourceSearchNodeS0_;
std::uint32_t sourceSearchNodesPC_=0x272a50;
const SourceZone275C50* sourceSearchZonesIdentity_=nullptr;
std::optional<std::int32_t> sourceSearchExclusionCount_;
const std::int32_t* sourceSearchExclusionArg_=nullptr; //Historical after withdrawal; never dereferenced then.
std::optional<std::uint32_t> sourceSearchHelperA2_,sourceSearchHelperA3_,sourceSearchHelperSourceOffset_;
std::optional<std::uint32_t> sourceSearchLoadedWord_;
bool sourceSearchStorePending_=false;
bool sourceWorldSearchNodesInvocationCurrent()const noexcept; //Parent owns sameWorld/manager/350 gate.
bool continueSourceWorldSearchNodes272A50(std::uint32_t&)noexcept;
bool preflightSourceWorldSearchNodesNative()const noexcept;
bool sourceWorldSearchNodesCounted(std::size_t&)const noexcept;
bool abortSourceWorldSearchNodesNative(std::uint32_t&)noexcept;

    enum class SourceMaterialAssignmentPhase {unreached,entered,bounded,returned,nativeClosed};
    SourceMaterialAssignmentPhase sourceMaterialAssignmentPhase_=SourceMaterialAssignmentPhase::unreached;
    struct SourceMaterialAssignmentFrame {
        std::optional<std::int32_t> materialCount284,defaultMaterial288,objectCount28C,s0,s1;
        std::array<char,272> name80;std::size_t nameKnown=0;
    };
    std::optional<SourceMaterialAssignmentFrame> sourceMaterialAssignmentFrame_;
    const rr::platform::OriginalWorldSourceConfig50LA* sourceMaterialAssignmentConfigArg_=nullptr;
    std::uint32_t sourceMaterialAssignmentPC_=0;
    bool sourceAudioBodyReturned_=false;std::optional<std::uint32_t> sourceAudioBodyV0_;
    bool loadSourceWorldMaterialAssignments271600(std::uint32_t&)noexcept;
    bool preflightSourceWorldMaterialAssignmentsNative()const noexcept;
    bool abortSourceWorldMaterialAssignmentsNative(std::uint32_t&)noexcept;
    bool sourceAudioCloseAttempted_=false,sourceAudioCloseReturned_=false,sourceAudioCloseNativeWithdrawn_=false;
    const void* sourceAudioRetiredAllocation_=nullptr;
    bool closeSourceAudioConfig272930(std::uint32_t&)noexcept;

    struct NativeMaterialAssignmentLocals271600 {
        std::int32_t materialCount284=0,defaultMaterial288=0,objectCount28C=0;
        bool complete=false;
    };
    std::optional<NativeMaterialAssignmentLocals271600> nativeMaterialAssignments271600_;
    bool completeNativeAudio2728E0(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool withdrawNativeAub38_272930(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool nativeAubRoleRetired272930_=false; //NOT a sourceWorld38null store.
    std::shared_ptr<rr::platform::OriginalUiLightManagerOwner50KG> nativeRaLightZones_;
    bool nativeZoneTopologyPrefix272A48_=false;
    // Explicit native descriptor adoption at actual+110; not source270B10 replay.
    struct NativeFogDescriptor3AC {
bool sourceMode=false,sourceConstructed=false,sourcePublished=false;
const g::OriginalApplicationEntities50LA* sourceApplication=nullptr;
std::uint64_t sourceGeneration=0;
std::thread::id sourceThread;
std::array<std::uint32_t,4> sourceColor00;
std::uint32_t sourceVisibility10,sourceHaziness14,sourceVtable18;

        std::shared_ptr<const ra_cold_sources50LA::Owner> source;
        const rr::assets::ParameterBlock48F* block=nullptr;
    };
    std::unique_ptr<NativeFogDescriptor3AC> nativeFogDescriptor3AC_;
    std::optional<NativeFogDescriptor3AC*> sourceFog3AC_; // Unknown until actual2705A0.
    enum class SourceFogPhase:std::uint8_t {unreached,entered,bounded,returnedZero,returnedOne,nativeClosed};
    SourceFogPhase sourceFogPhase_=SourceFogPhase::unreached;
    std::uint32_t sourceFogPC_=0x270b14;

    //Fresh source variant, mutually exclusive with native CameraBinding
    //adoption. These are actual owned output resources, not copied fog values.
    struct SourceFogObject13E6B0 : NativeFogEntity {
        std::array<std::optional<std::uint32_t>,14> words30To64;
        std::unique_ptr<NativeFogBuffer133F80> buffer5C;
        std::optional<NativeFogBuffer133F80*> published5C;
        std::uint32_t vtable08=0;bool bufferAttempted=false;
    };
    struct SourceFogTransfer273C30 {
        NativeFogDescriptor3AC* descriptor=nullptr;
        std::unique_ptr<SourceFogObject13E6B0> fog;
        std::unique_ptr<NativeFogEntity> off,on;
        std::optional<SourceFogObject13E6B0*> world3EC;
        std::optional<NativeFogEntity*> world3F4,world3F0;
        std::uint32_t offVtable08=0,onVtable08=0;
        std::optional<std::uint32_t> packedColor;
        std::shared_ptr<rr::platform::OriginalUiGarageFogColorLease50KG> renderer;
        std::optional<SourceFogObject13E6B0*> onFog30;
        bool rendererStored=false,offInitialized=false,onInitialized=false,
            descriptorFreed=false,descriptorNullStored=false;
        bool returned=false,nativeCancelled=false;
        std::uint32_t pc=0x273c30;
    };
    std::unique_ptr<SourceFogTransfer273C30> sourceFogTransfer_;
    const g::OriginalWorldFogInvocation50LA* sourceFogTransferInvocation_=nullptr;
    std::size_t sourceFogTransferBorrowers_=0;
    bool beginSourceFog273C30(const g::OriginalWorldFogInvocation50LA&,std::uint32_t&)noexcept;
    bool abortSourceFogTransferNative(std::uint32_t&,const g::OriginalWorldFogInvocation50LA* = nullptr)noexcept;
    static bool releaseSourceFogInvocationNative(const void*,const g::OriginalWorldFogInvocation50LA&)noexcept;
    bool nativeFogLoadAttempted_=false,nativeFogLoadComplete_=false;
    bool nativeGeometryDirectory1AA1DC_=false;
    std::size_t nativeStandardZoneNext_=0,nativeStandardSlotNext_=0;
    std::size_t nativeStandardVisuals_=0,nativeStandardNegativeLoads_=0;
    bool nativeStandardServiceTerminal_=false,nativeStandardServiceComplete_=false;
    bool nativeBuoyAttachNoEffect1B3AE0_=false;
    bool nativeCollisionAnimationDirectory1AA2A0_=false;
    bool nativeEmptyToyService1B4328Complete_=false;
    bool continueNativeCollisionObjects2743B0(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeFinishLine1B3B80(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeStandardModels274160(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeInstances1AA194(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeParticleManager1B4DC0(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeFog273C30(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeZoneServices2729E0(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool retireNativeRaLightZones148110()noexcept;






    // Uncounted World3B0 terrain metadata. Native data mirror of eight24-byte
    // rows; vtable provenance is not a callable guest/host pointer.
    struct NativeTerrainRow {
        float friction00=1.f,drag04=1.f,traction08=1.f;
        std::uint32_t dust0C=1,dirt10=1,sourceVtable14=0x003d2b70;
    };
    static_assert(sizeof(NativeTerrainRow)==24,"source terrain row width");
// Replace existing NativeTerrain3B0 declaration; NativeTerrainRow/legacy fields unchanged.
    struct SourceTerrainRow276700 {
        std::uint32_t friction00,drag04,traction08; std::int32_t dust0C,dirt10; std::uint32_t vtable14;
    };
    struct SourceTerrain276830 {
        std::uint32_t count00;
        std::array<SourceTerrainRow276700,8> rows04;
        std::uint32_t vtableC4;
    };
    static_assert(sizeof(SourceTerrainRow276700)==24,"actual terrain row");
    static_assert(sizeof(SourceTerrain276830)==200,"actual terrain allocation");
    struct NativeTerrain3B0 {
        std::uint32_t count00=0;
        std::array<NativeTerrainRow,8> rows04{};
        bool mappingsComplete=false;
        // Exclusive native custody for the genuine source allocation, NOT a
        // publication of the legacy metadata rows or mappingsComplete flag.
        bool sourceMode=false,sourceAllocationReturned=false,sourcePublished=false;
        bool sourceVtableKnown=false,sourceCountKnown=false,sourceLoaderReturned=false;
        std::unique_ptr<SourceTerrain276830> sourcePayload;
        const g::OriginalApplicationEntities50LA* sourceApplication=nullptr;
        const rr::platform::OriginalWorldSourceConfig50LA* sourceConfig=nullptr;
        std::uint64_t sourceGeneration=0;
        std::size_t sourceRowsConstructed=0,sourceRowsSelected=0,sourceMappingsCompleted=0;
        std::int32_t sourceMappingCount=0,sourceTerrainIndex=0;
        bool sourceMappingCountKnown=false,sourceTerrainIndexKnown=false;
        std::array<char,256> sourceObjectName{};
        bool sourceObjectNameKnown=false;
    };
    // Existing optional<NativeTerrain3B0> remains the sole backing holder.
    std::optional<SourceTerrain276830*> sourceTerrain3B0_; //Actual2705A4 NULL producer belongs to caller join.
    enum class SourceTerrainPhase:std::uint8_t {unreached,entered,bounded,returned,nativeClosed};
    SourceTerrainPhase sourceTerrainPhase_=SourceTerrainPhase::unreached;
    std::uint32_t sourceTerrainPC_=0x271370;
    bool sourceWorldTerrainInvocationCurrent()const noexcept;
    bool loadSourceWorldTerrain271370(std::uint32_t&)noexcept;
    bool preflightSourceWorldTerrainNative()const noexcept;
    bool abortSourceWorldTerrainNative(std::uint32_t&)noexcept;
    std::optional<NativeTerrain3B0> nativeTerrain3B0_;
    std::optional<NativeEmptyToyList3C8*> sourceToy3C8_; //Actual2705AC NULL/reset, then271414 publication.
    enum class SourceToyPhase:std::uint8_t {unreached,entered,bounded,returned,nativeClosed};
    SourceToyPhase sourceToyPhase_=SourceToyPhase::unreached;
    std::uint32_t sourceToyPC_=0x2713e0;
    bool sourceToyPublished_=false,sourceToyLoaderReturnedZero_=false;
    bool sourceToyLogAttempted_=false,sourceToyLogError_=false;
    std::size_t sourceToyLogBytes_=0;
    bool sourceWorldToyInvocationCurrent()const noexcept;
    bool loadSourceWorldToy2713E0(std::uint32_t&)noexcept;
    bool preflightSourceWorldToyNative()const noexcept;
    bool abortSourceWorldToyNative(std::uint32_t&)noexcept;
    std::optional<NativeTriggerList3D0*> sourceTrigger3D0_; //UNKNOWN until actual1B502C. No270550 producer.
    enum class SourceTriggerPhase:std::uint8_t {unreached,entered,bounded,prefixComplete,radiusPrefix,instanceEntered,instancePrefix,actionEntered,firstRowPrefix,rowEntered,listReturned,nativeClosed};
    SourceTriggerPhase sourceTriggerPhase_=SourceTriggerPhase::unreached;
    std::uint32_t sourceTriggerPC_=0x1b5000;
    bool sourceTriggerPublished_=false;
    std::optional<std::uint32_t> sourceTriggerFactoryVtable_,sourceTriggerActionType_;
    const rr::platform::OriginalWorldSourceConfig50LA* sourceTriggerConfigArg_=nullptr;
    std::size_t sourceTriggerRow_=0; //Retained actual first loader frame, no returned group claim.
    std::array<char,256> sourceTriggerParentName_;
    bool sourceTriggerParentKnown_=false;
    bool sourceTriggerEulerAttempted_=false,sourceTriggerEulerReturned_=false,sourceTriggerRadiusReturned_=false;
    bool continueSourceWorldTrigger2DBF98(std::uint32_t&)noexcept;
    bool continueSourceTriggerInstance2DBFE0(std::uint32_t&)noexcept;
    std::optional<std::uint32_t> sourceTriggerActionSuccessCount_,sourceTriggerActionLoopIndex_,sourceTriggerDescriptorReturn_;
    std::array<char,256> sourceTriggerActionSection_,sourceTriggerActionName_;
    bool sourceTriggerActionSectionKnown_=false;
    bool sourceTriggerActionLogAttempted_=false,sourceTriggerActionLogError_=false;
    std::size_t sourceTriggerActionLogBytes_=0;
    bool continueSourceWorldTriggerAction2DC024(std::uint32_t&)noexcept;
    std::optional<std::uint32_t> sourceTriggerListSuccessCount_,sourceTriggerListStartIndex_,sourceTriggerListLimit_;
    std::optional<std::uint32_t> sourceTriggerRawOffset_,sourceTriggerSectionNumber_,sourceTriggerListReturn_;
    bool sourceTriggerCohortAttempted_=false,sourceTriggerListLogAttempted_=false,sourceTriggerListLogError_=false;
    std::size_t sourceTriggerListLogBytes_=0;
    bool loadSourceWorldTriggerDescriptor2DC53C(std::uint32_t&)noexcept;
    bool continueSourceWorldTriggerList2DC5D0(std::uint32_t&)noexcept;
    bool sourcePhysicsObjectsAttempted_=false,sourcePhysicsObjectsReturned_=false;
    std::optional<std::uint32_t> sourcePhysicsObjectStack14C_;
    bool continueSourceWorldPhysicsObjects2718C0(std::uint32_t&)noexcept;

    bool sourceWorldTriggerInvocationCurrent()const noexcept;
    bool loadSourceWorldTriggers1B5000(std::uint32_t&)noexcept;
    bool preflightSourceWorldTriggerNative()const noexcept;
    bool abortSourceWorldTriggerNative(std::uint32_t&)noexcept;
    bool nativeEmptyAnimations2713E0Complete_=false;
    bool nativePhysicsObjects2718C0Complete_=false;
    using NativeCsCameraRows=std::array<std::optional<std::uint32_t>,7>;
    std::optional<std::uint32_t> nativeCsCameraCount3E4_;
    std::optional<NativeCsCameraRows*> nativeCsCameraZones3E8_;
    struct NativeCsCameraTable3E4 {
        bool sourceMode=false;
        std::unique_ptr<NativeCsCameraRows> sourceRows;
        const g::OriginalApplicationEntities50LA* sourceApplication=nullptr;
        std::uint64_t sourceGeneration=0;
        bool sourceAllocationAttempted=false,sourcePublished=false;

        const original_camera_session50KX::Session* session=nullptr;
        const rr::assets::OriginalTaCutAuthoringCandidate* authoring=nullptr;
        //The sole backing; World3E8 alias publishes it only after3E4 store.
        //Unknown until each genuine read; no manufactured zero/unread zones.
        NativeCsCameraRows zones{};
        std::size_t next=0;
    };
    std::optional<NativeCsCameraTable3E4> nativeCsCameraTable3E4_;
    enum class SourceCsCameraPhase {unreached,nativeAdmitted,entered,bounded,returned,nativeClosed};
    SourceCsCameraPhase sourceCsCameraPhase_=SourceCsCameraPhase::unreached;
    std::uint32_t sourceCsCameraPC_=0;
    std::optional<std::int32_t> sourceCsCameraStack8C_;
    std::optional<std::uint32_t> sourceCsCameraIndex_,sourceCsCameraLimit_,sourceCsCameraSectionNumber_;
    bool sourceWorldCsCamerasInvocationCurrent()const noexcept;
    bool loadSourceWorldCsCameras272380(std::uint32_t&)noexcept;
    bool preflightSourceWorldCsCamerasNative()const noexcept;
    bool abortSourceWorldCsCamerasNative(std::uint32_t&)noexcept;

    enum class SourceSunFlarePhase {unreached,nativeAdmitted,entered,bounded,materialPrefix,materialEntered,materialReturned,packetEntered,packetReturned,retrieveEntered,retrieveMaterialEntered,retrieveMaterialReturned,retrieveResourcesReturned,depthEntered,depthReturned,saturationEntered,saturationReturned,nativeClosed};
    SourceSunFlarePhase sourceSunFlarePhase_=SourceSunFlarePhase::unreached;
    std::uint32_t sourceSunFlarePC_=0;
    std::optional<std::uint8_t> sourceSunFlareProfileByte_;
    std::optional<std::uint32_t> sourceSunFlareModeA1_,sourceSunFlareIndex_,sourceSunFlareLimit_;
    struct SourceSunFlareStack {
        std::optional<std::int32_t> count4B4,saturation4A8,width4B8,height4BC;
        std::optional<std::uint32_t> min4AC,max4B0,callMode0;
        std::array<char,272> texture70,retrieve180;
        std::array<char,128> color310;
        bool textureKnown=false,retrieveKnown=false,colorTextKnown=false;
        std::optional<std::array<std::uint32_t,4>> direction400,position410,target420,down430,color3F0;
        std::array<std::optional<std::uint32_t>,4> difference450;
        std::optional<std::array<std::uint32_t,4>> calleeColor80;
    };
    std::optional<SourceSunFlareStack> sourceSunFlareStack_;
    bool sourceWorldSunFlareInvocationCurrent()const noexcept;
    bool sourceWorldSunFlareMaterialInvocationCurrent()const noexcept;
    bool continueSourceSunFlareMaterial2C5AE0(std::uint32_t&)noexcept;
    bool sourceWorldSunFlarePacketInvocationCurrent()const noexcept;
    bool continueSourceSunFlarePacket2C5AE8(std::uint32_t&)noexcept;
    struct SourceSunFlarePacketFrame {
        std::optional<std::int32_t> video60;
        std::optional<std::array<std::int32_t,4>> rectangle50,viewport60,rectangle70;
        std::array<std::uint32_t,4> command40{};
        bool commandKnown=false;
        std::optional<std::array<std::uint32_t,4>> tag80;
    };
    std::optional<SourceSunFlarePacketFrame> sourceSunFlarePacketFrame_;
bool sourceWorldSunFlareRetrieveInvocationCurrent()const noexcept; //Parent.
bool constructSourceSunFlareMaterial245DA0(bool embedded,std::uint32_t&)noexcept;
bool continueSourceSunFlareRetrieve2C5C3C(std::uint32_t&)noexcept;
bool continueSourceSunFlareDepth2C5CF0(std::uint32_t&)noexcept;
bool readSourceSunFlareCameraWords2C5CF0(std::uint16_t,std::uint32_t,std::uint32_t*)const noexcept; //Parent strict whitelist.
struct SourceSunFlareDepthFrame {
    std::array<std::optional<std::uint32_t>,4> result20{},far120{},scaled110{},point100{},inputF0{};
    std::array<std::optional<std::uint32_t>,16> projection30{},combined70{},screenB0{};
    std::optional<std::array<std::uint32_t,16>> viewC0,screen140;
    std::optional<std::array<std::uint32_t,3>> look190,position180;
    bool entered=false,returned=false;
};
std::optional<SourceSunFlareDepthFrame> sourceSunFlareDepthFrame_;
bool sourceSunFlareSelectedCameraPresent2C5CF0()const noexcept; //Parent, no matrix reads.

struct SourceSunFlareColorCall {
    std::array<std::optional<std::uint32_t>,4> stack40{},output{};
};
struct SourceSunFlareSaturationFrame {
    std::array<char,64> min290{},max2D0{};
    std::array<char,128> name350{};
    bool minKnown=false,maxKnown=false,nameKnown=false;
    std::optional<bool> globalMinReturned,globalMaxReturned;
    std::optional<std::uint32_t> zoneS2;
    std::array<SourceSunFlareColorCall,20> colors{};
    std::array<std::optional<std::uint32_t>,4> min3D0{},max3E0{},calleeMin30{},calleeMax40{};
};
std::optional<SourceSunFlareSaturationFrame> sourceSunFlareSaturationFrame_;
bool sourceWorldSunFlareSaturationInvocationCurrent()const noexcept;
bool continueSourceSunFlareSaturation2720D0(std::uint32_t&)noexcept;

    bool readSourceSunFlareVideo60(std::int32_t&)const noexcept;
    bool readSourceSunFlareViewport115990(std::array<std::int32_t,4>&)const noexcept;
    bool loadSourceWorldSunFlare1B46E0(std::uint32_t&)noexcept;
    bool preflightSourceWorldSunFlareNative()const noexcept;
    bool abortSourceWorldSunFlareNative(std::uint32_t&)noexcept;

    //Retained native continuation locals, NOT World4A4/4A8 or a flare owner.
    struct NativeSunFlareRead271DD0 {
        bool modeA1=false;
        std::int32_t localCount4B4=0;
    };
    std::optional<NativeSunFlareRead271DD0> nativeSunFlareRead271DD0_;
    std::shared_ptr<original_camera_session50KX::Session> borrowNativeLoadCamera(
        const std::shared_ptr<Owner>&,const original_camera_session50KX::Session&)noexcept;
    bool buildNativeSunFlarePacket2E6060(const original_main_entry50KG::Application&,
        const original_main_entry50KG::RaceDisplayLifetime50LA&,
        const original_camera_session50KX::Session&,std::uint32_t&)noexcept;
    struct NativeParticleDescriptor27C430 {
NativeParticleDescriptor27C430()=default;
struct SourceTag {};
explicit NativeParticleDescriptor27C430(SourceTag)noexcept:attach08(),emitter210(){sourceMode=true;}
bool sourceMode=false,sourceLoaded=false;
std::uint32_t sourceKnownMask=0;
std::array<char,260> sourceAttach08,sourceChild10C,sourceEmitter210;
std::optional<std::size_t> sourceAttachBytes,sourceChildBytes,sourceEmitterBytes;
std::array<std::uint32_t,4> sourceOffset320;
std::uint8_t sourceOffsetKnown=0;

        const std::uint32_t sourceVtable338=0x3d2588;
        std::int32_t startingZone00=0,type04=0;
        std::string attach08="none",emitter210="none";
        std::optional<std::string> child10C;
        //Constructor copies global442F80; no invented current global contents.
        //The reached authored OffsetFromSoultree read replaces it below.
        std::optional<std::array<float,3>> offset320;
        //158C00 storesXYZ only;27C5DC copied unknown global442F80W.
        //Actual native emitter offset50W1 is NOT source descriptor32C evidence.
        std::optional<std::uint32_t> offsetW32C;
        std::optional<std::int32_t> instances330,startOn334;
        const ra_particle_authoring50LA::Owner::AttachmentSource* actualAttachment=nullptr;
        bool loaded=false;
    };
    //Private actual27C7B0 allocation custody; never a prepared/render emitter.
    struct SourceEmitter27C890 {
        Entity entity{};bool counted=false,enrolled=false,constructorReturned=false;
        std::uint32_t dispatch08=0;
        //Typed raw source70/74 lanes with explicit native pins; no guest
        //address encoding, no ownership transfer or added original reference.
        g::OriginalTrackParticleCold50LA* manager70=nullptr;
        g::OriginalRacerModelSceneOwner50LA* modelOwner=nullptr;
        const g::OriginalRacerSceneObject50LA* model74=nullptr;
        bool managerStored=false,modelStored=false,offsetStored=false,borrowsWithdrawn=false;
        std::array<char,260> filename78{};std::optional<std::size_t> filenameBytes;
        bool directoryCopied=false,filenameAppended=false;
        // The array belongs to this source emitter, not the stack Config.
        // Declaration order destroys Config first; native array disposal is
        // exactly once and never claims an original emitter destructor call.
        a::OriginalParticleForcedSets50LA forcedSets1C4;
        std::unique_ptr<a::OriginalParticleSourceConfig50LA> config15AE00;
        bool configConstructAttempted=false,configConstructReturned=false,configOpenAttempted=false,configNativeWithdrawn=false;
        std::optional<bool> configOpenReturned;
        bool type0PropertiesReturned=false,type0LoaderReturned=false;
        std::optional<bool> type0GroupsNonNull;
        std::optional<std::uint32_t> type0GroupIndex;
        std::uint8_t type0GroupCopyMask=0;
        bool type1PropertiesReturned=false,type1LoaderReturned=false;
        std::optional<bool> type1GroupsNonNull;
        std::optional<std::uint32_t> type1GroupIndex;
        std::uint8_t type1GroupCopyMask=0;
        struct Type1Position15CFF0 {
            std::optional<std::uint32_t> argumentF12;
            std::array<std::uint32_t,4> temporary00{},previous30{};
            //Ordered source stores: stack00,previous40,current30,return200.
            std::uint8_t copyMask=0;
        };
        std::array<Type1Position15CFF0,2> type1PositionCalls;
        bool type2PropertiesReturned=false,type2LoaderReturned=false;
        std::optional<bool> type2GroupsNonNull;
        std::optional<std::uint32_t> type2GroupIndex;
        std::uint8_t type2GroupCopyMask=0;
        bool type9VelocityCopied=false,type9PropertiesReturned=false,type9LoaderReturned=false;
        std::optional<bool> type9GroupsNonNull;
        std::optional<std::uint32_t> type9GroupIndex;
        std::uint8_t type9GroupCopyMask=0;
        //Fresh subtype9 StartOff invokes15F070, never generic15AD00.
        std::optional<std::uint32_t> type9StopMode1D8;
        std::optional<bool> type9StopActiveBit;
        std::uint8_t type9StopWriteMask=0; //Ordered bit1,bit2,previous40,19C stores.
        bool type9StopReturned=false;
        bool type11PropertiesReturned=false,type11LoaderReturned=false;
        std::optional<bool> type11GroupsNonNull;
        std::optional<std::uint32_t> type11GroupIndex;
        std::uint8_t type11GroupCopyMask=0;
        std::array<std::uint32_t,4> type12PositionInput{};
        std::uint8_t type12PositionMask=0; //LQ input, current30, previous40; not15ACB0 calls.
        bool type12AttachmentPositionPending=false,type12PopulationPending=false;
        bool type12PropertiesReturned=false,type12LoaderReturned=false;
        //Fog's own+84 target160490, never generic15AD00 or subtype9's15F070.
        std::optional<std::uint32_t> type12StopMode1D8;
        std::optional<bool> type12StopActiveBit;
        std::uint8_t type12StopWriteMask=0; //Ordered bit1,bit2,previous40,19C stores.
        bool type12StopReturned=false;
        g::OriginalRacerModelSceneOwner50LA::SourcePosition28B1A0 type12AttachmentPosition;
        g::OriginalRacerModelSceneOwner50LA::SourceDirection28B9B0 type12AttachmentDirection;
        std::array<std::uint32_t,3> type12AttachmentPositionXYZ{},type12AttachmentDirectionXYZ{},type12AttachmentAddResult{};
        std::uint8_t type12AttachmentAddMask=0;
        bool type12AttachmentPreviousStored=false;
        struct Type12Population160520 {
            const g::OriginalTrackParticleCold50LA* manager=nullptr;
            bool entered=false,retirementEmpty=false,randomPending=false,returned=false;
            g::OriginalTrackParticleCold50LA::SourceFogRetirement160570 retirementScan;
            std::optional<std::int32_t> retirementCount128,creationCount128,capacity130;
            std::optional<std::uint32_t> creationIndexInitial; //Actual1605C0 s0=0, separate from manager128.
            std::optional<std::uint32_t> volume228;
            std::optional<bool> volumePositive,capacityAvailable;
            //Native pin to the SAME Application process owner, never a copy,
            //new seed, per-emitter RNG or original guest retain operation.
            std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> randomOwner;
            const original_main_entry50KG::Application* randomApplication=nullptr;
            std::array<std::uint32_t,3> randomBefore{},randomAfter{},randomValue{},randomConverted{},randomUnit{};
            std::uint8_t randomDrawCount=0,randomConvertedMask=0,randomUnitMask=0,randomStack90Mask=0;
            std::array<std::uint32_t,4> randomStack90{},randomStack70{};
            bool randomStack70Copied=false;
            //Immutable bits observed by the160654 copy;randomStack70 remains
            //the same mutable source scratch vector.
            std::array<std::uint32_t,4> randomStack70Initial{};
            struct RangeAxis160658 {
                std::uint32_t maximum=0,minimum=0,unit=0,difference=0,product=0,sum=0;
                //Ordered max,min,unit reads;SUB,MUL,ADD;stack70 store.
                std::uint8_t step=0;
            };
            std::array<RangeAxis160658,3> rangeAxes;
            //One ordered1606AC..1606FC prefix. These are actual fresh reads,
            //not aliases of the earlier1605C8 count/capacity receipt.
            std::array<std::uint32_t,3> positionInput{},positionLocal{},positionResult{};
            std::optional<std::int32_t> positionIndex128;
            std::optional<const void*> positionPublication138;
            std::array<std::uint32_t,4> positionStackA0{};
            std::uint8_t positionStep=0;
            g::OriginalTrackParticleCold50LA::SourceFogPosition1606FC positionStore;
            //Actual22C comparison/branch after the committed position SQ.
            //Common step0:before700;1:before704;2:branch/delay completed.
            std::uint8_t oscillationStep=0,oscillationRandomStep=0,fieldTailStep=0;
            std::optional<std::uint32_t> oscillation22C,oscillationMagnitude234;
            std::optional<bool> oscillationDisabled,initialMirrorXLeHalf;
            std::uint32_t oscillationRandomBefore=0,oscillationRandomAfter=0,oscillationRandomValue=0;
            std::uint32_t oscillationRandomConverted=0,oscillationRandomUnit=0;
            //Following real draws5..9:two magnitude draws,then XYZ signs.
            //velocityStep authenticates the exact reached failure frontier.
            std::uint8_t velocityStep=0;
            std::array<std::uint32_t,5> velocityRandomBefore{},velocityRandomAfter{},velocityRandomValue{},velocityRandomConverted{},velocityRandomUnit{};
            std::array<std::optional<std::uint32_t>,2> velocityMagnitude234;
            std::array<std::uint32_t,3> velocityProducts{}; //f20,f23,f0, then Z,Y,X.
            std::array<std::uint32_t,4> velocityStackB0{},velocityStack80{};
            std::uint8_t velocityStackB0Mask=0;
            bool velocityStackB0Loaded=false,velocityStack80Copied=false;
            std::array<std::optional<bool>,3> velocitySignLeHalf;
            //Each reached lane multiplication and SWC1 retains its own receipt.
            //B0 stays the pre-sign vector;80 reflects only completed lane stores.
            std::array<std::optional<std::uint32_t>,3> velocityNegationInput,velocityNegationResult;
            std::uint8_t velocityNegationStoredMask=0;
            g::OriginalTrackParticleCold50LA::SourceFogVelocity16088C velocityStore;
            std::optional<std::uint32_t> sizeInput184,initialSizeInput184,lifeInput17C,ageInput1A0,frameInput190;
            g::OriginalTrackParticleCold50LA::SourceFogFields1608AC fieldStores;
            std::uint8_t postDivideStep=0;
            std::optional<std::uint32_t> reciprocalBits,initialFlags198,mirrorEnable1CC,mirrorEnable1D0;
            std::optional<std::uint32_t> firstFlagsResult,secondFlagsResult;
            std::optional<bool> mirrorYLeHalf,mirrorXSet,mirrorYSet;
            //True means this DIV newly asserted D/SD. False on a completed
            //normal reciprocal does NOT clear unknown prior D/SD or sticky
            //state. reciprocalBits separately proves operation completion.
            bool reciprocalDAndSD=false;
            g::OriginalTrackParticleCold50LA::SourceFogTail1608E0 tailStores;
            //Distinct fresh160988/98C observations and160990 arithmetic.
            //Step0..4 is pending988,98C,990,994,998; no helper return.
            std::uint8_t countStep=0;
            const g::OriginalTrackParticleCold50LA* countManager70=nullptr;
            std::optional<std::int32_t> countBefore128,countAfter128;
            g::OriginalTrackParticleCold50LA::SourceFogCount160994 countStore;
            struct FrameSelection160998 {
                std::uint8_t step=0;
                std::optional<bool> seedGuard80A8,rangeGuard80AC;
                std::optional<std::uint32_t> first1B0,last1B4,seedCache3D6B30;
                bool seedCacheCleared=false,seedGuardStored=false;
                std::optional<std::uint32_t> differenceWord,spanWord,spanConverted;
                std::uint32_t randomBefore=0,randomAfter=0,randomValue=0;
                bool randomDrawn=false;
                std::optional<std::uint32_t> randomConverted,randomUnit;
                std::optional<std::uint32_t> productBits,offsetWord,selectedWord,rangeCheck3D6B38;
                bool rangeCacheStored=false,rangeGuardStored=false;
                std::optional<bool> selectedBelowFirst,lastBelowSelected,rangeCondition;
                std::optional<std::uint32_t> assertionArgument,assertionReturnV0;
                bool frameStored190=false;
                std::optional<std::uint32_t> creationIndexAfter,loopVolume228,indexConverted;
                std::optional<bool> loopAgain;
            } frameSelection;
            std::uint32_t pc=0;
        } type12Population;
        std::optional<bool> type12GroupsNonNull;
        std::optional<std::uint32_t> type12GroupIndex;
        std::uint8_t type12GroupCopyMask=0;
        bool type8VelocityCopied=false,type8PropertiesReturned=false,emitterFileLoadReturned=false,type8LoaderReturned=false;
        std::optional<bool> type8GroupsNonNull;
        std::optional<std::uint32_t> type8GroupIndex;
        std::uint8_t type8GroupCopyMask=0,positionCopyCount=0;
        std::optional<bool> sourceStartOn;
        bool particleActivationReturned=false,entityActivityStored=false,entityActivityReturned=false,rowConfigReturned=false;
        bool sourceRegistered=false,nativeTopologyWithdrawn=false;
        std::array<std::uint32_t,0x260/4> words{},known{};
        void word(std::uint32_t off,std::uint32_t v)noexcept {words[off/4]=v;known[off/4]=0xffffffffu;}
        void vector(std::uint32_t off)noexcept {word(off,0);word(off+4,0);word(off+8,0);word(off+12,0x3f800000);}
    };
    struct NativeParticleRow27DE00 {
// Copied proof values only: never a descriptor pointer, source allocation,
// parser owner, or substitute for a live source read after27D754.
struct DescriptorHistory {
    std::uint32_t type=0;std::int32_t count=0,start=0;
    std::array<std::uint32_t,4> offset{};
};
std::optional<DescriptorHistory> sourceDescriptorHistory;
bool sourceDescriptorFreed=false,sourceRowServiceReturned=false;
bool sourceMode=false,sourceDescriptorAttempted=false,sourceDescriptorPublished=false;
std::uint8_t sourceConstructorMask=0;
std::optional<NativeParticleDescriptor27C430*> sourceDescriptor04;
//Native owner metadata is not a source initialized slot. Only publishedSlots
//entries may be read; allocation NULL and partial constructor remain owned.
bool sourceEmitterArrayAttempted=false,sourceEmitterArrayPublished=false;
std::unique_ptr<Entity*[]> sourceEmitterPointers;
std::optional<Entity**> sourceEmitterArray00;
std::vector<std::unique_ptr<SourceEmitter27C890>> sourceEmitters;
std::size_t sourceEmitterAttempts=0,sourceEmitterPublishedSlots=0;


        const std::uint32_t sourceVtable14=0x3d2570;
        //27CA44 raw pointer-array custody, borrowing the SAME existing emitter.
        //No second emitter allocation/count and no array-owned Entity release.
        std::unique_ptr<Entity*[]> emitterPointers00;
        bool emitterPublished=false,emitterEnrolled=false,configured=false;
        bool started=false,enabled=false,attached=false,descriptorRetired=false;
        const g::OriginalRacerSceneObject50LA* attachment74=nullptr;
        std::unique_ptr<NativeParticleDescriptor27C430> descriptor04;
        std::optional<std::uint32_t> unknown08,unknown0C,unknown10;
    };
    struct NativeParticleList3D8 {
bool sourceMode=false,sourceEnrolled=false,sourceConstructorReturned=false;
std::uint8_t sourceConstructorMask=0;
bool sourceArrayAttempted=false,sourceArrayPublished=false;
std::optional<NativeParticleRow27DE00**> sourceArray34;
std::array<bool,4> sourceRowAttempted{},sourceSlotPublished{},sourceRowReturned{};

        Entity entity;bool counted=false,complete=false;
        const std::uint32_t sourceVtable08=0x3d24f0;
        std::int32_t count30=0;
        std::unique_ptr<NativeParticleRow27DE00*[]> pointers34;
        std::array<std::unique_ptr<NativeParticleRow27DE00>,4> rows;
        std::size_t published=0,loaded=0;
        std::size_t servicesCompleted=0;
        bool serviceEntered=false,serviceTerminal=false,serviceComplete=false;
        ra_particle_authoring50LA::Owner* actualOwner=nullptr;
    };
    //New native exclusive World3D8 metadata domain; not a270550 reset receipt.
    std::unique_ptr<NativeParticleList3D8> nativeParticleList3D8_;
enum class SourceParticleDescriptorPhase {unreached,entered,bounded,returned,nativeClosed};
SourceParticleDescriptorPhase sourceParticleDescriptorPhase_=SourceParticleDescriptorPhase::unreached;
std::uint32_t sourceParticleDescriptorPC_=0;
bool sourceParticleDescriptorAttempted_=false,sourceParticleDescriptorPublished_=false;
std::optional<NativeParticleList3D8*> sourceParticleList3D8_; //Actual2705B0 reset producer owned by parent.
const rr::platform::OriginalWorldSourceConfig50LA* sourceParticleDescriptorConfigArg_=nullptr;
std::optional<std::int32_t> sourceParticleDescriptorCountStack16C_,sourceParticleDescriptorSuccessS1_,sourceParticleDescriptorRowS0_;
std::optional<std::uint32_t> sourceParticleDescriptorOffsetS2_;
std::array<char,272> sourceParticleDescriptorName60_;
bool sourceParticleDescriptorNameKnown_=false,sourceParticleDescriptorLogAttempted_=false;
std::optional<std::size_t> sourceParticleDescriptorLogWritten_;
std::optional<bool> sourceParticleDescriptorLogError_;
bool sourceWorldParticleDescriptorsInvocationCurrent()const noexcept; //Parent.
bool loadSourceWorldParticleDescriptors2722F0(std::uint32_t&)noexcept;
bool preflightSourceWorldParticleDescriptorsNative()const noexcept;
bool abortSourceWorldParticleDescriptorsNative(std::uint32_t&)noexcept;
bool preflightSourceWorldParticleCountNative()const noexcept; //Parent combined count floor, calls local preflight only.

// Private actual271A00 stack/loop history, not legacy model preparation.
enum class SourceStaticModelParamPhase {unreached,entered,bounded,returned,nativeClosed};
SourceStaticModelParamPhase sourceStaticModelParamPhase_=SourceStaticModelParamPhase::unreached;
std::uint32_t sourceStaticModelParamPC_=0;
struct SourceStaticModelParamFrame {
    std::array<char,272> name90;
    std::array<char,260> slt1A0;
    bool nameKnown=false,sltKnown=false,descriptorBorrowWithdrawn=false;
    std::optional<std::size_t> sltBytes,lastDot;
    std::optional<std::int32_t> count2A8,allow2AC,s7,s0,s1;
    std::optional<std::uint32_t> s5,s4;
    ra_static_model_cold50LA::Owner::Parent* parentS2=nullptr;
    ra_static_model_cold50LA::Owner::StandardWrapper* wrapperS3=nullptr;
    ra_static_model_cold50LA::Owner::Descriptor* descriptorA0=nullptr;
    std::optional<std::size_t> parentZone,wrapperZone,wrapperSlot,descriptorZone,descriptorSlot;
    std::uint64_t comparisons=0,shadowStores=0;
};
std::optional<SourceStaticModelParamFrame> sourceStaticModelParamFrame_;
bool sourceWorldStaticModelParamsInvocationCurrent()const noexcept; //Parent true271DA0/currentTOB.
bool loadSourceWorldStaticModelParams271A00(std::uint32_t&)noexcept;
bool preflightSourceWorldStaticModelParamsNative()const noexcept;
bool abortSourceWorldStaticModelParamsNative(std::uint32_t&)noexcept;

    std::optional<std::uint32_t> sourceTobA4V0_;
    bool sourceTobA4Returned_=false;
    bool completeSourceWorldObjects271DB0(std::uint32_t&)noexcept;
    std::size_t nativeStaticModelParamsNext_=0; //Authored row-order prefix only.
    bool nativeStaticModelParamEffectsComplete_=false;
    bool loadNativeStaticModelParams271A00(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
// Private same-owner World538 continuation. Existing nativeBuoyCount538_ is
// sole actual scalar output; no53C storage/NULL producer is added.
enum class SourceBuoysPhase {unreached,entered,bounded,returned,nativeClosed};
SourceBuoysPhase sourceBuoysPhase_=SourceBuoysPhase::unreached;
std::uint32_t sourceBuoysPC_=0;
std::array<std::optional<std::array<std::uint32_t,4>>,3> sourceBuoyStack260To280_;
bool sourceWorldBuoysInvocationCurrent()const noexcept; //Parent actual271D90 gate.
bool loadSourceWorldBuoys1B4890(std::uint32_t&)noexcept;
bool preflightSourceWorldBuoysNative()const noexcept;
bool abortSourceWorldBuoysNative(std::uint32_t&)noexcept;

    std::optional<std::int32_t> nativeBuoyCount538_; //Unknown until1B50E0 actual default store.
    bool loadNativeEmptyBuoys1B5090(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool loadNativeParticleDescriptors2722F0(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool skipNativeEcosystem1AA410(const original_main_entry50KG::Application&,std::uint32_t&)const noexcept;
    bool continueNativeWater274570(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeSky274AE0(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeTriggerServices274E90(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeParticleServices274370(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool canRetireNativeParticleList3D8()const noexcept;
    bool retireNativeParticleList3D8()noexcept;
    bool loadNativeSunFlareSaturation2720D0(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool loadNativeSunFlareDepth2C5CF0(const original_main_entry50KG::Application&,
        const original_main_entry50KG::RaceDisplayLifetime50LA&,
        const original_camera_session50KX::Session&,std::uint32_t&)noexcept;
    bool loadNativeSunFlareResources2C5C44(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool loadNativeSunFlareMaterials2C5A10(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool retainNativeSunFlareMaterial245DA0(NativeSunFlareMaterial&,const std::string&,
        const std::shared_ptr<a::OriginalStandardMaterialTable50LA>&,const std::string&,std::uint32_t&)noexcept;
    bool continueNativeSunFlareRead1B46E0(const original_main_entry50KG::Application&,
        const rr::platform::OriginalUiRiderProfileOwner50KG&,std::uint32_t&)noexcept;
    bool continueNativeCsCameras272380(const original_main_entry50KG::Application&,
        const original_camera_session50KX::Session&,std::uint32_t&)noexcept;
    bool continueNativePhysicsObjects2718C0(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeTriggers1B5000(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeTerrain271370(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool withdrawNativeScb34_271C10(const original_main_entry50KG::Application&,std::uint32_t&)noexcept;
    bool continueNativeTob1B4330(const original_main_entry50KG::Application&,
        const rr::platform::OriginalUiRiderProfileOwner50KG&,std::uint32_t&)noexcept;

    std::shared_ptr<ra_static_candidates50LA::Owner> candidates_;
    std::unique_ptr<ra_static_registry50LA::ColdStage> registry_;
    std::shared_ptr<ra_trigger_authoring50LA::Owner> triggers_;
    std::unique_ptr<original_track_sky50KX::Owner> sky_;
    std::unique_ptr<sky50gw_fixture::Owner> skyImages_;
    std::shared_ptr<const ra_cold_sources50LA::ColdNavigationLease> navigationLease_;
    std::unique_ptr<ra_atm_authoring50LA::Owner> atm_;
    std::unique_ptr<ra_audio_authoring50LA::Owner> audio_;
    std::unique_ptr<ra_particle_authoring50LA::Owner> particles_;
    std::shared_ptr<ra_static_world50LA::Owner> world_;
    // One actual scene pool. Future camera children must retain this SAME
    // private owner; no public mutable manager/pool getter is provided.
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> trackParticleDomain_;
    std::shared_ptr<g::OriginalApplicationEntities50LA> trackParticleApplication_;
    const original_main_entry50KG::Application* trackParticleHostApplication_=nullptr;
    std::uint64_t trackParticleGeneration_=0;
    std::shared_ptr<g::OriginalApplicationEntities50LA> emitterApplication_;
    std::uint64_t emitterGeneration_=0;
    bool trackParticleAttempted_=false;
    std::unique_ptr<ra_static_lighting50LA::Binding> staticLighting_;
    std::unique_ptr<ra_static_fog50LA::Binding> staticFog_;
    Prefix prefix_{};
    bool sourceAttempted_=false,authoringAttempted_=false,lightingAttempted_=false;
    bool candidatesAttempted_=false,registryAttempted_=false,particlesAttempted_=false;
    bool runtimeStorageAttempted_=false,runtimeQueriesAttempted_=false,runtimeRegistrationAttempted_=false;
    bool worldAttempted_=false,worldImagesAttempted_=false,worldDrawPlanAttempted_=false;
    bool modelPrefixesAttempted_=false;
    bool staticModelResourcesAttempted_=false;
    bool staticObserversAttempted_=false;
    bool terminal_=false,closed_=false;
    bool rosterModelResourcesAttempted_=false,rosterModelResourcesComplete_=false;
    bool rosterMountAttempted_=false,rosterMountComplete_=false;
    const original_main_entry50KG::Application* rosterMountApplication_=nullptr;
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> rosterMountDomain_;
    a::NativeRaceMountLease50LA nativeRosterMounts_;
    // Application-aware definitions beside the retained Operation. Only the
    // actual Bank pin can supply the operation and its stable arena arrays.
    std::shared_ptr<ra_cold_roster50LA::Operation> retainedRosterForHandoff(
        native_racer50HN::Bank&,Diagnostic&)noexcept;
    bool failRetainedHandoff(Diagnostic&,const char*)noexcept;
    bool fail(Diagnostic& d,const char* why) noexcept;
    bool enter(Diagnostic& d,Stage stage,bool& attempted,bool dependency) noexcept {
        d={};d.stage=stage;
        if(sourceTrackNativeAttempted_||terminal_||closed_||attempted||!dependency)return fail(d,"RA aggregate stage dependency/lifetime/source-route rejected");
        attempted=true;return true;
    }
#include "original_source_world_physics_model_primary50la.inl"
public:
    bool constructWorldCold1B2870(const original_main_entry50KG::Application&,
        rr::platform::RaceAudioGroups50IC&,rr::platform::AudioMixer50HD&,std::uint64_t,std::uint32_t&)noexcept;
    bool prepareWorldPhysicsIndex270CDC(std::uint32_t& pc)noexcept {
        pc=0;
        if(sourceTrackNativeAttempted_||closed_||terminal_||!worldColdConstructed_||trackParticleAttempted_||
           modelPrefixesAttempted_||particlesAttempted_||runtimeCameraBorrows_||runtimeWorldBorrows_){
            pc=0x270cc4;return false;
        }
        return ra_pod_scene50LA::Owner::prepareWorldPhysicsIndex270CDC(source_,pc);
    }
    bool prepareWorldCollisionObjects271230(std::uint32_t& pc)noexcept {
        pc=0;
        if(sourceTrackNativeAttempted_||closed_||terminal_||trackParticleAttempted_||modelPrefixesAttempted_||particlesAttempted_||
           runtimeCameraBorrows_||runtimeWorldBorrows_){pc=0x271230;return false;}
        return ra_pod_scene50LA::Owner::prepareWorldCollisionObjects271230(source_,pc);
    }
    bool prepareWorldCollisionResources(const original_main_entry50KG::Application&,
        rr::assets::OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool rollbackNativeWorldConstructionFailure(std::uint32_t& pc)noexcept {
        pc=0;
        if(nativeScbThread_!=std::this_thread::get_id()||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||
           runtimeCameraBorrows_||runtimeWorldBorrows_)return false;
        // The private source loader introduces real Config/transport owners.
        // Never retire their counted World through the earlier constructor-
        // failure entry before exact payload/handle/native mount withdrawal.
        if(sourceTrackNativeAttempted_){
            if(!preflightSourceWorldStaticModelParamsNative()||!preflightSourceWorldBuoysNative()||!preflightSourceWorldParticleCountNative()||!preflightSourceWorldSunFlareNative()||!preflightSourceWorldCsCamerasNative()||!abortSourceTrackNative(pc))return false;
            //Exact manager/Base borrows leave with source payloads first; only
            //then may the unchanged World-prefix releaseability gate pass.
            if(!canRetireNativeWorldPrefix())return false;
        }
        return ra_pod_scene50LA::Owner::rollbackNativeWorldConstructionFailure(pc);
    }
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    ~Owner();
    // Typed adoption precedes runtime reservation. Definition is isolated in
    // the Application-aware cold roster header; no mutable roster escapes.
    bool prepareRuntimeRosterQueryStorage(
        std::shared_ptr<g::OriginalApplicationEntities50LA>,
        std::shared_ptr<const ra_cold_roster50LA::Resources>,
        undertrack_owners50ew_fixture::Owners*,
        const undertrack_owners50ew_fixture::SourceGlobals&,
        const terrain_probe_authoring50fq_fixture::Owner&,const g::Type3QueryInput48P&,
        const std::vector<ra_source_camera50LA::WreckReservation>&,Diagnostic&)noexcept;

    // Borrow the actual shared serial numeric owner and real category masks.
    // Caller owns their identity; this class neither creates nor resets them.
    bool decodeSources(const std::vector<std::uint8_t>& originalRaArchive,
        g::SourceLibmState49T& actualLibm,a::TrackCollisionMasks50AN actualMasks,
        Diagnostic& d) noexcept {
        if(!enter(d,Stage::sources,sourceAttempted_,true))return false;
        try{
            source_=std::make_shared<ra_cold_sources50LA::Owner>();
            if(!source_->decode(originalRaArchive,actualLibm,actualMasks,d.source))
                return fail(d,"RA source decode failed; actual libm prefix is not rolled back");
            prefix_.sources=true;return true;
        }catch(...){return fail(d,"RA source stage allocation failure");}
    }
    std::shared_ptr<const ra_cold_sources50LA::ColdNavigationLease> borrowColdNavigation(Diagnostic& d)noexcept {
        d={};d.stage=Stage::sources;
        if(terminal_||closed_||!prefix_.sources||!source_||!source_->sourcesDecoded()){
            fail(d,"RA navigation borrow requires this aggregate's actual decoded source");return {};}
        try {
            if(!navigationLease_)navigationLease_.reset(new ra_cold_sources50LA::ColdNavigationLease(source_));
            return navigationLease_;
        }catch(...){fail(d,"RA navigation lease allocation failed");return {};}
    }
    bool prepareStaticCandidates(const track_static_candidates50IG::Bindings& actualNamespace,
        Diagnostic& d) noexcept {
        if(!enter(d,Stage::staticCandidates,candidatesAttempted_,prefix_.sources))return false;
        try{
            candidates_=std::make_shared<ra_static_candidates50LA::Owner>();
            if(!candidates_->prepare(source_,actualNamespace,d.candidates))return fail(d,"RA static namespace preparation failed");
            prefix_.staticCandidates=true;return true;
        }catch(...){return fail(d,"RA static candidate allocation failure");}
    }
    // Explicit user-approved RA compatibility policy. Must precede every
    // registry/storage borrow; keeps authentic source and shared action owner.
    bool prepareNativeTriggers(Diagnostic& d)noexcept {
        d={};d.stage=Stage::staticCandidates;
        if(terminal_||closed_||!prefix_.staticCandidates||!prefix_.triggers||
           registryAttempted_||runtimeStorageAttempted_)
            return fail(d,"RA trigger preparation requires fresh authored candidates");
        try {if(!candidates_->prepareTriggers(triggers_))return fail(d,"RA trigger candidates rejected");return true;}
        catch(...){return fail(d,"RA trigger candidate allocation failure");}
    }
    // Actual synchronous racer visit only. C80 is obtained internally from
    // the retained original roster operation, never from a caller's substitute.
    bool beginRuntimeTriggerQuery(native_racer50HN::Bank&,std::size_t,Diagnostic&)noexcept;
    bool endRuntimeTriggerQuery(native_racer50HN::Bank&,Diagnostic&)noexcept;
    bool observeRuntimeTriggerPoses(native_racer50HN::Bank&,std::size_t,Diagnostic&)noexcept;
    bool serviceRuntimeTriggers(native_racer50HN::Bank&,Diagnostic&)noexcept;
    bool constructClosedStaticRegistry(std::shared_ptr<g::OriginalApplicationEntities50LA> app,Diagnostic& d) noexcept {
        if(!app||!app->application10()||(worldApplication_&&worldApplication_!=app)||
           (collisionApplication_&&collisionApplication_!=app))
            return fail(d,"actual same Application collision authority required");
        if(!enter(d,Stage::staticRegistry,registryAttempted_,prefix_.staticCandidates&&!runtimeStorageAttempted_))return false;
        collisionApplication_=app;
        try{
            registry_=std::make_unique<ra_static_registry50LA::ColdStage>();
            if(!registry_->construct(std::move(app),candidates_,d.registry))return fail(d,"RA exclusive closed static prefix failed");
            prefix_.staticRegistry=true;return true;
        }catch(...){return fail(d,"RA static registry allocation failure");}
    }
    // Mutually exclusive fresh route. This reserves and owns real backing,
    // but its static prefix is not the complete RA gameplay collider extent.
    bool prepareRuntimeQueryStorage(std::shared_ptr<g::OriginalApplicationEntities50LA> app,
        const ra_source_camera50LA::RuntimeQueryCapacityInputs& input,
        Diagnostic& d) noexcept {
        if(!app||!app->application10()||(worldApplication_&&worldApplication_!=app)||
           (collisionApplication_&&collisionApplication_!=app))
            return fail(d,"actual same Application collision authority required");
        if(!enter(d,Stage::runtimeQueryStorage,runtimeStorageAttempted_,
            prefix_.staticCandidates&&!registryAttempted_))return false;
        collisionApplication_=app;
        try{
            runtimeQueries_.reset(new ra_source_camera50LA::OwnershipBridge());
            if(!runtimeQueries_->constructStorage(std::move(app),candidates_,input,d.runtimeQueries))
                return fail(d,"RA fresh runtime query storage rejected; prefix retained");
            prefix_.runtimeQueryStorage=true;return true;
        }catch(...){return fail(d,"RA runtime query storage allocation failed");}
    }
    bool bindRuntimeQueryOwners(const ra_source_camera50LA::ActualQueryOwners& input,
        Diagnostic& d) noexcept {
        if(!enter(d,Stage::runtimeQueryOwners,runtimeQueriesAttempted_,
            prefix_.runtimeQueryStorage&&!registryAttempted_))return false;
        if(!runtimeQueries_->bindQueries(input,d.runtimeQueries))
            return fail(d,"RA actual borrowed-arena query construction rejected; prefix retained");
        prefix_.runtimeQueryOwners=true;return true;
    }
    // Registers only the actual candidate lanes and Storage backing. Caller
    // retains existing arena registration and per-racer constructor order.
    // The exact Bank privately pins this runtime before any pointer escapes.
    bool registerRuntimeQueryWorldOwners(native_racer50HN::Bank& actualBank,
        Diagnostic& d)noexcept {
        if(!enter(d,Stage::runtimeQueryWorldOwners,runtimeRegistrationAttempted_,
            prefix_.runtimeQueryOwners&&!registryAttempted_))return false;
        if(!runtimeQueries_->registerWorldOwners(actualBank,runtimeQueries_,d.runtimeQueries))
            return fail(d,"RA world Bank registration rejected; actual partial registration remains pinned");
        prefix_.runtimeQueryWorldOwners=true;return true;
    }
    // Synchronous native cold geometry only. This executes no setup tick,
    // recovery/action dispatch or camera algorithm and publishes no capability.
    bool bindRuntimePodLiveQuerySlot(native_racer50HN::Bank& actualBank,std::size_t slot,
        g::VehicleFrameBindings49W& actualBindings,g::VehicleFrameParameters49W& actualParameters,
        Diagnostic& d,std::uint32_t humanCount=0)noexcept {
        d={};d.stage=Stage::runtimeQueryWorldOwners;
        if(terminal_||closed_||!prefix_.runtimeQueryWorldOwners||!runtimeQueries_)
            return fail(d,"RA pod context requires actual registered runtime backing");
        if(!runtimeQueries_->bindPodLiveQuery(actualBank,slot,actualBindings,actualParameters,d.runtimeQueries,humanCount))
            return fail(d,"RA actual pod context binding failed; retained partial slot effects");
        return true;
    }
    bool placeRetainedRuntimeGridSlot(native_racer50HN::Bank&,std::size_t,Diagnostic&,
        bool canonicalTournamentGrid=false)noexcept;
    bool prepareRetainedRuntimeModelResources(native_racer50HN::Bank&,Diagnostic&)noexcept;
    bool mountRetainedRuntimeRosterArchives(const original_main_entry50KG::Application&,Diagnostic&)noexcept;
    bool attachRetainedRuntimeFreshSlot(native_racer50HN::Bank&,std::size_t,
        const ra_source_camera50LA::FreshRecoveryArenaOwners&,Diagnostic&)noexcept;
    bool placeRuntimeGridSlot(native_racer50HN::Bank& actualBank,std::size_t slot,
        const ra_source_camera50LA::GridArenaOwners& actualArena,Diagnostic& d)noexcept {
        d={};d.stage=Stage::runtimeQueryWorldOwners;
        if(terminal_||closed_||!prefix_.runtimeQueryWorldOwners||!runtimeQueries_)
            return fail(d,"RA grid requires its actual registered runtime owner");
        if(runtimeQueries_->rosterResources_){
            runtimeQueries_->rosterSlotComplete_.fill(false);
            runtimeQueries_->fail(d.runtimeQueries,"retained roster requires its private grid handoff");
            return fail(d,"caller-array grid entry cannot bypass retained roster ownership");
        }
        if(!runtimeQueries_->placeGrid(actualBank,slot,actualArena,d.runtimeQueries))
            return fail(d,"RA cold grid/query attachment rejected; partial effects retained");
        return true;
    }
    bool attachRuntimeFreshSlot(native_racer50HN::Bank& actualBank,std::size_t slot,
        const ra_source_camera50LA::FreshRecoveryArenaOwners* actualRecoveryArena,Diagnostic& d)noexcept {
        d={};d.stage=Stage::runtimeQueryWorldOwners;
        if(terminal_||closed_||!prefix_.lighting||!prefix_.runtimeQueryWorldOwners||!runtimeQueries_)
            return fail(d,"RA fresh visibility requires same aggregate lighting and runtime owners");
        if(runtimeQueries_->rosterResources_){
            runtimeQueries_->rosterSlotComplete_.fill(false);
            runtimeQueries_->fail(d.runtimeQueries,"retained roster requires its private fresh handoff");
            return fail(d,"raw fresh entry cannot bypass retained roster ownership");
        }
        if(!runtimeQueries_->attachFresh(actualBank,slot,lighting_,actualRecoveryArena,d.runtimeQueries))
            return fail(d,"RA fresh attachment incomplete; actual partial effects remain owned");
        return true;
    }
    bool retireRuntimeFreshSlot(native_racer50HN::Bank& actualBank,std::size_t slot,Diagnostic& d)noexcept {
        d={};d.stage=Stage::runtimeQueryWorldOwners;
        if(closed_||!runtimeQueries_){d.failure="RA fresh cleanup owner unavailable";return false;}
        if(runtimeQueries_->rosterResources_)runtimeQueries_->rosterSlotComplete_.fill(false);
        if(!runtimeQueries_->retireFresh(actualBank,slot,d.runtimeQueries)){
            d.failure="RA fresh cleanup incomplete; retain actual Bank and arena";return false;}
        return true;
    }
    bool retireRuntimeGridSlot(native_racer50HN::Bank& actualBank,std::size_t slot,
        Diagnostic& d)noexcept {
        d={};d.stage=Stage::runtimeQueryWorldOwners;
        if(closed_||!runtimeQueries_){d.failure="RA grid cleanup owner unavailable";return false;}
        if(runtimeQueries_->rosterResources_)runtimeQueries_->rosterSlotComplete_.fill(false);
        if(!runtimeQueries_->retireGrid(actualBank,slot,d.runtimeQueries)){
            d.failure="RA grid cleanup remains incomplete; retain Bank and aggregate";return false;}
        return true;
    }
    // Counts expose allocation only. No live mutable registry or query owner
    // escapes; attachment/camera/Bank ownership is a subsequent typed join.
    track_runtime_storage50IG::Counts reservedRuntimeQueryCounts()const noexcept {
        const auto* storage=runtimeQueries_?runtimeQueries_->storage_.get():nullptr;
        return storage?storage->count:track_runtime_storage50IG::Counts{};
    }
    std::shared_ptr<const ra_cold_sources50LA::DecodedSources> borrowNativeSource(
        const std::shared_ptr<Owner>& self,Diagnostic& d)noexcept {
        d={};d.stage=Stage::sources;
        if(self.get()!=this||terminal_||closed_||!source_||!source_->sourcesDecoded()){
            d.failure="actual decoded RA source unavailable";return {};
        }
        try {
            ++runtimeWorldBorrows_;
            return std::shared_ptr<const ra_cold_sources50LA::DecodedSources>(source_->sourcesDecoded(),
                [self](const ra_cold_sources50LA::DecodedSources*)noexcept{--self->runtimeWorldBorrows_;});
        }catch(...){d.failure="RA native source borrow allocation failed";return {};}
    }
    // The native driver shares this exact query service with the camera. The
    // retained runtime view pins it until all frame/recovery clients retire.
    original_follow_query_bridge::QueryBridge* nativeFollowQueries()const noexcept {
        return !closed_&&runtimeQueries_&&prefix_.runtimeQueryOwners?runtimeQueries_->follow_.get():nullptr;
    }
    struct NativeRuntimeView {
        const ra_cold_sources50LA::DecodedSources& source;
        const std::vector<track_runtime_storage50IG::Binding>& bindings;
        track_runtime_storage50IG::Storage& storage;
        std::vector<g::Type2StaticCandidate49X>& candidates;
        light::Owner& lighting;
    };
    // Native algorithms use the same fixed registry and immutable source
    // records. This view does not construct a replacement world or physics
    // arena. The caller must retire it before closing the aggregate; no
    // candidate vector or backing-container resize operation is exposed.
    std::shared_ptr<NativeRuntimeView> borrowNativeRuntime(
        const std::shared_ptr<Owner>& self,Diagnostic& d)noexcept {
        d={};d.stage=Stage::runtimeQueryStorage;
        if(self.get()!=this||terminal_||closed_||!prefix_.runtimeQueryStorage||
           !runtimeQueries_||!source_||!source_->sourcesDecoded()||!candidates_||
           !candidates_->staticBindings()||!runtimeQueries_->storage_.get()||!lighting_){
            d.failure="same retained RA source and fixed runtime storage required";return {};
        }
        try {
            auto view=std::unique_ptr<NativeRuntimeView>(new NativeRuntimeView{
                *source_->sourcesDecoded(),*candidates_->staticBindings(),*runtimeQueries_->storage_.get(),
                candidates_->candidates_,*lighting_});
            ++runtimeWorldBorrows_;
            return std::shared_ptr<NativeRuntimeView>(view.release(),[self](NativeRuntimeView* value)noexcept{
                delete value;--self->runtimeWorldBorrows_;
            });
        }catch(...){d.failure="RA native runtime borrow allocation failed";return {};}
    }
    // Pure source inspection stages. This does NOT call material Provider::
    // prepare, loadAudioBank, image decode, converter, resource population or GL.
    // Earlier closures remain owned when a later closure fails.
    // Native cold image/mesh ownership from this SAME authenticated archive.
    // Source CUB/SCB and every retained decoded tile are independently pinned.
    bool prepareSkyImages(const std::filesystem::path& textureRoot,Diagnostic& d)noexcept {
        d={};d.stage=Stage::authoring;
        if(terminal_||closed_||!prefix_.sky||!source_||!source_->sourcesDecoded()||skyImages_)
            return fail(d,"RA sky images require the actual fresh source/selection owner");
        try {
            auto images=std::make_unique<sky50gw_fixture::Owner>();
            sky50gw_fixture::Request request;
            request.track=track_source_fixture::Track::ra;request.textureRoot=textureRoot;
            request.retainedArchive=&source_->sourcesDecoded()->archive;
            request.retainedRaSource=source_;
            request.decodeOriginalImages=!runtime_asset_paths50LB::configuredRoot("RR_ORIGINAL_ASSET_ROOT").empty();
            sky50gw_fixture::Diagnostic detail;
            if(!images->load(request,detail)){d.sky=detail.message;return fail(d,"RA original sky image preparation failed");}
            skyImages_=std::move(images);return true;
        }catch(...){return fail(d,"RA original sky image allocation failed");}
    }
    // Decode directly from this aggregate's already authenticated original
    // archive. The ordinary image verifier still admits the final resources.
    bool decodeCrowdImages(std::array<std::vector<std::uint8_t>,6>& images,Diagnostic& d)const noexcept {
        d={};d.stage=Stage::staticWorldImages;
        if(terminal_||closed_||!source_||!source_->sourcesDecoded()){
            d.failure="RA crowd decode requires the actual original source owner";return false;}
        try {
            std::vector<std::uint8_t> atm;a::DecodeError48C error;
            if(!a::extractResMember48C(source_->sourcesDecoded()->archive,"textures\\crowd.atm",atm,error)){
                d.sky=error.detail;d.failure="RA original crowd member extraction failed";return false;}
            if(!ra_crowd_image50LA::Verifier::decodeRaSixOriginal(atm,images,d.sky)){
                d.failure="RA original crowd image decode failed";return false;}
            return true;
        }catch(...){d.failure="RA original crowd image allocation failed";return false;}
    }
    // Borrowed through this aggregate's real renderer/session lifetime.
    const sky50gw_fixture::Owner* skyAssets()const noexcept {
        return !terminal_&&!closed_&&skyImages_&&skyImages_->loaded()?skyImages_.get():nullptr;
    }
    bool sampleSky(std::uint32_t actualCameraZone48,bool& enabled)const noexcept {
        return !terminal_&&!closed_&&prefix_.sky&&sky_&&sky_->select(actualCameraZone48,enabled);
    }
    bool prepareAuthoring(Diagnostic& d) noexcept {
        if(!enter(d,Stage::authoring,authoringAttempted_,prefix_.sources))return false;
        try{
            triggers_=std::make_shared<ra_trigger_authoring50LA::Owner>();
            if(!triggers_->decode(source_,d.triggers))return fail(d,"RA trigger authoring failed");
            prefix_.triggers=true;
            sky_=std::make_unique<original_track_sky50KX::Owner>();
            if(!sky_->prepareRaSource(*source_,d.sky))return fail(d,"RA sky source selection failed");
            prefix_.sky=true;
            atm_=std::make_unique<ra_atm_authoring50LA::Owner>();
            if(!atm_->decode(source_,d.atm))return fail(d,"RA ATM raw closure failed");
            prefix_.atm=true;
            audio_=std::make_unique<ra_audio_authoring50LA::Owner>();
            if(!audio_->decode(source_,d.audio))return fail(d,"RA audio raw closure failed");
            prefix_.audio=true;
            particles_=std::make_unique<ra_particle_authoring50LA::Owner>();
            if(!particles_->decode(source_,d.particles))return fail(d,"RA particle source closure failed");
            prefix_.particleAuthoring=true;return true;
        }catch(...){return fail(d,"RA authoring allocation failure");}
    }
    // actualRacerCount must be read from the existing actual cold roster owner,
    // as required by the underlying light API. No default8 or alternate roster.
    // This stage is independent of static registry/particle construction order.
    bool prepareLighting(std::uint32_t actualRacerCount,Diagnostic& d) noexcept {
        if(!enter(d,Stage::lighting,lightingAttempted_,prefix_.sources))return false;
        try{
            lighting_=std::make_shared<light::Owner>();
            const auto* decoded=source_->sourcesDecoded();
            if(!decoded||!lighting_->prepareRaNative(decoded->memberBytes[0],actualRacerCount,d.lighting))
                return fail(d,"RA actual-roster lighting preparation failed");
            prefix_.lighting=true;return true;
        }catch(...){return fail(d,"RA lighting allocation failure");}
    }
    // Counter mutation is explicit and separate from immutable authoring.
    // Actual Application is strongly pinned by the existing particle owner.
    // The real session generation is passed without minting a substitute.
    bool constructDetachedParticlePrefixes(std::shared_ptr<g::OriginalApplicationEntities50LA> actualApplication,
        std::uint64_t actualSessionGeneration,Diagnostic& d) noexcept {
        if(!enter(d,Stage::detachedParticles,particlesAttempted_,prefix_.particleAuthoring))return false;
        if(trackParticleAttempted_&&(actualApplication!=trackParticleApplication_||
            actualSessionGeneration!=trackParticleGeneration_))
            return fail(d,"RA emitter/manager Application or session differs");
        emitterApplication_=actualApplication;emitterGeneration_=actualSessionGeneration;
        if(!particles_->constructDetached(std::move(actualApplication),actualSessionGeneration,d.particles))
            return fail(d,"RA detached particle constructor prefix failed");
        prefix_.detachedParticles=true;return true;
    }
    // Actual-source CPU stage only. No images, population, collector, scene
    // hierarchy, renderer or SessionAssets capability is constructed here.
    bool prepareStaticWorld(std::shared_ptr<a::OriginalStandardMaterialDomain50LA> domain,
        std::shared_ptr<const a::OriginalStandardMaterialContext50LA> context,
        const a::OriginalStandardMaterialPath50LA& actualPath,Diagnostic& d)noexcept {
        if(runtimeQueries_&&runtimeQueries_->rosterResources_&&
           (!rosterMountComplete_||domain!=rosterMountDomain_)){
            d={};return fail(d,"retained roster archives must mount in this domain before world revision capture");
        }
        if(!enter(d,Stage::staticWorld,worldAttempted_,prefix_.sources))return false;
        try {
            world_.reset(new ra_static_world50LA::Owner);
            if(!world_->prepare(source_,std::move(domain),std::move(context),actualPath,d.world))
                return fail(d,"RA full static-world cold prefix failed");
            prefix_.staticWorld=true;return true;
        }catch(...){return fail(d,"RA full static-world allocation failure");}
    }
    // Definition lives in the Application-aware RA header. Requires the same
    // staged world/domain, before image population/collector capture. This is
    // a genuine5000-slot constructor, not attachment or renderer publication.
    bool constructRuntimeTrackParticleManager(
        const original_main_entry50KG::Application& actualApplication,
        std::uint64_t actualSessionGeneration,Diagnostic& d)noexcept;
    // Explicit cold stage; no automatic call from any active route. Source
    // StaticModels slot order and model construction only, no observer/append.
    bool constructStaticModelPrefixes(const original_main_entry50KG::Application&,
        Diagnostic&)noexcept;
    bool completeStaticModelResourcesAndPlacement(Diagnostic& d)noexcept {
        if(!enter(d,Stage::staticWorld,staticModelResourcesAttempted_,prefix_.staticModelPrefixes))return false;
        if(!world_||!world_->completeModelResourcesAndPlacement(d.staticModels))
            return fail(d,"RA static resource/placement stage failed; retained prefix requires close");
        prefix_.staticModelResources=true;return true;
    }
    bool registerStaticModelObserversAndParents(Diagnostic& d)noexcept {
        if(!enter(d,Stage::staticWorld,staticObserversAttempted_,prefix_.staticModelResources&&prefix_.lighting))return false;
        if(!world_||!world_->registerModelObservers(lighting_,d.staticModels))
            return fail(d,"RA static observer/parent prefix failed; actual ownership retained");
        prefix_.staticModelObservers=true;return true;
    }
    // Private same-owner camera construction/retirement. No Session pointer
    // escapes; construction alone creates no camera publication or readiness.
    bool constructRuntimeSourceCamera(const original_main_entry50KG::Application&,
        const ra_source_camera50LA::CameraCallerOwners&,
        const std::vector<std::uint8_t>& selectedOriginalCam,
        original_camera_session50KX::DisplayInputs&,Diagnostic&)noexcept;
    bool constructRetainedRuntimeSourceCamera(const original_main_entry50KG::Application&,
        const ra_source_camera50LA::CameraRuntimeOwners&,
        const std::vector<std::uint8_t>& selectedOriginalCam,
        original_camera_session50KX::DisplayInputs&,Diagnostic&)noexcept;
    g::OriginalWreckComponent50LA* introController174A30(Diagnostic& d)noexcept {
        d={};d.stage=Stage::sourceCamera;
        if(terminal_||closed_||!runtimeQueries_)return nullptr;
        return runtimeQueries_->currentIntroController174A30();
    }
    bool constructIntroAudio177750(std::shared_ptr<const a::AudioBank50HC> bank,
        std::size_t firstVoice,std::size_t voiceCount,Diagnostic& d)noexcept {
        d={};d.stage=Stage::sourceCamera;
        return !terminal_&&!closed_&&runtimeQueries_&&
            runtimeQueries_->constructCurrentIntroAudio177750(std::move(bank),firstVoice,voiceCount);
    }
    // A native consumer borrows the actual retained Session. The strong
    // aggregate capture pins every world/query/model dependency. Explicit
    // close/abandon rejects while a borrow survives, so no reset can invalidate
    // the driver pointer between the real countdown/frame/display calls.
    std::shared_ptr<original_camera_session50KX::Session> borrowRuntimeSourceCamera(
        const std::shared_ptr<Owner>& self,Diagnostic& d)noexcept {
        d={};d.stage=Stage::sourceCamera;
        if(self.get()!=this||terminal_||closed_||!runtimeQueries_||
           !runtimeQueries_->cameraBankCurrent()||!runtimeQueries_->camera_||
           !runtimeQueries_->camera_->session||!runtimeQueries_->camera_->session->ready()){
            d.failure="actual prepared RA camera and retained aggregate required";return {};
        }
        try {
            ++runtimeCameraBorrows_;
            return std::shared_ptr<original_camera_session50KX::Session>(
                runtimeQueries_->camera_->session.get(),[self](original_camera_session50KX::Session*)noexcept{
                    --self->runtimeCameraBorrows_;
                });
        }catch(...){
            // shared_ptr invokes the supplied deleter if control-block allocation
            // fails; it has already discharged the borrow count in that case.
            d.failure="RA native camera borrow allocation failed";return {};
        }
    }
    original_camera_session50KX::Result prepareRuntimeSourceCamera(
        std::uint64_t actualSessionGeneration,Diagnostic& d)noexcept {
        d={};d.stage=Stage::sourceCamera;
        if(terminal_||closed_||!runtimeQueries_){d.failure="RA camera owner unavailable";
            d.runtimeQueries.camera.status=original_camera_session50KX::Status::owner;return d.runtimeQueries.camera;}
        return runtimeQueries_->prepareCamera(actualSessionGeneration,d.runtimeQueries);
    }
    original_camera_session50KX::Result closeRuntimeSourceCamera(Diagnostic& d)noexcept {
        d={};d.stage=Stage::sourceCamera;
        if(runtimeCameraBorrows_){d.failure="retire actual native camera borrows before camera destruction";
            d.runtimeQueries.camera.status=original_camera_session50KX::Status::owner;return d.runtimeQueries.camera;}
        if(!runtimeQueries_){d.runtimeQueries.camera.status=original_camera_session50KX::Status::complete;return d.runtimeQueries.camera;}
        std::uint32_t fogPC=0;
        if(!runtimeQueries_->canWithdrawNativeFogBorrow()||!retireNativeFogCustody(fogPC)){
            d.failure="actual Fog custody must retire before CameraBinding destruction";
            d.runtimeQueries.camera.status=original_camera_session50KX::Status::owner;
            d.runtimeQueries.camera.sourcePC=fogPC;return d.runtimeQueries.camera;}
        return runtimeQueries_->closeCamera(false,d.runtimeQueries);
    }
    // As on the existing route, root must actually cancel/retire pending fade
    // work before using abandonment. This operation performs no fake drain.
    original_camera_session50KX::Result abandonRuntimeSourceCamera(Diagnostic& d)noexcept {
        d={};d.stage=Stage::sourceCamera;
        if(runtimeCameraBorrows_){d.failure="retire actual native camera borrows before camera destruction";
            d.runtimeQueries.camera.status=original_camera_session50KX::Status::owner;return d.runtimeQueries.camera;}
        if(!runtimeQueries_){d.runtimeQueries.camera.status=original_camera_session50KX::Status::complete;return d.runtimeQueries.camera;}
        std::uint32_t fogPC=0;
        if(!runtimeQueries_->canWithdrawNativeFogBorrow()||!retireNativeFogCustody(fogPC)){
            d.failure="actual Fog custody must retire before CameraBinding destruction";
            d.runtimeQueries.camera.status=original_camera_session50KX::Status::owner;
            d.runtimeQueries.camera.sourcePC=fogPC;return d.runtimeQueries.camera;}
        return runtimeQueries_->closeCamera(true,d.runtimeQueries);
    }
    // Caller supplies only actual pinned root-produced crowd RRTEX bytes.
    // This stage may construct canonical CPU resources, never a draw route.
    bool prepareStaticWorldImages(const std::array<std::vector<std::uint8_t>,6>& actualCrowdRrtex,
        Diagnostic& d)noexcept {
        // Retained racer rows must have their real resource/ref/source prefix
        // before the one whole-domain scan captures the combined collector.
        if(runtimeQueries_&&runtimeQueries_->rosterResources_&&
           (!rosterModelResourcesComplete_||runtimeQueries_->terminal_||runtimeQueries_->closed_||
            runtimeQueries_->cameraAttempted_||!runtimeQueries_->cameraBankCurrent()||
            std::find(runtimeQueries_->rosterSlotComplete_.begin(),runtimeQueries_->rosterSlotComplete_.end(),false)!=
                runtimeQueries_->rosterSlotComplete_.end())){
            d={};d.stage=Stage::staticWorldImages;
            return fail(d,"retained racer model resources must precede world population");
        }
        if(!enter(d,Stage::staticWorldImages,worldImagesAttempted_,prefix_.staticWorld))return false;
        if(!world_->prepareImages(actualCrowdRrtex,d.world))
            return fail(d,"RA whole-world source/image/population prefix failed");
        prefix_.staticWorldImages=true;return true;
    }
    // Requires an already genuinely published SourceCamera and idle Bank.
    // This never advances or activates RA; failure discards previous snapshots.
    bool prepareNativeStaticLighting(Diagnostic& d)noexcept {
        d={};staticLighting_.reset();
        if(terminal_||closed_||!prefix_.staticWorldDrawPlan||!lighting_||!runtimeQueries_){
            d.failure="actual world/light/camera publication required";return false;}
        try {
            std::unique_ptr<ra_static_lighting50LA::Binding> next(new ra_static_lighting50LA::Binding);
            if(!next->prepare(world_,lighting_,runtimeQueries_,d.lighting)){
                d.failure="native static lighting binding rejected; no camera publication or source proof";return false;}
            staticLighting_=std::move(next);return true;
        }catch(...){d.failure="native static lighting allocation failed";return false;}
    }
    bool readNativeStaticLighting(std::size_t surface,ra_static_lighting50LA::Snapshot& out)const noexcept {
        return !terminal_&&!closed_&&staticLighting_&&staticLighting_->read(surface,out);
    }
    // Explicit unused native preparation: rebuilds the SAME published camera
    // from its retained RA fog; does not advance simulation or activate RA.
    bool prepareNativeStaticFogObservations(Diagnostic& d)noexcept {
        d={};staticFog_.reset();
        if(terminal_||closed_||!prefix_.staticWorldDrawPlan||!world_||!runtimeQueries_){
            d.failure="actual RA world and published SourceCamera required";return false;}
        if(!runtimeQueries_->prepareFog(d.runtimeQueries)){
            d.failure="retained RA fog preparation unavailable";return false;}
        try {
            std::unique_ptr<ra_static_fog50LA::Binding> next(new ra_static_fog50LA::Binding);
            if(!next->prepare(world_,runtimeQueries_)){
                d.failure="RA fog source operands rejected; selected scheduling remains unjoined";return false;}
            staticFog_=std::move(next);return true;
        }catch(...){d.failure="RA fog observation allocation failed";return false;}
    }
    bool prepareNativeStaticDrawPublication(Diagnostic& d)noexcept {
        d={};
        if(terminal_||closed_||!world_||!world_->drawPlanCurrent()||!runtimeQueries_||
           !runtimeQueries_->prepareNativeStaticDraw(d.runtimeQueries)){
            d.failure="current RA native static draw publication unavailable";return false;}
        return true;
    }
    const std::vector<g::original_gc_gameplay_lighting_candidate::NativeRaStaticDraw50LA>*
    nativeStaticDrawPublication()const noexcept {
        return !terminal_&&!closed_&&world_&&world_->drawPlanCurrent()&&runtimeQueries_?
            runtimeQueries_->nativeStaticDraw():nullptr;
    }
    bool readNativeStaticFogObservation(std::size_t placement,
        ra_static_fog50LA::CameraObservation& camera,ra_static_fog50LA::PlacementObservation& out)const noexcept {
        return !terminal_&&!closed_&&staticFog_&&staticFog_->read(placement,camera,out);
    }
    bool readNativeStaticRawPosition(std::size_t surface,std::size_t vertex,
        std::array<std::int32_t,3>& out)const noexcept {
        return !terminal_&&!closed_&&staticFog_&&staticFog_->rawPosition(surface,vertex,out);
    }
    // CPU generated-color publication only. No raster, GPU or RA admission.
    bool prepareNativeStaticDiffuseStream(ra_static_lighting50LA::StaticDiffuseDiagnostic& d)noexcept {
        if(terminal_||closed_||!staticLighting_){d={};
            d.status=ra_static_lighting50LA::StaticDiffuseStatus::stale;return false;}
        return staticLighting_->prepareDiffuseStream(d);
    }
    bool readNativeStaticDiffuseVertex(std::size_t surface,std::size_t vertex,
        ra_static_lighting50LA::StaticDiffuseSurface& identity,
        g::original_racer_lighting50KM::DiffuseVertexRgba& rgba)const noexcept {
        return !terminal_&&!closed_&&staticLighting_&&
            staticLighting_->readDiffuseVertex(surface,vertex,identity,rgba);
    }
    // Complete CPU source/raster inventory; no renderer-ready capability.
    // Must precede private GPU publication. All unresolved native lighting and
    // coverage requirements remain visible rather than dropping surfaces.
    bool prepareStaticWorldDrawPlan(Diagnostic& d)noexcept {
        if(!enter(d,Stage::staticWorldDrawPlan,worldDrawPlanAttempted_,prefix_.staticWorldImages))return false;
        if(!world_->prepareDrawPlan(d.world))return fail(d,"RA full-world draw inventory failed");
        prefix_.staticWorldDrawPlan=true;return true;
    }
    // Explicit native model-service join; no frame clock, material row or
    // external model pointer can be injected. No renderer activation here.
    bool serviceStaticCrowdSurface241848(std::size_t surface,Diagnostic& d)noexcept {
        d={};d.stage=Stage::staticWorldImages;
        if(terminal_||closed_||!prefix_.staticWorldImages||!world_)
            return fail(d,"actual full-world image owner required for model service");
        if(!world_->serviceCrowdSurface241848(surface,d.world))
            return fail(d,"actual retained crowd model service rejected");
        return true;
    }
    const ra_static_world50LA::Owner* coldStaticWorld()const noexcept {
        return prefix_.staticWorld&&!terminal_&&!closed_?world_.get():nullptr;
    }
    // Accounting only: prefixes may remain true after terminal failure/close.
    // No complete()/ready()/SessionAssets or mutable runtime getter exists.
    Prefix completedPrefixes()const noexcept{return prefix_;}
    bool terminalFailure()const noexcept{return terminal_;}
    bool closed()const noexcept{return closed_;}
    track_runtime_storage50IG::Counts reservedStaticCounts()const noexcept {
        return registry_?registry_->reservedCounts():track_runtime_storage50IG::Counts{};
    }
    // Explicit native normal ownership cleanup, after real external borrows
    // retire. Does not dispatch or claim the original26FD80 destructor.
    struct NativeCloseDiagnostic {
        const char* gate=nullptr;
        bool counted=false,retireCounted=false,topologyAttempted=false,topologyClosed=false;
        std::uint64_t cameraBorrows=0,worldBorrows=0,worldReferences=0;
        long runtimeOwners=0,navigationOwners=0,lightingOwners=0,particleOwners=0;
        std::size_t expectedLightingOwners=0;
        std::uint32_t sourcePC=0,worldFlags=0,worldMask=0;
    };
    const NativeCloseDiagnostic& nativeCloseDiagnostic()const noexcept{return nativeCloseDiagnostic_;}
    bool closeNativeLoadedWorld() noexcept {return closeOwnedNativeResources(true);}
    bool close() noexcept {return closeOwnedNativeResources(false);}
private:
    // Only the concrete native renderer (existing friend) may borrow this
    // same actual strip. Record identity is its address in the current mixed
    // publication, not a caller-created identity/scissor or readiness flag.
    template<class Visitor>
    bool visitNativeLocalFlareEmission(
        const g::original_gc_gameplay_lighting_candidate::NativeRaStaticDraw50LA& record,
        Visitor&& visitor,rr::assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        d={};g::CameraViewOutput49F camera;
        if(terminal_||closed_||!world_||!world_->drawPlanCurrent()||!runtimeQueries_||!sceneApplication_||
           !runtimeQueries_->readNativeLocalFlareCamera(record,*sceneApplication_,camera)){
            d.status=rr::assets::OriginalStandardMaterialStatus50LA::input;d.sourcePC=0x14b2c0;return false;}
        g::OriginalPodCableStripCold50LA* strip=nullptr;
        std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table;
        //After entry, any failure retains partial stores/resources and closes
        //this actual owner's consumer permanently; no callback replay/reset.
        const auto stop=[&]()noexcept {const auto index=nativeLocalFlareAuthoredIndex(record.flareIdentity);
            if(index)nativePoints_[*index-2u].flare->consumerTerminal=true;return false;};
        if(!prepareNativeLocalFlareStrip(record,camera,strip,table,d))return stop();
        if(strip){try {if(!visitor(*strip,table,d))return stop();}catch(...){
            d.status=rr::assets::OriginalStandardMaterialStatus50LA::allocation;return stop();}}
        g::CameraViewOutput49F after;
        if(!runtimeQueries_->readNativeLocalFlareCamera(record,*sceneApplication_,after)||
           !nativeLocalFlareCurrent()){
            d.status=rr::assets::OriginalStandardMaterialStatus50LA::input;d.sourcePC=0x14b8e0;return stop();}
        return true;
    }
    NativeCloseDiagnostic nativeCloseDiagnostic_;
    bool closeOwnedNativeResources(bool retireCountedWorld) noexcept {
        if(nativeScbThread_!=std::this_thread::get_id()||constructorRacerWorldBorrows_||sourceParticleInvocationBorrowers_||sourceFogTransferBorrowers_)return false;
        if(closed_)return true;
        auto& diagnostic=nativeCloseDiagnostic_;diagnostic={};
        diagnostic.counted=worldColdConstructed_;diagnostic.retireCounted=retireCountedWorld;
        diagnostic.topologyAttempted=topologyAttempted_;diagnostic.topologyClosed=topologyClosed_;
        diagnostic.cameraBorrows=runtimeCameraBorrows_;diagnostic.worldBorrows=runtimeWorldBorrows_;
        diagnostic.runtimeOwners=runtimeQueries_.use_count();diagnostic.navigationOwners=navigationLease_.use_count();
        diagnostic.lightingOwners=lighting_.use_count();diagnostic.particleOwners=trackParticles_.use_count();
        diagnostic.worldReferences=worldEntity_.reference00;diagnostic.worldFlags=worldEntity_.flags29;
        diagnostic.worldMask=worldEntity_.mask20;
        const auto reject=[&](const char* gate){diagnostic.gate=gate;return false;};
        if(worldColdConstructed_&&!retireCountedWorld)
            return reject("counted world requires explicit retirement");
        std::uint32_t sourceTrackClosePC=0;
        if(!abortSourceTrackNative(sourceTrackClosePC))return reject("source Track/Config native withdrawal");
        //Revoke native load observation BEFORE any fallible cleanup. This is
        //not a claim that retail1573B0 ran or World34 was source-null-stored.
        nativeScbRevoked_=true;nativeAub38_.reset();nativeSunFlareRead271DD0_.reset();if(nativeCsCameraTable3E4_&&!nativeCsCameraTable3E4_->sourceMode){nativeCsCameraZones3E8_=nullptr;nativeCsCameraTable3E4_.reset();}nativeScb34_.reset();nativeTob34_.reset();if(!sourceConfig34Published_)worldParser34_=nullptr;nativeSceneInfo270900_.reset();
        if(worldColdConstructed_){
            const char* prefix=nullptr;
            if(!canRetireNativeWorldPrefix(&prefix))return reject(prefix?prefix:"world prefix preflight");
        }
        if(runtimeCameraBorrows_||runtimeWorldBorrows_)return reject("live camera/world borrows");
        std::uint32_t fogPC=0;
        if((runtimeQueries_&&!runtimeQueries_->canWithdrawNativeFogBorrow())||
           !retireNativeFogCustody(fogPC))return reject("native Fog custody retirement");
        nativeFogDescriptor3AC_.reset();

        // These aggregate-owned observation bindings each retain the actual
        // runtime bridge. Retire them before counting external runtime owners.
        staticFog_.reset();
        staticLighting_.reset(); // Retire typed camera/world/light borrows before preflight.
        //Concrete LightManager RA source-zone custody borrows the SAME
        //canonical owner. Retire this private partial-load domain before normal
        //canonical-owner use-count preflight and before native gameplay ends.
        if(!retireNativeRaLightZones148110())return reject("RA LightManager zone retirement");
        diagnostic.runtimeOwners=runtimeQueries_.use_count();
        // This explicit counted-world route must actually destroy its runtime
        // storage below; an external strong pin cannot postpone that teardown.
        if(worldColdConstructed_&&runtimeQueries_&&runtimeQueries_.use_count()!=1)return reject("runtime strong owners");
        diagnostic.navigationOwners=navigationLease_.use_count();
        if(navigationLease_&&navigationLease_.use_count()!=1)return reject("navigation strong owners");
        // Preflight the sole actual Application reference/count owner BEFORE
        // releasing anything. Unexpected live links/refs reject destruction.
        if(!canRetireNativeInstanceMaster())return reject("actual InstanceMaster custody preflight");
        if(!canCloseTopology())return reject("scene topology preflight");
        if(world_&&!world_->canClose())return reject("static world publication/material preflight");
        if(runtimeQueries_&&!runtimeQueries_->canClose())return reject("runtime query retirement preflight");
        if(lighting_){
            const auto expected=1u+std::size_t(runtimeQueries_&&runtimeQueries_->freshLighting_==lighting_)+
                std::size_t(world_&&world_->holdsStaticVisibility(lighting_.get()));
            diagnostic.expectedLightingOwners=expected;
            diagnostic.lightingOwners=lighting_.use_count();
            if(lighting_.use_count()!=static_cast<long>(expected))return reject("lighting strong owners");
        }
        if(trackParticles_&&trackParticles_.use_count()!=1)return reject("track particle strong owners");
        if(!canRetireNativeCollisionQueries27B040())return reject("native collision query lifetime preflight");
        if(!canRetireNativeFinishLine540())return reject("native finishline lifetime preflight");
        if(!canRetireNativeSunFlare())return reject("native SunFlare lifetime preflight");
        if(!canRetireNativeSky3F8())return reject("native sky resource prefix preflight");
        if(!canRetireNativeTriggerList3D0())return reject("native trigger lifetime preflight");
        if(!canRetireNativeParticleList3D8())return reject("native emitter descriptor preflight");
        if(!canRetireNativeSoulTreeSounds3B8())return reject("native SoulTreeSound descriptor preflight");
        if(!canRetireNativeMaterialSounds3BC())return reject("native material sound descriptor preflight");
        //Unconsumed native descriptors borrow audio_'s immutable full AUB.
        //Dispose this genuine private metadata ownership before audio_ reset.
        if(!retireNativeSky3F8())return reject("native sky resource prefix retirement");
        //Withdraw the immutable Environment+110 borrow before actual lighting/
        //Environment teardown. This does not touch or infer World400.
        nativeWaterNone274570_.reset();
        if(!retireNativeSoulTreeSounds3B8())return reject("native SoulTreeSound descriptor retirement");
        if(!retireNativeMaterialSounds3BC())return reject("native material sound descriptor retirement");
        //Descriptors borrow actual immutable attachment rows; retire before that owner.
        if(!retireNativeParticleList3D8())return reject("native emitter descriptor retirement");
        if(particles_&&!particles_->close())return reject("detached particles close");
        // Detached emitters/real camera borrowers must retire before the
        // source pool/material150/Entity cleanup. Failures retain the owner.
        if(trackParticles_&&!trackParticles_->close())return reject("track particles close");
        nativeParticleManager4A0_=nullptr;nativeParticleTexture41C_=nullptr;

        //Finishline's dedicated strip/material and copied anchor data retire
        //before existing model resources. Never leave a borrowed model alias.
        if(!retireNativeFinishLine540())return reject("native finishline retirement");
        {std::uint32_t pc=0;if(!detachNativeCollisionQueries27B170(pc)){diagnostic.sourcePC=pc;return reject("collision suffix before model retirement");}}
        if(!withdrawModelParents3C4())return reject("source World3C4 publication withdrawal");
        if(world_&&!world_->close())return reject("static world close");
        trackParticles_.reset();trackParticleApplication_.reset();trackParticleDomain_.reset();
        rosterModelResourcesComplete_=false; // Mount custody survives fallible later cleanup.
        world_.reset();
        particles_.reset();emitterApplication_.reset();audio_.reset();atm_.reset();
        // World publication preflight above prevents any backing from retiring
        // while its image or future renderer lease is live.
        if(!retireNativeInstanceMaster())return reject("actual InstanceMaster custody retirement");
        if(!withdrawNativeTriggerObserverStorage())return reject("trigger observer borrowed topology withdrawal");
        if(!closeTopology())return reject("scene topology close");
        lighting_.reset();skyImages_.reset();sky_.reset();triggers_.reset();
        // Storage drops its exclusive serial world lease before radii/source.
        runtimeQueries_.reset();registry_.reset();
        {std::uint32_t pc=0;if(!retireNativeCollisionQueries11B4A0(pc)){diagnostic.sourcePC=pc;return reject("collision resources after registry retirement");}}
        // Actual bank/query/membership gates passed, then Storage retired.
        // Child collider identities retire before triggers, and list last.
        if(!retireNativeSunFlare())return reject("native SunFlare lifetime retirement");
        if(!retireNativeTriggerList3D0())return reject("native trigger lifetime retirement");
        // Queries are retired above. Revoke candidate-owned metadata aliases.
        if(candidates_)for(auto& candidate:candidates_->candidates_)candidate.terrainDescriptor180=nullptr;
        if(worldColdConstructed_){std::uint32_t pc=0;
            if(!retireNativeWorldPrefixAfterOwnedCleanup(pc)){diagnostic.sourcePC=pc;return reject("counted world prefix retirement");}}
        candidates_.reset();nativeTerrain3B0_.reset();navigationLease_.reset();source_.reset();
        // Actual model/PST/World/query/read owners have retired above. Keep
        // exact partial mount custody on refusal; no whole-provider reset.
        if(nativeRosterMounts_.provider_&&!nativeRosterMounts_.closed_){
            a::OriginalStandardTextureDiagnostic50LA mountDiagnostic;
            if(!rosterMountDomain_||!rosterMountDomain_->resolver3E1A70()||
               !rosterMountDomain_->resolver3E1A70()->retireNativeRaceMounts1316A0(nativeRosterMounts_,mountDiagnostic))
                return reject("actual RA mount consumers still live or custody changed");
        }
        rosterMountDomain_.reset();rosterMountApplication_=nullptr;
        collisionApplication_.reset();closed_=true;return true;
    }
};
}
