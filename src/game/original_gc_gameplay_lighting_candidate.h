#pragma once
// Limited native selected-light policy, already joined to gameplay.
// No original cache or camera parity claim. MA/SA additions are source-only
// and have not been built or tested.
#include "vehicle_frame49w.h"
#include "original_zone_scissor50la.h"
#include "camera_view49f.h"
#include "live_zone_membership50en.h"
#include "reset_visibility50ey.h"
#include "original_racer_material50km.h"
#include "native_gc_selected_lights_candidate.h"
#include "../assets/import_world48f.h"
#include <array>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace rr::assets { class OriginalStandardMaterialTable50LA; }
namespace rr::platform { class VehicleWindowGl50DS; struct VehicleWindowDiagnostic50DS; }
namespace rr::game { struct OriginalWreckComponent50LA; struct OriginalCameraViewportFields; struct OriginalCameraSceneOwner; struct OriginalRendererDispatchFields; }
namespace ra_source_camera50LA { class OwnershipBridge; }
namespace rr::game::original_world_environment50KG {struct SpatialBounds;class Owner;}
namespace ra_pod_scene50LA { class Owner; }
namespace mb_source_trigger50MB { class Owner; }
namespace tc_cold_visibility50TC { class Retirement; }
namespace tb_cold_visibility50TB { class Retirement; }
namespace gb_cold_visibility50GB { class Retirement; }
namespace mc_cold_visibility50MC { class Retirement; }
namespace track_animation_binding50AN { class Binding; }
namespace mc_cold_sources50LA { class Owner; }
namespace mc_lighting_authoring50MC { struct Record; }
namespace original_cable_selection50LA { class Binding; }
namespace ra_static_lighting50LA { class Binding; }
namespace ra_static_model_cold50LA { class Owner; }
namespace rr::game { class OriginalNativeListMethodCustody50LA; class OriginalApplicationEntities50LA; class OriginalRacerEntityCold50LA; class OriginalRacerModelSceneOwner50LA; }
namespace rr::game::original_gc_gameplay_lighting_candidate {
constexpr std::uint32_t maximumRacers=8, gcZones=12, gcPortals=13,
    gaPortals=18, maZones=9, maPortals=10, saZones=25, saPortals=25,
    maximumZones=saZones, maximumPortals=saPortals;
enum class Status : std::uint8_t {
    complete, unprepared, alreadyPrepared, sourceIdentity, sourceShape,
    sourceFields, unsupportedScene, phase, identity, bounds, registration,
    activeDraw, stalePublication, unsupportedPolicy, arithmetic, allocation,
    missingResetOwner
};
struct Diagnostic {
    Status status=Status::complete;
    std::uint32_t sourcePC=0, racer=0;
    LiveZoneResult50EN membership{};
    ResetVisibilityResult50EY resetVisibility{};
    assets::DecodeError48C decode{};
};
//Private producer APIs alone fill these immutable RA light identities. This
//is not a renderer Light or an authority/registration capability.
struct RaDirectionalLightInputs {
    std::array<float,4> positionA0{},lookB0{};
    std::array<float,3> look78{},attenuation8C{};
    float range84=0;
};
//Read-only source initializer inputs, NOT a renderer Light or a readiness token.
//Ambient uses colors/type/zone only; pose/cones are not constructor-write authority.
struct RaRemainingLightInputs {
    RaDirectionalLightInputs pose;
    std::array<float,4> diffuse{},specular{},ambient{};
    std::uint32_t type=0,zone=0,cone98=0,cone9C=0;
    bool lookRevision=false;
};
struct RaAuthoredLightIdentity {
    const assets::ParameterBlock48F* retainedSource=nullptr;
    const void* canonicalStorage=nullptr;
    const void* localPrimitive=nullptr; //Null for ambient/directional: no distinct native Light exists yet.
    std::uint32_t ordinal=0,effectiveOrdinal=0,type00=0,zone20=0;
};
class CameraVisibilityLease;
class Owner;
// Read-only live original particle inputs. This lease grants no renderer or
// gameplay admission. A copied value is observation, not a current borrow.
struct GbParticleLightInputs {
    std::array<float,4> globalAmbient50{}, registeredDirectional3C{}, positionA0{};
    std::uint64_t timedServiceRevision=0;
};
class GbParticleLightLease final {
    friend class Owner;
    std::shared_ptr<Owner> owner_;
    std::uint64_t revision_=0;
public:
    GbParticleLightLease() noexcept=default;
    ~GbParticleLightLease();
    GbParticleLightLease(const GbParticleLightLease&)=delete;
    GbParticleLightLease& operator=(const GbParticleLightLease&)=delete;
    GbParticleLightLease(GbParticleLightLease&&)=delete;
    GbParticleLightLease& operator=(GbParticleLightLease&&)=delete;
    bool read(GbParticleLightInputs&) const noexcept;
};
struct McParticleLightInputs {
    std::array<float,4> globalAmbient50{}, registeredDirectional3C{}, positionA0{};
    std::uint64_t timedServiceRevision=0;
};
class McParticleLightLease final {
    friend class Owner;
    std::shared_ptr<Owner> owner_;
    std::uint64_t revision_=0;
public:
    McParticleLightLease() noexcept=default;
    ~McParticleLightLease();
    McParticleLightLease(const McParticleLightLease&)=delete;
    McParticleLightLease& operator=(const McParticleLightLease&)=delete;
    McParticleLightLease(McParticleLightLease&&)=delete;
    McParticleLightLease& operator=(McParticleLightLease&&)=delete;
    bool read(McParticleLightInputs&) const noexcept;
};
// Concrete native bridge only. No public constructor, arbitrary callback
// registration, caller context, success flag or substitute racer collection.
class RaRacerSelectionDispatch50LA final {
    friend class Owner;
    friend class ::original_cable_selection50LA::Binding;
    using Call=bool(*)(const std::shared_ptr<::ra_source_camera50LA::OwnershipBridge>&,
        std::uint32_t,const CameraVisibilityLease&,Diagnostic&) noexcept;
    std::shared_ptr<::ra_source_camera50LA::OwnershipBridge> bridge_;
    Call call_;
    RaRacerSelectionDispatch50LA(std::shared_ptr<::ra_source_camera50LA::OwnershipBridge> bridge,Call call)
        :bridge_(std::move(bridge)),call_(call){}
    bool select(std::uint32_t slot,const CameraVisibilityLease& lease,Diagnostic& d)const noexcept {
        return bridge_&&call_&&call_(bridge_,slot,lease,d);
    }
};

enum class CachePolicy : std::uint8_t {
    unspecified,
    // Deliberate native policy. NEVER asserts source manager+A4 is null,
    // lookup14A860 missed, or shared UI lifetime became tracked again.
    inactiveCandidateNativePerPublicationRecompute,
    // One native authored-zone/top-two selection policy for every course that
    // reads its lights from its SCB (not retail light-volume ownership). The
    // former per-course policy labels were identical and are retired.
    nativeAuthoredZoneLightSelection
};
class Owner;
// Immutable authored render metadata, owned beside the actual 4C0 rows.
// This is not a completed visibility result or an admission stamp. Exclusions
// retain source order/duplicates and use the original zero-based zone ordinal.
struct AuthoredRenderZone {
    std::array<std::uint32_t,127> exclusions{};
    std::uint32_t exclusionCount=0;
    bool environmentNodeVisible=true;
};
// Native value publication for actual retained RA static models. This does
// not complete the separate original callback/FIFO traversal.
struct NativeRaStaticDraw50LA {
    std::size_t worldInstance=0;
    std::uint32_t sourceLod=0,zone=0;
    std::array<std::uint32_t,4> scissor{}; // inclusive original Xmin,Xmax,Ymin,Ymax
    std::array<std::int32_t,4> viewport{}; // original X,Y,width,height
    enum class Kind:std::uint8_t {model,lightFlare};
    Kind kind=Kind::model;
    const OriginalWreckComponent50LA* flareIdentity=nullptr;
};
// Semantic equivalent of229E08..229EF4's temporary component scene+19C
// borrowing. This is not a generic success Boolean or persistent scene field.
// Serial, stack-scoped lease; use inside the actual selected racer's bank visit.
class DrawLease final {
    friend class Owner;
    Owner* owner_=nullptr;
    const VehicleFrameState49W* frame_=nullptr; // Borrow only within actual draw lease.
    std::uint64_t generation_=0, membershipRevision_=0;
    std::uint32_t racer_=0;
    std::array<SceneNodeIndex49P,maxVehicleDrawComponents50CM> roots_{};
    std::uint32_t componentCount_=defaultVehicleDrawComponents50CM;
public:
    DrawLease() noexcept=default;
    ~DrawLease();
    DrawLease(const DrawLease&)=delete;DrawLease& operator=(const DrawLease&)=delete;
    DrawLease(DrawLease&&)=delete;DrawLease& operator=(DrawLease&&)=delete;
    std::uint64_t generation() const noexcept {return generation_;}
};

// Serial access to the SAME application4C0 namespace used by all racers.
// It grants no camera activation or completed-frame receipt. Borrowed pointers
// are valid only during this lease; no draw, racer membership operation, reset
// or scene traversal may overlap it. The retained original camera session owns
// its distinct scene30 membership/backing, using cameraObject30() below.
class CameraVisibilityLease final {
    friend class ::original_main_entry50KG::NativeCollectorFifo50LA;
    friend class Owner;
    friend class ::original_cable_selection50LA::Binding;
    Owner* owner_=nullptr;
    std::uint64_t generation_=0;
    std::uint32_t racer_=0;
public:
    CameraVisibilityLease() noexcept=default;
    ~CameraVisibilityLease();
    CameraVisibilityLease(const CameraVisibilityLease&)=delete;
    CameraVisibilityLease& operator=(const CameraVisibilityLease&)=delete;
    CameraVisibilityLease(CameraVisibilityLease&&)=delete;
    CameraVisibilityLease& operator=(CameraVisibilityLease&&)=delete;
    ResetVisibilityWorld50EY* world() noexcept;
    LiveZoneWorkspace50EN* workspace() noexcept;
    const LiveZoneMembership50EN* racerVisibility7F0() const noexcept;
    CollectionRef50BX cameraObject30() const noexcept;
    // Native stable identities of actual authored-ordinal visibility rows.
    // Their source namespace is4C0; collision690 mapping still uses its actual
    // ordinal, never a collision pointer re-labelled as a visibility pointer.
    std::uint64_t zoneKey4C0(std::size_t ordinal) const noexcept;
    std::size_t zoneCount() const noexcept;
    // Closed SC topology identity, available only through this current lease.
    // Count alone never authorizes the distinct SC camera backing.
    bool isScSourceTopology() const noexcept;
    bool isTcSourceTopology() const noexcept;
    // Currently admitted for authenticated GA authoring only. Null for other
    // scenes, invalid ordinals or an expired lease. Borrow only within lease.
    const AuthoredRenderZone* authoredRenderZone(std::size_t ordinal) const noexcept;
};

// Serial original216A30/2167A0 reset borrow of the SAME live7F0 and4C0.
// No camera ownership, camera reset, frame advance or collision registration
// is implied. Keep this lease around the real ordered reset, including its
// collision operations. A successful removal survives a later rejected slice;
// ending an unfinished lease blocks other visibility consumers until resumed.
// Authenticated by the runtime source Owner against its exact suspended Bank
// query. The lighting owner validates the live frame/slot and pending event.
struct SourceTriggerVisibilityIdentity50MB {
    const void* issuer=nullptr;
    std::uint64_t generation=0,querySerial=0;
    std::size_t candidateOrdinal=0,actionIndex=0;
    std::uint32_t racer60=0;
};
inline bool sameSourceTriggerVisibilityIdentity50MB(const SourceTriggerVisibilityIdentity50MB& a,
    const SourceTriggerVisibilityIdentity50MB& b) noexcept {
    return a.issuer==b.issuer&&a.generation==b.generation&&a.querySerial==b.querySerial&&
        a.candidateOrdinal==b.candidateOrdinal&&a.actionIndex==b.actionIndex&&a.racer60==b.racer60;
}
class ResetVisibilityLease final {
    friend class Owner;
    Owner* owner_=nullptr;
    const VehicleFrameState49W* frame_=nullptr;
    const VehicleFrameBindings49W* bindings_=nullptr;
    const VehicleFrameParameters49W* parameters_=nullptr;
    std::uint64_t generation_=0,graphicsSequence_=0;
    std::uint32_t racer_=0;
    bool removed_=false,attached_=false,player_=false,input_=false,sourceTrigger_=false;
    SourceTriggerVisibilityIdentity50MB sourceIdentity_{};
public:
    ResetVisibilityLease() noexcept=default;
    ~ResetVisibilityLease();
    ResetVisibilityLease(const ResetVisibilityLease&)=delete;
    ResetVisibilityLease& operator=(const ResetVisibilityLease&)=delete;
    ResetVisibilityLease(ResetVisibilityLease&&)=delete;
    ResetVisibilityLease& operator=(ResetVisibilityLease&&)=delete;
    bool removed() const noexcept {return removed_;}
    bool attached() const noexcept {return attached_;}
    std::uint64_t zoneKey4C0(std::size_t ordinal) const noexcept;
    std::size_t zoneCount() const noexcept;
    CollectionRef50BX zoneRef4C0(std::uint64_t actualZoneKey) const noexcept;
};

// Stable SESSION owner, outside the transient native racer bank. Cold prepare
// authenticates actual admitted SCB bytes and owns a fresh decoded world,
// authored lights, and a separate4C0 visibility namespace. It never borrows collision
//4C4 membership or resets OriginalUiLightManagerLifetime50KG.
class Owner final {
    //Out-of-line access to the SAME World fields. Keep its enormous private
    //definition out of this lighting implementation TU; no snapshot or new gate.
    static std::optional<original_world_environment50KG::Owner*>& podEnvironment3A4(::ra_pod_scene50LA::Owner&)noexcept;
    static const std::optional<original_world_environment50KG::Owner*>& podEnvironment3A4(const ::ra_pod_scene50LA::Owner&)noexcept;
    static const std::shared_ptr<OriginalApplicationEntities50LA>& podApplication(const ::ra_pod_scene50LA::Owner&)noexcept;
    static bool podColdConstructed(const ::ra_pod_scene50LA::Owner&)noexcept;
    static bool podTopologyClosed(const ::ra_pod_scene50LA::Owner&)noexcept;
    static const std::shared_ptr<Owner>& podLighting(const ::ra_pod_scene50LA::Owner&)noexcept;
    static bool podFlareIdentityCurrent(const ::ra_pod_scene50LA::Owner&,const OriginalWreckComponent50LA*)noexcept;
    static std::optional<std::size_t> podFlareZoneIndex(const ::ra_pod_scene50LA::Owner&,const OriginalWreckComponent50LA*)noexcept;
    static std::optional<std::size_t> podFlareAuthoredIndex(const ::ra_pod_scene50LA::Owner&,const OriginalWreckComponent50LA*)noexcept;

