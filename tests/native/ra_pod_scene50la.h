#include "../../src/game/original_source_portal_overlap231150.h"
#pragma once
#include "ra_source_camera_ownership50la.h"
#include "../../src/game/original_world_trigger_instance50la.h"
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
#include <thread>
#include "../../src/game/original_pod_cable_allocation50la.h"
#include "../../src/game/original_light_flare_numeric50la.h"
#include "../../platform/linux/native/race_audio_groups50ic.h"

namespace rr::platform {class OriginalUiLightManagerOwner50KG;class OriginalUiSearchListCold50KG;
struct OriginalUiMainApplication50KG;class OriginalUiGarageInstanceMaster50KG;}
namespace rr::game {class OriginalRaceConstructor50LA;class OriginalRacerModelSceneOwner50LA;struct OriginalRacerSceneObject50LA;}
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
    friend class ::ra_source_camera50LA::OwnershipBridge;
    friend class rr::platform::OriginalUiLightManagerOwner50KG;
    friend class g::original_gc_gameplay_lighting_candidate::Owner;
    friend class rr::game::OriginalRaceConstructor50LA;
    friend class rr::game::OriginalRacePauseWorldAuthority50LA;
protected:
// Inside ra_pod_scene50LA::Owner protected area; existing Application friend.
// Copies the SAME genuine startup-owned vectors, no new constant owner.
bool copySourceWorldStartDefaults(std::array<std::uint32_t,4>& origin,
    std::array<std::uint32_t,4>& forward)const noexcept {
    if(!worldColdConstructed_||!worldApplication_||
       worldEntity_.reference00!=1)return false;
    static_assert(sizeof(float)==sizeof(std::uint32_t));
    std::memcpy(origin.data(),worldApplication_->worldOrigin442F80_.data(),sizeof(origin));
    std::memcpy(forward.data(),worldApplication_->worldForward442FB0_.data(),sizeof(forward));
    return true;
}

bool copySourceWorldUpDefault(std::array<std::uint32_t,4>& up)const noexcept {
    if(!worldColdConstructed_||!worldApplication_||worldEntity_.reference00!=1)return false;
    static_assert(sizeof(float)==sizeof(std::uint32_t));
    std::memcpy(up.data(),worldApplication_->worldUp442FA0_.data(),sizeof(up));return true;
}

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
    //Authoritative World3C4: actual pointer-array metadata borrowing the SAME
    //prewarmed counted StaticModels Parent.entity identities. No reinterpret
    //of vector<unique_ptr<Parent>>, copied rows or duplicate model owner.
    std::optional<Entity**> sourceModels3C4_;
    std::unique_ptr<Entity*[]> sourceModelParents3C4_;
    ra_static_model_cold50LA::Owner* sourceModelOwner3C4_=nullptr;
    std::size_t sourceModelParentsPublished3C4_=0;
    bool modelParents3C4Attempted_=false,modelParents3C4Complete_=false;
    std::uint32_t modelParents3C4PC_=0x271124;
    bool initializeModels3C42705A8()noexcept {
        if(sourceModels3C4_||sourceModelParents3C4_||sourceModelOwner3C4_||modelParents3C4Attempted_)return false;
        sourceModels3C4_=nullptr;return true;
    }
    bool modelParents3C4Current(const ra_static_model_cold50LA::Owner*,bool complete=true)const noexcept;
    bool withdrawModelParents3C4()noexcept;
    const std::string* nativeParticleTexture41C_=nullptr; //Actual EnvironmentInfo immutable value.
    bool nativeParticleManagerAttempted_=false,nativeParticleManagerComplete_=false;
    bool completeNativeParticleManager1B4DC0(const ra_cold_sources50LA::Owner&,
        const std::vector<std::uint8_t>&,std::uint32_t&)noexcept;
    // Only the existing derived aggregate's sound-list constructor/retirement
    // cuts use these private base-owned authority leaves. No authority escapes.
    bool reserveNativeSoundListCustody()noexcept;
    bool reserveSourceAmbientListNative()noexcept;
    bool reserveSourceSoulTreeSoundListNative()noexcept;
    bool reserveSourceMaterialSoundListNative()noexcept;
    bool constructSourceMaterialSoundList279AA0(Entity&,bool drive,bool&,bool&,std::uint32_t&)noexcept;
    bool preflightSourceMaterialSoundListCountNative(const Entity&,bool,bool)const noexcept;
    bool releaseSourceMaterialSoundListCountNative(Entity&,bool&,bool&)noexcept;
    bool constructSourceSoulTreeSoundList279850(Entity&,bool&,bool&,std::uint32_t&)noexcept;
    bool preflightSourceSoulTreeSoundListCountNative(const Entity&,bool,bool)const noexcept;
    bool releaseSourceSoulTreeSoundListCountNative(Entity&,bool&,bool&)noexcept;
    bool constructSourceAmbientList2792E0(Entity&,bool&,bool&,std::uint32_t&)noexcept;
    bool preflightSourceAmbientListCountNative(const Entity&,bool,bool)const noexcept;
    bool releaseSourceAmbientListCountNative(Entity&,bool&,bool&)noexcept;
    bool destroySourceEmptyAmbient279320(Entity&,bool&,bool&,std::optional<std::uint32_t>&,std::uint32_t&)noexcept;
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
        bool published=false,complete=false;
        //Exclusive actual270FD0 route; legacy completed metadata is not adopted.
        bool sourceMode=false,sourceOuterReturned=false,sourceConstructorPublished=false;
        bool sourceCallerPublished=false;
        const Entity* sourceWorld=nullptr;
        std::uint64_t sourceGeneration=0;
        std::thread::id sourceThread;
    };
    //Authoritative source lanes, unknown until fresh2705B4/B8. Resource
    //domain construction must not silently stand in for these World stores.
    std::optional<const void*> nativeInstanceDescriptors3DC_;
    std::optional<std::int32_t> nativeInstanceCount3E0_;
    std::optional<NativeEmptyInstances3DC> nativeEmptyInstances3DC_;
    //Dedicated actual-source1B3B80 constructor custody. Never aliases the
    //legacy finish-line owner or its material/anchor readiness flags.
    std::unique_ptr<g::OriginalPodCableStripCold50LA> sourceFinishLineStripOwner_;
    g::OriginalPodCableStripCold50LA* sourceFinishLine540_=nullptr;
    bool sourceFinishLineConstructorEntered_=false,sourceFinishLine540Published_=false;
    std::optional<std::uint32_t> sourceFinishLineGroupWordC8_;
    bool sourceFinishLineSetupEntered_=false,sourceFinishLineMaterialReturned_=false;
    bool sourceFinishLineBaseReturned_=false;
    std::uint16_t sourceFinishLineSetupMask_=0;
    std::uint32_t sourceFinishLineSetupPC_=0x1b3bf4;
    std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> sourceFinishLineMaterialTable_;
    const std::string* sourceFinishLineDirectory_=nullptr;
    struct SourceFinishLineAttributes {
        bool stripAttributesEntered=false,stripAttributeGroupAttempted=false,stripAttributeGroupPublished=false;
        std::uint16_t stripAttributeDescriptorMask=0;std::optional<std::int32_t> stripAttributeDescriptor10;
        bool stripCommonAttempted=false,stripCommonPublished=false,stripCommonZeroed=false,stripCommonReturned=false;
        std::uint16_t stripCommon40Mask=0,stripCommonE0Mask=0;std::uint8_t stripCommon90Mask=0,stripCommon130Mask=0;
        std::optional<std::uint32_t> stripAttributeCount5C;
        bool stripAttributeCountsAttempted=false,stripAttributeCountsPublished=false,stripAttributeCountStored=false;
    } sourceFinishLineAttributes_; //Dedicated receipts; no typed-light owner/packet is borrowed.
    bool continueSourceFinishLineAttributes23BED8(std::uint32_t&)noexcept;
    bool preflightSourceFinishLineAttributesNative()const noexcept;
    //Dedicated finish-line134000 owner. No typed-light packet or serial alias.
    struct alignas(64) SourceFinishLinePacket {std::array<std::uint32_t,128> words;};
    struct SourceFinishLineParameters {
        std::array<std::optional<std::uint32_t>,8> header00To1C{};
        std::unique_ptr<SourceFinishLinePacket> packet;
        std::uint32_t* buffer20=nullptr;
        std::uint32_t* nativePayload04=nullptr; //Typed borrow, never a guest address.
        std::uint32_t count24=0,count28=0;
        std::int32_t field2C=-1;
        std::uint8_t constructorMask=0;
        bool packetAttempted=false,bufferPublished=false;
        std::optional<bool> bufferAssertionEnabled; //Produced only by successful guard1 admission.
        std::uint64_t serial=0;
    };
    std::unique_ptr<SourceFinishLineParameters> sourceFinishLineParameters54_;
    bool sourceFinishLineParameterAttempted_=false,sourceFinishLineParameterPublished_=false;
    bool sourceFinishLineParameterReturned_=false,sourceFinishLineParameterWithdrawn_=false;
    std::uint64_t sourceFinishLineParameterSerial_=0;
    bool continueSourceFinishLineParameters23BF78(std::uint32_t&)noexcept;
    bool preflightSourceFinishLineParametersNative()const noexcept;
    struct SourceFinishLineStreams {
        bool streamPayloadEntered=false,streamPayloadNativeBacking=false,streamReturned=false;
        std::array<std::uint32_t,4> streamStackTagD0{};bool streamStackTagKnown=false;
        std::array<std::optional<std::uint32_t>,4> streamPayloadOffsets;
        std::array<std::optional<std::uint32_t>,128> streamPayloadWords;
        std::array<std::uint8_t,4> streamPayloadCopies{};std::uint8_t streamPayloadFinished=0;
        std::uint32_t streamPayloadCount=0;
        std::optional<std::uint32_t> streamDescriptorParameterEnd,streamDescriptorParameterSpan;
        bool streamEntered=false,streamCountStored=false;std::uint8_t streamDescriptorMask=0;
        std::uint8_t streamArraysAttempted=0,streamArraysPublished=0;
        bool streamAliasAllocationAttempted=false,streamAliasPublished=false,streamBatchCountStored=false;
        std::uint8_t streamAliasConstructed=0;std::array<std::uint8_t,4> streamAliasStoreMask{};
        std::optional<std::uint32_t> streamDescriptorParameterBegin;
        g::OriginalStripStreamHistory23CCA0::Receipt streamBatchAssertion;
        std::array<g::OriginalStripStreamHistory23CCA0::Receipt,4> streamAliasAssertions;
    } sourceFinishLineStreams_;
    struct SourceFinishLineHeader {
        bool entered=false,returned=false,setupReturned=false;
        std::uint8_t paddingWords=0,storeMask=0,primitiveMask=0;
        SourceFinishLineParameters* primitiveParameters10=nullptr;
        g::OriginalPodCableStripCold50LA* setupReturn=nullptr;
        bool borrowersWithdrawn=false;
    } sourceFinishLineHeader_;
    bool continueSourceFinishLineHeader23BFBC(std::uint32_t&)noexcept;
    bool preflightSourceFinishLineHeaderNative()const noexcept;
    bool continueSourceFinishLineStreams23CCA0(std::uint32_t&)noexcept;
    bool continueSourceFinishLinePayload23D088(std::uint32_t&)noexcept;
    bool preflightSourceFinishLineStreamsNative()const noexcept;
    struct SourceFinishLineGeometry {
        bool entered=false,returned=false;
        std::array<std::uint8_t,3> setterMasks{}; //Positions, triples, UV; ordered vertex prefixes.
        std::array<std::array<std::array<g::OriginalStripStreamHistory23CCA0::Receipt,3>,4>,3> assertions;
        std::optional<std::array<std::uint32_t,4>> zeroNormalOriginWords;
        std::uint32_t pc=0x1b3ddc;
    } sourceFinishLineGeometry_;
    bool continueSourceFinishLineGeometry1B3DDC(const std::array<std::uint32_t,4>&,
        const std::array<std::uint32_t,4>&,std::uint32_t&)noexcept;

    bool constructSourceFinishLine1B3B80(std::uint32_t&)noexcept;
    bool setupSourceFinishLine150450(const std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>&,
        const std::string&,std::uint32_t&)noexcept;
    //World544 is a numeric table index, not another material reference.
    bool sourceFinishLineIndexLookupEntered_=false;
    std::optional<std::int32_t> sourceFinishLineMaterial544_;
    bool continueSourceFinishLineIndex1B3BFC(std::uint32_t&)noexcept;
    bool preflightSourceFinishLineNative()const noexcept;
    bool retireSourceFinishLineNative()noexcept;
    //Original negative-anchor branch only; native abort never sets this receipt.
    struct SourceFinishLineMissingCleanup {
        bool entered=false,returned=false;
        std::uint16_t releaseMask=0;
        std::optional<std::int32_t> material544Before;
    } sourceFinishLineMissingCleanup_;
    bool destroySourceMissingFinishLine1B3DC0(std::uint32_t&)noexcept;
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
    bool livePhysicsIndexAttempted_=false,livePhysicsIndexComplete_=false;
    std::uint32_t livePhysicsIndexPC_=0x270cc4;
    bool constructLivePhysicsIndex270CDC(std::shared_ptr<const ra_cold_sources50LA::Owner>,std::uint32_t&)noexcept;
    bool restartWorldPhysicsAttempted_=false;
    std::uint32_t restartWorldPhysicsPC_=0x27030c;
    std::optional<std::uint32_t> restartTriggerCount30_;
    std::uint32_t restartTriggerIndexS3_=0,restartTriggerOffsetS1_=0;
    std::uint32_t restartActionIndexS4_=0,restartActionOffsetS0_=0;
    struct NativeTriggerDescriptor;
    const NativeTriggerDescriptor* restartTriggerDescriptor_=nullptr;
    std::optional<std::int32_t> restartActionCount04_,restartActionType00_;
    bool restartWorldPhysicsReturned_=false;
    bool resetLiveWorldPhysics2702E0(const std::shared_ptr<g::OriginalApplicationEntities50LA>&,
        std::uint64_t,std::uint32_t&)noexcept;
    bool prepareWorldPhysicsIndex270CDC(std::shared_ptr<const ra_cold_sources50LA::Owner>,std::uint32_t&)noexcept;
    std::unique_ptr<g::OriginalWorldCollisionObjects50LA> worldCollisionObjects3CC_;
    //270550 does not write3CC. Only actual1B4EFC publishes this lane.
    std::optional<Entity**> sourceCollisionObjects3CC_;
    bool sourceCollisionObjectsAttempted_=false; //Sticky source/native-mode exclusion.
    bool reserveSourceCollisionAuthorityNative()noexcept;
    bool constructSourceCollisionGroup27BB40(std::size_t,std::uint32_t&)noexcept;
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
// Replace NativeEmptyToyList3C8 declaration, retaining its single unique_ptr holder.
    struct NativeEmptyToyList3C8 {
        Entity entity;
        std::uint32_t count30=0;
        bool sourceMode=false,sourceCounted=false,sourceEnrolled=false;
        bool sourceVtableKnown=false,sourceCountKnown=false,sourceArrayKnown=false;
        std::uint32_t sourceVtable08=0;
        const void* sourceArray34=nullptr;
    };
    bool sourceToyAttempted_=false; //Sticky source/legacy exclusion, no guest readiness.
    bool reserveSourceToyAuthorityNative()noexcept;
    bool constructSourceToy2D9B40(std::uint32_t&)noexcept;
    bool canRetireSourceToyNative()const noexcept;
    bool retireSourceToyCountNative(std::uint32_t&,bool sourceDestructor=false)noexcept;
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
    // Real load-issued descriptor metadata; no second trigger or action engine.
    using NativeActionDescriptor=ra_trigger_actions50LA::Owner::NativeActionDescriptor;
