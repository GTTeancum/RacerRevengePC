#pragma once
#include "ra_source_camera_ownership50la.h"
#include "original_world_physics_index50la.h"
#include "../../src/game/original_native_list_method_custody50la.h"
#include "original_world_collision_objects50la.h"
#include "original_world_sky_palette50la.h"
#include "original_world_sky_commands50la.h"
#include "original_world_sky_acb_reader50la.h"
#include "original_native_world_water50la.h"
#include "ra_collision_resources50la.h"
#include "../../src/game/original_track_particle_cold50la.h"
#include "../../src/game/original_visibility_quadtree50la.h"
#include "../../src/game/source_acos49t.h"
#include <array>
#include <limits>
#include <memory>
#include <optional>
#include "../../src/game/original_pod_cable_allocation50la.h"
#include "../../src/game/original_light_flare_numeric50la.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"

namespace rr::platform {class OriginalUiLightManagerOwner50KG;class OriginalUiSearchListCold50KG;
struct OriginalUiMainApplication50KG;class OriginalUiGarageInstanceMaster50KG;}
namespace rr::game {class OriginalRaceConstructor50LA;}
namespace rr::game { class OriginalRacePauseWorldAuthority50LA; }
namespace ra_cold_aggregate50LA {class Owner;}
namespace ra_static_model_cold50LA {class Owner;}
namespace sky50gw_fixture {class Owner;}
namespace ra_pod_scene50LA {
namespace g=rr::game;
// The scene owns the EXISTING canonical visibility, collision and particle
// owners. The derived cold aggregate supplies their source-bound construction
// steps; these are not copies, replacement registries or alternate query worlds.
class Owner {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class g::original_gc_gameplay_lighting_candidate::Owner;
    friend class rr::game::OriginalRaceConstructor50LA;
    friend class rr::game::OriginalRacePauseWorldAuthority50LA;
protected:
    using Entity=g::OriginalWreckComponent50LA;
    std::shared_ptr<g::original_gc_gameplay_lighting_candidate::Owner> lighting_;
    std::shared_ptr<ra_source_camera50LA::OwnershipBridge> runtimeQueries_;
    std::shared_ptr<g::OriginalTrackParticleCold50LA> trackParticles_;
private:
    // +110: missing counted resource custody around the SAME existing camera
    // fog-value owner. These constructor records never replay its numeric data.
    struct NativeFogEntity {
        Entity entity;bool counted=false,published=false;
        std::uint32_t constructorPC=0;
    };
    struct NativeFogBuffer133F80 {
        // Source0..1C not written by133F80; no fabricated zero initialization.
        std::array<std::optional<std::uint32_t>,8> unknown00To1C;
        void* field20=nullptr;void* field24=nullptr;void* field28=nullptr;
        std::int32_t field2C=-1;
    };
    struct NativeFog13E6B0 : NativeFogEntity {
        const g::OriginalRaceFogFields* actualFields=nullptr; //Unretained actual CameraBinding borrow.
        std::uint32_t field44=0,field48=1,field4C=0;
        static constexpr std::uint32_t constructorDirty60=1; //Proven ctor write, NOT a second live dirty value.
        std::unique_ptr<NativeFogBuffer133F80> buffer5C;
        bool bufferAttempted=false;
    };
    struct NativeFogOff13F850 : NativeFogEntity {bool initialized=false;};
    struct NativeFogOn13F980 : NativeFogEntity {
        // Source constructor does NOT establish30; known only after13F9C0.
        std::optional<NativeFog13E6B0*> borrowedFog30;
    };
    std::unique_ptr<NativeFog13E6B0> nativeFog3EC_;
    std::unique_ptr<NativeFogOff13F850> nativeFogOff3F4_;
    std::unique_ptr<NativeFogOn13F980> nativeFogOn3F0_;
    bool nativeFogAttempted_=false,nativeFogFailed_=false,nativeFogRetired_=false;
    std::uint32_t nativeFogFailurePC_=0;
    bool constructNativeFog273CAC(const g::OriginalRaceFogFields&,std::uint32_t&)noexcept;
    bool allocateNativeFogBuffer13E780(std::uint32_t&)noexcept;
    bool initializeNativeFogOff13F890(std::uint32_t&)noexcept;
    bool bindNativeFogOn13F9C0(std::uint32_t&)noexcept;
    bool nativeFogCustodyCurrent(const g::OriginalRaceFogFields*)const noexcept;
    bool canRetireNativeFogCustody()const noexcept;
    bool retireNativeFogCustody(std::uint32_t&)noexcept;
    // Actual World4A0 role aliases the already counted/pinned native owner.
    g::OriginalTrackParticleCold50LA* nativeParticleManager4A0_=nullptr;
    const std::string* nativeParticleTexture41C_=nullptr; //Actual EnvironmentInfo immutable value.
    bool nativeParticleManagerAttempted_=false,nativeParticleManagerComplete_=false;
    bool completeNativeParticleManager1B4DC0(const ra_cold_sources50LA::Owner&,
        const std::vector<std::uint8_t>&,std::uint32_t&)noexcept;
    // Only the existing derived aggregate's sound-list constructor/retirement
    // cuts use these private base-owned authority leaves. No authority escapes.
    bool reserveNativeSoundListCustody()noexcept;
    bool canEnrollNativeSoundListCustody(const Entity&)const noexcept;
    void constructNativeSoundListCustody(Entity&)noexcept;
    bool canRetireNativeSoundListCustody(const Entity&)const noexcept;
    void retireNativeSoundListCustody(Entity&)noexcept;
    bool nativeEmitterManager274370Current(g::OriginalGcParticleManager50KG*&,Entity*&,std::uint32_t&)const noexcept;
    bool constructNativeParticleList27DC9C(Entity&,std::uint32_t&)noexcept;
    bool enrollNativeParticleEntity274370(Entity&,std::uint32_t&)noexcept;
    bool attachNativeParticleEntity27CE6C(Entity&,Entity&,std::uint32_t&)noexcept;
    bool canRetireNativeParticleEntity274370(const Entity&,bool)const noexcept;
    bool releaseNativeParticleEntity142590(Entity&,bool,Entity*,Entity*)noexcept;
    // Native late adoption of missing270FD0 resource custody at actual+118.
    // Uses the SAME Application state/global and existing InstanceMaster class.
    struct NativeEmptyInstances3DC {
        std::shared_ptr<rr::platform::OriginalUiMainApplication50KG> application;
        std::shared_ptr<rr::platform::OriginalUiGarageInstanceMaster50KG> master;
        const void* descriptors3DC=nullptr; //Fresh native no-allocation role, not270550 receipt.
        std::int32_t count3E0=0;
        bool published=false,complete=false;
    };
    std::optional<NativeEmptyInstances3DC> nativeEmptyInstances3DC_;
    struct NativeFinishLine540 {
        std::unique_ptr<g::OriginalPodCableStripCold50LA> strip;
        std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table;
        std::optional<std::int32_t> material544; //298C80 INDEX, no separate retain.
        //Explicit fresh native immutable constructor-value domain. Startup
        //3C9CCC produced4342D0=(0,0,0,1); guest later-currentness unclaimed.
        const std::array<float,4> nativeStartupSentinel4342D0{{0.f,0.f,0.f,1.f}};
        std::array<std::array<float,3>,2> anchorXYZ{}; //Owned getter outputs, not model borrows.
        bool counted=false,setupStarted=false,setupComplete=false,anchorsWritten=false,complete=false,terminal=false;
        std::uint32_t failurePC=0;
    };
    std::unique_ptr<NativeFinishLine540> nativeFinishLine540_;
    bool setupNativeFinishLine540(std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>,
        const std::string&,std::uint32_t&)noexcept;
    bool commitNativeFinishLineGeometry1B3DDC(std::uint32_t&)noexcept;
    bool canRetireNativeFinishLine540()const noexcept;
    bool retireNativeFinishLine540()noexcept;
    bool completeNativeEmptyInstances274080(const ra_cold_sources50LA::Owner&,
        const std::shared_ptr<rr::platform::OriginalUiMainApplication50KG>&,std::uint32_t&)noexcept;
    bool canRetireNativeInstanceMaster()const noexcept;
    bool retireNativeInstanceMaster()noexcept;
    // Constructor-only prefix on this SAME concrete scene. No second world.
    std::unique_ptr<g::OriginalWorldPhysicsIndex50LA> worldPhysics3D4_;
    bool prepareWorldPhysicsIndex270CDC(std::shared_ptr<const ra_cold_sources50LA::Owner>,std::uint32_t&)noexcept;
    std::unique_ptr<g::OriginalWorldCollisionObjects50LA> worldCollisionObjects3CC_;
    bool prepareWorldCollisionObjects271230(std::shared_ptr<const ra_cold_sources50LA::Owner>,std::uint32_t&)noexcept;
    bool canReleaseWorldCollisionObjects27BB80(std::uint32_t&)const noexcept;
    bool releaseWorldCollisionObjects27BB80(std::uint32_t&)noexcept;
    std::unique_ptr<ra_collision_resources50LA::Owner> worldCollisionResources_;
    //Reached native+128 custody; same model forest and query backing only.
    ra_static_model_cold50LA::Owner* nativeCollisionModelBorrow_=nullptr;
    bool nativeCollisionPrefixEntered_=false;
    std::uint32_t nativeCollisionPrefixPC_=0x2743b0;
    std::size_t nativeCollisionNegativeLoads_=0;
    std::size_t nativeCollisionPositiveLoads_=0;
    bool nativeCollisionServiceTerminal_=false,nativeCollisionServiceComplete_=false;
    bool completeNativeCollisionPlacement27B174(std::size_t,std::size_t,std::uint32_t&)noexcept;
    bool nativeCollisionPrimaryCurrent27B174(const g::OriginalWorldCollisionObjects50LA::NativeQuery&)const noexcept;
    bool canRetireNativeCollisionObserver27B174(const g::OriginalWorldCollisionObjects50LA::NativeQuery&)const noexcept;
    bool retireNativeCollisionObserver27B174(g::OriginalWorldCollisionObjects50LA::NativeQuery&)noexcept;