    friend class ::mb_source_trigger50MB::Owner;
    friend class ::tc_cold_visibility50TC::Retirement;
    friend class ::tb_cold_visibility50TB::Retirement;
    friend class ::gb_cold_visibility50GB::Retirement;
    friend class ::mc_cold_visibility50MC::Retirement;
    friend class ::ra_pod_scene50LA::Owner;
    //Actual RA source flare identity in the SAME canonical visibility domain.
    //Private World producer only; no public admission/ready flag.
    bool bindRaFlareIdentity(const ::ra_pod_scene50LA::Owner*,OriginalWreckComponent50LA&,
        const std::uint8_t&,const std::uint8_t&,std::uint32_t&,std::uint32_t&)noexcept;
    bool registerRaFlarePrimary(const ::ra_pod_scene50LA::Owner*,OriginalNativeListMethodCustody50LA*,
        const OriginalWreckComponent50LA&,std::uint32_t,std::uint32_t&,std::uint32_t&)noexcept;
    bool raFlareIdentityCurrent(const ::ra_pod_scene50LA::Owner*,const OriginalWreckComponent50LA&,
        std::uint32_t,const OriginalNativeListMethodCustody50LA*,std::uint32_t)const noexcept;
    bool releaseRaFlarePrimary(const ::ra_pod_scene50LA::Owner*,const OriginalWreckComponent50LA&,
        std::uint32_t,std::uint32_t&)noexcept;
    bool releaseRaFlareIdentity(const ::ra_pod_scene50LA::Owner*,const OriginalWreckComponent50LA&,
        std::uint32_t,std::uint32_t&)noexcept;
    bool raFlareDomainIdle(const ::ra_pod_scene50LA::Owner*,const OriginalWreckComponent50LA&)const noexcept;
    bool nativeRaRemainingInputs(const ::ra_pod_scene50LA::Owner*,const assets::World48F&,
        const CollectionZone50BX*,std::size_t,RaRemainingLightInputs&,std::uint32_t&)const noexcept;
    bool nativeRaLocalInputs(const ::ra_pod_scene50LA::Owner*,const assets::World48F&,
        const CollectionZone50BX*,std::size_t,RaDirectionalLightInputs&,std::uint32_t&)const noexcept;
    bool nativeRaFirstLocalInputs(const ::ra_pod_scene50LA::Owner*,const assets::World48F&,
        const CollectionZone50BX*,RaDirectionalLightInputs&,std::uint32_t&)const noexcept;
    bool nativeRaDirectionalInputs(const ::ra_pod_scene50LA::Owner*,const assets::World48F&,
        const CollectionZone50BX*,RaDirectionalLightInputs&,std::uint32_t&)const noexcept;
    bool nativeRaLightIdentity(const ::ra_pod_scene50LA::Owner*,const assets::World48F&,
        const CollectionZone50BX*,std::size_t,RaAuthoredLightIdentity&,std::uint32_t&)const noexcept;
    const original_world_environment50KG::SpatialBounds* sceneBounds534(const ::ra_pod_scene50LA::Owner*) const noexcept;
    const CollectionZone50BX* retainSceneZones272A78(const ::ra_pod_scene50LA::Owner*,std::size_t&) noexcept;
    // Same canonical namespace under the retained scene owner, not a camera lease.
    ResetVisibilityWorld50EY* nativeWreckVisibility165E90(const ::ra_pod_scene50LA::Owner*,
        const CollectionZone50BX*) noexcept;
    bool canReleaseSceneZones272A78(const ::ra_pod_scene50LA::Owner*) const noexcept;
    // Same retained RA EnvironmentInfo/light/ambient/visibility backend only.
    // Private actual scene caller; no parallel decoder or collection lease.
    const char* nativeRaEcosystemSelector1AA410(const ::ra_pod_scene50LA::Owner*,
        const assets::World48F&,const CollectionZone50BX*,std::uint32_t&)const noexcept;
    const char* nativeRaWaterSelector274570(const ::ra_pod_scene50LA::Owner*,
        const assets::World48F&,const CollectionZone50BX*,std::uint32_t&)const noexcept;
    const std::string* nativeRaSkySelector274AE0(const ::ra_pod_scene50LA::Owner*,
        const assets::World48F&,const CollectionZone50BX*,std::uint32_t&)const noexcept;
    const std::string* nativeRaParticleSelector1B4DC0(const ::ra_pod_scene50LA::Owner*,
        const assets::World48F&,const CollectionZone50BX*,std::uint32_t&)const noexcept;
    //Actual World publication alias to SAME prewarmed environment ownership.
    //The fresh World constructor must first issue its known-null270598 store.
    bool publishNativeEnvironment3A4(::ra_pod_scene50LA::Owner&,
        const assets::World48F&,std::uint32_t&)noexcept;
    bool withdrawNativeEnvironment3A4(::ra_pod_scene50LA::Owner&)noexcept;
    bool nativeSceneRoles270900Current(const ::ra_pod_scene50LA::Owner*,
        const assets::World48F&,const CollectionZone50BX*,std::uint32_t&)const noexcept;
    const AuthoredRenderZone* nativeRaZoneExclusions272B08(const ::ra_pod_scene50LA::Owner*,
        const assets::World48F&,const CollectionZone50BX*,std::size_t,std::uint32_t&)const noexcept;
    const PortalGeometry50EO* nativeRaPortal230450(const ::ra_pod_scene50LA::Owner*,
        const assets::World48F&,const CollectionZone50BX*,const OriginalApplicationEntities50LA*,
        std::size_t,std::uint32_t&)const noexcept;
    OriginalNativeListMethodCustody50LA* adoptRaVisibilityList(
        const ::ra_pod_scene50LA::Owner*,const assets::World48F&,const CollectionZone50BX*,
        const OriginalApplicationEntities50LA*,std::size_t,std::uint32_t&)noexcept;
    bool raVisibilityListCustodyCurrent(const ::ra_pod_scene50LA::Owner*,
        const OriginalNativeListMethodCustody50LA*)const noexcept;
    bool retainRaVisibilityListNode(const ::ra_pod_scene50LA::Owner*,
        OriginalNativeListMethodCustody50LA*,std::uint32_t&)noexcept;
    bool releaseRaVisibilityListReference(const ::ra_pod_scene50LA::Owner*,
        OriginalNativeListMethodCustody50LA*,bool nodeAlias)noexcept;
    bool releaseSceneZones272A78(const ::ra_pod_scene50LA::Owner*) noexcept;
    friend class ::original_cable_selection50LA::Binding;
    // Private114D00 callback lookup: the viewing lease remains unchanged.
    // Only the source bridge's canonical Bank visit may supply the callback
    // racer, live frame/bindings and actual cable254 cache. Borrow never escapes.
    const LiveZoneMembership50EN* cableCallbackVisibility7F0(
        const CameraVisibilityLease&,std::uint32_t callbackRacer,
        const VehicleFrameState49W&,const VehicleFrameBindings49W&,
        const std::uint32_t& actualCableField50)const noexcept;
    friend class ::track_animation_binding50AN::Binding;
    // ONE visual-zone lane for the track's animation rows (every course on this
    // owner's visibility world; work\scratch	rack-data-gapsnim\ANIM.md piece 3).
    // Row k is identity base+k after the racers, the camera, RA static models and
    // flares; its count is the course's animation rows (planTrackAnimationRows).
    // Replaces the TB door / TC gate / GB four / MC fourteen per-course lanes.
    bool bindTrackAnimationRow(const ::track_animation_binding50AN::Binding*,std::size_t,const LiveZoneMembership50EN*) noexcept;
    bool beginTrackAnimationAccess(const ::track_animation_binding50AN::Binding*,std::size_t,ResetVisibilityWorld50EY*&,
        LiveZoneWorkspace50EN*&,CollectionRef50BX&) noexcept;
    void endTrackAnimationAccess(const ::track_animation_binding50AN::Binding*,std::size_t) noexcept;
    bool commitTrackAnimationRow(const ::track_animation_binding50AN::Binding*,std::size_t) noexcept;
    bool releaseTrackAnimationRow(const ::track_animation_binding50AN::Binding*,std::size_t) noexcept;
    bool currentTrackAnimationRow(const ::track_animation_binding50AN::Binding*,std::size_t) const noexcept;
    friend class ::ra_static_model_cold50LA::Owner;
    // Only the retained static-world owner can bind these authored-instance
    // identities. One namespace and one lifecycle, never a copied registry.
    bool bindRaStaticOwner(const ::ra_static_model_cold50LA::Owner*,
        OriginalApplicationEntities50LA*,const assets::OriginalStandardMaterialTable50LA*,const assets::World48F&) noexcept;
    bool beginRaStaticAccess(const ::ra_static_model_cold50LA::Owner*,bool retiring) noexcept;
    void endRaStaticAccess(const ::ra_static_model_cold50LA::Owner*) noexcept;
    bool releaseRaStaticOwner(const ::ra_static_model_cold50LA::Owner*) noexcept;
    bool commitRaStaticOwner(const ::ra_static_model_cold50LA::Owner*) noexcept;
    bool nativeStaticZone2741C0Current(const ::ra_pod_scene50LA::Owner*,
        const ::ra_static_model_cold50LA::Owner*,const assets::World48F&,
        const CollectionZone50BX*,std::size_t,std::uint32_t&)const noexcept;
    bool publishRaStaticZone2741C0(const ::ra_static_model_cold50LA::Owner*,std::uint32_t actualZone) noexcept;
    ResetVisibilityWorld50EY* raStaticVisibility(const ::ra_static_model_cold50LA::Owner*) noexcept;
    LiveZoneWorkspace50EN* raStaticWorkspace(const ::ra_static_model_cold50LA::Owner*) noexcept;
    CollectionRef50BX claimRaStaticModel(const ::ra_static_model_cold50LA::Owner*,
        std::size_t, const OriginalRacerModelSceneOwner50LA*) noexcept;
    bool bindRaStaticSelection(const ::ra_static_model_cold50LA::Owner*,std::size_t,
        OriginalRacerModelSceneOwner50LA*,const SceneGraph49P&,const void*,const LiveZoneMembership50EN*) noexcept;
    bool releaseRaStaticModel(const ::ra_static_model_cold50LA::Owner*,
        std::size_t,const OriginalRacerModelSceneOwner50LA*) noexcept;
    bool raStaticOwnerIdle(const ::ra_static_model_cold50LA::Owner*) const noexcept;
    // One private suspended end caller, actual attached7F0 branch only.
    // No public racer/cache/phase input or fresh-grid attach substitution.
    bool readRecordedZoneC38(std::uint32_t,std::uint32_t&)const noexcept;
    bool readRecordedCenterC40(std::uint32_t,std::array<std::uint32_t,4>&)const noexcept;
    bool recordAttachedEndRacer229F20(std::uint32_t,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,Diagnostic&)noexcept;
    friend class ::ra_source_camera50LA::OwnershipBridge;
    // Internal partial source operation, never a selection-ready capability.
    // Only the actual SourceCamera bridge can supply these retained borrows.
    // Separate114D50 ->114E00 draw callback, never the114D00 selection tail.
    // Caller supplies the actual Application collector slot, not a copied list.
    bool drawRaZonePrefix27FEE0(CameraVisibilityLease&,OriginalApplicationEntities50LA&,
        std::uint32_t actualParent38,const OriginalCameraViewportFields&,
        OriginalZoneScissorPublication50LA&,Diagnostic&) noexcept;
    bool prepareNativeRaStaticDraw(const CameraVisibilityLease&,
        const OriginalApplicationEntities50LA&,const CameraViewOutput49F&,
        std::uint32_t actualParent38,const OriginalCameraViewportFields&,
        std::vector<NativeRaStaticDraw50LA>&,Diagnostic&) const noexcept;
    bool drawRaSelectedMethod48(const CameraVisibilityLease&,std::uint32_t,Diagnostic&) noexcept;
    bool prepareRaZoneScissor280230(const CameraVisibilityLease&,std::uint32_t,
        const OriginalApplicationEntities50LA&,const OriginalCameraViewportFields&,Diagnostic&) noexcept;
    bool prepareRaZonePrefix114D00(CameraVisibilityLease&,
        OriginalApplicationEntities50LA&,const CameraViewOutput49F&,
        std::uint32_t actualParent38,const OriginalCameraViewportFields&,
        const assets::OriginalStandardMaterialTable50LA&,
        std::array<OriginalRacerEntityCold50LA,8>&,const OriginalCameraSceneOwner&,
        const RaRacerSelectionDispatch50LA&,const OriginalRendererDispatchFields&,
        platform::VehicleWindowGl50DS&,platform::VehicleWindowDiagnostic50DS&,Diagnostic&) noexcept;
    bool selectRaDynamicMethod44(CameraVisibilityLease&,OriginalApplicationEntities50LA&,
        std::uint32_t,std::array<OriginalRacerEntityCold50LA,8>&,const OriginalCameraSceneOwner&,
        const RaRacerSelectionDispatch50LA&,const OriginalRendererDispatchFields&,platform::VehicleWindowGl50DS&,
        platform::VehicleWindowDiagnostic50DS&,Diagnostic&) noexcept;
    bool selectRaModel26EFC0(const CameraVisibilityLease&,OriginalApplicationEntities50LA&,
        OriginalRacerModelSceneOwner50LA&,const SceneGraph49P&,const LiveZoneMembership50EN*,
        const CameraViewOutput49F&,const OriginalCameraViewportFields&,
        const assets::OriginalStandardMaterialTable50LA&,Diagnostic&) noexcept;
    bool selectRaStaticMethod44(CameraVisibilityLease&,OriginalApplicationEntities50LA&,
        std::uint32_t method,const CameraViewOutput49F&,const OriginalCameraViewportFields&,
        const assets::OriginalStandardMaterialTable50LA&,const OriginalRendererDispatchFields&,
        platform::VehicleWindowGl50DS&,platform::VehicleWindowDiagnostic50DS&,Diagnostic&) noexcept;