// Base protected actual-global copier, next to existing World vector copiers.
    bool copySourceTriggerOrigin(std::array<std::uint32_t,4>& out)const noexcept {
        if(!worldColdConstructed_||!worldApplication_||worldEntity_.reference00!=1)return false;
        std::memcpy(out.data(),worldApplication_->triggerOrigin444560_.data(),sizeof(out));return true;
    }
// Add before NativeTriggerDescriptor; genuine source geometry64 plain80B backing.
    struct SourceTriggerGeometry2DBEB4 {
        std::array<std::uint32_t,4> position00,box10,look20,up30;
        std::uint32_t yaw40,pitch44,roll48,radius4C;
    };
    static_assert(sizeof(SourceTriggerGeometry2DBEB4)==80,"source trigger geometry width");
    struct SourceTriggerDescriptor2DC578 {
        std::int32_t zone08=0,enabled0C=0,random10=0;
        std::array<char,40> name14,ignore3C;
        std::uint32_t vtable6C=0,known=0;
        std::unique_ptr<SourceTriggerGeometry2DBEB4> geometry64; //Native backing custody.
        std::optional<SourceTriggerGeometry2DBEB4*> sourceGeometry64; //Actual descriptor64 lane.
        bool geometryAllocationReturned=false,geometryPublished=false,geometryConstructed=false;
        bool geometrySourceFreeEntered=false,geometrySourceFreed=false,geometrySourceNullStored=false;
        std::optional<SourceTriggerGeometry2DBEB4> geometrySourceHistory;
        bool nameKnown=false,ignoreKnown=false;
    };
    struct NativeTriggerDescriptor {
    bool sourceActionLogAttempted=false,sourceActionLogError=false,sourceDescriptorReturned=false;
    std::size_t sourceActionLogBytes=0;

    bool sourceActionAttempted=false,sourceActionsAllocated=false,sourceActionAllocated=false;
    bool sourceActionTypeWritten=false,sourceActionReaderComplete=false,sourceActionNativeWithdrawn=false;

    std::unique_ptr<g::OriginalWorldTriggerInstance50LA> sourceInstance;
    bool sourceInstanceAttempted=false,sourceInstancePublished=false;
    //Retained native representation of the actual callee stack50/60/70/80;
    //not additional source descriptor fields.
    std::optional<std::array<std::array<std::uint32_t,4>,4>> sourceInstanceStackVectors;
    bool sourceInstanceNoOpReturned=false;

        std::optional<SourceTriggerDescriptor2DC578> sourceFields;
        Entity* trigger00=nullptr;
        std::int32_t actionCount04=0;
        const ra_trigger_authoring50LA::Source* authoring=nullptr;
        std::unique_ptr<NativeActionDescriptor*[]> actions68;
        std::unique_ptr<NativeActionDescriptor> action;
        std::size_t actionSlotsPublished=0;
        bool complete=false;
    };
    struct NativeTriggerList3D0 {
    std::array<bool,2> sourceDescriptorNativeRetired{};

    bool sourceMode=false,sourceEnrolled=false,sourceCountKnown=false,sourceArrayKnown=false;
    bool sourceArrayAllocationReturned=false;
    std::uint32_t sourceVtable08=0;
    bool sourceVtableKnown=false;

        Entity entity;
        bool counted=false;
        std::uint32_t count30=0;
        std::size_t joined=0;
        std::shared_ptr<ra_static_candidates50LA::Owner> candidates;
        std::shared_ptr<const ra_trigger_authoring50LA::Owner> authoring;
        std::shared_ptr<ra_trigger_actions50LA::Owner> actions;
        std::unique_ptr<NativeTriggerDescriptor*[]> descriptors34;
        std::array<std::unique_ptr<NativeTriggerDescriptor>,2> descriptorOwners;
        std::size_t descriptorSlotsPublished=0;
        bool readerEntered=false,descriptorsRetired=false;
        std::uint32_t readerPC=0x2dc430;
        std::array<std::unique_ptr<NativeTriggerObserver>,2> observers;
        std::array<bool,2> parented{},geometryRoleConsumed{};
        std::size_t servicesCompleted=0;
        bool serviceEntered=false,serviceTerminal=false,serviceComplete=false;
        bool complete=false;
    };
    bool sourceTriggerAttempted_=false;
    bool reserveSourceTriggerAuthorityNative()noexcept;
    bool constructSourceTriggerList2DC320(std::uint32_t&)noexcept;
    bool canRetireSourceTriggerNative()const noexcept;
    bool retireSourceTriggerCountNative(std::uint32_t&)noexcept;
    bool copySourceTriggerInstanceOrigin(std::array<std::uint32_t,4>& out)const noexcept {
        if(!worldColdConstructed_||!worldApplication_||worldEntity_.reference00!=1)return false;
        std::memcpy(out.data(),worldApplication_->triggerInstanceOrigin4438D0_.data(),sizeof(out));return true;
    }
    bool sourceTriggerLoadedDescriptorCurrentNative(const NativeTriggerDescriptor& d)const noexcept {
        //Producer-only loaded-owner admission; cleanup preflights deliberately
        //also accept withdrawn storage, which cannot authorize a fresh service.
        if(!d.sourceDescriptorReturned||!d.sourceActionLogAttempted||!d.sourceActionReaderComplete||
           d.sourceActionNativeWithdrawn||!d.sourceInstance||!d.sourceInstancePublished||
           !preflightSourceTriggerInstanceNative(d)||!preflightSourceTriggerActionNative(d)||
           !sourceTriggerGeometryCurrentNative(d,false))return false;
        const auto& t=*d.sourceInstance;
        return !t.nativeClosed_&&t.counted_&&t.enrolled_&&t.colliderEnrolled_&&t.colliderEntity()&&
            t.constructed_&&t.setupReturned_&&!t.serviceEntered_&&!t.membershipNativeWithdrawn_;
    }
    bool sourceTriggerGeometryCurrentNative(const NativeTriggerDescriptor& d,bool returned)const noexcept {
        if(!d.sourceInstance||!d.sourceInstancePublished||d.trigger00!=&d.sourceInstance->entity_||
           d.sourceInstance->application_!=worldApplication_.get()||
           d.sourceInstance->entity_.sessionGeneration!=worldEntity_.sessionGeneration||
           !d.sourceInstance->preflightNative())return false;
        // The descriptor read survives a bounded refusal before callee entry.
        return d.sourceInstance->serviceGeometryReturned_==returned&&
            (!returned||d.sourceInstance->serviceEntered_);
    }
    bool refreshSourceTriggerFogGeometry2A0A80(NativeTriggerDescriptor& d,
        g::OriginalTriggerFogGeometryPrefix50LA& history,std::uint32_t& pc)noexcept {
        if(pc!=0x2a0a80||history.step||!history.historyCurrent()||!sourceTriggerLoadedDescriptorCurrentNative(d))return false;
        const bool returned=d.sourceInstance->refreshFogService2A0A80(pc);
        history=d.sourceInstance->fogGeometry_; //Pointer-free receipt also survives native instance disposal.
        return returned;
    }
    bool sourceTriggerFogGeometryCurrentNative(const NativeTriggerDescriptor& d,
        const g::OriginalTriggerFogGeometryPrefix50LA& history)const noexcept {
        //Historical/live consistency, not positive fresh-entry permission.
        //Cleanup can withdraw membership before the outer service is marked withdrawn.
        return history.historyCurrent()&&sourceTriggerGeometryCurrentNative(d,history.step==22)&&
            d.sourceInstance->serviceEntered_==(history.step!=0)&&d.sourceInstance->fogGeometry_==history;
    }
    bool beginSourceTriggerFogMembership120070(NativeTriggerDescriptor& d,const void* actualNode,
        const g::OriginalTriggerFogGeometryPrefix50LA& geometry,
        g::OriginalTriggerFogMembershipPrefix50LA& history,std::uint32_t& pc)noexcept {
        if(pc!=0x2a0b0c||!actualNode||history.step||!history.historyCurrent()||geometry.step!=22||
           !sourceTriggerFogGeometryCurrentNative(d,geometry)||!preflightSourceTriggerInstanceNative(d)||
           !preflightSourceTriggerActionNative(d)||!d.sourceDescriptorReturned||!d.sourceActionReaderComplete||
           d.sourceActionNativeWithdrawn||!worldApplication_||!worldApplication_->constructed_||worldApplication_->released_)return false;
        const auto& t=*d.sourceInstance;
        if(t.nativeClosed_||!t.counted_||!t.enrolled_||!t.colliderEnrolled_||!t.colliderEntity()||
           !t.constructed_||!t.setupReturned_||t.membershipNativeWithdrawn_)return false;
        //The aggregate authenticates actualNode against the exact retained4C4 slot immediately before this handoff.
        const bool published=d.sourceInstance->beginFogServiceMembership120070(worldApplication_->garageObserverShared_,actualNode,pc);
        history=d.sourceInstance->fogMembership_; //Copy partials too; no live pointers survive in this receipt.
        return published;
    }
    bool sourceTriggerFogMembershipCurrentNative(const NativeTriggerDescriptor& d,
        const g::OriginalTriggerFogMembershipPrefix50LA& history)const noexcept {
        //Cleanup-compatible consistency is deliberately not fresh-entry permission.
        return history.historyCurrent()&&d.sourceInstance&&d.sourceInstance->fogMembership_==history&&
            d.sourceInstance->fogMembershipCurrentNative();
    }
    bool continueSourceTriggerFogBounds1200F0(NativeTriggerDescriptor& d,
        const g::OriginalTriggerFogGeometryPrefix50LA& geometry,
        const g::OriginalTriggerFogMembershipPrefix50LA& membership,
        g::OriginalTriggerFogBoundsPrefix50LA& history,std::uint32_t& pc)noexcept {
        if(pc!=0x1200f0||history.step||!history.historyCurrent()||geometry.step!=22||membership.step!=11||
           !sourceTriggerFogGeometryCurrentNative(d,geometry)||!sourceTriggerFogMembershipCurrentNative(d,membership)||
           !preflightSourceTriggerInstanceNative(d)||!preflightSourceTriggerActionNative(d)||
           !d.sourceDescriptorReturned||!d.sourceActionReaderComplete||d.sourceActionNativeWithdrawn||
           !worldApplication_||!worldApplication_->constructed_||worldApplication_->released_)return false;
        const auto& t=*d.sourceInstance;
        if(t.nativeClosed_||!t.counted_||!t.enrolled_||!t.colliderEnrolled_||!t.colliderEntity()||
           !t.constructed_||!t.setupReturned_||t.membershipNativeWithdrawn_)return false;
        const bool returned=d.sourceInstance->continueFogBoundsPrefix1200F0(pc);
        history=d.sourceInstance->fogBounds_; //Own partials or exact tailStep100 return120170; never registration readiness.
        return returned;
    }
    bool sourceTriggerFogBoundsCurrentNative(const NativeTriggerDescriptor& d,
        const g::OriginalTriggerFogBoundsPrefix50LA& history)const noexcept {
        return history.historyCurrent()&&d.sourceInstance&&d.sourceInstance->fogBounds_==history&&
            d.sourceInstance->fogBoundsCurrentNative();
    }
    template<class ActualMethod>
    bool continueSourceTriggerFogPrimary120170(NativeTriggerDescriptor& d,const void* node,
        const g::OriginalTriggerFogBoundsPrefix50LA& bounds,g::OriginalTriggerFogPrimaryPrefix50LA& history,
        ActualMethod&& actualMethod,std::uint32_t& pc)noexcept {
        if(pc!=0x120170||history.step||!history.historyCurrent()||bounds.tailStep!=100||!node||
           !sourceTriggerFogBoundsCurrentNative(d,bounds)||!preflightSourceTriggerInstanceNative(d)||
           !preflightSourceTriggerActionNative(d)||!d.sourceDescriptorReturned||!d.sourceActionReaderComplete||
           d.sourceActionNativeWithdrawn||!worldApplication_||!worldApplication_->constructed_||worldApplication_->released_)return false;
        const auto& t=*d.sourceInstance;
        if(t.nativeClosed_||!t.counted_||!t.enrolled_||!t.colliderEnrolled_||!t.constructed_||!t.setupReturned_||
           t.membershipNativeWithdrawn_||t.fogMembershipNode_!=node)return false;
        const bool returned=d.sourceInstance->continueFogPrimary120170(this,node,actualMethod,pc);
        history=d.sourceInstance->fogPrimary_; //Copy every original partial; field publication/callee entry is not registration return.
        return returned;
    }
    bool sourceTriggerFogPortalInputsOwnerNative(const NativeTriggerDescriptor& d,const void* cursor,
        const g::OriginalWreckComponent50LA*& portal,
        const std::array<std::array<std::uint32_t,4>,4>*& vertices)const noexcept {
        if(!d.sourceInstance||!cursor)return false;
        const auto& t=*d.sourceInstance;const auto& h=t.servicePrimary_;
        const auto* method=t.fogPortalMethod_;
        if(!sourceTriggerPrimaryInputsNative(d,&h,t.collider_.get())||!t.fogPortalGraphPin_||
           t.fogPortalCustodyWithdrawn_||t.fogPortalHead_!=cursor||!method||method!=h.method||
           method->world_!=this||method->application_!=worldApplication_.get()||
           method->ordinal_<10u||method->ordinal_>=20u)return false;
        const auto z=method->ordinal_-10u;const auto& root=sourceSearchNodes_[1][z];
        if(!sourceTopologyMode_||sourceTopologyWithdrawn_||!root.owner||root.withdrawn||!root.published||
           !sourceCollisionSlots4C4_||sourceCollisionSlots4C4_[z]!=root.owner.get()||
           t.fogPortalZone_!=root.owner.get()||root.owner->portalLinks30.get()!=cursor||
           !preflightSourceSearchNodeNative(root,z,true)||!sourcePortalWorldLinksCurrent(*root.owner,true,z))return false;
        unsigned pins=0;for(const auto& b:sourceCollisionPortalBorrows_)if(b.registration==&h){
            const auto token=b.token.lock();if(token&&token==t.fogPortalGraphPin_)++pins;}
        if(pins!=1)return false;
        std::size_t index=40;unsigned matches=0;
        for(std::size_t i=0;i<40;++i)if(sourcePortalWorldLinks_[i/4][i%4].published==cursor){index=i;++matches;}
        if(matches!=1||!(index&1u))return false; //Only the actual collision head, never visibility or an ordinal-as-address.
        const auto& link=sourcePortalWorldLinks_[index/4][index%4];const auto& p=nativePortals_[index/4];
        if(link.withdrawn||!link.attempted||link.pending||link.knownMask!=31u||link.from!=root.owner.get()||!link.to||
           !p||sourcePortalFrames_[index/4].withdrawn||!p->sourceMode||!p->counted||!p->source.enrolled||
           !p->attached||!p->source.constructorReturned||!p->source.loadReturned||p->source.copiedVertices!=4||
           p->source.worldLinksProduced!=4||!p->source.connectA||!p->source.connectB)return false;
        const auto from=index%4<2?*p->source.connectA:*p->source.connectB;
        const auto to=index%4<2?*p->source.connectB:*p->source.connectA;
        if(from!=z||to>=10||to==z)return false;
        const auto& neighbor=sourceSearchNodes_[1][to];
        if(!neighbor.owner||neighbor.withdrawn||!neighbor.published||link.to!=neighbor.owner.get()||
           sourceCollisionSlots4C4_[to]!=neighbor.owner.get()||!preflightSourceSearchNodeNative(neighbor,to,true)||
           link.published->neighbor04!=&neighbor.owner->entity||link.published->portal08!=&p->entity)return false;
        portal=link.published->portal08;vertices=&p->source.vertices30;return true;
    }
    bool sourceTriggerFogPrimaryCurrentNative(const NativeTriggerDescriptor& d,
        const g::OriginalTriggerFogPrimaryPrefix50LA& history)const noexcept {
        if(!history.historyCurrent()||!d.sourceInstance||d.sourceInstance->fogPrimary_!=history||
           !d.sourceInstance->fogPrimaryCurrentNative())return false;
        const auto& t=*d.sourceInstance;
        if(!history.portalHead.nativePinAcquired||t.fogPortalCustodyWithdrawn_)return true;
        const auto& h=t.servicePrimary_;const auto* method=t.fogPortalMethod_;
        if(!method||method!=h.method||method->ordinal_<10u||method->ordinal_>=20u||
           method->world_!=this||method->application_!=worldApplication_.get())return false;
        const auto z=method->ordinal_-10u;const auto& node=sourceSearchNodes_[1][z];
        if(!sourceTopologyMode_||sourceTopologyWithdrawn_||!node.owner||node.withdrawn||!node.published||
           !node.counted||!node.enrolled||!node.ordinalKnown||node.constructorMask!=511||
           !sourceCollisionSlots4C4_||sourceCollisionSlots4C4_[z]!=node.owner.get()||
           t.fogPortalZone_!=node.owner.get()||node.owner->nativeList34!=method||
           !preflightSourceSearchNodeNative(node,z,true)||!sourcePortalWorldLinksCurrent(*node.owner,true,z))return false;
        unsigned pins=0;for(const auto& b:sourceCollisionPortalBorrows_)if(b.registration==&h){
            const auto token=b.token.lock();if(token&&token==t.fogPortalGraphPin_)++pins;}
        if(pins!=1||(history.portalHead.headNull&&t.fogPortalHead_!=node.owner->portalLinks30.get()))return false;
        if(history.portalInputs.step){
            const g::OriginalWreckComponent50LA* portal=nullptr;
            const std::array<std::array<std::uint32_t,4>,4>* vertices=nullptr;
            if(!sourceTriggerFogPortalInputsOwnerNative(d,t.fogPortalHead_,portal,vertices)||
               t.fogPortalOperandA0_!=portal||t.fogPortalVertices_!=vertices)return false;
        }
        return true;
    }
    bool sourceTriggerFogPortalHeadPinnedNative(const NativeTriggerDescriptor& d)const noexcept {
        return d.sourceInstance&&bool(d.sourceInstance->fogPortalGraphPin_);
    }
    template<class ActualHead>
    bool continueSourceTriggerFogPortalHead280BF0(NativeTriggerDescriptor& d,
        g::OriginalTriggerFogPrimaryPrefix50LA& history,ActualHead&& actualHead,std::uint32_t& pc)noexcept {
        if(pc!=0x280bf0||history.publication.step!=12||history.portalHead.step||
           !sourceTriggerFogPrimaryCurrentNative(d,history)||!preflightSourceTriggerInstanceNative(d)||
           !preflightSourceTriggerActionNative(d)||!d.sourceDescriptorReturned||!d.sourceActionReaderComplete||
           d.sourceActionNativeWithdrawn||!worldApplication_||!worldApplication_->constructed_||worldApplication_->released_)return false;
        const auto& t=*d.sourceInstance;
        if(t.nativeClosed_||!t.counted_||!t.enrolled_||!t.colliderEnrolled_||t.membershipNativeWithdrawn_)return false;
        const bool returned=d.sourceInstance->continueFogPortalHead280BF0(actualHead,pc);
        history=d.sourceInstance->fogPrimary_; //Copy partial admission/read or actual portal return; no primary return.
        return returned;
    }
    template<class ActualPortal>
    bool continueSourceTriggerFogPortalInputs280C40(NativeTriggerDescriptor& d,
        g::OriginalTriggerFogPrimaryPrefix50LA& history,ActualPortal&& actualPortal,std::uint32_t& pc)noexcept {
        if(pc!=0x280c40||history.portalHead.step!=9||history.portalHead.headNull!=false||
           history.portalInputs.nativeAdmissionAttempted||!sourceTriggerFogPrimaryCurrentNative(d,history)||
           !preflightSourceTriggerInstanceNative(d)||!preflightSourceTriggerActionNative(d)||
           !d.sourceDescriptorReturned||!d.sourceActionReaderComplete||d.sourceActionNativeWithdrawn||
           !worldApplication_||!worldApplication_->constructed_||worldApplication_->released_)return false;
        const auto& t=*d.sourceInstance;
        if(t.nativeClosed_||!t.counted_||!t.enrolled_||!t.colliderEnrolled_||t.membershipNativeWithdrawn_)return false;
        const bool returned=d.sourceInstance->continueFogPortalInputs280C40(actualPortal,pc);
        history=d.sourceInstance->fogPrimary_; //Own inputs or refusal, not the generic overlap receipt.
        return returned;
    }
    bool sourceTriggerFogPrimaryNodeCurrentNative(const NativeTriggerDescriptor& d,const void* node)const noexcept {
        if(!d.sourceInstance||d.sourceInstance->fogMembershipNode_!=node)return false;
        const auto& h=d.sourceInstance->servicePrimary_;
        return !h.entered||(h.zone==node&&h.world==this);
    }
    bool refreshSourceTriggerGeometry2A0A80(NativeTriggerDescriptor& d,std::uint32_t& pc)noexcept {
        if(!preflightSourceTriggerInstanceNative(d)||!d.sourceInstance||!d.sourceInstancePublished||
           d.trigger00!=&d.sourceInstance->entity_||d.sourceInstance->application_!=worldApplication_.get()||
           d.sourceInstance->entity_.sessionGeneration!=worldEntity_.sessionGeneration)return false;
        return d.sourceInstance->refreshService2A0A80(pc);
    }
    bool beginSourceTriggerMembership120070(NativeTriggerDescriptor& d,std::uint32_t& pc)noexcept {
        if(!sourceTriggerGeometryCurrentNative(d,true)||!worldApplication_)return false;
        if(!d.sourceInstance->beginServiceMembership120070(worldApplication_->garageObserverShared_,pc))return false;
        return d.sourceInstance->continueServiceBounds1200F0(pc);
    }
    bool sourceTriggerPrimaryInputsNative(const NativeTriggerDescriptor& d,
        const void* occurrence,const void* collider)const noexcept {
        return d.sourceInstance&&d.sourceInstancePublished&&d.trigger00==&d.sourceInstance->entity_&&
            d.sourceInstance->application_==worldApplication_.get()&&
            occurrence==&d.sourceInstance->servicePrimary_&&collider==d.sourceInstance->collider_.get();
    }
    template<class ActualMethod>
    bool continueSourceTriggerPrimary2809C0(NativeTriggerDescriptor& d,const void* zone,
        ActualMethod&& actualMethod,std::uint32_t& pc)noexcept {
        if(!sourceTriggerGeometryCurrentNative(d,true))return false;
        return d.sourceInstance->continueServicePrimary2809C0(this,zone,actualMethod,pc);
    }
    bool sourceTriggerPortalCenterNative(const NativeTriggerDescriptor& d,
        const std::array<std::uint32_t,4>& center)const noexcept {
        return d.sourceInstance&&d.sourceInstance->servicePrimary_.center==center;
    }
    template<class ActualPortal>
    bool continueSourceTriggerPortals280BF0(NativeTriggerDescriptor& d,ActualPortal&& portal,std::uint32_t& pc)noexcept {
        if(!sourceTriggerGeometryCurrentNative(d,true)||!d.sourceInstance->servicePrimary_.published)return false;
        const bool returned=d.sourceInstance->continueServicePortals280BF0(portal);
        pc=d.sourceInstance->pc_=d.sourceInstance->servicePrimary_.pc;return returned;
    }
    bool finishSourceTriggerMembership120190(NativeTriggerDescriptor& d,std::uint32_t& pc)noexcept {
        return sourceTriggerGeometryCurrentNative(d,true)&&d.sourceInstance->finishServiceMembership120190(pc);
    }
    bool sourceTriggerCallbackReturnedNative(const NativeTriggerDescriptor& d)const noexcept {
        return d.sourceInstance&&d.sourceInstance->serviceReturn_.triggerReturned&&
            d.sourceInstance->serviceReturnCurrentNative();
    }
    bool installSourceTriggerLostAi1B5AD0(NativeTriggerDescriptor& d,std::uint32_t& pc)noexcept {
        if(!preflightSourceTriggerActionNative(d)||!sourceTriggerCallbackReturnedNative(d)||
           !d.sourceActionReaderComplete||!d.action||!d.actions68||d.actionSlotsPublished!=1||
           d.actions68[0]!=d.action.get()||d.action->type00!=22||d.action->vtable108!=0x3cf0b0u)return false;
        pc=0x1b5ad0;const void* payload=&d.action->payload10C;
        pc=0x1b5ae4;if(!d.sourceInstance->installServiceAction2A0A30(payload,pc))return false;
        pc=0x1b5af0;pc=0x1b5afc;return true; //Actual V0=1; no LostAI gameplay callback invoked.
    }
    bool sourceTriggerActionInstalledNative(const NativeTriggerDescriptor& d)const noexcept {
        return d.sourceInstance&&d.sourceInstance->serviceAction_.returned&&
            d.sourceInstance->serviceActionCurrentNative();
    }
    bool registerSourceTrigger141960(NativeTriggerDescriptor& d,std::uint32_t& pc)noexcept {
        if(!nativeTriggerList3D0_||!nativeTriggerList3D0_->sourceMode||!worldApplication_||
           !canRetireSourceTriggerNative()||!sourceTriggerCallbackReturnedNative(d)||
           !sourceTriggerActionInstalledNative(d)||!d.sourceFields||!d.sourceFields->geometrySourceNullStored||
           !d.sourceInstance||d.trigger00!=&d.sourceInstance->entity_)return false;
        auto& t=*d.sourceInstance;auto& parent=nativeTriggerList3D0_->entity;
        if(t.serviceRegistrationAttempted_)return false;
        t.serviceParent_=&parent;t.serviceRegistrationAttempted_=true;pc=0x2dcc30;
        const auto result=worldApplication_->entityAuthority_.attachChildren(parent,t.entity_,0x402,
            worldApplication_->attachmentLock423F58());
        pc=result.sourcePC;if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
        t.serviceRegistered_=true;pc=0x2dcc38;return true;
    }
    bool sourceTriggerRegistrationCurrentNative(const NativeTriggerDescriptor& d)const noexcept {
        return nativeTriggerList3D0_&&d.sourceInstance&&d.sourceInstance->serviceRegistered_&&
            d.sourceInstance->serviceParent_==&nativeTriggerList3D0_->entity&&d.sourceInstance->preflightNative();
    }
    bool detachSourceTriggerRegistrationNative(NativeTriggerDescriptor& d)noexcept {
        if(!d.sourceInstance)return true;auto& t=*d.sourceInstance;
        if(!t.serviceRegistered_||t.serviceTopologyWithdrawn_)return true;
        if(!nativeTriggerList3D0_||!worldApplication_||t.application_!=worldApplication_.get()||
           t.serviceParent_!=&nativeTriggerList3D0_->entity||!t.preflightNative())return false;
        const auto result=worldApplication_->entityAuthority_.detach(t.entity_,worldApplication_->attachmentLock423F58());
        if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return false;
        t.serviceTopologyWithdrawn_=true;return true; //Native cancellation only; source402 history survives.
    }
    bool withdrawSourceTriggerServiceMembershipNative()noexcept {
        if(!nativeTriggerList3D0_)return true;
        for(std::size_t i=nativeTriggerList3D0_->descriptorOwners.size();i-->0;){
            auto& d=nativeTriggerList3D0_->descriptorOwners[i];
            if(d&&d->sourceInstance&&(!d->sourceInstance->withdrawServiceMembershipNative()||
               !detachSourceTriggerRegistrationNative(*d)))return false;
        }
        return true;
    }
    bool reserveSourceTriggerInstanceNative()noexcept;
    bool constructSourceTriggerInstance2DBFE0(NativeTriggerDescriptor&,const std::array<std::uint32_t,4>&,std::uint32_t&)noexcept;
    bool setupSourceTriggerInstance2DC01C(NativeTriggerDescriptor&,const std::array<std::uint32_t,4>&,std::uint32_t,std::uint32_t&)noexcept;
    std::size_t sourceTriggerInstanceCountNative()const noexcept;
    bool preflightSourceTriggerInstanceNative(const NativeTriggerDescriptor&)const noexcept;
    bool retireSourceTriggerInstanceNative(NativeTriggerDescriptor&)noexcept;

    bool copySourceTriggerCollisionDefaults(std::array<std::uint32_t,4>& sentinel,
        std::uint32_t& category)const noexcept {
        if(!worldColdConstructed_||!worldApplication_||worldEntity_.reference00!=1)return false;
        const auto& startup=worldApplication_->collisionStartup3C81A0_;
        sentinel=startup.vectors3E02C0[0];category=startup.globals3E0708[4];return true;
    }
    bool sourceTriggerOneTime2DC060(NativeTriggerDescriptor&,std::int32_t)noexcept;
    bool sourceTriggerParent2DC068(NativeTriggerDescriptor&,const char*,std::size_t)noexcept;
    bool preflightSourceTriggerActionNative(const NativeTriggerDescriptor&)const noexcept;
    bool retireSourceTriggerActionNative(NativeTriggerDescriptor&)noexcept;

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
    bool nativeTriggerDescriptorCurrent(std::size_t)const noexcept;
    bool canRetireNativeTriggerDescriptors()const noexcept;
    bool canRetireNativeTriggerList3D0()const noexcept;
    // Private actual2DCC70 leaf only. No World2702E0/restart1AC8E0 caller
    // or reset completion is inferred; collider membership stays untouched.
    bool resetNativeTriggerList2DCC70(const std::shared_ptr<g::OriginalApplicationEntities50LA>&,
        std::uint64_t generation,std::uint32_t& sourcePC)noexcept;
    struct RestartParticleReset {
        bool attempted=false,returned=false;
        std::uint32_t stoppedPC=0x1ac96c;
        std::optional<std::uint32_t> auxiliaryGate18;
        std::optional<std::uint32_t> auxiliaryIndexA2,auxiliaryOffsetA3,auxiliaryFillA1;
    } restartParticleReset_;
    bool resetNativeParticles161F60(const std::shared_ptr<g::OriginalApplicationEntities50LA>&,
        std::uint64_t,std::uint32_t&)noexcept;
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
    struct NativeSunFlareCallback {bool sourceMode=false;std::uint8_t sourceKnownMask=0;std::uint32_t sourceFunction=0x2c5610;NativeSunFlare* self=nullptr;};
    struct alignas(64) NativeSunFlareReadback {std::array<std::uint8_t,1024> bytes;};
    struct NativeSunFlareParameters {
        bool sourcePacketAttempted=false,sourceBufferPublished=false;
        std::optional<std::uint32_t*> sourceBuffer20;
        std::array<bool,128> sourcePacketWordKnown{};
        std::uint32_t sourceHeaderKnownMask=0;
        std::uint64_t sourcePacketHistorySerial=0;
        bool sourcePacketHistoryWithdrawn=false;
        bool sourceMode=false;
        std::uint32_t sourceKnownMask=0;
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
        bool sourceMode=false;
        std::uint32_t sourceKnownMask=0;
        std::optional<std::uint32_t> sourceVtable08;
        std::optional<NativeSunFlareParameters*> sourceParameters10;
        const std::string* sourceRowName=nullptr;
        bool sourceReferenceReleased=false;
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
        bool sourceCallbackAttempted=false,sourceCallbackPublished=false;
        std::optional<NativeSunFlareCallback*> sourceCallback04;
        NativeSunFlare* sourceCallbackOwner=nullptr;
        bool sourcePacketNativeWithdrawn=false;
        std::optional<std::uint32_t> sourcePacketRetiredCount,sourcePacketRetiredCapacity,sourcePacketRetiredHeaderMask;
        std::optional<std::uint64_t> sourcePacketRetiredSerial;
        bool sourceMode=false,sourceNativeClosed=false;
        bool sourceParametersAttempted=false,sourceParametersPublished=false;
        bool sourcePrimitiveAttempted=false,sourcePrimitivePublished=false;
        std::optional<NativeSunFlareParameters*> sourceParameters08;
        std::optional<NativeSunFlarePrimitive*> sourcePrimitive04;
        std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA> sourceTable;
        std::shared_ptr<const rr::assets::OriginalUiMaterialApplication50LA> sourceApplication;
        std::array<char,256> sourcePath40{};
        bool sourcePathKnown=false;
        std::string sourceRequested;
        std::unique_ptr<NativeSunFlarePrimitive> primitive04;
        std::unique_ptr<NativeSunFlareParameters> parameters08;
    };
    struct NativeSunFlareSprite {
        std::uint32_t sourceSetterMask=0;
        bool sourceMode=false;
        std::optional<std::uint32_t> sourceVtable00;
        std::uint32_t sourceKnownMask=0;

        NativeSunFlareMaterial material;
        std::optional<std::int32_t> x0C,y10; //Unwritten by2E5E60.
        std::uint32_t width14=0,height18=0;
        float scale1C=1.f,scale20=1.f,u24=0.f,v28=1.f,u2C=0.f,v30=1.f;
        std::int32_t depth34=0;
        std::array<std::uint32_t,4> colors38{{0x80808080,0x80808080,0x80808080,0x80808080}};
        std::uint8_t alpha48=255;
    };
    struct NativeSunFlare {
bool sourceCallbackSelfStored=false;
bool sourceColors44Attempted=false,sourceColors44Published=false;
bool sourceColors48Attempted=false,sourceColors48Published=false;
bool sourceReadbackAttempted=false,sourceReadbackPublished=false;
std::optional<std::array<float,4>*> sourceColors44,sourceColors48;
std::optional<NativeSunFlareReadback*> sourceReadback78;
std::optional<std::uint32_t> sourceColors44Count,sourceColors48Count;
bool sourceDepthStored=false;

std::array<std::uint8_t,10> sourceSaturation44Known{},sourceSaturation48Known{};
        bool sourceMode=false,sourceEnrolled=false;
        std::uint32_t sourceConstructorMask=0;
        bool sourceConstructorReturned=false,sourceZonesStored=false;
        bool sourceDirectionStored=false,sourceMinStored=false,sourceMaxStored=false;
        bool sourceModeStored=false,sourceSpriteAttempted=false,sourceSpritePublished=false;
        std::optional<NativeSunFlareSprite*> sourceSprite64;

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
        bool sourceMode=false,sourceNativeClosed=false;
        bool sourcePointerPublished=false,sourceOwnerAttempted=false,sourceSlotPublished=false;
        const g::OriginalApplicationEntities50LA* sourceApplication=nullptr;
        std::thread::id sourceThread;

        //A NEW native exclusive domain, NOT a receipt that2705F0 ran.
        //No prior native flare service exists, and this cannot reset another.
        std::uint64_t generation=0;
        std::optional<std::uint32_t> count4A8;
        std::unique_ptr<NativeSunFlare*[]> pointerBacking;
        bool pointerAllocationAttempted=false;
        std::unique_ptr<NativeSunFlare> owner;
    };
    //World4A4 publication, separate from its sole owned pointer backing.
    //Fresh2705F0 writes null;270550 NEVER writes count4A8.
    std::optional<NativeSunFlare**> nativeSunFlarePointers4A4_;
    std::optional<NativeSunFlareDomain> nativeSunFlares_;
    bool reserveSourceSunFlareNative()noexcept;
    bool constructSourceSunFlare2C5730(std::uint32_t&)noexcept;
    bool constructSourceSunFlareSprite2E5E60(std::uint32_t,std::uint32_t&)noexcept;
    bool canRetireSourceSunFlareNative()const noexcept;
    bool retireSourceSunFlareNative()noexcept;
    bool beginNativeSunFlareDomain271E5C(std::uint32_t&)noexcept;
    bool constructNativeSunFlare2C5730(NativeSunFlareInputs,std::uint32_t&)noexcept;
    static bool canRetireSourceSunFlareMaterialNative(const NativeSunFlareMaterial&)noexcept;
    static bool retireSourceSunFlareMaterialNative(NativeSunFlareMaterial&)noexcept;
    static bool canRetireNativeSunFlareMaterial(const NativeSunFlareMaterial&)noexcept;
    static void retireNativeSunFlareMaterial(NativeSunFlareMaterial&)noexcept;
    bool canRetireNativeSunFlare()const noexcept;
    bool retireNativeSunFlare()noexcept;
    // Fresh native optional-water ownership domain, created ONLY at the real
    // guarded countedWorld constructor. Historical guest400/3FC remain unknown.
    std::shared_ptr<native_world_water50LA::Domain> nativeWaterDomain_;
    const native_world_water50LA::Domain* readNativeAbsentWater400(std::uint32_t&)const noexcept;
    //Canonical source publications missing from earlier native adaptations.
    //No historical zero ledger: load writers/readers use these same lanes.
    std::array<std::optional<std::uint8_t>,260> worldText3C_,worldText140_,worldText244_;
    std::optional<const void*> worldBoundary348_,worldZoneInfo350_;
    std::optional<std::uint32_t> worldZones34C_,worldPortals358_;
    std::uint8_t world390KnownValue_=0,world390KnownMask_=0;
    std::uint8_t world4F0KnownValue_=0,world4F0KnownMask_=0;
    std::optional<g::original_world_environment50KG::Owner*> nativeEnvironment3A4_;
    //No positive owner/producer is admitted for these source roles yet.
    //Unknown before270550; null only after its actual fresh ordered store.
    std::optional<const void*> worldWater3FC_,worldWater400_,worldWater404_,worldEcosystem408_;
    struct UnjoinedToyManager101330;
    std::optional<UnjoinedToyManager101330*> worldToyManager49C_;
    std::optional<const void*> worldPhysicsObjectPointers4B4_,worldRole4B8_;
    std::optional<std::uint32_t> worldPhysicsObjectCount4B0_;
    std::array<std::optional<const void*>,5> worldRoles4DCTo4EC_;
    bool worldReset270550Attempted_=false,worldReset270550Complete_=false;
    std::uint32_t worldReset270550PC_=0x270550;
    // Native read-through Racer leases pin these actual World source lanes.
    std::size_t constructorRacerWorldBorrows_=0;
    // SAME aggregate-issued leases, also read by scene light retirement.
    std::size_t runtimeCameraBorrows_=0,runtimeWorldBorrows_=0;
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
        const track_height_cold50JB::Owner&,std::uint32_t&,ra_cold_aggregate50LA::Owner&)noexcept;
    bool rollbackNativeWorldConstructionFailure(std::uint32_t&)noexcept;
