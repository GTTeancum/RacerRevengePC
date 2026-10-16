#pragma once
#include "ra_static_registry50la.h"
#include "ra_cold_navigation_lease50la.h"
#include "original_racer_material_sources50la_fixture.h"
#include "ra_source_camera_ownership50la.h"
#include "ra_pod_scene50la.h"
#include "ra_static_world50la.h"
#include "ra_static_lighting50la.h"
#include "ra_static_fog_binding50la.h"
#include "ra_trigger_authoring50la.h"
#include "original_track_sky50kx_fixture.h"
#include "sky50gw_fixture.h"
#include "ra_atm_authoring50la.h"
#include "ra_audio_authoring50la.h"
#include "ra_particle_authoring50la.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/original_gc_gameplay_lighting_candidate.h"
#include <exception>
#include <algorithm>
#include <memory>
#include <string>
#include <utility>

namespace original_main_entry50KG { struct Application; }

namespace ra_cold_aggregate50LA {
namespace a=rr::assets;namespace g=rr::game;
namespace light=g::original_gc_gameplay_lighting_candidate;
// These are accounting stages, not original source constructor/scene receipts.
// Dependency order is explicit; no unproved full retail loader order is implied.
enum class Stage : std::uint8_t {none,sources,staticCandidates,staticRegistry,
    authoring,lighting,detachedParticles,closed,staticWorld,staticWorldImages,staticWorldDrawPlan,runtimeQueryStorage,runtimeQueryOwners,runtimeQueryWorldOwners,trackParticleManager,sourceCamera};
struct Prefix {
    bool sources=false,staticCandidates=false,staticRegistry=false;
    bool runtimeQueryStorage=false,runtimeQueryOwners=false,runtimeQueryWorldOwners=false;
    bool triggers=false,sky=false,atm=false,audio=false,particleAuthoring=false;
    bool staticWorldDrawPlan=false,trackParticleManager=false,staticModelPrefixes=false,staticModelResources=false;
    bool staticModelObservers=false;
    bool lighting=false,detachedParticles=false,staticWorld=false,staticWorldImages=false;
};
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
    friend class rr::game::OriginalRaceConstructor50LA;
    friend class rr::platform::MaterialTextureGl50EB;
    friend class rr::platform::VehicleWindowGl50DS;
    friend class ra_cold_roster50LA::Operation;
    // One actual decoded source owner. All child borrows die before it. No
    // external source/registry injection or second source decode is possible.
    std::shared_ptr<ra_cold_sources50LA::Owner> source_;
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
    std::size_t runtimeCameraBorrows_=0,runtimeWorldBorrows_=0;
    bool rosterModelResourcesAttempted_=false,rosterModelResourcesComplete_=false;
    bool rosterMountAttempted_=false,rosterMountComplete_=false;
    const original_main_entry50KG::Application* rosterMountApplication_=nullptr;
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> rosterMountDomain_;
    // Application-aware definitions beside the retained Operation. Only the
    // actual Bank pin can supply the operation and its stable arena arrays.
    std::shared_ptr<ra_cold_roster50LA::Operation> retainedRosterForHandoff(
        native_racer50HN::Bank&,Diagnostic&)noexcept;
    bool failRetainedHandoff(Diagnostic&,const char*)noexcept;
    bool fail(Diagnostic& d,const char* why) noexcept {staticFog_.reset();staticLighting_.reset();terminal_=true;d.failure=why;return false;}
    bool enter(Diagnostic& d,Stage stage,bool& attempted,bool dependency) noexcept {
        d={};d.stage=stage;
        if(terminal_||closed_||attempted||!dependency)return fail(d,"RA aggregate stage dependency/lifetime rejected");
        attempted=true;return true;
    }
public:
    bool constructWorldCold1B2870(const original_main_entry50KG::Application&,
        rr::platform::RaceAudioGroups50IC&,rr::platform::AudioMixer50HD&,std::uint64_t,std::uint32_t&)noexcept;
    bool prepareWorldPhysicsIndex270CDC(std::uint32_t& pc)noexcept {
        pc=0;
        if(closed_||terminal_||!worldColdConstructed_||trackParticleAttempted_||
           modelPrefixesAttempted_||particlesAttempted_||runtimeCameraBorrows_||runtimeWorldBorrows_){
            pc=0x270cc4;return false;
        }
        return ra_pod_scene50LA::Owner::prepareWorldPhysicsIndex270CDC(source_,pc);
    }
    bool prepareWorldCollisionObjects271230(std::uint32_t& pc)noexcept {
        pc=0;
        if(closed_||terminal_||trackParticleAttempted_||modelPrefixesAttempted_||particlesAttempted_||
           runtimeCameraBorrows_||runtimeWorldBorrows_){pc=0x271230;return false;}
        return ra_pod_scene50LA::Owner::prepareWorldCollisionObjects271230(source_,pc);
    }
    bool prepareWorldCollisionResources(const original_main_entry50KG::Application&,
        rr::assets::OriginalStandardTextureDiagnostic50LA&)noexcept;
    bool rollbackNativeWorldConstructionFailure(std::uint32_t& pc)noexcept {
        return ra_pod_scene50LA::Owner::rollbackNativeWorldConstructionFailure(pc);
    }
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    ~Owner(){if(!close())std::terminate();}
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
        Diagnostic& d)noexcept {
        d={};d.stage=Stage::runtimeQueryWorldOwners;
        if(terminal_||closed_||!prefix_.runtimeQueryWorldOwners||!runtimeQueries_)
            return fail(d,"RA pod context requires actual registered runtime backing");
        if(!runtimeQueries_->bindPodLiveQuery(actualBank,slot,actualBindings,actualParameters,d.runtimeQueries))
            return fail(d,"RA actual pod context binding failed; retained partial slot effects");
        return true;
    }
    bool placeRetainedRuntimeGridSlot(native_racer50HN::Bank&,std::size_t,Diagnostic&)noexcept;
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
            sky50gw_fixture::Diagnostic detail;
            if(!images->load(request,detail)){d.sky=detail.message;return fail(d,"RA original sky image preparation failed");}
            skyImages_=std::move(images);return true;
        }catch(...){return fail(d,"RA original sky image allocation failed");}
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
        return runtimeQueries_->closeCamera(false,d.runtimeQueries);
    }
    // As on the existing route, root must actually cancel/retire pending fade
    // work before using abandonment. This operation performs no fake drain.
    original_camera_session50KX::Result abandonRuntimeSourceCamera(Diagnostic& d)noexcept {
        d={};d.stage=Stage::sourceCamera;
        if(runtimeCameraBorrows_){d.failure="retire actual native camera borrows before camera destruction";
            d.runtimeQueries.camera.status=original_camera_session50KX::Status::owner;return d.runtimeQueries.camera;}
        if(!runtimeQueries_){d.runtimeQueries.camera.status=original_camera_session50KX::Status::complete;return d.runtimeQueries.camera;}
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
    NativeCloseDiagnostic nativeCloseDiagnostic_;
    bool closeOwnedNativeResources(bool retireCountedWorld) noexcept {
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
        if(worldColdConstructed_){
            if(!retireCountedWorld)return reject("counted world requires explicit retirement");
            const char* prefix=nullptr;
            if(!canRetireNativeWorldPrefix(&prefix))return reject(prefix?prefix:"world prefix preflight");
        }
        if(runtimeCameraBorrows_||runtimeWorldBorrows_)return reject("live camera/world borrows");
        // These aggregate-owned observation bindings each retain the actual
        // runtime bridge. Retire them before counting external runtime owners.
        staticFog_.reset();
        staticLighting_.reset(); // Retire typed camera/world/light borrows before preflight.
        diagnostic.runtimeOwners=runtimeQueries_.use_count();
        // This explicit counted-world route must actually destroy its runtime
        // storage below; an external strong pin cannot postpone that teardown.
        if(worldColdConstructed_&&runtimeQueries_&&runtimeQueries_.use_count()!=1)return reject("runtime strong owners");
        diagnostic.navigationOwners=navigationLease_.use_count();
        if(navigationLease_&&navigationLease_.use_count()!=1)return reject("navigation strong owners");
        // Preflight the sole actual Application reference/count owner BEFORE
        // releasing anything. Unexpected live links/refs reject destruction.
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
        if(particles_&&!particles_->close())return reject("detached particles close");
        // Detached emitters/real camera borrowers must retire before the
        // source pool/material150/Entity cleanup. Failures retain the owner.
        if(trackParticles_&&!trackParticles_->close())return reject("track particles close");
        if(world_&&!world_->close())return reject("static world close");
        trackParticles_.reset();trackParticleApplication_.reset();trackParticleDomain_.reset();
        rosterMountDomain_.reset();rosterMountApplication_=nullptr;rosterModelResourcesComplete_=false;
        world_.reset();
        particles_.reset();emitterApplication_.reset();audio_.reset();atm_.reset();
        // World publication preflight above prevents any backing from retiring
        // while its image or future renderer lease is live.
        if(!closeTopology())return reject("scene topology close");
        lighting_.reset();skyImages_.reset();sky_.reset();triggers_.reset();
        // Storage drops its exclusive serial world lease before radii/source.
        runtimeQueries_.reset();registry_.reset();
        if(worldColdConstructed_){std::uint32_t pc=0;
            if(!retireNativeWorldPrefixAfterOwnedCleanup(pc)){diagnostic.sourcePC=pc;return reject("counted world prefix retirement");}}
        candidates_.reset();navigationLease_.reset();source_.reset();
        collisionApplication_.reset();closed_=true;return true;
    }
};
}