    bool beginNativeCollisionPrefix2743B0(ra_static_model_cold50LA::Owner&,
        ra_static_candidates50LA::Owner&,std::uint32_t&)noexcept;
    bool canRetireNativeCollisionQueries27B040()const noexcept;
    bool detachNativeCollisionQueries27B170(std::uint32_t&)noexcept;
    bool retireNativeCollisionQueries11B4A0(std::uint32_t&)noexcept;

    bool prepareWorldCollisionResources(std::shared_ptr<ra_static_candidates50LA::Owner>,
        std::shared_ptr<rr::assets::OriginalStandardTextureResolver50LA>,rr::assets::OriginalStandardTextureDiagnostic50LA&)noexcept;
    // Actual transient World3C8 ToyList on the authenticated RA no-Animations
    // branch. No toy records, pointer array, manager or surrogate count owner.
    struct NativeEmptyToyList3C8 {
        Entity entity;
        std::uint32_t count30=0; //2D9B60; source34 remains null on this branch.
    };
    std::unique_ptr<NativeEmptyToyList3C8> nativeEmptyToyList3C8_;
    bool loadNativeEmptyAnimations2713E0(const rr::assets::ParameterDocument48F&,std::uint32_t&)noexcept;
    struct NativeSkyInner128930 {
        struct Face {
            std::uint32_t sizes50=0;std::uint16_t mask54=0,flags56=0;
            std::array<std::shared_ptr<rr::assets::OriginalStandardTextureResource50LA>,16> textures58;
            std::array<std::uint32_t,16> values98{};
            std::array<std::int16_t,16> indicesD8;
            std::array<std::int32_t,16> valuesF8;
            std::uint32_t value140=0,value144=0,value148=0,value14C=0x3f800000u; //1289B0 vf0 W survives128880.
        };
        std::uint64_t references00=1;std::uint32_t vtable08=0x3cce40u;
        std::array<std::uint32_t,4> position10{},look20{},up30{};
        original_world_sky50LA::NativeSkyCUBFile* file40=nullptr;
        bool file40KnownInvalid=true;
        std::uint32_t format48=0;std::uint16_t flags4C=0,value4E=0xffffu;
        std::array<Face,6> faces;
        std::uint32_t tileOffset650=0;
        std::optional<std::uint16_t> headerFlags;
        bool constructed=false,readerComplete=false,texturesEntered=false,texturesComplete=false;
        std::size_t texturesPublished=0,texturesLoaded=0;
        std::shared_ptr<rr::assets::OriginalStandardTextureResource50LA> partialTexture;
    };
    using NativeSkyCommandPacket=original_world_sky50LA::Commands::Packet;
    using NativeSkyPlain133F80=original_world_sky50LA::Commands::Plain133F80;
    struct NativeSkyCommands1298AC {
        std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context;
        std::unique_ptr<NativeSkyCommandPacket> packet7E0,packet7E4,pendingPacket;
        std::array<std::unique_ptr<NativeSkyPlain133F80>,96> objects7E8;
        std::array<std::unique_ptr<NativeSkyCommandPacket>,96> packets968;
        std::size_t objectsPublished=0,packetsPublished=0;
        bool entered=false,terminal=false,complete=false;
        bool geometryEntered=false,geometryTerminal=false,geometryComplete=false;
        std::uint32_t geometryPolicy=0;
        std::size_t geometryCells=0,geometryFaces=0;
    };
    struct NativeSkyCube288A50 {
        Entity entity;bool counted=false,constructorComplete=false,published=false;
        std::uint16_t query30=0;std::uint8_t type32=0xff,flags33=0,knownFlags33=7;
        std::optional<std::uint32_t> sourceVtable08;
        const sky50gw_fixture::Owner* images=nullptr;
        const std::string* environmentName=nullptr;
        std::string filename;
        std::uint32_t count40=0,count44=0,count48=0,count7D8=0;
        NativeSkyInner128930* inner38=nullptr;
        std::unique_ptr<NativeSkyInner128930*[]> array3C;
        std::unique_ptr<NativeSkyInner128930> innerOwner;
        std::nullptr_t array4C=nullptr,resource7D4=nullptr;
        bool resourceEntered=false,resourceTerminal=false,resourceComplete=false;
        std::optional<std::uint32_t> innerOffset,innerSavedCursor;
        bool innerPublished=false;
        std::unique_ptr<NativeSkyCommands1298AC> commands;
        bool tailEntered=false,tailTerminal=false,tailComplete=false;
        std::optional<original_world_sky50LA::NativeAbsentSkyACB157320> acbReader;
        std::optional<float> offsetAE8; // Written only AFTER successful1295B0.
        original_world_sky50LA::NativeSkyCUBFile file;
        std::unique_ptr<original_world_sky50LA::NativeSkyPalette2E38C0> palette;
        bool palettePublished7D0=false,prefixComplete=false;
        std::optional<std::uint32_t> paletteCount,paletteOffset,savedCursor;
    };
    struct NativeSkyArray3F8 {
        std::unique_ptr<NativeSkyCube288A50*[]> pointers;
        std::unique_ptr<NativeSkyCube288A50> owner;
        bool entered=false,terminal=false;
        std::uint8_t profileBits=0;std::uint32_t policy=0;
    };
    std::unique_ptr<NativeSkyArray3F8> nativeSky3F8_;
    bool beginNativeSkyPalette274AE0(const ra_cold_sources50LA::Owner&,
        const sky50gw_fixture::Owner&,const std::string&,std::uint8_t,
        std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA>,std::uint32_t&)noexcept;
    bool continueNativeSkyTextures129750(
        std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA>,std::uint32_t&)noexcept;
    bool canRetireNativeSkyInner128930(const NativeSkyCube288A50&)const noexcept;
    bool retireNativeSkyInner128930(NativeSkyCube288A50&)noexcept;
    bool continueNativeSkyCommands1298AC(
        std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA>,std::uint32_t&)noexcept;
    bool continueNativeSkyGeometry12A8A0(const sky50gw_fixture::Owner&,std::uint32_t&)noexcept;
    bool continueNativeSkyTail129924(
        std::shared_ptr<rr::assets::OriginalStandardArchiveProvider50LA>,std::uint32_t&)noexcept;
    bool canRetireNativeSkyCommands1298AC(const NativeSkyCube288A50&)const noexcept;
    bool retireNativeSkyCommands1298AC(NativeSkyCube288A50&)noexcept;
    bool canRetireNativeSky3F8()const noexcept;
    bool retireNativeSky3F8()noexcept;
    struct NativeWaterNone274570 {
        const char* environment110=nullptr;
        std::uint8_t profile1125C=0,course1125FLow4=0;
        std::uint32_t argumentA2=0,argumentA3=3;
        // Source none branch returns1 WITHOUT any read/write of World400.
        // This receipt deliberately contains no pointer/null/owner400 value.
        std::uint32_t returned2745CC=1;
        std::optional<std::uint8_t> profile134_1125C;
        std::optional<std::uint32_t> argument134A2,returned274800;
    };
    std::optional<NativeWaterNone274570> nativeWaterNone274570_;
    bool acceptNativeWaterNone274570(const rr::assets::World48F&,std::uint8_t,std::uint8_t,std::uint32_t&)noexcept;
    bool acceptNativeWaterNone2747C0(const rr::assets::World48F&,std::uint8_t,std::uint32_t&)noexcept;
    bool nativeEcosystemNone1AA410(const rr::assets::World48F&,std::uint32_t&)const noexcept;
    struct NativeTriggerObserver {
        using Link=g::OriginalGarageObserverShared50KG::Link105720;
        struct Record {
            Link* next00=nullptr;
            const Entity* child04=nullptr;
            const g::OriginalNativeListMethodCustody50LA* method08=nullptr;
            std::uint32_t field0C=0,field10=0,slot14=0;
            std::array<float,4> center20{{0,0,0,1}};
        } temporary,persistent;
        struct Secondary {Link* link=nullptr;std::size_t membershipIndex=0;bool sourceInitialHit=false;};
        std::vector<Secondary> links;
        const void* storageBorrow=nullptr;
        const void* membershipBorrow=nullptr;
        std::uint64_t membershipRevision=0;
        bool attempted=false,placementAdopted=false,temporaryConstructed=false;
        bool persistentConstructed=false,complete=false,retiring=false;
        std::uint32_t failurePC=0;
    };
    struct NativeTriggerList3D0 {
        Entity entity;
        bool counted=false;
        std::uint32_t count30=0;
        std::size_t joined=0;
        std::shared_ptr<ra_static_candidates50LA::Owner> candidates;
        std::shared_ptr<const ra_trigger_authoring50LA::Owner> authoring;
        std::shared_ptr<ra_trigger_actions50LA::Owner> actions;
        std::array<const ra_trigger_authoring50LA::Source*,2> descriptors34{};
        std::array<std::unique_ptr<NativeTriggerObserver>,2> observers;
        std::array<bool,2> parented{},geometryRoleConsumed{};
        std::size_t servicesCompleted=0;
        bool serviceEntered=false,serviceTerminal=false,serviceComplete=false;
        bool complete=false;
    };
    std::unique_ptr<NativeTriggerList3D0> nativeTriggerList3D0_;
    bool createNativeTriggerList2DC320(std::shared_ptr<ra_static_candidates50LA::Owner>,
        std::shared_ptr<const ra_trigger_authoring50LA::Owner>,std::uint32_t&)noexcept;
    bool countNativeTriggerPair2A0290(std::size_t,std::uint32_t&)noexcept;
    bool configureNativeTriggerObserver2A0A80(std::size_t,std::uint32_t&)noexcept;
    bool nativeTriggerObserverCurrent(std::size_t)const noexcept;
    bool canRetireNativeTriggerObservers()const noexcept;
    bool withdrawNativeTriggerObserverStorage()noexcept;
    bool retireNativeTriggerObserver2A0420(std::size_t,std::uint32_t&)noexcept;
    bool completeNativeTriggerServices2DCB30(std::uint32_t&)noexcept;
    bool canRetireNativeTriggerList3D0()const noexcept;
    // Private actual2DCC70 leaf only. No World2702E0/restart1AC8E0 caller
    // or reset completion is inferred; collider membership stays untouched.
    bool resetNativeTriggerList2DCC70(const std::shared_ptr<g::OriginalApplicationEntities50LA>&,
        std::uint64_t generation,std::uint32_t& sourcePC)noexcept;
    bool retireNativeTriggerList3D0()noexcept;
    struct NativeSunFlareInputs {
        //Actual retained reader inputs; not sprite/material/backend resources.
        std::string texture,retrieveMaterial;
        bool modeA1=false,saturation=false;
        float minScale=0,maxScale=0;
        std::uint32_t width=0,height=0;
        std::array<float,4> vertexColor{},direction{};
    };
    struct alignas(64) NativeSunFlarePacket {std::array<std::uint32_t,128> words;};
    static_assert(sizeof(NativeSunFlarePacket)==512,"134000 capacity128 DWORDs");
    struct NativeSunFlare;
    struct NativeSunFlareCallback {std::uint32_t sourceFunction=0x2c5610;NativeSunFlare* self=nullptr;};
    struct alignas(64) NativeSunFlareReadback {std::array<std::uint8_t,1024> bytes;};
    struct NativeSunFlareParameters {
        std::array<std::optional<std::uint32_t>,8> unknown00To1C{};
        std::unique_ptr<NativeSunFlarePacket> packet;
        const std::uint32_t* nativePayload04=nullptr;
        std::uint32_t* buffer20=nullptr;
        enum class PacketKind {none,sprite,retrieve};
        PacketKind packetKind=PacketKind::none;
        bool packetStarted=false,packetComplete=false;
        std::uint32_t count24=0,count28=0;std::int32_t field2C=-1;
    };
    struct NativeSunFlarePrimitive {
        std::int32_t material00=-1;
        std::unique_ptr<NativeSunFlareCallback> optional04;
        std::nullptr_t optional14=nullptr;
        std::uint8_t type0C=0,field0D=0,field0E=255,flags0F=0;
        NativeSunFlareParameters* parameters10=nullptr;
        float scale18=1.f,scale1C=1.f;
        std::uint16_t field20=0,field22=0;
        bool stored=false,retained=false,configured=false;
        std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table;
        std::string requested;
    };
    struct NativeSunFlareMaterial {
        std::unique_ptr<NativeSunFlarePrimitive> primitive04;
        std::unique_ptr<NativeSunFlareParameters> parameters08;
    };
    struct NativeSunFlareSprite {
        NativeSunFlareMaterial material;
        std::optional<std::int32_t> x0C,y10; //Unwritten by2E5E60.
        std::uint32_t width14=0,height18=0;
        float scale1C=1.f,scale20=1.f,u24=0.f,v28=1.f,u2C=0.f,v30=1.f;
        std::int32_t depth34=0;
        std::array<std::uint32_t,4> colors38{{0x80808080,0x80808080,0x80808080,0x80808080}};
        std::uint8_t alpha48=255;
    };
    struct NativeSunFlare {
        Entity entity;
        bool counted=false;
        const std::uint32_t sourceVtable08=0x003d4d90;
        std::uint32_t zones30=1,field3C=0,field60=0,field7C=0;
        std::array<float,4> direction50{};
        std::array<std::int32_t,4> indices80{{-1,-1,-1,-1}};
        //Only low three bits are constructor-known; no fabricated high bits.
        std::uint8_t flags98KnownMask=7,flags98KnownValue=0;
        const std::uint32_t materialVtable68=0x003d4e10;
        //Constructor-null resources; the reached loader alone acquires typed
        //sprite/material ownership; array contents remain unknown until2C5F80.
        std::unique_ptr<std::array<float,4>[]> colors44,colors48;
        std::unique_ptr<NativeSunFlareReadback> buffer78;
        bool colorsInitialized=false;
        std::uint32_t colorRowsInitialized=0; //Only this prefix of44/48 may be read.
        std::unique_ptr<NativeSunFlareSprite> sprite64;
        NativeSunFlareMaterial material68;
        NativeSunFlare* callback74=nullptr;
        std::optional<float> minScale34,maxScale38;
        std::optional<std::int32_t> depth40; //Unknown until real2C5CF0, not constructor-zero.
        enum class Load {unreached,fields,spritePublished,spriteLoaded,spriteConfigured,spritePacket,retrieveLoaded,retrievePacket,callbackBound,colors44Allocated,colors48Allocated,readbackAllocated,depthCommitted,saturationComplete};
        Load nextLoad=Load::unreached;
        std::shared_ptr<const rr::assets::OriginalStandardMaterialContext50LA> context;
        std::optional<std::string> directory36B970;
        NativeSunFlareInputs inputs;
    };
    struct NativeSunFlareDomain {
        //A NEW native exclusive domain, NOT a receipt that2705F0 ran.
        //No prior native flare service exists, and this cannot reset another.
        std::uint64_t generation=0;
        std::optional<std::uint32_t> count4A8;
        std::unique_ptr<NativeSunFlare*[]> pointers4A4;
        std::unique_ptr<NativeSunFlare> owner;
    };
    std::optional<NativeSunFlareDomain> nativeSunFlares_;
    bool beginNativeSunFlareDomain271E5C(std::uint32_t&)noexcept;
    bool constructNativeSunFlare2C5730(NativeSunFlareInputs,std::uint32_t&)noexcept;
    static bool canRetireNativeSunFlareMaterial(const NativeSunFlareMaterial&)noexcept;
    static void retireNativeSunFlareMaterial(NativeSunFlareMaterial&)noexcept;
    bool canRetireNativeSunFlare()const noexcept;
    bool retireNativeSunFlare()noexcept;
    // Fresh native optional-water ownership domain, created ONLY at the real
    // guarded countedWorld constructor. Historical guest400/3FC remain unknown.
    std::shared_ptr<native_world_water50LA::Domain> nativeWaterDomain_;
    const native_world_water50LA::Domain* readNativeAbsentWater400(std::uint32_t&)const noexcept;
    Entity worldEntity_;
    std::shared_ptr<g::OriginalApplicationEntities50LA> worldApplication_;
    // Native collision storage can precede the counted world prefix. Retain
    // its actual process identity so either acquisition order must agree.
    std::shared_ptr<g::OriginalApplicationEntities50LA> collisionApplication_;
    rr::platform::RaceAudioGroups50IC* worldAudio_=nullptr; //Caller lifetime through rollback/retirement.
    std::unique_ptr<Entity*[]> worldList500_;
    std::uint32_t worldListCount50C_=0,worldListCapacity504_=0,worldListGrowth508_=0;
    std::uint32_t worldMode30_=0,worldMode524_=0;
    //Unknown before the reached constructor1B28DC. Actual1B44B0 later writes
    //the same World51C from TOB authoring; no native dust-event authority.
    std::optional<std::uint32_t> worldTrackEmitsDust51C_;
    bool worldColdConstructed_=false,worldColdAttempted_=false;
    bool constructWorldColdPrefix(std::shared_ptr<g::OriginalApplicationEntities50LA>,
        rr::platform::RaceAudioGroups50IC&,rr::platform::AudioMixer50HD&,std::uint64_t,
        const track_height_cold50JB::Owner&,std::uint32_t&)noexcept;
    bool rollbackNativeWorldConstructionFailure(std::uint32_t&)noexcept;
    bool canRetireNativeWorldPrefix(const char** gate=nullptr)const noexcept;
    bool retireNativeWorldPrefixAfterOwnedCleanup(std::uint32_t&)noexcept;
    //New source Entity custody on the existing canonical portal geometry;
    //not a second portal geometry/visibility collection. Exact source-order
    //ten-Portal prefix, each actual owner retained before its fallible loader.
    struct NativePortal230370 {
        Entity entity;
        const std::uint32_t vtable08=0x003d18f0;
        const g::PortalGeometry50EO* geometry=nullptr;
        const rr::assets::WorldPortal48F* authored=nullptr;
        const g::PortalProjectionQuad50EO* viewport434940=nullptr;
        //276590 copies global442F80;158C00 writes XYZ only. Native Vector3
        //backing does not establish the copied source W at3C/4C/5C/6C.
        std::array<std::optional<std::uint32_t>,4> vertexW{};
        std::optional<float> maximumDiagonalC0;
        bool counted=false,loaded=false,attached=false,lightLinksComplete=false;
        std::size_t worldLinksPublished=0;
    };
    std::array<std::unique_ptr<NativePortal230370>,10> nativePortals_;
    std::size_t nativePortalNext_=0;
    struct NativePortalParameterRole3A0 {
        //Explicit native immutable-authoring borrower role, NOT retail
        //270C64 array allocation or destructive270550 reset execution.
        const rr::assets::WorldPortal48F* rows=nullptr;
        std::size_t count=0;
        bool retired=false;
    };
    std::optional<NativePortalParameterRole3A0> nativePortalParameters3A0_;
    using NativeListMethodCustody=g::OriginalNativeListMethodCustody50LA;
    std::unique_ptr<NativeListMethodCustody*[]> nativeMethods4C8_,nativeMethods4CC_,nativeMethods4D0_,nativeMethods4D4_;
    std::size_t nativeMethodRowsInitialized_=0,nativeMethodZoneNext_=0;
    bool continueNativeMethodArrays272C78(const ra_cold_sources50LA::Owner&,std::uint32_t&)noexcept;
    bool nativeMethodArraysCurrent()const noexcept;
    bool prepareNativeMethodRow272E80(std::size_t,std::uint32_t&)noexcept;
    bool continueNativeVisibilityList(const ra_cold_sources50LA::Owner&,std::size_t,std::uint32_t&)noexcept;
    bool continueNativeCollisionList(const ra_cold_sources50LA::Owner&,std::size_t,std::uint32_t&)noexcept;
    bool nativeCollisionListsCurrent()const noexcept;
    bool releaseNativeCollisionListReference(std::size_t,bool nodeAlias)noexcept;
    bool continueNativePortal230450(const ra_cold_sources50LA::Owner&,std::size_t,std::uint32_t&)noexcept;
    bool continueNativePortalWorldLinks272B80(std::size_t,std::uint32_t&)noexcept;
    bool canRetireNativePortals()const noexcept;
    bool canRetireNativePortalWorldLinks()const noexcept;
    void retireNativePortalWorldLinks()noexcept;
    void retireNativePortal(std::size_t)noexcept;
    struct NativePortalLink27F3B0 {
        std::unique_ptr<NativePortalLink27F3B0> next00;
        Entity* neighbor04=nullptr;
        Entity* portal08=nullptr; //Raw borrow; NO1048F0 retain in27F3B0.
        std::uint32_t field0C=0,field10=0x7f7fffff;
    };
    struct SearchNode {
        Entity entity;
        std::unique_ptr<NativePortalLink27F3B0> portalLinks30;
        NativeListMethodCustody* nativeList34=nullptr;
        NativeListMethodCustody* nativeList38=nullptr; //Ordinary node List alias only; fallback method unchanged.
        // Exact stable canonical row. This borrow does NOT claim the source
        //27F420/27F490 reference acquisitions have run for its methods.
        const g::CollectionZone50BX* collectionBacking=nullptr;
        std::uint32_t ordinal48=0;
        //Native first-entry source3C/40 metadata domain on this SAME existing
        //counted node. No reinitialization of its live collection/membership.
        std::unique_ptr<std::uint32_t[]> exclusions3C;
        std::optional<std::uint32_t> exclusionCount40;
        const g::original_gc_gameplay_lighting_candidate::AuthoredRenderZone* exclusionSource=nullptr;
        bool exclusionsComplete=false;
        bool constructed=false,attached=false;
    };
    // RA's authenticated ten zones; capacity is not a caller-provided count.
    std::array<SearchNode,10> visibility4C0_,collision4C4_;
    std::size_t nativeZoneExclusionsNext_=0;
    std::optional<std::uint32_t> nativeModelZone354_;
    bool adoptNativeStaticZone2741C0(const ra_cold_sources50LA::Owner&,
        const ra_static_model_cold50LA::Owner*,std::size_t,std::uint32_t&)noexcept;
    bool continueNativeZoneExclusions272A58(const ra_cold_sources50LA::Owner&,std::uint32_t&)noexcept;
    Entity container4BC_;
    SearchNode environment443108_;
    std::unique_ptr<g::OriginalVisibilityQuadTree50LA> environmentMethod_;
    bool method34Retained_=false,method38Retained_=false,constructorReferenceReleased_=false;
    bool nativeEnvironmentAliases273174_=false; //Reached second272FC0; same existing tree/node.
    bool continueNativeEnvironmentAliases272FC0(const ra_cold_sources50LA::Owner& source,std::uint32_t& pc)noexcept {
    //272F28 observes the real earlier particlePredecessor1B4DC0/1B4E20
    //write, not SCB's absent UseEnvironment default. Actual+100=272FC0.
    pc=0x272f28;
    if(useEnvironment35C_!=1u||!nativeSceneRoles270900Current(source,pc)||
       !nativeMethodArraysCurrent()||!environmentMethod_||!sceneApplication_||
       sceneApplication_->visibility443A98_!=environmentMethod_.get()||
       sceneApplication_->environment443108_!=&environment443108_.entity||
       !environment443108_.constructed||!environment443108_.attached||
       !method34Retained_||!method38Retained_||!constructorReferenceReleased_)return false;
    auto& environment=*environmentMethod_;
    if(environment.application_!=sceneApplication_.get()||!environment.configured_||
       !environment.published_||environment.reference00_!=2u||environment.usage10_!=2u||
       !environment.constructorStateOnly())return false;
    if(!nativeEnvironmentAliases273174_){
        //Both actual globals are nonnull:272FDC->2730C8,2730D0->27311C.
        //No constructor/configuration/attachment replay and s1 stays zero.
        //27F420 DOES release the old34 alias before assigning/retaining
        //the identical method. Its other38 reference keeps it alive.
        pc=0x273128;
        --environment.reference00_;method34Retained_=false; //27F448,2->1.
        environment.usage10_=1; //27F454, before alias publication27F458.
        method34Retained_=true;++environment.reference00_; //27F468,1->2.
        pc=0x27313c;
        --environment.reference00_;method38Retained_=false; //27F4B8,2->1.
        environment.usage10_=2; //27F4C4, before alias publication27F4C8.
        method38Retained_=true;++environment.reference00_; //27F4D8,1->2.
        //273144 skips27315C: there is NO surviving constructor reference
        //to release a second time. Same real owner, no new Entity count.
        nativeEnvironmentAliases273174_=true;
    }
        pc=0x272f48;return true;
    }
    struct NativeDefaultObserver280890 {
        //280890: uncounted observer, NOT Entity141300 or Reference1048F0.
        const void* next00=nullptr;
        const Entity* child04=nullptr;
        const g::OriginalVisibilityQuadTree50LA* method08=nullptr;
        std::uint32_t field0C=0,field10=0,field14=0;
        std::array<std::optional<std::uint32_t>,2> untouched18To1C;
        std::array<std::uint32_t,4> vector20{{0,0,0,0x3f800000u}};
        bool sharedConstructed=false,published=false;
    };
    std::unique_ptr<NativeDefaultObserver280890> nativeDefaultObserver443148_;
    struct NativeLightObserver280890 {
        g::OriginalGarageObserverShared50KG::Link105720* next00=nullptr;
        const Entity* child04=nullptr;
        rr::platform::OriginalUiSearchListCold50KG* method08=nullptr;
        std::uint32_t field0C=0,field10=0,slot14=0;
        std::array<std::optional<std::uint32_t>,2> unknown18To1C;
        std::array<std::uint32_t,4> center20{{0,0,0,0x3f800000u}};
        bool sharedConstructed=false,primaryRegistered=false;
        //Native suspended-call custody, not additional guest observer fields.
        const void* portalCursor=nullptr;
        rr::platform::OriginalUiSearchListCold50KG* cursorMethod=nullptr;
        bool portalLoopStarted=false,portalLoopComplete=false;
        struct PendingMembership {
            const Entity* child=nullptr;rr::platform::OriginalUiSearchListCold50KG* method=nullptr;
            const Entity* portal=nullptr;std::uint32_t slot=0,distance=0;
        };
        std::optional<PendingMembership> pending;
    };
    struct NativeRaLight146720 {
        Entity entity;
        std::uint16_t query30=0;std::uint8_t query32=0xff,flags33=0,knownFlags33=7;
        std::uint32_t type34=0,mode38=3;
        std::array<float,4> diffuse3C{{128,128,128,0}},specular4C{{128,128,128,0}},ambient5C{{128,128,128,0}};
        std::array<float,3> position6C{},look78{{1,0,0}},attenuation8C{{1,0,0}};
        std::uint32_t range84=0,field88=0x3f800000u,cone98=0x3f490fdbu,cone9C=0x3fc90fdbu;
        std::array<std::uint32_t,4> positionA0{},lookB0{{0,0,0,0x3f800000u}};
        const void* fieldC0=nullptr;const void* fieldC4=nullptr;
        std::int32_t ordinalC8=-1;std::uint32_t luminanceCC=0xbf800000u;
        const void* observerD0=nullptr;const void* fieldD4=nullptr;const void* fieldD8=nullptr;
        std::unique_ptr<NativeLightObserver280890> observerOwner;
        bool constructed=false,typed=false,colorsWritten=false,revisionWritten=false,configured=false;
        bool registered=false,attached=false,retiring=false;
        bool positionWritten=false,positionRevision=false,lookWritten=false,lookRevision=false,rangeWritten=false,rangeRevision=false;
    };
    std::unique_ptr<NativeRaLight146720> nativeAmbientLight_;
    std::unique_ptr<NativeRaLight146720> nativeDirectionalLight_;
    struct NativeLocalFlareObserver {
        const void* next00=nullptr;const Entity* child04=nullptr;
        NativeListMethodCustody* method08=nullptr;
        std::uint32_t field0C=0,field10=0,slot14=0;
        std::array<std::optional<std::uint32_t>,2> unknown18To1C;
        std::array<std::uint32_t,4> center20{{0,0,0,0x3f800000u}};
        bool sharedConstructed=false,primaryRegistered=false,portalLoopComplete=false;
        const NativePortalLink27F3B0* portalCursor=nullptr;
        bool portalLoopStarted=false;
    };
    struct NativeLocalFlare {
        Entity entity;
        std::uint16_t query30=0;std::uint8_t query32=0xffu,flags33=0u,knownFlags33=7u;
        std::array<std::array<std::uint32_t,4>,5> vectors40To80{{
            {{0,0,0,0x3f800000u}},{{0,0,0,0x3f800000u}},{{0,0,0,0x3f800000u}},
            {{0,0,0,0x3f800000u}},{{0,0,0,0x3f800000u}}}};
        std::unique_ptr<g::OriginalPodCableStripCold50LA> strip90;
        const void* observer94=nullptr;
        std::unique_ptr<NativeLocalFlareObserver> observerOwner;
        std::uint32_t visibilityIdentity=0;
        bool lightPublished=false,lightAttached=false,cornersWritten=false;
        bool consumerTerminal=false; //Partial native update/material callback failure; no replay.
        std::optional<float> size98;std::uint32_t turning9C=1,packedColor=0xffffffffu;
        std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> table;
        std::string directory;
        bool entityConstructed=false,constructed=false,typed=false,inputsWritten=false;
        bool stripConstructed=false,stripPublished=false,setupStarted=false,setupComplete=false,stripAttached=false;
        std::uint32_t failurePC=0;
    };
    struct NativePointOwned {
        std::unique_ptr<NativeRaLight146720> light;
        std::unique_ptr<NativeLocalFlare> flare;
        std::optional<g::original_gc_gameplay_lighting_candidate::RaDirectionalLightInputs> inputs;
        const Entity* retiredFlareIdentity=nullptr; //Dead sourceC4 identity; never dereferenced.
    };
    std::array<NativePointOwned,15> nativePoints_;
    std::size_t nativePointNext_=0; //Completed actual source rows3..17 only.
    struct NativeRemainingOwned {
        std::unique_ptr<NativeRaLight146720> light;
        std::optional<g::original_gc_gameplay_lighting_candidate::RaRemainingLightInputs> inputs;
        bool luminanceWritten=false,conesWritten=false;
    };
    std::array<NativeRemainingOwned,168> nativeRemainder_;
    std::size_t nativeRemainderNext_=0,nativeDirectionalColorNext_=0;
    bool nativeRemainingLightCurrent(const Entity*)const noexcept;
    bool continueNativeRemainingLight(std::size_t,const ::ra_cold_aggregate50LA::Owner&,
        const ra_cold_sources50LA::Owner&,rr::platform::OriginalUiLightManagerOwner50KG&,std::uint32_t&)noexcept;
    bool retireNativeRemainingLight(std::size_t,const ::ra_cold_aggregate50LA::Owner&,
        rr::platform::OriginalUiLightManagerOwner50KG&)noexcept;
    NativeRaLight146720* nativeLocalLightFor(const Entity* e)const noexcept {
        for(const auto& p:nativePoints_)if(p.light && &p.light->entity==e)return p.light.get();
        for(const auto& p:nativeRemainder_)if(p.light && &p.light->entity==e)return p.light.get();return nullptr;
    }
    std::optional<std::size_t> nativeLocalLightAuthoredIndex(const Entity*)const noexcept;
    std::optional<std::size_t> nativeLocalZoneIndex(const Entity*)const noexcept;
    std::optional<std::size_t> nativeLocalFlareZoneIndex(const Entity*)const noexcept;
    std::optional<std::size_t> nativeLocalFlareAuthoredIndex(const Entity*)const noexcept;
    std::uint32_t nativePointDescendants()const noexcept {
        std::uint32_t mask=0;for(const auto& p:nativePoints_)if(p.light&&p.light->attached)mask|=p.light->entity.descendants24;return mask;
    }
    bool nativePointOwnersEmpty()const noexcept {
        for(const auto& p:nativePoints_)if(p.light||p.flare)return false;
        for(const auto& p:nativeRemainder_)if(p.light)return false;return true;
    }
    bool nativeLocalFlareIdentityCurrent(const Entity*)const noexcept;
    bool continueNativeLocalFlareObserver(std::size_t,std::uint32_t&)noexcept;
    bool completeNativeLocalPoint(std::size_t,std::uint32_t&)noexcept;
    bool nativeLocalFlareCurrentAt(std::size_t)const noexcept;
    bool nativeLocalFlareCurrent()const noexcept {
        for(std::size_t i=0;i<nativePoints_.size();++i)if(!nativeLocalFlareCurrentAt(i))return false;return true;
    }
    bool continueNativeLocalFlare(std::size_t,const std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>&,
        const std::string&,std::uint32_t&)noexcept;
    bool setupNativeLocalFlareStrip(NativeLocalFlare&,std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>,
        const std::string&,std::uint32_t&)noexcept;
    bool retireNativeLocalFlare(std::size_t)noexcept;
    bool prepareNativeLocalFlareStrip(
        const g::original_gc_gameplay_lighting_candidate::NativeRaStaticDraw50LA& record,
        const g::CameraViewOutput49F& camera,g::OriginalPodCableStripCold50LA*& output,
        std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>& table,
        rr::assets::OriginalStandardMaterialDiagnostic50LA& d)noexcept {
        using Status=rr::assets::OriginalStandardMaterialStatus50LA;
        d={};output=nullptr;table.reset();
        const auto authored=nativeLocalFlareAuthoredIndex(record.flareIdentity);
        if(!authored){d.status=Status::input;d.sourcePC=0x14b2c0;return false;}
        auto& owned=nativePoints_[*authored-2u];auto& nativeLocalFlare_=owned.flare;auto& nativeLocalLight_=owned.light;
        const auto zone=nativeLocalFlareZoneIndex(record.flareIdentity);
        const auto fail=[&](Status status,std::uint32_t pc){d.status=status;d.sourcePC=pc;return false;};
        if(!sceneApplication_||sceneApplication_->released_||!nativeLocalFlare_||
           !nativeLocalFlareCurrent()||!nativeLocalLight_||!nativeLocalLight_->attached||
           !nativeLocalFlare_->lightAttached||!nativeLocalFlare_->setupComplete||nativeLocalFlare_->consumerTerminal||
           record.kind!=g::original_gc_gameplay_lighting_candidate::NativeRaStaticDraw50LA::Kind::lightFlare||
           record.flareIdentity!=&nativeLocalFlare_->entity||!zone||record.zone!=*zone)
            return fail(Status::input,0x14b2c0);
        auto& f=*nativeLocalFlare_;auto& strip=*f.strip90;
        if(!strip.primitive30_||!f.size98)return fail(Status::source,0x14b87c);
        std::array<float,4> position;for(std::size_t k=0;k<4;++k)
            std::memcpy(&position[k],&f.vectors40To80[0][k],4);
        g::light_flare_numeric50LA::Inputs input{&camera,&position,*f.size98,f.turning9C,
            sceneApplication_->nativeFlareEnabled382B28_,sceneApplication_->nativeFlareDistance427040_,std::nullopt};
        g::light_flare_numeric50LA::Corners corners;bool updated=false;std::uint32_t pc=0;
        if(!g::light_flare_numeric50LA::update14B2C0(input,corners,updated,pc))return fail(Status::source,pc);
        if(!updated)return true; //Actual disabled control: neither updatechildren nor drawchildren.
        const std::array<std::array<float,4>,4> fields{{corners.field50,corners.field60,corners.field70,corners.field80}};
        for(std::size_t i=0;i<4;++i)for(std::size_t k=0;k<4;++k)
            std::memcpy(&f.vectors40To80[i+1][k],&fields[i][k],4);
        f.cornersWritten=true;d.sourceEffectsCommitted=true;
        //14B888 skips only invalid handle demand; the child continuation still
        //runs. Admission currently owns a retained real flare1 handle.
        if(strip.primitive30_->reference.material00()!=-1){
            rr::assets::OriginalStandardMaterialDiagnostic50LA material;
            if(!strip.primitive30_->reference.selectLocalFlare29BFF0(*f.table,material)){
                d=material;d.sourceEffectsCommitted=true;return false;}
        }
        //14B8A8 ->141C30 sees actual flare aggregate24=8: AND1000 is
        //zero, so141C44 reaches return1 without any child-update dispatch.
        //The one actual draw-mask8 child is the owned strip.14B8E0 updates
        //XYZ only, preserving control lanes. The following direct native
        //23CA50 consumer substitutes for unjoined141D70 renderer virtual20
        //at141DD8; it is NOT a source141D70/full14B8E0 return receipt.
        constexpr std::array<std::size_t,4> order{{1u,0u,3u,2u}}; //60,50,80,70.
        if(!strip.attributes_.vertices_||!strip.attributes_.group_||strip.attributes_.group_->vertexCount!=4u)
            return fail(Status::source,0x14b8e0);
        for(std::size_t i=0;i<4;++i)for(std::size_t k=0;k<3;++k)
            strip.attributes_.vertices_->positions[i].xyz[k]={fields[order[i]][k],0xffu};
        output=&strip;table=f.table;d.sourcePC=0x23ca50;return true;
    }