// Protected base Owner declarations. Exact same World/Application authority;
// no sceneApplication_/native-emitter adoption.
bool reserveSourceParticleListNative()noexcept;
bool constructSourceParticleList27DC90(Entity&,bool& counted,bool& enrolled,std::uint32_t&)noexcept;
bool constructSourceEmitterEntity15A5B0(Entity&,bool&,bool&,std::uint32_t&)noexcept;
bool preflightSourceParticleListCountNative(const Entity&,bool counted,bool enrolled)const noexcept;
bool preflightSourceParticleEntityIdentityNative(const Entity&,bool,bool,std::uint8_t)const noexcept;
bool releaseSourceParticleEmitterCountNative(Entity&,bool&,bool&,std::uint8_t,std::uint32_t)noexcept;
bool releaseSourceParticleListCountNative(Entity&,bool& counted,bool& enrolled)noexcept;

    bool canRetireNativeWorldPrefix(const char** gate=nullptr,std::size_t extraSourceOwnedCount=0)const noexcept;
    bool retireNativeWorldPrefixAfterOwnedCleanup(std::uint32_t&)noexcept;
    //New source Entity custody on the existing canonical portal geometry;
    //not a second portal geometry/visibility collection. Exact source-order
    //ten-Portal prefix, each actual owner retained before its fallible loader.
