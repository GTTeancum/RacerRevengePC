#pragma once
// Private retained constructor prefix, NOT Page ancestry or a complete Scene.
#include "original_ui_garage_droid_audio50gn.h"
#include "original_ui_garage_visibility50kg.h"
#include "original_audio_sample_cache50kg.h"
#include "original_ui_garage_scene_config50kg.h"
#include "original_ui_garage_scene_setup50kg.h"
#include "original_ui_garage_scene_dispatch50kg.h"
#include "original_ui_garage_toy_fields50kg.h"
#include "original_ui_garage_audio_fields50kg.h"
#include <optional>
#include <array>
#include <memory>
namespace original_main_entry50KG {struct Application;}
namespace rr::platform {
class OriginalUiMainAudio50KG;
struct OriginalUiMainApplication50KG;
struct OriginalUiMainProfile50KG;
class OriginalUiGarageStaticModels50KG;
class OriginalUiGarageSceneSetup50KG;
class OriginalUiGarageSceneLighting50KG;
class OriginalUiGarageSceneParticles50KG;
class OriginalUiGarageSceneCollision50KG;
class OriginalUiGarageSceneEmitters50KG;
class OriginalUiLightManagerOwner50KG;
class OriginalUiGarageFogColorLease50KG;
class OriginalUiGaragePageOwner50KG;
class OriginalUiGarageRendererLease50KG;
struct VehicleWindowDiagnostic50DS;
class OriginalUiGarageSession50KG;
class OriginalAudioPendingWaves50KG;
class OriginalUiGaragePodScene50KG final {
    friend struct ::original_main_entry50KG::Application;
    friend class OriginalUiGarageSession50KG;
    friend class OriginalAudioPendingWaves50KG;
    friend class OriginalUiGarageDroidAudio50GN;
    friend class OriginalUiMainAudio50KG;
    friend class OriginalUiGarageStaticModels50KG;
    friend class OriginalUiGarageSceneLighting50KG;
    friend class OriginalUiGarageSceneCollision50KG;
    friend class OriginalUiGaragePageOwner50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    friend class OriginalUiGarageVisibility50KG;
    friend class OriginalUiGarageActors50GN;
    using Grant=game::OriginalUiEntityScope50KG::GarageConstructionGrant;
    using C=game::OriginalWreckComponent50LA;
    using K=game::OriginalWreckComponentKind50LA;
    using Assets=assets::OriginalUiGarageSceneAssets50KG;
    using D=assets::OriginalStandardMaterialDiagnostic50LA;
    OriginalUiGaragePodScene50KG()noexcept;
    bool construct1B2870(std::shared_ptr<Grant>,std::shared_ptr<const Assets>,
        std::shared_ptr<OriginalUiMainAudio50KG>,OriginalUiGarageDroidAudio50GN&,
        std::shared_ptr<OriginalAudioPendingWaves50KG>,std::string&)noexcept;
    // Actual1FB1CC namespace publication and1FB208 slot30->slot78 reset.
    bool beginLoad1B2AB0(std::shared_ptr<OriginalUiMainApplication50KG>,
        std::shared_ptr<const OriginalUiMainProfile50KG>,
        std::shared_ptr<OriginalUiGarageModelResources50KG>,D&)noexcept;
    bool loadSceneConfig1B3880(D&)noexcept;
    bool readSceneInfo270900Prefix(D&)noexcept;
    bool prepareSceneStructure270900(D&)noexcept;
    bool readScenePrefix270900(D&)noexcept;
    bool closeSceneStructurePartial270900(D&)noexcept;
    bool closeSceneDispatchPartial270900(D&)noexcept;
    bool finishSceneRead270900(D&)noexcept;
    bool closeSceneConfig271C10(D&)noexcept;
    bool loadToyConfig1B4330(D&)noexcept;
    bool prepareToyDispatch271CF0Prefix(D&)noexcept;
    bool finishToyDispatch271CF0(D&)noexcept;
    bool closeToyFieldsPartial271CF0(D&)noexcept;
    C* pageEntity3D8()noexcept;
    bool closeToyConfig272800(D&)noexcept;
    bool loadAudioConfig1B4720(D&)noexcept;
    bool readAudioDispatch2728E0(D&)noexcept;
    bool closeAudioConfig272930(D&)noexcept;
    bool closeAudioFieldsPartial271470(D&)noexcept;
    C* pageEntity3A8()noexcept;
    C* pageEntity3CC(std::size_t zone)noexcept;
    C* pageEntity3BC()noexcept;
    C* pageEntity3C0()noexcept;
    const assets::OriginalUiDat50KG* liveSceneConfig34()const noexcept;
    const assets::OriginalUiDat50KG* liveToyConfig34()const noexcept;
    // Explicit partial-construction retirement; no success/admission implied.
    void closeConfig34ForRetirement1573B0()noexcept;
    bool prepareStaticDescriptors2710F0(std::shared_ptr<OriginalUiGarageModelResources50KG>,D&)noexcept;
    bool populateStaticModels274160(D&)noexcept;
    bool prepareVisibility2729E0(std::shared_ptr<OriginalUiGarageVisibility50KG>,D&)noexcept;
    std::unique_ptr<OriginalUiGarageSceneEmitters50KG> sceneEmitters_;
    bool populateEmitters274370(D&)noexcept;
    bool retireSceneEmitters15A720(D&,const OriginalUiGaragePageOwner50KG* attachedPage=nullptr)noexcept;
    bool releaseEmitterArrays27DD60(D&)noexcept;
    bool retireSceneParticleList27DCD0(D&)noexcept;
    bool retirePageParticleList27DCD0(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retirePageMaterialSoundList(const OriginalUiGaragePageOwner50KG&,std::size_t,D&)noexcept;
    bool retireFailedPageMaterialSoundList(const OriginalUiGaragePageOwner50KG&,std::size_t,D&)noexcept;
    bool retireLateBorrowMaterialSoundList(const OriginalUiGaragePageOwner50KG&,std::size_t,D&)noexcept;
    bool resolveEmitterActor27CFF8(const char* mcf,const char* child,
        std::shared_ptr<OriginalUiGarageModelResource50KG>&,
        game::SceneNodeIndex49P&,std::uint32_t& sourceRow,D&)noexcept;
    bool populateLights273AA0(std::shared_ptr<OriginalUiLightManagerOwner50KG>,D&)noexcept;
    bool prepareFog273C30(float farDistance,float nearDistance,
        std::shared_ptr<OriginalUiGarageFogColorLease50KG>,D&)noexcept;
    //Final load-return implementation is delivered by the concrete Scene tail.
    bool borrowPageChildren1B48A0(const OriginalUiGaragePageOwner50KG&,std::array<C*,10>&,D&)noexcept;
    bool borrowLatePageChildren1B4CA0(const OriginalUiGaragePageOwner50KG&,std::array<C*,3>&,D&)noexcept;
    bool prepareParticleManager1B4DC0(D&)noexcept;
    bool updatePageParticleManager161180(const OriginalUiGaragePageOwner50KG&,float,D&)noexcept;
    // Actual list+7C first substring match, then emitter+88/+20/+80. Only
    // the real Page timer cursor can issue this call; no synthetic emitter.
    bool runPageTimerEmitter27E050(OriginalUiGaragePageOwner50KG&,const char*,
        std::uint32_t lookupPC,std::uint32_t resetPC,D&)noexcept;
    bool updatePageEmitters27DFC0(const OriginalUiGaragePageOwner50KG&,float,
        OriginalUiGarageRendererLease50KG&,VehicleWindowDiagnostic50DS&,D&)noexcept;
    bool retireParticleManager160C70(D&)noexcept;
    bool retirePageParticleManager160C70(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool retirePageFog13E700(const OriginalUiGaragePageOwner50KG&,std::size_t,D&)noexcept;
    bool retirePageLightList2781B0(const OriginalUiGaragePageOwner50KG&,D&)noexcept;
    bool populateInstances274080(D&)noexcept;
    bool populateCollisionModels2743B0(D&)noexcept;
    bool closeCollisionPrefix2743B0(D&)noexcept;
    bool finishActorPopulation2742F0(D&)noexcept;
    bool sceneLightingOwnerCurrent273AA0(const OriginalUiGarageSceneLighting50KG&)const noexcept;
    bool retireSceneLight146980(std::size_t,D&)noexcept;
    bool retireSceneFog13E700(std::size_t,D&)noexcept;
    bool finishSceneLightingPayload273AA0(D&)noexcept;
    bool currentPrefix()const noexcept;
    // Native lifetime identity, not source refcount or Page readiness. Only
    // Root issues the weak alias of its genuine shared Page/unique Scene.
    bool coldRoutingIdentityCurrent(const OriginalUiMainAudio50KG*)const noexcept;
    bool configureReverb235520(std::string&)noexcept;
    bool routingEnqueueCurrent235520()const noexcept;
    bool preflightRoutingRetirement235150()const noexcept;
    bool preflightLateBorrowActorAudio(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool preflightLoadedPageActorAudio(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool preflightWattoBindingActorAudio(const OriginalUiGaragePageOwner50KG&)const noexcept;
    bool canRetireUnresolvedSoundGroupForEntryFailure(
        const OriginalUiGarageSession50KG&,bool sceneRetiring)const noexcept;
    bool retireUnresolvedSoundGroupForEntryFailure(
        const OriginalUiGarageSession50KG&,std::string&)noexcept;
    bool canRetireUnresolvedSoundGroupForLateBorrowFailure(
        const OriginalUiGarageSession50KG&,bool sceneRetiring)const noexcept;
    bool retireUnresolvedSoundGroupForLateBorrowFailure(
        const OriginalUiGarageSession50KG&,std::string&)noexcept;
    bool canRetireUnresolvedSoundGroupForLoadedPageFailure(
        const OriginalUiGarageSession50KG&,bool sceneRetiring)const noexcept;
    bool retireUnresolvedSoundGroupForLoadedPageFailure(
        const OriginalUiGarageSession50KG&,std::string&)noexcept;
    bool canRetireUnresolvedSoundGroupForWattoBindingFailure(
        const OriginalUiGarageSession50KG&,bool sceneRetiring)const noexcept;
    bool retireUnresolvedSoundGroupForWattoBindingFailure(
        const OriginalUiGarageSession50KG&,std::string&)noexcept;
    enum class UnresolvedEntryFailure { partialAttachment,lateBorrow,loadedPageHide,wattoBinding };
    bool retireUnresolvedSoundGroupForFailure(const OriginalUiGarageSession50KG&,
        UnresolvedEntryFailure,std::string&)noexcept;
    bool nativeUnresolvedEntryRetiring_=false; //Only actual failed-entry Group begin.
    bool appendSoundPrefix278F60(OriginalUiGarageDroidAudio50GN&,std::size_t actor,
        std::size_t row,const assets::AudioClip50HC&,std::string&)noexcept;
    // Pure owner/row half: deliberately does NOT call backend.current(), so
    // transport validation and retained native bindings never recurse.
    bool sourceRowIdentityCurrent(const OriginalUiGarageDroidAudio50GN&,std::size_t,
        bool requireRegistered)const noexcept;
    // One actual source reference per row, never an Entity. Group235270 and
    // actor callback+10C borrow it. Native mixer/LogicalAudioOwner remain the
    // sole audio backend; no guest Sound allocation/layout or RPC is emulated.
    struct Group;
    struct Sound final {
        Group* group34=nullptr;
        OriginalUiMainAudio50KG* manager30=nullptr;
        const assets::AudioClip50HC* authenticatedSource=nullptr;
        LogicalAudioOwner::Sound* nativeSound=nullptr; //Absent until real publication.
        std::uint64_t reference00=1;
        std::size_t ordinal=0;
        std::uint32_t flags10=0;
        // Constructor value only until nativeSound supplies the canonical
        // backend channel/elapsed/volume/pitch fields; no duplicate live voice.
        std::int32_t channel2C=-1;
        // Absence is the native unissued equivalent of source sample28=-1.
        // No fabricated sample/channel integer can authorize publication.
        std::optional<std::uint32_t> resourceA0;
        std::array<char,260> originalNameA4{};
        std::optional<OriginalAudioSampleCache50KG::NativeSampleHandle> nativeSample28;
        OriginalAudioSampleCache50KG::NativeSampleBinding nativeSample;
        float duration20=0.f;
        std::int32_t basePitch50=4096;
        bool sourceRegistered=false;
        bool stream1A8=false,stereo1AC=false,loop1B0=false;
        bool sampleContinuationPending=false;
    };
    std::shared_ptr<Grant> grant_;
    std::shared_ptr<const Assets> assets_; //NOT source config34 ownership.
    enum class LoadStage {empty,reset,sceneOpen,sceneInfo,sceneStructure,statics,
        sceneTail,sceneClosed,toyOpen,toyPrefix,toyDescriptors,toyTail,toyClosed,
        audioOpen,audioDescriptors,audioClosed};
    LoadStage loadStage_=LoadStage::empty;
    // Insert PRIVATE members in the existing concrete PodScene owner.
    //These are ordered source execution phases, not public readiness setters.
    enum class FinalLoadStage {none,dynamicObjects,ambientAudio,soultreeAudio,
        water,waterRenderer,ecosystem,namespaceRestored,complete};
    FinalLoadStage finalLoadStage_=FinalLoadStage::none;
    std::optional<std::string> savedModelDirectory1FB1B4_;
    bool loadDirectorySaveAttempted_=false;
    //Actual270550 constructor/reset stores2705D0/D4/D8. Positive branch owners
    //are unavailable; actual live Environment 'none' paths preserve these nulls.
    C* water400_=nullptr;
    C* waterRenderer404_=nullptr;
    C* ecosystem408_=nullptr;
    bool captureModelDirectory1FB178(D&)noexcept;
    bool sceneLoadTailPrerequisitesCurrent()const noexcept;
    bool populateWater274570(D&)noexcept;
    bool populateWaterRenderer2747C0(D&)noexcept;
    bool dispatchEcosystem1FB47C(D&)noexcept;
    bool finishSceneLoad1FB594(std::string& actualMaterialDirectory,D&)noexcept;
    bool sourceLoadReturned1FB59C()const noexcept;
    //The worker's three trigger/audio wrappers and target helpers are declared
    //in the accompanying declarations.inc; apply both inside this same class.
        // Private Scene load slots144/14C/150. These use the actual retained
        // ToyFields/AudioFields owners, not an authenticated-assets empty proxy.
        bool populateDynamicObjects274E90(D&)noexcept;
        bool populateAmbientAudio275140(D&)noexcept;
        bool populateSoultreeAudio275180(D&)noexcept;
        bool populateTriggerList2DCB30(OriginalUiGarageToyFields50KG::TriggerList&,D&)noexcept;
        bool populateAmbientList2794B0(OriginalUiGarageAudioFields50KG::List&,D&)noexcept;
        bool populateSoultreeList279A20(OriginalUiGarageAudioFields50KG::List&,D&)noexcept;
    enum class PopulationStage {empty,visibility,lights,fog,particleManager,instances,statics,collisions,actors,emitters};
    PopulationStage populationStage_=PopulationStage::empty;
    //Actual base constructor/reset fields, not race caller defaults. Garage
    //does not call slots12C/13C/140, the writers of terrain/cubes/racer count.
    std::unique_ptr<C*[]> cubeModels3F8_;
    C* terrainModel3FC_=nullptr;
    std::array<C*,12> racerHelpers40C_{},racers43C_{},racerHelpers46C_{};
    std::int32_t racerCount4AC_=0;
    bool attachmentReset270550Known_=false,attachmentArrays26FD68Known_=false;
    bool buoyNull1B2904Observed_=false; //TOY owns53C after its actual reader.

    std::shared_ptr<OriginalUiMainApplication50KG> nativeApplication_;
    std::shared_ptr<const OriginalUiMainProfile50KG> nativeProfile_;
    std::shared_ptr<OriginalUiGarageModelResources50KG> configResources_;
    std::unique_ptr<OriginalUiGarageSceneConfig50KG> config34_,config38_;
    std::unique_ptr<OriginalUiGarageSceneSetup50KG> sceneSetup_;
    std::unique_ptr<OriginalUiGarageSceneLighting50KG> sceneLighting_;
    //Native provenance/pin of the exact input that source uses through global
    //426F60. This is NOT an extra guest reference or a second LightManager.
    std::shared_ptr<OriginalUiLightManagerOwner50KG> retainedLightManager426F60_;
    struct FogInterpolation final {std::array<std::uint32_t,5> words;};
    std::array<std::unique_ptr<FogInterpolation>,5> fogInterpolation4DC_;
    bool fogInterpolationReset27061C_=false,fogInterpolationBranch270D24_=false;
    std::unique_ptr<OriginalUiGarageSceneParticles50KG> sceneParticles_;
    std::unique_ptr<OriginalUiGarageSceneCollision50KG> sceneCollision_;
    std::unique_ptr<OriginalUiGarageSceneDispatch50KG> sceneDispatch_;
    std::unique_ptr<OriginalUiGarageToyFields50KG> toyFields_;
    std::unique_ptr<OriginalUiGarageAudioFields50KG> audioFields_;
    std::string path3C_,base140_,sceneName244_;
    std::int32_t numPortals358_=0,numZones34C_=0,currentZone354_=0,useEnvironment35C_=0;
    bool trackEmitsDust51C_=false;
    std::int32_t num321Anims528_=0;
    bool sceneConfigAttempted_=false,toyConfigAttempted_=false;
    bool sceneInfoAttempted_=false,sceneStructureAttempted_=false;
    bool sceneTailAttempted_=false,toyDispatchAttempted_=false;
    bool audioConfigAttempted_=false,audioDispatchAttempted_=false;
    bool retiredAudioConfig38_=false; //Safe native tombstone for source272954 stale38.
    bool configFailure_=false;
    std::shared_ptr<OriginalUiMainAudio50KG> main_; //Aliases full53-slot backing.
    std::weak_ptr<OriginalUiGaragePodScene50KG> nativeRoutingLifetime_;
    bool reverbAttempted_=false; //Source call attempt, never wet/Page readiness.
    std::shared_ptr<OriginalAudioPendingWaves50KG> pendingWaves_; //Same root namespace.
    OriginalUiGarageDroidAudio50GN* backend_=nullptr;
    std::shared_ptr<OriginalUiGarageVisibility50KG> visibility_;
    //Actual source271124 allocation: one zone pointer, not a second list.
    //The backing StaticModels C++ owner survives as a safe retired tombstone.
    std::unique_ptr<C*[]> staticZoneLists3C4_;
    //Actual270594/2705FC null slots. Positive Portal/Physics owners are not
    //implemented; their producer paths reject, rather than invent ownership.
    void* portals3A0_=nullptr;
    C* physicsManager4B8_=nullptr;
    std::shared_ptr<OriginalUiGarageStaticModels50KG> staticModels_;
    OriginalUiGarageActors50GN* actorOwner3C8_=nullptr; //Native lifetime borrow of actual ToyList owner.
    bool staticDescriptorsAttempted_=false;
    C podScene_; //Separate from Garage manager AND Visibility's4BC Entity.
    struct Group final {
        C entity; //Detached, single reference; NOT a4BC or PodScene child.
        OriginalUiMainAudio50KG* manager5C=nullptr;
        std::unique_ptr<Sound*[]> members4C_;
        std::array<std::unique_ptr<Sound>,4> sounds_{};
        std::size_t count48_=0,capacity54_=0,growth50_=0;
        std::int32_t slot40_=2,volume38_=0,pan3C_=0;
        std::uint32_t state44_=0;
        bool enabled_=false;
    } group4D8_;
    std::unique_ptr<C*[]> worldList500_;
    struct CameraInterpolation; //Positive camdroid branch is not yet admitted.
    std::unique_ptr<CameraInterpolation*[]> interpolation510_;
    std::unique_ptr<std::array<float,4>[]> interpolation514_;
    bool actorOverrideScan1B40B0_=false;
    std::uint32_t field518_=0,field534_=0,field540_=0;
    //1B28F4/FC source pointer tables. Positive321 rows remain unsupported;
    //these are real absent constructor slots, not prefilled dummy arrays.
    std::unique_ptr<std::uint8_t*[]> animationRows52C_,animationRows530_;
    std::size_t capacity504_=0,growth508_=0,count50C_=0;
    std::uint32_t mode30_=0,mode524_=0;
    bool attempted_=false,sceneLive_=false,groupLive_=false,constructed_=false;
    bool visibilityAttempted_=false,sourceConstructionFailed_=false;
    std::array<bool,4> soundAttempted_{};
    // Add to the coherent PodScene's PRIVATE declarations, not public APIs.
    // Preserve all first routing successor declarations and Root-issued weak alias.
    // retireConfig34At26FDB8 is implemented by the Scene config-owner successor.
    enum class RetirementStage {none,lastReference,interpolation510Released,
        interpolation514Released,buoysReleased,animationArraysReleased,
        worldListReleased,configClosed,groupReleased,
        startDataReleased,zonesReleased,portalArrayReleased,environmentReleased,
        fogDescriptorReleased,terrainInfoReleased,staticPointerArrayReleased,
        physicsPointerArrayReleased,physicsManagerReleased,zoneListsReleased,instanceMasterReleased,
        primarySearchPointersReleased,secondarySearchPointersReleased,collisionPointersReleased,
        environmentGlobalCleared,skyPointerArrayReleased,searchListPayloadsReleased,
        cameraZonePointersReleased,sunFlarePointersReleased,lightManagerZonesReleased,
        defaultZoneObserverReleased,auralRegistrationsCleared,fogInterpolationSlotsReleased,complete};
    RetirementStage retirementStage_=RetirementStage::none;
    enum class SoundGroupRetirement {live,registryReleased,complete};
    SoundGroupRetirement soundGroupRetirement_=SoundGroupRetirement::live;
    bool retirePodScenePrefix1B2920(D&)noexcept;
    bool scenePayloadRetirementCurrent()const noexcept;
    bool sourceCollisionListRetiredForPayload()const noexcept;
    bool retireStartData26FDEC(D&)noexcept;
    bool retireZones26FE08(D&)noexcept;
    bool retirePortalArray26FE14(D&)noexcept;
    bool retireEnvironment276B30(D&)noexcept;
    bool retireFogDescriptor26FE58(D&)noexcept;
    bool retireTerrainInfo26FE70(D&)noexcept;
    bool retireStaticPointerArray26FE88(D&)noexcept;
    bool retirePhysicsPointerArray26FEA0(D&)noexcept;
    bool retirePhysicsManager26FEC8(D&)noexcept;
    bool retireZoneLists27C030(D&)noexcept;
    bool retireCurrentInstanceMaster26FEF8(D&)noexcept;
    bool retirePrimarySearchPointers26FF14(D&)noexcept;
    bool retireSecondarySearchPointers26FF2C(D&)noexcept;
    bool retireCollisionPointers26FF44(D&)noexcept;
    bool clearEnvironmentGlobal26FF54(D&)noexcept;
    bool retireSkyPointerArray26FF64(D&)noexcept;
    bool searchListPayloadRetirementCurrent26FF6C()const noexcept;
    bool retireSearchListPayloads26FF6C(D&)noexcept;
    bool retireCameraZonePointers2700F8(D&)noexcept;
    bool retireSunFlarePointers270110(D&)noexcept;
    bool retireLightManagerZones270120(D&)noexcept;
    bool retireDefaultZoneObserver270138(D&)noexcept;
    bool retireAuralRegistrations27015C(D&)noexcept;
    bool retireFogInterpolationSlots270164(D&)noexcept;
    bool finishSceneRetirement2701CC(D&)noexcept;

    bool retireConfig34At26FDB8(D&)noexcept;
    bool retireSoundGroup26FDD4(std::string&)noexcept;
    bool soundGroupRetirementEntered235150()const noexcept;
    bool soundGroupSourceRetired235150()const noexcept;
    bool retiredSoundGroupRoutingIdentity(const OriginalUiMainAudio50KG*)const noexcept;
    bool beginRetirementAt1FA1B4(const OriginalUiGaragePageOwner50KG&,std::string&)noexcept;
    bool beginFailedPartialRetirement1FA1B4(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageSession50KG&,std::string&)noexcept;
    bool beginFailedLateBorrowRetirement1FA1B4(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageSession50KG&,std::string&)noexcept;
    const OriginalUiGaragePageOwner50KG* failedLateBorrowRetirementOwner_=nullptr;
    bool beginFailedWattoBindingRetirement1FA1B4(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageSession50KG&,std::string&)noexcept;
    const OriginalUiGaragePageOwner50KG* failedWattoBindingRetirementOwner_=nullptr;
    bool beginFailedLoadedHideRetirement1FA1B4(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageSession50KG&,std::string&)noexcept;
    const OriginalUiGaragePageOwner50KG* failedLoadedHideRetirementOwner_=nullptr;
    enum class PageRetirementOrigin:std::uint8_t {normal,partialScene,lateBorrow,loadedHide,wattoBinding};
    bool beginRetirementBody1FA1B4(const OriginalUiGaragePageOwner50KG&,
        const OriginalUiGarageSession50KG*,PageRetirementOrigin,std::string&)noexcept;
    const OriginalUiGaragePageOwner50KG* failedPageRetirementOwner_=nullptr;
public:
    ~OriginalUiGaragePodScene50KG()noexcept;
    OriginalUiGaragePodScene50KG(const OriginalUiGaragePodScene50KG&)=delete;
    OriginalUiGaragePodScene50KG& operator=(const OriginalUiGaragePodScene50KG&)=delete;
};
}