    bool nativeLocalLightCurrent(const Entity*)const noexcept;
    bool continueNativeLocalPrimary(std::size_t,const ::ra_cold_aggregate50LA::Owner&,const ra_cold_sources50LA::Owner&,
        rr::platform::OriginalUiLightManagerOwner50KG&,std::uint32_t&)noexcept;
    bool retireNativeLocalPrimary(std::size_t,const ::ra_cold_aggregate50LA::Owner&,
        rr::platform::OriginalUiLightManagerOwner50KG&)noexcept;
    std::optional<g::original_gc_gameplay_lighting_candidate::RaDirectionalLightInputs> nativeDirectionalInputs_;
    bool nativeDirectionalLightCurrent(const Entity*)const noexcept;
    bool continueNativeDirectionalLight(const ::ra_cold_aggregate50LA::Owner&,const ra_cold_sources50LA::Owner&,
        rr::platform::OriginalUiLightManagerOwner50KG&,std::uint32_t&)noexcept;
    bool retireNativeDirectionalLight(const ::ra_cold_aggregate50LA::Owner&,
        rr::platform::OriginalUiLightManagerOwner50KG&)noexcept;
    bool nativeAmbientLightCurrent(const Entity*)const noexcept;
    bool continueNativeAmbientLight(const ::ra_cold_aggregate50LA::Owner&,
        rr::platform::OriginalUiLightManagerOwner50KG&,std::uint32_t&)noexcept;
    bool retireNativeAmbientLight(const ::ra_cold_aggregate50LA::Owner&,
        rr::platform::OriginalUiLightManagerOwner50KG&)noexcept;
    struct NativeLightWrapper277C10 {
        Entity* live00=nullptr; //277D80/146720 still unjoined.
        std::optional<g::original_gc_gameplay_lighting_candidate::RaAuthoredLightIdentity> descriptor04;
    };
    struct NativeLightList3A8 {
        Entity entity;bool counted=false;
        const g::original_gc_gameplay_lighting_candidate::Owner* canonical=nullptr;
        std::uint32_t count30=0;
        std::optional<std::uint32_t> zoneCount34;
        std::unique_ptr<NativeLightWrapper277C10[]> wrappers38;
        struct Color {std::array<std::optional<float>,4> lanes;};
        std::unique_ptr<Color[]> colors3C;
        std::size_t loaded=0,colorsLoaded=0;
    };
    std::unique_ptr<NativeLightList3A8> nativeLightList3A8_;
    bool adoptNativeLightWrappers273AA0(const ra_cold_sources50LA::Owner&,std::uint32_t&)noexcept;
    bool canRetireNativeLightWrappers()const noexcept {
        if(!nativeLightList3A8_)return true;
        if(!sceneApplication_||sceneApplication_->attachmentLock423F58()||!lighting_)return false;
        const auto& l=*nativeLightList3A8_;const auto& e=l.entity;
        if(!l.counted||l.canonical!=lighting_.get()||l.loaded>185u||l.colorsLoaded>10u||
           (l.count30!=0&&l.count30!=185u)||(l.zoneCount34&&*l.zoneCount34!=10u)||
           (l.wrappers38&&l.count30!=185u)||(l.loaded&&!l.wrappers38)||
           (l.colors3C&&!l.zoneCount34)||(l.colorsLoaded&&!l.colors3C)||
           e.reference00!=1u||e.kind!=g::OriginalWreckComponentKind50LA::entity141300||
           e.objectKey!=key(&e)||e.sessionGeneration!=sceneGeneration_||e.category28!=0xffu||
           (e.flags29&15u)!=3u||e.nativeFlags29||e.parent1C||e.previous10||e.next14||e.mask20||
           e.firstChild18!=(nativeAmbientLight_&&nativeAmbientLight_->attached?&nativeAmbientLight_->entity:nullptr)||
           e.descendants24!=(nativeAmbientLight_&&nativeAmbientLight_->attached?
               (2u|nativePointDescendants()):0u)||
           (nativeAmbientLight_&&!nativeAmbientLightCurrent(&nativeAmbientLight_->entity))||
           (nativeDirectionalLight_&&!nativeDirectionalLightCurrent(&nativeDirectionalLight_->entity))||
           !nativeLocalFlareCurrent()||!sceneApplication_->objectCount423F60()||
           !sceneApplication_->entityAuthority_.canRetire(e))return false;
        for(const auto& p:nativePoints_)if(p.light&&!nativeLocalLightCurrent(&p.light->entity))return false;
        for(const auto& p:nativeRemainder_)if(p.light&&!nativeRemainingLightCurrent(&p.light->entity))return false;
        if(l.wrappers38)for(std::size_t i=0;i<185u;++i){const auto& row=l.wrappers38[i];
            const auto* expected=i==0u&&nativeAmbientLight_&&nativeAmbientLight_->typed?&nativeAmbientLight_->entity:
                i==1u&&nativeDirectionalLight_&&nativeDirectionalLight_->typed?&nativeDirectionalLight_->entity:
                i>=2u&&i<17u&&nativePoints_[i-2u].light&&nativePoints_[i-2u].light->typed?&nativePoints_[i-2u].light->entity:
                i>=17u&&nativeRemainder_[i-17u].light&&nativeRemainder_[i-17u].light->typed?&nativeRemainder_[i-17u].light->entity:nullptr;
            if(row.live00!=expected||bool(row.descriptor04)!=(i<l.loaded))return false;
            if(row.descriptor04){const auto& d=*row.descriptor04;
                if(!d.retainedSource||!d.canonicalStorage||d.ordinal!=i+1u||d.zone20>=10u||
                   d.effectiveOrdinal!=((i==89u||i==106u)?i:i+1u)||
                   (d.type00!=1u&&d.type00!=2u&&d.type00!=3u&&d.type00!=4u)||
                   bool(d.localPrimitive)!=(d.type00==1u||d.type00==3u))return false;
            }
        }
        return true;
    }
    bool retireNativeLightWrappers()noexcept {
        if(!canRetireNativeLightWrappers()||nativeAmbientLight_||nativeDirectionalLight_||!nativePointOwnersEmpty())return false;
        if(!nativeLightList3A8_)return true;
        auto& l=*nativeLightList3A8_;
        //2781B0 destroys wrapper38 (277C30 only owns temp04, NOT live00),
        //then color3C, then1413E0. No live lights exist in this bounded prefix.
        l.wrappers38.reset();l.colors3C.reset();
        if(!sceneApplication_->entityAuthority_.retire(l.entity))std::terminate();
        --l.entity.reference00;--sceneApplication_->objectCount423F60();
        nativeLightList3A8_.reset();return true;
    }