//Insert immediately before SAME NativePortal230370 declaration.
struct SourcePortal230450State {
    using Words=std::array<std::uint32_t,4>;
    std::optional<std::uint32_t> vtable08;
    std::array<Words,4> vertices30,edges80;
    Words normal70;
    std::uint8_t constructorVertices=0,constructorEdges=0,copiedVertices=0,normalKnown=0;
    std::array<std::uint8_t,4> edgeKnown{};
    std::array<bool,4> edgeCrossStored{},edgeNormalized{};
    bool normalCrossStored=false,normalNormalized=false,normalConstructor=false;
    bool constructorReturned=false,enrolled=false,loadReturned=false;
    bool quadCopied=false,quadReturned=false,diagonalReturned=false;
    std::array<std::optional<std::uint32_t>,24> loaderStack;
    std::array<std::optional<std::uint32_t>,8> reorderStack;
    std::optional<std::uint32_t> quadAreaA,quadAreaB;
    std::array<std::optional<std::uint32_t>,20> diagonalStack,normalStack;
    std::optional<std::uint32_t> connectA,connectB; //Actual zero-based caller copies, parent stores at272BA8/AC.
    std::size_t worldLinksProduced=0;
};

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
//Append inside SAME NativePortal230370; existing fields unchanged.
        bool sourceMode=false;
        SourcePortal230450State source;

    };
    std::array<std::unique_ptr<NativePortal230370>,10> nativePortals_;
    std::size_t nativePortalNext_=0;
    // Genuine plain276590 parameter rows. NOT counted Portal230450 objects.
    struct SourcePortalParameter276590 {
        std::array<std::array<std::uint32_t,4>,4> vertices00;
        std::int32_t connect40,connect44;
        std::uint32_t vtable48;
        // Original4C..4F padding has no source producer/knowledge here.
    };
    struct NativePortalParameterRole3A0 {
        //Explicit native immutable-authoring borrower role, NOT retail
        //270C64 array allocation or destructive270550 reset execution.
        const rr::assets::WorldPortal48F* rows=nullptr;
        std::size_t count=0;
        bool retired=false;
        // Exclusive source-array custody. Above legacy metadata stays empty.
        bool sourceMode=false,sourcePublished=false,sourceReleased=false;
        std::unique_ptr<SourcePortalParameter276590[]> sourceArray;
        const g::OriginalApplicationEntities50LA* sourceApplication=nullptr;
        std::uint64_t sourceGeneration=0;
        std::thread::id sourceThread;
        // Native preadmitted immutable actual App442F80 words, not source fields.
        std::array<std::uint32_t,4> sourceOrigin;
    };
    std::optional<NativePortalParameterRole3A0> nativePortalParameters3A0_;
    std::optional<SourcePortalParameter276590*> sourcePortal3A0_;
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
        //Distinct typed alternatives for the actual Environment SearchNode only.
        g::OriginalVisibilityQuadTree50LA* sourceTree34=nullptr;
        g::OriginalVisibilityQuadTree50LA* sourceTree38=nullptr;
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
//Insert after SearchNode (NativePortalLink27F3B0 and NativePortal230370 complete).
struct SourcePortalAttempt272B3C {
    bool attempted=false,loadAttempted=false,withdrawn=false;
    std::optional<SourcePortal230450State::Words> lastParameterLoad;
    std::uint8_t parameterLoads=0;
    std::uint32_t pc=0;
};
struct SourcePortalWorldLink27F3B0 {
    std::unique_ptr<NativePortalLink27F3B0> pending;
    NativePortalLink27F3B0* published=nullptr;
    const NativePortalLink27F3B0* sourceNext=nullptr;
    SearchNode* from=nullptr;SearchNode* to=nullptr;
    bool attempted=false,withdrawn=false;
    std::optional<std::uint32_t> distance0C,distance10; //Latest actual280C60/64 stores, retained after borrower withdrawal.
    std::uint8_t knownMask=0;
};
std::array<SourcePortalAttempt272B3C,10> sourcePortalFrames_;
std::array<std::array<SourcePortalWorldLink27F3B0,4>,10> sourcePortalWorldLinks_;
//Native lifetime only; COL owns each strong token until all secondary raw
//portal/node pointers and list/pool borrows have been withdrawn. No callback.
struct SourceCollisionPortalBorrow {const void* registration=nullptr;std::weak_ptr<const void> token;};
std::vector<SourceCollisionPortalBorrow> sourceCollisionPortalBorrows_;
struct SourcePhysicsModelPortalBorrow {const void* registration=nullptr;std::weak_ptr<const void> token;};
std::vector<SourcePhysicsModelPortalBorrow> sourcePhysicsModelPortalBorrows_;
bool sourcePhysicsModelPortalBorrowedNative()const noexcept {
    for(const auto& borrow:sourcePhysicsModelPortalBorrows_)if(!borrow.token.expired())return true;
    return false;
}
bool sourceCollisionPortalBorrowedNative()const noexcept {
    for(const auto& borrow:sourceCollisionPortalBorrows_)if(!borrow.token.expired())return true;
    return false;
}

