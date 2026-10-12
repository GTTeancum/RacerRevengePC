#pragma once
// INACTIVE SOURCE-ONLY CANDIDATE. No renderer activation or retail cache claim.
#include "vehicle_frame49w.h"
#include "live_zone_membership50en.h"
#include "original_racer_material50km.h"
#include "native_gc_selected_lights_candidate.h"
#include "../assets/import_world48f.h"
#include <array>
#include <memory>
#include <vector>

namespace rr::game::original_gc_gameplay_lighting_candidate {
constexpr std::uint32_t maximumRacers=8, gcZones=12, gcPortals=13;
enum class Status : std::uint8_t {
    complete, unprepared, alreadyPrepared, sourceIdentity, sourceShape,
    sourceFields, unsupportedScene, phase, identity, bounds, registration,
    activeDraw, stalePublication, unsupportedPolicy, arithmetic, allocation
};
struct Diagnostic {
    Status status=Status::complete;
    std::uint32_t sourcePC=0, racer=0;
    LiveZoneResult50EN membership{};
    assets::DecodeError48C decode{};
};
enum class CachePolicy : std::uint8_t {
    unspecified,
    // Deliberate native policy. NEVER asserts source manager+A4 is null,
    // lookup14A860 missed, or shared UI lifetime became tracked again.
    inactiveCandidateNativePerPublicationRecompute,
    // Explicit TA authored-zone scan policy; not original light-volume query.
    nativeTaAuthoredZoneSpotSelection
};
class Owner;
// Semantic equivalent of229E08..229EF4's temporary component scene+19C
// borrowing. This is not a generic success Boolean or persistent scene field.
// Serial, stack-scoped lease; use inside the actual selected racer's bank visit.
class DrawLease final {
    friend class Owner;
    Owner* owner_=nullptr;
    const VehicleFrameState49W* frame_=nullptr; // Borrow only within actual draw lease.
    std::uint64_t generation_=0, membershipRevision_=0;
    std::uint32_t racer_=0;
    std::array<SceneNodeIndex49P,3> roots_{};
public:
    DrawLease() noexcept=default;
    ~DrawLease();
    DrawLease(const DrawLease&)=delete;DrawLease& operator=(const DrawLease&)=delete;
    DrawLease(DrawLease&&)=delete;DrawLease& operator=(DrawLease&&)=delete;
    std::uint64_t generation() const noexcept {return generation_;}
};

// Stable SESSION owner, outside the transient native racer bank. Cold prepare
// authenticates actual GC SCB bytes and owns a fresh decoded world, real small
// registry, and a separate4C0 visibility namespace. It never borrows collision
//4C4 membership or resets OriginalUiLightManagerLifetime50KG.
class Owner final {
    friend class DrawLease;
    struct Storage;
    std::unique_ptr<Storage> storage_;
    bool invalidated_=false; // Terminal even before a successful cold prepare.
    void endDraw(DrawLease&) noexcept;
    bool prepareScene(const std::vector<std::uint8_t>&,std::uint32_t,bool,Diagnostic&) noexcept;
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
    const assets::World48F* world() const noexcept;
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
    // Call around the real three-component draw preparation, inside that same
    // racer's visit, with its canonical current frame/bindings. Owns an increasing
    // draw publication generation. No lighting cache policy is selected here.
    // expectedGraphicsSequence comes from that racer's actual graphics caller,
    // including steps whose visibility update failed; stale values cannot be
    // substituted to authorize a draw after skipped/failed membership work.
    bool beginDraw(std::uint32_t actualRosterSlot,const VehicleFrameState49W&,
        const VehicleFrameBindings49W&,std::uint64_t expectedGraphicsSequence,
        DrawLease&,Diagnostic&) noexcept;
    // component0/1/2 is chariot/left/right. Must name the exact bound model root.
    // Genuine registry and actual membership are read afresh on each new lease;
    // result retained once per real model/publication. Retail cache policy remains
    // unimplemented. Root must explicitly review/activate this native branch.
    bool select(const DrawLease&,std::uint32_t component,SceneNodeIndex49P modelRoot,
        CachePolicy,original_racer_lighting50KM::LightMatrices&,Diagnostic&) noexcept;
    // Value-copy all three actual component selections only while their real
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