    friend class ::ra_static_lighting50LA::Binding;
    // Private native static policy; no membership or racer DrawLease is made.
    bool selectRaStatic(std::uint32_t,const std::array<float,3>&,
        original_racer_lighting50KM::LightMatrices&,Diagnostic&) const noexcept;
    bool raStaticCurrent() const noexcept;
    friend class GbParticleLightLease;
    void endGbParticleLightBorrow(GbParticleLightLease&) noexcept;
    bool readGbParticleLightBorrow(const GbParticleLightLease&,GbParticleLightInputs&) const noexcept;
    friend class McParticleLightLease;
    void endMcParticleLightBorrow(McParticleLightLease&) noexcept;
    bool readMcParticleLightBorrow(const McParticleLightLease&,McParticleLightInputs&) const noexcept;
    friend class DrawLease;
    friend class CameraVisibilityLease;
    friend class ResetVisibilityLease;
    enum class Scene : std::uint8_t { gc,ta,ga,ma,sa,ra,mb,sc,tc,rc,tb,gb,mc };
    struct Storage;
    std::unique_ptr<Storage> storage_;
    bool invalidated_=false; // Terminal even before a successful cold prepare.
    std::size_t plannedAnimationRows_=0; // planTrackAnimationRows: the course's animation rows
    void endDraw(DrawLease&) noexcept;
    void endCameraVisibility(CameraVisibilityLease&) noexcept;
    bool cameraLeaseCurrent(const CameraVisibilityLease&) const noexcept;
    void endResetVisibility(ResetVisibilityLease&) noexcept;
    bool resetLeaseCurrent(const ResetVisibilityLease&) const noexcept;
    bool beginResetVisibility(std::uint32_t,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
        std::uint32_t*,ResetVisibilityLease&,Diagnostic&,bool player,bool input=false,
        const SourceTriggerVisibilityIdentity50MB* source=nullptr) noexcept;
    bool prepareScene(const std::vector<std::uint8_t>&,std::uint32_t,Scene,Diagnostic&,std::shared_ptr<const ::mc_cold_sources50LA::Owner> = {}) noexcept;
    // Native cold teardown only; never a live reset or a public lease escape.
    bool retireRaFreshVehicle(std::uint32_t,Diagnostic&) noexcept;
    // Terminal MB cold-session unwind only, with the actual Bank slot installed.
    // No source query/reset receipt is invented; active reset/borrowers reject.
    bool retireMbColdVehicle(std::uint32_t,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
        std::uint32_t* actualCablesField50,Diagnostic&) noexcept;
    // TC terminal native cold unwind, callable only through the actual Bank
    // selection/typed Rig-Cables bridge. No source query/reset receipt exists.
    bool retireTcColdVehicle(std::uint32_t,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
        std::uint32_t* actualCablesField50,Diagnostic&) noexcept;
    // Read-only proof for a missing cold Rig/Cables prefix. A reached attach,
    // partial registration, graphics publication or prior retirement rejects.
    bool tcColdVehicleUntouched(std::uint32_t,Diagnostic&) const noexcept;
    // Distinct TB terminal cold bridge; actual Bank/Rig/Cables authority only.
    bool retireTbColdVehicle(std::uint32_t,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
        std::uint32_t* actualCablesField50,Diagnostic&) noexcept;
    bool tbColdVehicleUntouched(std::uint32_t,Diagnostic&) const noexcept;
    bool retireGbColdVehicle(std::uint32_t,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
        std::uint32_t* actualCablesField50,Diagnostic&) noexcept;
    bool gbColdVehicleUntouched(std::uint32_t,Diagnostic&) const noexcept;
    bool retireMcColdVehicle(std::uint32_t,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,
        std::uint32_t* actualCablesField50,Diagnostic&) noexcept;
    bool mcColdVehicleUntouched(std::uint32_t,Diagnostic&) const noexcept;
    // Only the exact source Owner may call this inside Bank's private suspended
    // retirement visit, after every raw query/result/visibility borrow closes.
    // Never reports reset/graphics completion. A failed removal retains prefix.
    bool retireSourceTriggerVisibility50MB(std::uint32_t,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,const VehicleFrameParameters49W&,std::uint32_t*,
        const SourceTriggerVisibilityIdentity50MB&,Diagnostic&) noexcept;