bool sourcePortalCohortAdmitted_=false;
std::uint8_t sourcePortalViewportKnown_=0;
bool reserveSourcePortalCohortNative()noexcept;
bool allocateSourcePortal230370(std::size_t,std::uint32_t&)noexcept;
bool loadSourcePortal230450(std::size_t,std::uint32_t&)noexcept;
bool attachSourcePortal141960(std::size_t,Entity*,std::uint32_t&)noexcept;
bool linkSourcePortalWorld27F3B0(std::size_t,std::size_t,std::uint32_t,std::uint32_t,std::uint32_t&)noexcept;
bool preflightSourcePortalOwnersNative(const Entity*)const noexcept;
bool sourcePortalOwnersCounted(std::size_t&)const noexcept;
bool sourcePortalWorldLinksCurrent(const SearchNode&,bool,std::size_t)const noexcept;
bool preflightSourcePortalContainerNative(const Entity&,bool,bool)const noexcept;
bool withdrawSourcePortalWorldLinksNative(std::uint32_t&)noexcept;
bool retireSourcePortalsNative(Entity*,std::uint32_t&)noexcept;

// Protected base Owner additions beside SearchNode. Same actual node type/backing.
struct SourceSearchNode27F120 {
    std::unique_ptr<SearchNode> owner;
    bool attempted=false,counted=false,enrolled=false,published=false,ordinalKnown=false,withdrawn=false;
    std::uint16_t constructorMask=0;
    std::optional<std::uint32_t> vtable08;
    std::optional<const void*> pointer3C,pointer44,pointerA0; //Constructor NULL history.
    std::optional<std::uint32_t> zoneVisibility44; //Later actual2741C0 raw word, never a guest pointer.
    bool exclusionAttempted=false,exclusionAllocationReturned=false,exclusionReturned=false;
    std::size_t exclusionProduced=0;
};
//Actual fresh272FC0 ownership, separate from the twenty zone method nodes.
struct SourceVisibilityEnvironment272FC0 {
    std::unique_ptr<g::OriginalVisibilityQuadTree50LA> tree;
    SourceSearchNode27F120 node;
    bool attempted=false,outerReturned=false,treePublished=false,activeNullPublished=false;
    bool nodeAllocationReturned=false,nodePublished=false,attachEntered=false,attachReturned=false;
    bool alias34Stored=false,alias34Retained=false,alias38Stored=false,alias38Retained=false;
    bool constructorReleased=false,returned=false,withdrawn=false;
    bool nativeNodeDetached=false,nativeAliasesWithdrawn=false,nativeConstructorReleased=false;
    std::optional<std::uint32_t> attachLock;
    std::optional<std::array<std::uint32_t,4>> bounds;
    std::uint32_t pc=0x272fc0;
};
SourceVisibilityEnvironment272FC0 sourceVisibilityEnvironment_;
bool allocateSourceVisibilityTree272FE4(std::uint32_t&)noexcept;
bool finishSourceVisibilityEnvironment27303C(const std::array<std::uint32_t,4>&,std::uint32_t&)noexcept;
bool preflightSourceVisibilityEnvironmentOwnerNative()const noexcept;
bool preflightSourceVisibilityEnvironmentForestNative(const Entity&,const Entity*&,const Entity*&)const noexcept;
bool retireSourceVisibilityEnvironmentNative(std::uint32_t&)noexcept;
//Protected base additions after complete SearchNode; parent merges only.
struct SourceListMethodAttempt {
    std::unique_ptr<NativeListMethodCustody> owner;
    SearchNode* aliasNode=nullptr;
    bool attempted=false,returned=false,arrayPublished=false,aliasAttempted=false,aliasPublished=false,aliasRetained=false,withdrawn=false;
    std::uint32_t pc=0;
};
std::array<std::array<SourceListMethodAttempt,10>,4> sourceListMethods_;
bool allocateSourceListMethod27E260(std::size_t,std::size_t,std::uint32_t&)noexcept;
bool sourceListMethodReturned(std::size_t,std::size_t,NativeListMethodCustody*&)const noexcept;
bool sourceListMethodArrayCurrent(std::size_t,std::size_t,const NativeListMethodCustody*)const noexcept;
bool aliasSourceListMethod27F420(std::size_t,std::size_t,SearchNode*,std::uint32_t&)noexcept;
bool preflightSourceListMethodsNative()const noexcept;
bool sourceListMethodAliasesCurrent(const SearchNode&,bool,std::size_t)const noexcept;
bool retireSourceListMethodsNative(std::uint32_t&)noexcept;