    bool nativeDefaultObserverCurrent()const noexcept {
        if(!nativeDefaultObserver443148_)return true;
        if(!sceneApplication_)return false;
        const auto& o=*nativeDefaultObserver443148_;
        const auto& shared=sceneApplication_->garageObserverShared_;
        if(!shared.coherent()||o.next00||o.field0C||o.field10||o.field14||
           o.untouched18To1C[0]||o.untouched18To1C[1]||
           o.vector20!=std::array<std::uint32_t,4>{{0,0,0,0x3f800000u}})return false;
        if(!o.sharedConstructed)return !o.published&&!o.child04&&!o.method08&&
            !shared.owns(&o)&&!sceneApplication_->defaultZoneObserver443148_;
        //Compare borrowed identities only.280920 does not traverse04/08;
        //this remains valid after node/tree source payload retirement.
        return o.published&&shared.owns(&o)&&sceneApplication_->defaultZoneObserver443148_==&o&&
            o.child04==&collision4C4_[0].entity&&o.method08==environmentMethod_.get();
    }
    bool continueNativeDefaultObserver272F48(std::uint32_t& pc)noexcept {
        pc=0x272f48;
        if(!nativeEnvironmentAliases273174_||nativeMethodZoneNext_!=10u||
           !sceneApplication_||!environmentMethod_||!canCloseTopology()||
           sceneApplication_->visibility443A98_!=environmentMethod_.get()||
           !collision4C4_[0].constructed||!collision4C4_[0].attached)return false;
        auto& shared=sceneApplication_->garageObserverShared_;
        if(!shared.coherent())return false; //Actual retained ledger, not assumed count0.
        if(nativeDefaultObserver443148_&&nativeDefaultObserver443148_->published){pc=0x272fac;return true;}
        if(sceneApplication_->defaultZoneObserver443148_)return false; //Never overwrite another World.
        if(!nativeDefaultObserver443148_){
            try{nativeDefaultObserver443148_=std::make_unique<NativeDefaultObserver280890>();}
            catch(...){return false;}
        }
        auto& o=*nativeDefaultObserver443148_;
        pc=0x272f58;
        if(!shared.construct280890(&o))return false; //Failed native allocation retained, no replayed count.
        o.sharedConstructed=true;
        //Actual280890 shared service has now supplied its one count. The
        //native cold fields represent source00..14/VF0;18/1C remain unknown.
        pc=0x272f64;sceneApplication_->defaultZoneObserver443148_=&o;o.published=true;
        pc=0x272f80;o.child04=&collision4C4_[0].entity; //Raw existing node, no retain.
        pc=0x272f90;o.method08=sceneApplication_->visibility443A98_; //Actual nonnull tree.
        pc=0x272fac;return nativeDefaultObserverCurrent();
    }
    bool retireNativeDefaultObserver270138()noexcept {
        if(!nativeDefaultObserver443148_)return true;
        if(!nativeDefaultObserverCurrent())return false;
        auto& o=*nativeDefaultObserver443148_;
        const bool published=o.published;
        if(o.sharedConstructed&&!sceneApplication_->garageObserverShared_.release280920(&o))return false;
        //Source280920 decrements443140 and releases/clears443138 only at
        //last owner. It neither follows nor releases borrowed04/08.
        nativeDefaultObserver443148_.reset(); //280920(a1=1) actual allocation release.
        if(published)sceneApplication_->defaultZoneObserver443148_=nullptr; //270144 AFTER release.
        return true;
    }