    bool matchesRaFreshRoster(std::uint32_t) const noexcept;
public:
    Owner() noexcept;
    ~Owner();
    Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
    // Exact original tracks\\gc.scb,359830 bytes,SHA256 d47a83...be210d.
    // Actual roster count is supplied by the native roster owner, never inferred
    // from geometry. Cold-only. Failure leaves an empty owner retryable.
    bool prepare(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    // Exact TA bytes, explicit host policy: frozen authored lights, genuine
    // racer visibility, native zone scan. No source light-D0/manager+88 claim.
    bool prepareTaNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    // Exact GA SCB,12 zones,18 portals,38 original lights. Its malformed
    // authoring section remains intact; Light9 retains genuine reader/Init
    // defaults. Uses the declared native spotlight selection policy.
    bool prepareGaNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    // Exact MA SCB,9 zones/10 portals,43 numerically reached lights. Retains
    // 14 emitting ObjectPoints,13 spotlights and6 non-emitting flare objects.
    // The stray Light276 block is outside NumberOfLights and is not registered.
    bool prepareMaNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    // Exact SA SCB,25 zones/25 portals,263 numerically reached lights. Its166
    // non-emitting ObjectPoints do not contribute or acquire a flare renderer.
    bool prepareCommonRouteNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    // Inactive RA source preparation: exact SCB,10 zones/10 portals,185
    // numeric reader slots. Failed Light90/107 selection retains89/106;
    // stray186/187 are excluded. Actual roster count remains caller-owned.
    // Same explicit native authored-zone policy; no course admission follows.
    bool prepareRaNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    // Exact MB source; all384 rows validated,154 emitting locals retained.
    // Native query only: authored flare rendering remains outside this owner.
    // Authenticated RC15/15 topology uses unchanged legacy backing and no local lights.
    // Exact TB10/11 source, including12 retained-selector repetitions ofLight92.
    // The track's animation rows for the one visual-zone lane (every course):
    // call before prepare*Native, with the count the course's [Animations] table
    // loads (0 = no lane). Refused above maximumTrackAnimationRows or after a prepare.
    static constexpr std::size_t maximumTrackAnimationRows=64; // = TrackAnimationLimits50AN::animations (checked by the binding)
    bool planTrackAnimationRows(std::size_t rows) noexcept;
    std::size_t trackAnimationRows() const noexcept;
    // Additional Storage backing for that lane at maximumRacers (lane state, the
    // extra registration records, padding). Charge before prepare*Native.
    static std::size_t trackAnimationStorageReservationBytes(std::size_t rows) noexcept;
    // Exact GB13/17 source, sixty emitted points and two authored flashers.
    // Authored flare rendering is a separate visual owner.
    // Requires this exact strong owner. Retains backing, excludes timed service,
    // camera/reset/geometry publication, and reports Light+A0, NOT toward/look78.
    bool beginGbParticleLightBorrow(const std::shared_ptr<Owner>&,
        GbParticleLightLease&,Diagnostic&) noexcept;
    bool beginMcParticleLightBorrow(const std::shared_ptr<Owner>&,
        McParticleLightLease&,Diagnostic&) noexcept;
    // MC retains the exact cold source owner; no caller-supplied World is admitted.
    bool prepareMcNative(std::shared_ptr<const ::mc_cold_sources50LA::Owner>,std::uint32_t,Diagnostic&) noexcept;
    const ::mc_cold_sources50LA::Owner* mcSourceIdentity() const noexcept;
    const ::mc_lighting_authoring50MC::Record* mcLightRecord(std::size_t) const noexcept;
    bool queryMcZoneLight(std::uint32_t,const std::array<float,3>&,
        original_racer_lighting50KM::LightMatrices&,Diagnostic&) const noexcept;
    static std::size_t mcTopologyStorageReservationBytes() noexcept;
    std::size_t mcRetainedPayloadBytes() const noexcept;
    bool prepareGbNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    static std::size_t gbTimedLightStorageReservationBytes() noexcept;
    // Native fixed-boundary scheduling; actual shared RNG and pause come from
    // the root. No draw/camera/reset/registration borrower may be active.
    bool serviceGbTimedLights(float actualDelta,std::uint32_t& actualSharedRng,
        bool paused,Diagnostic&) noexcept;
    bool prepareTbNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    bool prepareRcNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    // Exact TC25-zone/26-portal backing; legacy25/25 and local240 remain.
    bool prepareTcNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    // Exact SC30-zone/34-portal backing; legacy25/25 and local240 remain.
    bool prepareScNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    bool prepareMbNative(const std::vector<std::uint8_t>& scb,std::uint32_t actualRacers,
        Diagnostic&) noexcept;
    const assets::World48F* world() const noexcept;
    // Authenticated GC/GA/TA/MA/SA global ambient+50 with no flash authoring.
    // Retains the last numeric-order ambient registration, not selected zone60.
    // A separate method preserves currentGcGlobalAmbient50's GC-only contract.
    bool currentSourceGlobalAmbient50(std::array<float,4>&) const noexcept;
    // Bound GC global148F90..148FFC ambient50, independent of per-racer zones.
    // Only available when authenticated SCB omits the ambient-flash enabling
    // key270E9C and this original-lighting lifetime is still valid. Other
    // tracks and untracked scene changes reject. No white/selected-zone fallback.
    bool currentGcGlobalAmbient50(std::array<float,4>&) const noexcept;
    // Actual fresh-grid229F20->2167A0 slice after the final real grid placement.
    // Source1AE578..1AE594 supplied4C0[0] to2169C0, whose216A20 stores C30.
    // This owner binds its actual first visibility row, never collision C34.
    // Requires canonical fresh Ready/main world, frameFactor0 and actual694
    // E0-null path. Caller must prove completedFrames0/unadvanced generation
    // and call only before the first graphics step after final grid placement.
    // Prepares694 (11FE50/11F680), then performs2809C0 with mode2 and cached
    // radius. If attachment fails, the completed shape prefix remains and this
    // owner permits retrying that pending attachment without re-preparation.
    bool attachFreshVehicle(std::uint32_t actualRosterSlot,
        VehicleFrameState49W&,const VehicleFrameBindings49W&,
        const VehicleFrameParameters49W&,Diagnostic&) noexcept;
    //228844..228888 immediately AFTER successful graphics49W/PostGraphics,
    // BEFORE later physics. Consumes the EXISTING694, not a recomputed model
    // or current body pose. Sequence is the actual per-racer graphics sequence.
    // One successful update per sequence. Failure leaves visibility unchanged.
    bool updateAfterGraphics(std::uint32_t actualRosterSlot,
        const VehicleFrameState49W&,const VehicleFrameBindings49W&,
        const VehicleFrameParameters49W&,std::uint64_t graphicsSequence,
        Diagnostic&) noexcept;
    // Actual vehicle58=1/PostPlayer reset; never the fresh Ready/frameFactor0
    // adapter. The pointer must name the sole live *(vehicle254)+50 projection,
    // external to this owner, frame and descriptors. Missing ownership rejects
    // before any mutation; a temporary/scratch integer is not a valid binding.
    // Source254 is the actual PodCables owner, allocated by214200..214218;
    // its field50 caches7F0 at225580..22558C. This owner never invents it.
    // Bindings/parameters and the selected frame must stay at their addresses
    // for the lease. Retain the original five-model POD02 binding unchanged.
    bool beginSourceTriggerResetVisibility50MB(std::uint32_t actualRosterSlot,
        const VehicleFrameState49W&,const VehicleFrameBindings49W&,
        const VehicleFrameParameters49W&,std::uint32_t* actualCables254Field50,
        const SourceTriggerVisibilityIdentity50MB&,ResetVisibilityLease&,Diagnostic&) noexcept;
    bool beginAiResetVisibility(std::uint32_t actualRosterSlot,
        const VehicleFrameState49W&,const VehicleFrameBindings49W&,
        const VehicleFrameParameters49W&,std::uint32_t* actualObject254Field50,
        ResetVisibilityLease&,Diagnostic&) noexcept;
    // Genuine Player58=0/PostPackedBytes50S entry to the SAME216A30 body.
    // No phase/type rewrite or AI identity is used. The remaining ordered
    // methods below are shared216A30 operations despite their legacy names.
    bool beginPlayerResetVisibility(std::uint32_t actualRosterSlot,
        const VehicleFrameState49W&,const VehicleFrameBindings49W&,
        const VehicleFrameParameters49W&,std::uint32_t* actualObject254Field50,
        ResetVisibilityLease&,Diagnostic&) noexcept;
    // Genuine1A17A4 input-dispatch call while the same Player frame is Ready.
    // Completion requires the next ordinary graphics publication before drawing.
    bool beginPlayerInputResetVisibility(std::uint32_t actualRosterSlot,
        const VehicleFrameState49W&,const VehicleFrameBindings49W&,
        const VehicleFrameParameters49W&,std::uint32_t* actualObject254Field50,
        ResetVisibilityLease&,Diagnostic&) noexcept;
    //216A90..216AC8, REQUIRED before collision690/694/7A4/348 removals.
    // Mutates the same persistent membership used by graphics/draw/camera.
    bool removeAiResetVisibility(ResetVisibilityLease&,Diagnostic&) noexcept;
    //216874..21689C, REQUIRED after actual694 preparation, before collision
    // reattachment690/7A4/348/694. C30 must be from this actual4C0 namespace;
    // never cast a collision pointer into a visibility ref. Checks existing
    // detached694 and its authored shape binding without preparing it again.
    bool attachAiResetVisibility(ResetVisibilityLease&,
        CollectionRef50BX visibilityZoneC30,Diagnostic&) noexcept;
    // Caller has completed all four actual collision attachments and the
    // remaining source reset. Checks registered694 before releasing the pending
    // reset, but does not stand in for those operations or advance49W. AI
    // drawing still requires its following real graphics sequence. Player
    // reset follows current graphics; its final ready parentless model worlds
    // publish the new registration against that SAME existing graphics serial.
    bool finishAiResetVisibility(ResetVisibilityLease&,Diagnostic&) noexcept;
    // Borrow canonical4C0 after the selected racer's actual7F0 attachment.
    // One extra registration identity is reserved during fresh preparation;
    // racer1..N references and all existing registrations remain unchanged.
    // This never attaches or selects a camera. Cold175224 uses the borrowed
    // actual7F0; later114C44/177AB0 use this SAME world and scratch. Camera
    // state and membership must remain outside every racer-bank snapshot.
    // GC/GA/TA/MA/SA share this actual cold registration and serial lease. Authored
    // lighting policy remains independent of camera visibility ownership.
    bool beginCameraVisibility(std::uint32_t actualRosterSlot,
        CameraVisibilityLease&,Diagnostic&) noexcept;
    // Call around the real active-component draw preparation, inside that same
    // racer's visit, with its canonical current frame/bindings. Owns an increasing
    // draw publication generation. No lighting cache policy is selected here.
    // expectedGraphicsSequence comes from that racer's actual graphics caller,
    // including steps whose visibility update failed; stale values cannot be
    // substituted to authorize a draw after skipped/failed membership work.
    bool beginDraw(std::uint32_t actualRosterSlot,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,std::uint64_t expectedGraphicsSequence,
        DrawLease&,Diagnostic&) noexcept;
    //225580..22558C ->174360, after this actual frame's successful source
    // cable update. Binds and writes the sole real PodCables254+50 projection.
    // Native7F0 identity is its stable object30 token, not a truncated pointer
    // or collision identity. Reset clears this exact field. No cable position,
    // lighting selection, or successful cable build is implied by this store.
    bool writeCablesVisibility50(const DrawLease&,
        std::uint32_t& actualCablesField50,Diagnostic&) noexcept;
    // Graphics-side225588 store, including racers with no draw. Call directly
    // after this racer's successful updateAfterGraphics with that exact frame
    // and sequence, before any intervening consumer. Original225588 precedes
    //228844; this narrow identity copy commutes with that membership update:
    // both observe the same stable7F0, not its changing bounds/zone entries.
    // No draw lease, source cable geometry, or cable-lighting receipt is made.
    bool writeCablesVisibilityAfterGraphics(std::uint32_t actualRosterSlot,
        const VehicleFrameState49W&,const VehicleFrameBindings49W&,
        const VehicleFrameParameters49W&,std::uint64_t actualGraphicsSequence,
        std::uint32_t& actualCablesField50,Diagnostic&) noexcept;
    // Components are chariot/left/right, then optional bottom-left/bottom-right.
    // Must name the exact bound model root.
    // Genuine registry and actual membership are read afresh on each new lease;
    // result retained once per real model/publication. Retail cache policy remains
    // unimplemented. Root must explicitly review/activate this native branch.
    bool select(const DrawLease&,std::uint32_t component,SceneNodeIndex49P modelRoot,
        CachePolicy,original_racer_lighting50KM::LightMatrices&,Diagnostic&) noexcept;
    // Value-copy all active actual component selections only while their real
    // draw lease is active. The publication can outlive the arena/lease solely
    // alongside that exact completed geometry batch; never relabel its sequence.
    bool publishNativeDraw(const DrawLease&,CachePolicy,
        NativeGcSelectedLightsCandidate&,Diagnostic&) noexcept;
    // Call before ANY unjoined scene light update or scene/reset transition.
    // Invalidated owners cannot manufacture a new fresh lifetime or stale draw.
    void invalidate() noexcept;
};

// Minimal CPU StripPreamble light/normal composition for an actual current
// model palette entry. No camera, inverse transpose, renormalization, RGB
// modulation, or host directional/hemisphere constants. Not a provenance gate.
// Caller must supply the selected material/scene/palette from their real owners.
// RGB-only arithmetic: does not publish full DiffuseCompressed RGBA or satisfy
// actual model1B3/1F4/204, fog/clipping, alpha-test or material-default admission.
original_racer_lighting50KM::Status prepareDiffuseNormalAxes(
    const BodyMatrix48G& actualNodeWorld,float authoredNormalQuantization,
    const original_racer_lighting50KM::LightMatrices&,
    original_racer_lighting50KM::DiffuseProgramInputs&) noexcept;
// Exact reversible extraction from the existing semantic scene representation.
// Returns only signed-byte source normals; no approximate requantization.
bool recoverCompressedNormal(const std::array<double,3>& expanded,
    double authoredNormalQuantization,
    original_racer_lighting50KM::CompressedNormal&) noexcept;
}