//Insert after SourceSearchNode27F120 definition, protected base Owner.
struct SourceMethodNodeAttachment141960 {
    SearchNode* identity=nullptr; //Historical after withdrawal; never dereference then.
    bool attempted=false,returned=false,withdrawn=false;
};
std::array<SourceMethodNodeAttachment141960,20> sourceMethodNodeAttachments_;
std::size_t sourceMethodNodeNext_=0;
bool sourceMethodNodesDetaching_=false;
bool attachSourceMethodNode141960(SourceSearchNode27F120&,bool,std::size_t,Entity*,std::uint32_t&)noexcept;
bool sourceMethodNodeAttachmentCurrent(const SearchNode&,bool,std::size_t)const noexcept;
bool preflightSourceMethodNodeForestNative(const Entity&,const Entity*&,const Entity*&,std::uint32_t&)const noexcept;
bool preflightSourceMethodNodeAttachmentsNative(const Entity*)const noexcept;
bool detachSourceMethodNodesNative(Entity*,std::uint32_t&)noexcept;

bool reserveSourceSearchNodesNative()noexcept;
bool constructSourceSearchNode27F120(SourceSearchNode27F120&,std::uint32_t&)noexcept;
bool preflightSourceSearchNodeNative(const SourceSearchNode27F120&,std::size_t,bool collision=false)const noexcept;
bool releaseSourceSearchNodeNative(SourceSearchNode27F120&)noexcept;

    // RA's authenticated ten zones; capacity is not a caller-provided count.
    std::array<SearchNode,10> visibility4C0_,collision4C4_;
    std::size_t nativeZoneExclusionsNext_=0;
    std::optional<std::uint32_t> nativeModelZone354_;
    bool adoptNativeStaticZone2741C0(const ra_cold_sources50LA::Owner&,
        const ra_static_model_cold50LA::Owner*,std::size_t,std::uint32_t&)noexcept;
    bool continueNativeZoneExclusions272A58(const ra_cold_sources50LA::Owner&,std::uint32_t&)noexcept;
    Entity container4BC_;
    //Actual nullable World publications, separate from stable inline native
    //backing.4C0/4C4 are pointer ARRAYS, not SearchNode object arrays.
    std::optional<Entity*> sourceContainer4BC_;
    std::optional<SearchNode**> sourceVisibility4C0_,sourceCollision4C4_;
    std::unique_ptr<SearchNode*[]> sourceVisibilitySlots4C0_,sourceCollisionSlots4C4_;
    std::size_t sourceVisibilityPublished_=0,sourceCollisionPublished_=0;
    bool sourceTopologyWithdrawn_=false;
    bool sourceTopologyMode_=false; //Exclusive actual2729E0 custody, never legacy topology readiness.
    bool constructSourceTopologyContainer141300(Entity&,bool&,bool&,std::uint32_t&)noexcept;
    // Native lifetime pin of the ACTUAL two published row-zero SearchNodes.
    // Raw borrower identity avoids World -> bridge -> World ownership cycles.
    const ra_source_camera50LA::OwnershipBridge* nativeWreckZoneBorrower_=nullptr;
    const SearchNode* nativeWreckVisibilityNode_=nullptr;
    const SearchNode* nativeWreckCollisionNode_=nullptr;
    bool retainNativeWreckFirstZones(ra_source_camera50LA::OwnershipBridge*,std::uint64_t,
        g::OriginalWreckSetupZones50LA&,g::CollectionRef50BX&,g::CollectionRef50BX&)noexcept;
    bool releaseNativeWreckFirstZones(ra_source_camera50LA::OwnershipBridge*)noexcept;
    bool retainNativeWreckParticles(ra_source_camera50LA::OwnershipBridge*,
        g::OriginalTrackParticleCold50LA*)noexcept;
    //Fresh270550 only; core constructor invokes these individually in order.
    bool initializeTopology4BC270600()noexcept {
        if(sourceTopologyMode_||topologyAttempted_||sourceContainer4BC_||container4BC_.reference00)return false;
        sourceContainer4BC_=nullptr;return true;
    }
    bool initializeTopology4C0270604()noexcept {
        if(sourceTopologyMode_||topologyAttempted_||sourceVisibility4C0_||sourceVisibilitySlots4C0_)return false;
        sourceVisibility4C0_=nullptr;return true;
    }
    bool initializeTopology4C4270608()noexcept {
        if(sourceTopologyMode_||topologyAttempted_||sourceCollision4C4_||sourceCollisionSlots4C4_)return false;
        sourceCollision4C4_=nullptr;return true;
    }
    bool sourceTopologyCurrent(bool complete)const noexcept {
        if(sourceTopologyMode_||sourceTopologyWithdrawn_||!sourceContainer4BC_||!sourceVisibility4C0_||!sourceCollision4C4_||
           *sourceContainer4BC_!=(container4BC_.reference00?&container4BC_:nullptr)||
           *sourceVisibility4C0_!=sourceVisibilitySlots4C0_.get()||
           *sourceCollision4C4_!=sourceCollisionSlots4C4_.get()||
           sourceVisibilityPublished_>10u||sourceCollisionPublished_>sourceVisibilityPublished_)return false;
        for(std::size_t z=0;z<sourceVisibilityPublished_;++z)
            if(!sourceVisibilitySlots4C0_||sourceVisibilitySlots4C0_[z]!=&visibility4C0_[z]||!visibility4C0_[z].constructed)return false;
        for(std::size_t z=0;z<sourceCollisionPublished_;++z)
            if(!sourceCollisionSlots4C4_||sourceCollisionSlots4C4_[z]!=&collision4C4_[z]||!collision4C4_[z].constructed)return false;
        return !complete||(sourceVisibilityPublished_==10u&&sourceCollisionPublished_==10u&&
            *sourceContainer4BC_==&container4BC_);
    }
    void withdrawSourceTopologyAliases()noexcept {
        if(sourceTopologyMode_)return; //Dedicated source abort owns unknown withdrawal, never legacy NULL.
        //Native cleanup revocation BEFORE any source object payload retires.
        //Not a retail26FD80 completion or loaded-world reset.
        sourceContainer4BC_=nullptr;sourceVisibility4C0_=nullptr;sourceCollision4C4_=nullptr;
        sourceTopologyWithdrawn_=true;
    }
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
    // SAME source observer custody: the derived aggregate still constructs and
    // retires it; scene flare leaves consume its actual lease and identity.
    struct SourceDefaultObserver280890 final {
        const void* next00=nullptr;
        const g::OriginalWreckComponent50LA* child04=nullptr;
        const g::OriginalVisibilityQuadTree50LA* method08=nullptr;
        std::uint32_t field0C=0,field10=0,field14=0;
        std::array<std::optional<std::uint32_t>,2> unknown18To1C;
        std::array<std::uint32_t,4> vector20{};
        std::uint8_t scalarKnownMask=0;
        bool vectorKnown=false;
    };
    std::unique_ptr<SourceDefaultObserver280890> sourceDefaultObserver_;
    enum class SourceDefaultObserverPhase {unreached,entered,bounded,returned,nativeClosed};
    SourceDefaultObserverPhase sourceDefaultObserverPhase_=SourceDefaultObserverPhase::unreached;
    std::uint32_t sourceDefaultObserverPC_=0x272f28;
    bool sourceDefaultObserverOuterReached_=false,sourceDefaultObserverLease_=false;
    bool sourceDefaultObserverCtorReturned_=false,sourceDefaultObserverPublished_=false;
    bool sourceDefaultObserverChildStored_=false,sourceDefaultObserverMethodStored_=false;
    std::optional<const g::OriginalVisibilityQuadTree50LA*> sourceDefaultObserverMethodRead_;
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
    // Dedicated Model-base membership; child04 remains the actual Node Entity.
    struct SourceModelObserverRecord {
        const void* next00=nullptr;const Entity* child04=nullptr;
        NativeListMethodCustody* method08=nullptr;
        std::uint32_t field0C=0,field10=0,slot14=0;
        std::array<std::optional<std::uint32_t>,2> unknown18To1C;
        std::array<std::uint32_t,4> center20{};
        const NativePortalLink27F3B0* portalCursor=nullptr;
        NativeListMethodCustody* cursorMethod=nullptr;
    };
    struct SourceModelObserverFrame {
        const void* occurrence=nullptr;const void* wrapper=nullptr;
        g::OriginalRacerModelSceneOwner50LA* scene=nullptr;
        const g::OriginalRacerSceneObject50LA* member=nullptr;
        std::uint32_t zone=0,pc=0;
        SearchNode* node=nullptr;NativeListMethodCustody* method=nullptr;
        std::unique_ptr<SourceModelObserverRecord> owner;
        bool entered=false,outerReturned=false,lease=false,vectorKnown=false,constructorReturned=false,published=false;
        std::uint8_t scalarMask=0,primaryMask=0;
        bool boundsEntered=false,boundsReturned=false,selectionPending=false,inserted=false,primaryWithdrawn=false;
        std::optional<std::uint32_t> slot;
        std::array<float,4> center{},half{};
        std::array<std::uint32_t,4> centerWords{},halfWords{};
        g::OriginalGarageObserverShared50KG::SourceAssertionHistory allocationAssertion,childAssertion,methodAssertion,slotAssertion,returnAssertion;
        struct Secondary {
            bool entered=false,complete=false;std::size_t edgeIndex=0;
            NativePortalLink27F3B0* edge=nullptr;const Entity* neighbor=nullptr;const Entity* portal=nullptr;
            NativeListMethodCustody* method=nullptr;
            g::source_portal_overlap231150::History overlap;
            std::uint8_t edgeDistanceMask=0,linkStoreMask=0;
            bool methodReturned=false,selectionAttempted=false,selectionPending=false,inserted=false,nativeWithdrawn=false;
            std::optional<std::uint32_t> slot;
            bool allocationAttempted=false,allocationReturned=false,headPublished=false,linkReleased=false;
            g::OriginalGarageObserverShared50KG::Link105720* link=nullptr;
            const void* oldHead=nullptr;
        };
        std::array<Secondary,20> secondary;std::size_t secondaryCount=0;
        bool secondaryEntered=false,secondaryReturned=false,secondaryWithdrawn=false;
        bool returned=false,withdrawn=false;
    };
    std::vector<std::unique_ptr<SourceModelObserverFrame>> sourceModelObservers_;

    struct NativeLocalFlareObserver {
        const void* next00=nullptr;const Entity* child04=nullptr;
        NativeListMethodCustody* method08=nullptr;
        std::uint32_t field0C=0,field10=0,slot14=0;
        std::array<std::optional<std::uint32_t>,2> unknown18To1C;
        std::array<std::uint32_t,4> center20{{0,0,0,0x3f800000u}};
        bool sharedConstructed=false,primaryRegistered=false,portalLoopComplete=false;
        const NativePortalLink27F3B0* portalCursor=nullptr;
        bool portalLoopStarted=false;
        NativeListMethodCustody* cursorMethod=nullptr;
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
struct SourceLightDescriptor277220 {
    std::uint32_t type00;
    std::array<std::uint32_t,4> color04;
    std::unique_ptr<std::array<std::uint32_t,4>> position14,look18,attenuation1C;
    std::int32_t zone20;std::uint32_t range24;
    std::uint8_t flags28=0,knownFlags28=0; //Bit7 never inferred.
    std::optional<std::uint32_t> inner2C,outer30;
    std::array<std::uint8_t,5> lensLayers34;
    std::array<std::uint32_t,4> lensColor3C;
    std::uint32_t lensBrightness4C;
    std::array<char,64> lensTexture50;
    std::size_t lensTextureKnown=0;
    std::array<std::uint32_t,4> flareColor90;
    std::uint32_t flareSizeA0;
    std::array<char,64> flareTextureA4;
    std::size_t flareTextureKnown=0;
    std::int32_t flareTurningE4,flashPatternE8;
    std::uint32_t flashPeriodEC,flashFilterF0,vtableF4;
    bool constructed=false,loaded=false;
};

    struct NativeLightWrapper277C10 {
        Entity* live00=nullptr; //277D80/146720 still unjoined.
        std::optional<g::original_gc_gameplay_lighting_candidate::RaAuthoredLightIdentity> descriptor04;
        std::unique_ptr<SourceLightDescriptor277220> sourceDescriptor04;
        bool sourceConstructed=false,sourceDescriptorPublished=false;
    };
    struct NativeLightList3A8 {
        bool sourceMode=false,sourceConstructorReturned=false,sourcePublished=false;
        bool sourceReferenceReleased=false,sourcePayloadReleased=false,sourceEntityRetired=false;
        bool sourceZoneAllocationReturned=false,sourceArrayAllocationReturned=false;
        std::size_t sourceWrappersConstructed=0;
        std::uint32_t sourcePC=0;
        const std::thread::id sourceThread=std::this_thread::get_id();
        bool sourceAuthorityEnrolled=false;

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
    //Separate source custody, never adopted legacy nativeAmbientLight_.
    std::unique_ptr<NativeRaLight146720> sourceFirstLight_;
    bool sourceFirstLightOuterReached_=false,sourceFirstLightCounted_=false,sourceFirstLightEnrolled_=false;
    bool sourceFirstLightDefaults_=false,sourceFirstLightReturned_=false,sourceFirstLightPublished_=false;
    bool sourceFirstLightNativeDetached_=false;
    bool attachSourceFirstLight141960(std::uint32_t&)noexcept;
    bool detachSourceFirstLightNative(std::uint32_t&)noexcept;
    std::optional<std::uint32_t> sourceFirstLightVtable08_;
    bool preflightSourceFirstLightNative()const noexcept;
    bool constructSourceFirstLight146720(std::uint32_t&)noexcept;
    bool sourceFirstLightLuminance146E60(std::uint32_t&)const noexcept;
    bool abortSourceFirstLightNative()noexcept;
    std::unique_ptr<NativeRaLight146720> sourceSecondLight_;
    bool sourceSecondLightOuterReached_=false,sourceSecondLightCounted_=false,sourceSecondLightEnrolled_=false;
    bool sourceSecondLightDefaults_=false,sourceSecondLightReturned_=false,sourceSecondLightPublished_=false;
    std::optional<std::uint32_t> sourceSecondLightVtable08_;
    std::optional<std::uint32_t> sourceSecondLuminance_;
    std::optional<std::array<std::uint32_t,4>> sourceSecondPositionWords_,sourceSecondLookInputWords_,sourceSecondNormalizedWords_;
    bool sourceSecondPositionCompared_=false,sourceSecondLookCompared_=false;
    std::optional<std::array<std::uint32_t,4>> sourceSecondAttenuationWords_;
    std::optional<std::uint32_t> sourceSecondAttenuationRange_;
    bool sourceSecondConfigReturned_=false;
    std::uint8_t sourceSecondDescriptorFreeMask_=0; //Ordered14/18/1C/outer free receipts; retained after native close.
    bool sourceSecondLightNativeDetached_=false;
    //Reusable per-source-descriptor type1 invocation; never a nativePoints_ or
    //decoded-input projection. Only the actual next ordinal is currently joined.
    struct SourceTypedLightFrame {
        std::size_t ordinal=0;std::uint32_t pc=0x273ba8;
        NativeLightWrapper277C10* wrapper=nullptr;SourceLightDescriptor277220* descriptor=nullptr;
        struct DescriptorHistory {
            std::uint32_t type=0;std::int32_t zone=0;std::uint8_t flags=0,knownFlags=0;bool loaded=false;
            std::optional<std::array<std::uint32_t,4>> position,look,attenuation;
        };
        std::optional<DescriptorHistory> retiredDescriptor; //Native proof values, never a retained guest allocation.
        bool retirementEntered=false,descriptorDisposed=false,wrapperReturned=false;
        bool populationIncremented=false;
        std::uint8_t descriptorFreeMask=0;
        std::optional<std::uint32_t> wrapperV0,attachLock;
        bool attachCallReturned=false,nativeDetached=false;
        std::uint32_t descriptorType()const noexcept{return descriptor?descriptor->type00:retiredDescriptor?retiredDescriptor->type:0;}
        std::int32_t descriptorZone()const noexcept{return descriptor?descriptor->zone20:retiredDescriptor?retiredDescriptor->zone:0;}
        std::uint8_t descriptorFlags()const noexcept{return descriptor?descriptor->flags28:retiredDescriptor?retiredDescriptor->flags:0;}
        bool descriptorLoaded()const noexcept{return descriptor?descriptor->loaded:retiredDescriptor&&retiredDescriptor->loaded;}
        const std::array<std::uint32_t,4>* descriptorPosition()const noexcept{return descriptor?descriptor->position14.get():
            retiredDescriptor&&retiredDescriptor->position?&*retiredDescriptor->position:nullptr;}
        const std::array<std::uint32_t,4>* descriptorLook()const noexcept{return descriptor?descriptor->look18.get():
            retiredDescriptor&&retiredDescriptor->look?&*retiredDescriptor->look:nullptr;}
        const std::array<std::uint32_t,4>* descriptorAttenuation()const noexcept{return descriptor?descriptor->attenuation1C.get():
            retiredDescriptor&&retiredDescriptor->attenuation?&*retiredDescriptor->attenuation:nullptr;}
        std::unique_ptr<NativeRaLight146720> light;
        bool outerReached=false,counted=false,enrolled=false,defaults=false,returned=false,published=false;
        bool positionCompared=false,lookCompared=false,configured=false,nativeClosed=false;
        //Private conditional type3 configuration. No ordinal or loop admission.
        bool spotlightConfigEntered=false;
        bool laterAmbientConfigEntered=false,laterAmbientStoresReturned=false;
        std::optional<std::array<float,4>> spotlightHalfColor;
        std::optional<std::array<std::uint32_t,4>> spotlightLookWords;
        std::optional<std::uint32_t> spotlightCone98,spotlightCone9C;
        std::optional<std::uint32_t> vtable08,luminance,attenuationRange;
        std::optional<std::array<std::uint32_t,4>> positionWords,attenuationWords;
        std::uint8_t guard=0,enabled=0,guardMask=0;bool assertionPassed=false;
        std::optional<std::int32_t> zone,ordinalStack08;
        const char* lensStack00=nullptr;bool argumentsBorrowed=false;
        //Suspended1474A0/2809C0 custody; original observer fields remain in light.observerOwner.
        bool observerEntered=false,observerOuterReached=false,observerLease=false,observerReturned=false;
        bool observerVectorKnown=false,observerD0Published=false,observerD4Published=false,observerWithdrawn=false;
        std::uint8_t observerScalarMask=0,primaryStoreMask=0;
        g::OriginalGarageObserverShared50KG::SourceAssertionHistory observerD0Assertion,observerChildAssertion,
            observerMethodAssertion,observerSlotAssertion,observerReturnAssertion;
        rr::platform::OriginalUiLightManagerOwner50KG* observerManager=nullptr; //Borrowed; retained by actual source World.
        const Entity* primaryNode=nullptr;
        rr::platform::OriginalUiSearchListCold50KG* primaryList=nullptr;
        bool primaryMethodReturned=false,primarySelectionAttempted=false,primarySelectionPending=false,primaryInserted=false,primaryNativeWithdrawn=false;
        std::optional<std::uint32_t> primarySlot;
        struct SecondaryStep {
            bool entered=false,complete=false;std::size_t edgeIndex=0;
            const void* edge=nullptr;const Entity* neighbor=nullptr;const Entity* portal=nullptr;
            rr::platform::OriginalUiSearchListCold50KG* method=nullptr;
            g::source_portal_overlap231150::History overlap;
            std::uint8_t edgeDistanceMask=0,linkStoreMask=0;
            bool methodReturned=false,selectionAttempted=false,selectionPending=false,inserted=false,nativeWithdrawn=false;
            std::optional<std::uint32_t> slot;
            bool allocationAttempted=false,allocationReturned=false,headPublished=false,linkReleased=false;
            g::OriginalGarageObserverShared50KG::Link105720* link=nullptr;
            g::OriginalGarageObserverShared50KG::Link105720* oldHead=nullptr;
        };
        std::array<SecondaryStep,20> secondary;
        std::size_t secondaryCount=0;
        bool secondaryEntered=false,secondaryReturned=false,secondaryNativeWithdrawn=false;
        bool observerCallReturned=false,registrationEntered=false,registryA0Stored=false;
        std::optional<std::uint32_t> registryIndex; //Actual source count94 at148F30 append, not a List slot.
        bool registryGrowthEntered=false,registryGrowthReturned=false;
        std::uint8_t registryColorMask=0;std::array<std::optional<std::uint32_t>,4> registryColorWords;
        rr::platform::OriginalUiLightManagerOwner50KG* registryManager=nullptr; //Live registry custody, independent of observer ownership.
        const Entity* registryOwner=nullptr; //Historical identity after native withdrawal; never dereference.
        bool registrySlotPublished=false,registryCountStored=false,registrationReturned=false,registryWithdrawn=false;
        bool flareFlagRead=false,flareSelected=false;
        //Actual caller stack/constructor custody, separate from nativePoints_.
        std::array<std::optional<std::uint32_t>,4> flareConversions; //A,R,G,B order.
        std::optional<std::uint32_t> flarePackedStack6C;
        bool flareAllocationReached=false,flareAllocationReturned=false,flareCounted=false,flareEnrolled=false;
        std::unique_ptr<NativeLocalFlare> sourceFlare;
        std::optional<std::uint32_t> flareVtable08;
        std::uint8_t flareVectorMask=0;bool flareBaseReturned=false,flareCtorReturned=false,flareArgumentsBorrowed=false,flareNativeWithdrawn=false;
        const std::array<std::uint32_t,4>* flarePositionBorrow=nullptr;
        const char* flareTextureBorrow=nullptr;
        std::optional<std::uint32_t> flareSizeArgument,flareTurningArgument;
        std::optional<std::uint32_t> flareCountStack7C;
        bool flare141C20Returned=false;std::uint8_t flareInputMask=0;
        bool flareStripAllocationReached=false,flareStripCounted=false,flareStripEnrolled=false;
        bool flareStripPublished=false,flareStripCtorReturned=false,flareStripNativeWithdrawn=false;
        std::optional<std::uint32_t> flareStripVtable08;
        std::uint8_t flareStripCtorMask=0;
        bool flareMaterialAdmitted=false,flareSetupEntered=false,flareGroupsAttempted=false,flareGroupsPublished=false;
        bool flareGroupWordStored=false,flarePrimitiveAttempted=false,flarePrimitivePublished=false,flarePrimitiveCtorReturned=false;
        bool flareMaterialEntered=false,flareMaterialReturned=false,flareSetupReturned=false,flareResourcesWithdrawn=false;
        std::uint8_t flareSetupMask=0;std::string flareMaterialName;
        std::optional<std::uint32_t> flarePrimitiveVtable08;
        bool flareAttributesEntered=false,flareAttributeGroupAttempted=false,flareAttributeGroupPublished=false;
        std::uint16_t flareAttributeDescriptorMask=0;std::optional<std::int32_t> flareAttributeDescriptor10;
        bool flareCommonAttempted=false,flareCommonPublished=false,flareCommonZeroed=false,flareCommonReturned=false;
        std::uint16_t flareCommon40Mask=0,flareCommonE0Mask=0;std::uint8_t flareCommon90Mask=0,flareCommon130Mask=0;
        std::optional<std::uint32_t> flareAttributeCount5C;
        bool flareAttributeCountsAttempted=false,flareAttributeCountsPublished=false,flareAttributeCountStored=false;
        std::unique_ptr<NativeSunFlareParameters> flareParameters54;
        bool flareParameterAttempted=false,flareParameterPublished=false,flareParameterReturned134000=false;
        std::uint64_t flareParameterHistorySerial=0;bool flareParameterHistoryWithdrawn=false;
        bool flareAttributesWithdrawn=false;
        SearchNode* flareVisibilityNode=nullptr;NativeListMethodCustody* flareVisibilityMethod=nullptr;
        bool flareObserverEntered=false,flareObserverOuter=false,flareObserverLease=false,flareObserverReturned=false;
        bool flareObserverPublished=false,flareObserverWithdrawn=false,flareObserverVectorKnown=false;
        std::uint8_t flareObserverScalarMask=0,flarePrimaryStoreMask=0;
        g::OriginalGarageObserverShared50KG::SourceAssertionHistory flareObserverEntryAssertion,flareObserverChildAssertion,
            flareObserverMethodAssertion,flareObserverSlotAssertion;
        bool flareSelectionPending=false,flarePrimaryInserted=false,flarePrimaryWithdrawn=false;
        std::optional<std::uint32_t> flarePrimarySlot;
        struct FlareSecondaryStep {
            bool entered=false,complete=false;std::size_t edgeIndex=0;
            NativePortalLink27F3B0* edge=nullptr;const Entity* neighbor=nullptr;const Entity* portal=nullptr;
            NativeListMethodCustody* method=nullptr;
            g::source_portal_overlap231150::History overlap;
            std::uint8_t edgeDistanceMask=0,linkStoreMask=0;
            bool methodReturned=false,selectionAttempted=false,selectionPending=false,inserted=false,nativeWithdrawn=false;
            std::optional<std::uint32_t> slot;
            bool allocationAttempted=false,allocationReturned=false,headPublished=false,linkReleased=false;
            g::OriginalGarageObserverShared50KG::Link105720* link=nullptr;
            const void* oldHead=nullptr;
        };
        std::array<FlareSecondaryStep,20> flareSecondary;
        std::size_t flareSecondaryCount=0;
        bool flareSecondaryEntered=false,flareSecondaryReturned=false,flareSecondaryWithdrawn=false;
        g::OriginalGarageObserverShared50KG::SourceAssertionHistory flareObserverReturnAssertion;
        bool flareObserverCallReturned=false,flare146AE0Entered=false,flare146AE0Returned=false;
        bool flareC4Published=false,flareC4Withdrawn=false,flareLightAttachReturned=false,flareLightAttachmentWithdrawn=false;
        std::optional<std::uint32_t> flareLightAttachLock;

        std::uint8_t flareVectorCalls=0;std::array<std::uint8_t,8> flareVectorStoreMask{};
        std::array<std::array<std::optional<std::uint32_t>,3>,8> flareVectorReadWords;
        std::array<std::array<g::OriginalStripStreamHistory23CCA0::Receipt,3>,8> flareVectorAssertions;
        bool flareReturned14AB30=false;
        bool flareStripAttachReturned=false,flareStripAttachmentWithdrawn=false;
        std::optional<std::uint32_t> flareStripAttachLock;
        std::uint8_t flareUVCalls=0;std::array<std::uint8_t,4> flareUVStoreMask{};
        std::array<std::array<g::OriginalStripStreamHistory23CCA0::Receipt,3>,4> flareUVAssertions;
        bool flareHeaderEntered=false,flareHeaderReturned=false,flareAttributesReturned=false;
        std::uint8_t flareHeaderStoreMask=0,flareHeaderPaddingWords=0,flarePrimitiveStoreMask=0;
        NativeSunFlareParameters* flarePrimitiveParameters10=nullptr;
        bool flarePrimitiveParametersWithdrawn=false;
        bool flarePayloadEntered=false,flarePayloadNativeBacking=false,flareStreamReturned=false;
        std::array<std::uint32_t,4> flareStackTagD0{};bool flareStackTagKnown=false;
        std::array<std::optional<std::uint32_t>,4> flarePayloadOffsets;
        std::array<std::optional<std::uint32_t>,128> flarePayloadWords;
        std::array<std::uint8_t,4> flarePayloadCopies{};std::uint8_t flarePayloadFinished=0;
        std::uint32_t flarePayloadCount=0;
        std::optional<std::uint32_t> flareDescriptorParameterEnd,flareDescriptorParameterSpan;
        bool flareStreamEntered=false,flareStreamCountStored=false;std::uint8_t flareStreamDescriptorMask=0;
        std::uint8_t flareStreamArraysAttempted=0,flareStreamArraysPublished=0;
        bool flareAliasAllocationAttempted=false,flareAliasPublished=false,flareBatchCountStored=false;
        std::uint8_t flareAliasConstructed=0;std::array<std::uint8_t,4> flareAliasStoreMask{};
        std::optional<std::uint32_t> flareDescriptorParameterBegin;
        g::OriginalStripStreamHistory23CCA0::Receipt flareBatchAssertion;
        std::array<g::OriginalStripStreamHistory23CCA0::Receipt,4> flareAliasAssertions;


    };
    std::vector<std::unique_ptr<SourceTypedLightFrame>> sourceTypedLights_;
    bool sourceListEntriesCurrent(const NativeListMethodCustody&)const noexcept;
    bool preflightSourceFlareObserverNative(const SourceTypedLightFrame&)const noexcept;
    bool withdrawSourceFlareObserverNative(SourceTypedLightFrame&)noexcept;
    bool continueSourceFlareObserver14BA00(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool preflightSourceFlareSecondaryNative(const SourceTypedLightFrame&)const noexcept;
    bool withdrawSourceFlareSecondaryNative(SourceTypedLightFrame&)noexcept;
    bool continueSourceFlareSecondary280BF0(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool finishSourceFlare14BA00And146AE0(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool selectSourceObserverSlot27E3C0(NativeListMethodCustody&,const void*,const void*,std::optional<std::uint32_t>&,bool&,std::uint32_t&,std::uint32_t expectedUsage=1)noexcept;

    bool continueSourceTypedFlareVectors14AC74(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool continueSourceTypedFlareUV14ABD8(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool continueSourceTypedFlareHeader23BFBC(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool continueSourceTypedFlarePayload23D088(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool continueSourceTypedFlareStreams23CCA0(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool continueSourceTypedFlareAttributes23BED8(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool continueSourceTypedFlare14AB30(SourceTypedLightFrame&,std::uint32_t&)noexcept;
    bool continueSourceTypedFlare150450(SourceTypedLightFrame&,
        const std::shared_ptr<rr::assets::OriginalStandardMaterialTable50LA>&,const std::string&,std::uint32_t&)noexcept;
    bool preflightSourceTypedLightsNative()const noexcept;
    bool preflightSourceLightSiblingLinksNative()const noexcept;
    bool abortSourceTypedLightsNative()noexcept;
    static std::uint32_t sourceTypedRange146898()noexcept {
        return g::SourceUiLightMath50KG::range146898();
    }
    bool sourceTypedLuminance146E60(const SourceTypedLightFrame&,std::uint32_t&)const noexcept;
    bool sourceTypedAttenuation147268(const SourceTypedLightFrame&,std::uint32_t&)const noexcept;
    bool attachSourceSecondLight141960(std::uint32_t&)noexcept;
    bool detachSourceSecondLightNative(std::uint32_t&)noexcept;
    bool sourceSecondAttenuation147268(std::uint32_t&)const noexcept;
    bool sourceSecondLightLuminance146E60(std::uint32_t&)const noexcept;
    bool preflightSourceSecondLightNative()const noexcept;
    bool constructSourceSecondLight146720(std::uint32_t&)noexcept;
    bool abortSourceSecondLightNative()noexcept;

    std::optional<NativeLightList3A8*> sourceLight3A8_; // Unknown until actual27059C.
    bool allocateSourceLightList270AA8(std::uint32_t&)noexcept;
    bool preflightSourceLightListNative()const noexcept;
    bool abortSourceLightListNative(std::uint32_t&)noexcept;
    bool retireSourceLightList142590(std::uint32_t&)noexcept;

    bool adoptNativeLightWrappers273AA0(const ra_cold_sources50LA::Owner&,std::uint32_t&)noexcept;
    bool canRetireNativeLightWrappers()const noexcept {
        if(!nativeLightList3A8_)return true;
        if(nativeLightList3A8_->sourceMode)return false;
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
        if(!sourceTopologyCurrent(true)||!lighting_||!runtimeQueries_||!sceneApplication_||!sceneApplication_->application10()||
           runtimeQueries_->sceneOwner_!=this||!runtimeQueries_->storage_.get())return false;
        const auto& s=*runtimeQueries_->storage_.get();
        return s.zones.data()==collisionRows_&&s.zones.size()==10u&&
            s.lifecycle.storage.views.zones==collisionRows_;
    }
    bool constructTopology(std::shared_ptr<g::OriginalApplicationEntities50LA> application,
        std::uint64_t generation,std::uint32_t& pc)noexcept {
        pc=0;
        if(sourceTopologyMode_||topologyAttempted_||topologyClosed_||!worldReset270550Complete_)return fail(pc,0x2729e0);
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
        //Existing native topology replaces original Boundary272970 and
        //ZoneInfo270B78 producers. Their source pointers are not represented;
        //withdraw constructor-null knowledge, never substitute loaded null.
        worldBoundary348_.reset();worldZoneInfo350_.reset();
        worldRole4B8_.reset();for(auto& role:worldRoles4DCTo4EC_)role.reset();
        world390KnownMask_=0;world4F0KnownMask_=0; //Unjoined native LOAD writes are not known unchanged.
        if(!lighting_->publishNativeEnvironment3A4(*this,source,pc))return false;
        auto construct=[&](Entity& entity,g::OriginalWreckComponentKind50LA kind)->bool {
            const auto result=g::initializeOriginalWreckComponent50LA(entity,kind,generation,key(&entity),true,
                sceneApplication_->objectCount423F60());
            if(result.status!=g::OriginalWreckComponentStatus50LA::complete)return fail(pc,result.sourcePC);
            domain_[domainCount_++]=&entity;return true;
        };
        if(!construct(container4BC_,g::OriginalWreckComponentKind50LA::entity141300))return false;
        sourceContainer4BC_=&container4BC_; //272A1C after actual141300.
        //Two real native pointer-array allocations/publications, NOT aliases
        //to object arrays. Failure of the first does not skip the second call.
        try{sourceVisibilitySlots4C0_.reset(new SearchNode*[10]);}catch(...){sourceVisibilitySlots4C0_.reset();}
        sourceVisibility4C0_=sourceVisibilitySlots4C0_.get(); //272A2C.
        try{sourceCollisionSlots4C4_.reset(new SearchNode*[10]);}catch(...){sourceCollisionSlots4C4_.reset();}
        sourceCollision4C4_=sourceCollisionSlots4C4_.get(); //272A3C.
        std::size_t count=0;
        visibilityRows_=lighting_->retainSceneZones272A78(this,count);
        if(!visibilityRows_||count!=10u)return fail(pc,0x272a48);
        collisionRows_=collision.zones.data();runtimeQueries_->sceneOwner_=this;
        //272A58..272AE4: visibility constructor, collision constructor, then
        //both ordinal stores, in authored order. No existing row is rewritten.
        for(std::size_t z=0;z<10u;++z){
            auto& v=visibility4C0_[z];auto& c=collision4C4_[z];
            if(!construct(v.entity,g::OriginalWreckComponentKind50LA::searchNode27F120))return false;
            v.constructed=true;v.collectionBacking=&visibilityRows_[z];
            if(!sourceVisibilitySlots4C0_)return fail(pc,0x272a8c);
            sourceVisibilitySlots4C0_[z]=&v;++sourceVisibilityPublished_; //272A8C delay store.
            if(!construct(c.entity,g::OriginalWreckComponentKind50LA::searchNode27F120))return false;
            c.constructed=true;c.collectionBacking=&collisionRows_[z];
            if(!sourceCollisionSlots4C4_)return fail(pc,0x272ac0);
            sourceCollisionSlots4C4_[z]=&c;++sourceCollisionPublished_; //272AC0.
            v.ordinal48=static_cast<std::uint32_t>(z); //272AD4, AFTER both publications.
            c.ordinal48=static_cast<std::uint32_t>(z); //272AE4.
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
        if(sourceTopologyMode_)return sourceTopologyWithdrawn_;
        if(nativeWreckZoneBorrower_||constructorRacerWorldBorrows_)return false;
        if(topologyClosed_||!topologyAttempted_)return true;
        if(!sourceTopologyWithdrawn_){if(!sourceTopologyCurrent(false))return false;}
        else if(!sourceContainer4BC_||*sourceContainer4BC_||!sourceVisibility4C0_||*sourceVisibility4C0_||
            !sourceCollision4C4_||*sourceCollision4C4_)return false;
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
        if(sourceTopologyMode_)return sourceTopologyWithdrawn_; //No legacy arrays/history rewrite.
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
        withdrawSourceTopologyAliases();
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
        //No consumer can reach these arrays after root publication revocation;
        //release pointer metadata only after existing backing payload cleanup.
        sourceVisibilitySlots4C0_.reset();sourceCollisionSlots4C4_.reset();
        sourceVisibilityPublished_=sourceCollisionPublished_=0;
        container4BC_.descendants24=0;
        if(runtimeQueries_&&runtimeQueries_->sceneOwner_==this)runtimeQueries_->sceneOwner_=nullptr;
        if(lighting_&&!lighting_->withdrawNativeEnvironment3A4(*this))return false;
        visibilityRows_=collisionRows_=nullptr;sceneApplication_.reset();topologyClosed_=true;return true;
    }
protected:
    Owner()=default;
    ~Owner(){if(worldColdConstructed_||(topologyAttempted_&&!topologyClosed_))std::terminate();}
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
};
}