    std::array<Entity*,22> domain_{};
    std::size_t domainCount_=0;
    //Existing native topology already owns its source count/references. Late
    //authority enrollment adds only missing real lifetime visibility, no count
    //increment, constructor replay, reordered membership or dispatch admission.
    std::size_t nativeTopologyAuthorityEnrolled_=0;
    std::shared_ptr<g::OriginalApplicationEntities50LA> sceneApplication_;
    std::uint64_t sceneGeneration_=0;
    const g::CollectionZone50BX* visibilityRows_=nullptr;
    const g::CollectionZone50BX* collisionRows_=nullptr;
    bool topologyAttempted_=false,topologyConstructed_=false,topologyClosed_=false;
    bool particlePredecessorAttempted_=false;
    std::uint32_t useEnvironment35C_=0; //270578, then authenticated270A1C default0.
    static std::uint64_t key(const void* p)noexcept {
        return static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(p));
    }
    bool fail(std::uint32_t& pc,std::uint32_t source)const noexcept{pc=source;return false;}
    // Read-only validation of already-owned270900 service roles. It does
    // not reset/recreate topology, overwrite35C, or grant a source return.
    bool nativeSceneRoles270900Current(const ra_cold_sources50LA::Owner&,std::uint32_t&)const noexcept;
    bool identitiesCurrent()const noexcept {
        if(!lighting_||!runtimeQueries_||!sceneApplication_||!sceneApplication_->application10()||
           runtimeQueries_->sceneOwner_!=this||!runtimeQueries_->storage_.get())return false;
        const auto& s=*runtimeQueries_->storage_.get();
        return s.zones.data()==collisionRows_&&s.zones.size()==10u&&
            s.lifecycle.storage.views.zones==collisionRows_;
    }
    bool constructTopology(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::uint64_t generation,std::uint32_t& pc)noexcept {
        pc=0;
        if(topologyAttempted_||topologyClosed_)return fail(pc,0x2729e0);
        if(worldColdConstructed_&&(application!=worldApplication_||generation!=worldEntity_.sessionGeneration))
            return fail(pc,0x2729e0);
        if(!application||!generation||!application->application10()||
           application->attachmentLock423F58()||!lighting_||!runtimeQueries_||
           !runtimeQueries_->candidates_||!runtimeQueries_->candidates_->decodedSource()||
           !runtimeQueries_->storageConstructed_||runtimeQueries_->closed_||runtimeQueries_->terminal_||
           runtimeQueries_->sceneOwner_||!runtimeQueries_->storage_.get()||!lighting_->world())
            return fail(pc,0x2729e0);
        const auto& source=runtimeQueries_->candidates_->decodedSource()->world;
        const auto& visible=*lighting_->world();
        const auto& collision=*runtimeQueries_->storage_.get();
        if(source.parameters.original_bytes!=visible.parameters.original_bytes||source.zones.size()!=10u||
           collision.zones.size()!=10u||collision.lifecycle.zoneCount!=10u||
           collision.lifecycle.storage.views.zones!=collision.zones.data())return fail(pc,0x272a48);
        // The genuine RA fresh reader defaults UseEnvironmentNode to zero.
        // Unknown explicit authoring is rejected; it is never forced false.
        for(const auto& b:source.parameters.blocks)if(b.name=="sceneinfo")
            for(const auto& f:b.fields)if(f.name=="useenvironmentnode")return fail(pc,0x270a1c);
        if(application->objectCount423F60()>std::numeric_limits<std::uint32_t>::max()-22u)
            return fail(pc,0x1413b8);
        topologyAttempted_=true;sceneApplication_=std::move(application);sceneGeneration_=generation;
        std::size_t count=0;
        visibilityRows_=lighting_->retainSceneZones272A78(this,count);
        if(!visibilityRows_||count!=10u)return fail(pc,0x272a78);
        collisionRows_=collision.zones.data();runtimeQueries_->sceneOwner_=this;
        auto construct=[&](Entity& entity,g::OriginalWreckComponentKind50LA kind)->bool {
            const auto result=g::initializeOriginalWreckComponent50LA(entity,kind,generation,key(&entity),true,
                sceneApplication_->objectCount423F60());
            if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
            domain_[domainCount_++]=&entity;return true;
        };
        if(!construct(container4BC_,g::OriginalWreckComponentKind50LA::entity141300))return false;
        //272A58..272AE4: visibility constructor, collision constructor, then
        //both ordinal stores, in authored order. No existing row is rewritten.
        for(std::size_t z=0;z<10u;++z){
            auto& v=visibility4C0_[z];auto& c=collision4C4_[z];
            if(!construct(v.entity,g::OriginalWreckComponentKind50LA::searchNode27F120))return false;
            v.constructed=true;v.collectionBacking=&visibilityRows_[z];
            if(!construct(c.entity,g::OriginalWreckComponentKind50LA::searchNode27F120))return false;
            c.constructed=true;c.collectionBacking=&collisionRows_[z];
            v.ordinal48=c.ordinal48=static_cast<std::uint32_t>(z);
        }
        const g::OriginalWreckComponentDomain50LA domain{generation,domain_.data(),domainCount_};
        for(std::size_t z=0;z<10u;++z){
            auto& v=visibility4C0_[z];auto& c=collision4C4_[z];
            auto result=g::appendOriginalWreckComponent50LA(domain,container4BC_,v.entity,0x400u,
                sceneApplication_->attachmentLock423F58());
            if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
            v.attached=true;
            result=g::appendOriginalWreckComponent50LA(domain,container4BC_,c.entity,0u,
                sceneApplication_->attachmentLock423F58());
            if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
            c.attached=true;
        }
        topologyConstructed_=true;return true;
    }
    bool particlePredecessor1B4DC0(std::uint32_t& pc)noexcept {
        pc=0;
        if(particlePredecessorAttempted_||!topologyConstructed_||topologyClosed_||!identitiesCurrent())
            return fail(pc,0x1b4dc0);
        particlePredecessorAttempted_=true;
        //The caller checked the actual same-SCB particle selector. Read the
        //one Application slot, never a local stand-in for a surviving method.
        if(sceneApplication_->visibility443A98_||sceneApplication_->environment443108_)
            return fail(pc,0x1b4e10); //Existing-scene adoption needs its real owner.
        const auto* bounds=lighting_->sceneBounds534(this);
        if(!bounds)return fail(pc,0x273004);
        useEnvironment35C_=1; //1B4E20, before the actual272FC0 constructor.
        try {environmentMethod_.reset(new g::OriginalVisibilityQuadTree50LA);}
        catch(...){return fail(pc,0x272fe4);}
        if(!environmentMethod_->construct(*sceneApplication_,*bounds,pc))return false;
        auto& e=environment443108_;
        auto result=g::initializeOriginalWreckComponent50LA(e.entity,
            g::OriginalWreckComponentKind50LA::searchNode27F120,sceneGeneration_,key(&e.entity),true,
            sceneApplication_->objectCount423F60());
        if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
        e.constructed=true;e.ordinal48=0;domain_[domainCount_++]=&e.entity;
        sceneApplication_->environment443108_=&e.entity;
        const g::OriginalWreckComponentDomain50LA domain{sceneGeneration_,domain_.data(),domainCount_};
        result=g::appendOriginalWreckComponent50LA(domain,container4BC_,e.entity,0,
            sceneApplication_->attachmentLock423F58());
        if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
        e.attached=true;
        //27F420/27F490 both retain the SAME method; usage10's second store wins.
        auto& method=*environmentMethod_;
        method.usage10_=1;++method.reference00_;method34Retained_=true;
        method.usage10_=2;++method.reference00_;method38Retained_=true;
        --method.reference00_;constructorReferenceReleased_=true; //27315C.
        //A genuine newly-constructed tree has no registered objects yet. This
        //is NOT proof that any later camera may return an empty success.
        pc=0;return true;
    }
    bool canCloseTopology()const noexcept {
        if(topologyClosed_||!topologyAttempted_)return true;
        if(!sceneApplication_||sceneApplication_->attachmentLock423F58()||!canRetireNativePortals()||
           !nativeMethodArraysCurrent()||!nativeDefaultObserverCurrent()||!canRetireNativeLightWrappers())return false;
        if(visibilityRows_&&(!lighting_||!lighting_->canReleaseSceneZones272A78(this)))return false;
        if(collisionRows_&&(!runtimeQueries_||runtimeQueries_->sceneOwner_!=this))return false;
        std::size_t counted=0;
        for(std::size_t i=0;i<domainCount_;++i){const auto* e=domain_[i];
            if(!e||e->reference00!=1u||e->sessionGeneration!=sceneGeneration_||e->objectKey!=key(e))return false;
            if(e!=&container4BC_&&e->firstChild18)return false;
            if(i<nativeTopologyAuthorityEnrolled_&&!sceneApplication_->entityAuthority_.canRetire(*e))return false;
            ++counted;
        }
        if(counted>sceneApplication_->objectCount423F60()||container4BC_.parent1C||
           container4BC_.previous10||container4BC_.next14)return false;
        const Entity* previous=nullptr;const Entity* child=container4BC_.firstChild18;
        for(std::size_t z=0;z<10u;++z)for(const auto* node:{&visibility4C0_[z],&collision4C4_[z]}){
            if(node->attached){
                if(child!=&node->entity||child->parent1C!=&container4BC_||child->previous10!=previous||
                   child->mask20!=(node==&visibility4C0_[z]?0x400u:0u))return false;
                previous=child;child=child->next14;
            }else if(node->entity.parent1C||node->entity.previous10||node->entity.next14)return false;
        }
        for(std::size_t z=0;z<10u;++z){
            const auto& v=visibility4C0_[z];const auto& c=collision4C4_[z];
            if(c.exclusions3C||c.exclusionCount40||c.exclusionSource||c.exclusionsComplete)return false;
            if(v.exclusionCount40){
                if((*v.exclusionCount40!=6u&&*v.exclusionCount40!=7u)||!v.exclusionSource)return false;
            }else if(v.exclusions3C||v.exclusionSource||v.exclusionsComplete)return false;
            if(v.exclusionsComplete&&!v.exclusions3C)return false;
            if(z<nativeZoneExclusionsNext_&&!v.exclusionsComplete)return false;
            if(z>nativeZoneExclusionsNext_&&v.exclusionCount40)return false;
        }
        if(nativeTopologyAuthorityEnrolled_>domainCount_||nativeZoneExclusionsNext_>10u||!canRetireNativePortalWorldLinks())return false;
        const auto& environment=environment443108_;
        if(environment.attached){
            if(child!=&environment.entity||child->parent1C!=&container4BC_||
               child->previous10!=previous||child->mask20)return false;
            previous=child;child=child->next14;
        }else if(environment.entity.parent1C||environment.entity.previous10||environment.entity.next14)return false;
        for(const auto& owned:nativePortals_)if(owned&&owned->attached){
            const auto& portal=owned->entity;
            if(child!=&portal||portal.parent1C!=&container4BC_||portal.previous10!=previous||portal.mask20)return false;
            previous=child;child=portal.next14;
        }
        if(environment.constructed&&sceneApplication_->environment443108_!=&environment.entity)return false;
        if(environmentMethod_){
            const auto& method=*environmentMethod_;
            const std::uint64_t expected=(!method.application_||constructorReferenceReleased_?0u:1u)+
                std::uint64_t(method34Retained_)+std::uint64_t(method38Retained_);
            if(method.reference00_!=expected||!method.constructorStateOnly()||
               (method.published_&&sceneApplication_->visibility443A98_!=&method))return false;
        }
        return !child;
    }
    bool closeTopology()noexcept {
        if(topologyClosed_)return true;
        if(!canCloseTopology())return false;
        //Native adopted topology precedes the newly appended portal. Thus its
        //tail-first retirement differs from retail portals-before-nodes order.
        //Revoke every raw portal borrow FIRST, while the actual portal lives;
        //never leave dangling links for a later node destructor to observe.
        //Explicit native partial-load cleanup, not an invented retail3A8
        //World-release position (none proven). Same canonical backing is alive.
        if(!retireNativeLightWrappers())return false;
        retireNativePortalWorldLinks();
        for(std::size_t i=nativePortals_.size();i-- >0;)if(nativePortals_[i]){
            auto& owned=*nativePortals_[i];auto& portal=owned.entity;
            if(owned.attached){
                if(portal.next14)std::terminate();
                if(portal.previous10)portal.previous10->next14=nullptr;
                else container4BC_.firstChild18=nullptr;
                portal.parent1C=nullptr;portal.previous10=nullptr;portal.next14=nullptr;owned.attached=false;
            }
            retireNativePortal(i);
        }
        nativePortalNext_=0;
        nativePortalParameters3A0_.reset();
        //Method arrays survive node alias retirement below. Only known rows
        //can be read; existing canonical Storage retains actual payloads.
        if(visibilityRows_&&!lighting_->releaseSceneZones272A78(this))return false;
        //Canonical zone methods remain with their original owners. The
        //fallback node releases its real34 then38 references (27F1EC/27F208).
        //142590 ->1424E0 visits linked children tail-to-head, then4BC.
        const auto releaseExclusions=[&](Entity* child)noexcept {
            for(auto& node:visibility4C0_)if(child==&node.entity){
                //27F1D8/1E4 frees3C BEFORE34/38 aliases and1413E0.
                node.exclusions3C.reset();node.exclusionSource=nullptr;
                node.exclusionCount40.reset();node.exclusionsComplete=false;
            }
        };
        Entity* tail=container4BC_.firstChild18;
        while(tail&&tail->next14)tail=tail->next14;
        while(tail){auto* child=tail;tail=child->previous10;
            if(tail)tail->next14=nullptr;else container4BC_.firstChild18=nullptr;
            child->parent1C=nullptr;child->previous10=nullptr;child->next14=nullptr;
            for(std::size_t i=0;i<nativeTopologyAuthorityEnrolled_;++i)if(domain_[i]==child)
                if(!sceneApplication_->entityAuthority_.retire(*child))std::terminate();
            --child->reference00; //104970 reaches the concrete destructor at zero.
            releaseExclusions(child);
            for(std::size_t z=0;z<10u;++z){
                auto& v=visibility4C0_[z];auto& c=collision4C4_[z];
                if(child==&v.entity){
                    if(v.nativeList34){if(!lighting_->releaseRaVisibilityListReference(this,v.nativeList34,true))std::terminate();v.nativeList34=nullptr;}
                    if(v.nativeList38){if(!lighting_->releaseRaVisibilityListReference(this,v.nativeList38,true))std::terminate();v.nativeList38=nullptr;}
                }
                if(child==&c.entity){
                    if(c.nativeList34){if(!releaseNativeCollisionListReference(z*2u,true))std::terminate();c.nativeList34=nullptr;}
                    if(c.nativeList38){if(!releaseNativeCollisionListReference(z*2u+1u,true))std::terminate();c.nativeList38=nullptr;}
                }
            }
            if(child==&environment443108_.entity){
                auto& method=*environmentMethod_;
                if(method34Retained_){--method.reference00_;method34Retained_=false;}
                //Last release runs the concrete destructor while its ref is1.
                if(method38Retained_){
                    if(!method.closeConstructor())return false;
                    method38Retained_=false;
                }
                sceneApplication_->environment443108_=nullptr;
            }
            --sceneApplication_->objectCount423F60(); //Concrete destructor ->1413E0.
        }
        for(std::size_t i=1;i<domainCount_;++i)if(domain_[i]->reference00){
            if(i<nativeTopologyAuthorityEnrolled_&&!sceneApplication_->entityAuthority_.retire(*domain_[i]))std::terminate();
            domain_[i]->reference00=0;releaseExclusions(domain_[i]);--sceneApplication_->objectCount423F60();
        }
        //Explicit native partial cleanup: node alias first, array reference
        //second. Retail relative4BC teardown vs26FFA0 timing is not proven.
        //Source array final-release order:4C8,4D0,4CC,4D4, each ascending.
        for(std::size_t z=0;z<nativeMethodRowsInitialized_;++z)if(nativeMethods4C8_[z]){
            if(!lighting_->releaseRaVisibilityListReference(this,nativeMethods4C8_[z],false))std::terminate();nativeMethods4C8_[z]=nullptr;
        }
        nativeMethods4C8_.reset();
        for(std::size_t z=0;z<nativeMethodRowsInitialized_;++z)if(nativeMethods4D0_[z]){
            if(!lighting_->releaseRaVisibilityListReference(this,nativeMethods4D0_[z],false))std::terminate();nativeMethods4D0_[z]=nullptr;
        }
        nativeMethods4D0_.reset();
        for(std::size_t z=0;z<nativeMethodRowsInitialized_;++z){
            if(!releaseNativeCollisionListReference(z*2u,false))std::terminate();nativeMethods4CC_[z]=nullptr;
        }
        nativeMethods4CC_.reset();
        for(std::size_t z=0;z<nativeMethodRowsInitialized_;++z){
            if(!releaseNativeCollisionListReference(z*2u+1u,false))std::terminate();nativeMethods4D4_[z]=nullptr;
        }
        nativeMethods4D4_.reset();nativeMethodRowsInitialized_=nativeMethodZoneNext_=0;
        if(environment443108_.constructed)sceneApplication_->environment443108_=nullptr;
        if(environmentMethod_&&environmentMethod_->reference00_&&!environmentMethod_->closeConstructor())return false;
        //Aggregate already retired LightManager zones148110; all four method
        //arrays above are retired. Node/tree payloads are dead but their native
        //identities still exist. Preserve source270138->270144 before deleting
        //the borrowed tree allocation; never dereference the observer's borrows.
        if(!retireNativeDefaultObserver270138())return false;
        environmentMethod_.reset();nativeEnvironmentAliases273174_=false;
        if(container4BC_.reference00){
            if(nativeTopologyAuthorityEnrolled_&&!sceneApplication_->entityAuthority_.retire(container4BC_))std::terminate();
            container4BC_.reference00=0;--sceneApplication_->objectCount423F60();
        }
        nativeTopologyAuthorityEnrolled_=0;
        container4BC_.descendants24=0;
        if(runtimeQueries_&&runtimeQueries_->sceneOwner_==this)runtimeQueries_->sceneOwner_=nullptr;
        visibilityRows_=collisionRows_=nullptr;sceneApplication_.reset();topologyClosed_=true;return true;
    }
protected:
    Owner()=default;
    ~Owner(){if(worldColdConstructed_||(topologyAttempted_&&!topologyClosed_))std::terminate();}
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
};
}
