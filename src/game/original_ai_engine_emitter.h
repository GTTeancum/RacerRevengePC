#pragma once
#include "vehicle_frame49w.h"
#include "boost_controls50fk.h"
#include "../../platform/linux/native/logical_audio_owner.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
// Source-only native PodAIEngineEmitter D40. This is not Player19EEA0 and
// does not turn the host opponent-audio subscriber into an original owner.
struct OriginalAiEmitterVector { float v[4]; };
struct OriginalAiEmitterIdentity {
    std::uint64_t session=0,racer=0,mainBody=0,emitterD40=0;
    std::uint32_t managerOrdinal=0,pod60=0;
};

// Borrow the REAL232DB0/232ED0 logical sound fields. No second mutable54 or
// position60 is allocated here. Sound identity/lifetime belongs to its actual
// application group/sample owner, outside the serial racer arena. Position
// has FOUR backing words; source233DB0 writes XYZ only and preserves60.W.
// This view neither supplies a sample index nor claims a mixer voice exists.
struct OriginalAiEmitterSound {
    std::uint64_t session=0,soundKey=0;
    // Byte-copy views admit either the existing uint32_t-bits field owner or
    // an actual float field without pointer-punning/strict-alias violations.
    // Exact writable extents are4 and16 bytes. Never cast away owner constness.
    void* multiplier54=nullptr;
    void* position60=nullptr;
    // Concrete native transport for this SAME logical sound. No callback
    // success shim or duplicate channel/scalar fields are permitted.
    platform::LogicalAudioOwner* nativeOwner=nullptr;
    platform::LogicalAudioOwner::Sound* nativeSound=nullptr;
};
struct OriginalAiEmitterSpatial {
    std::uint64_t session=0,emitterKey=0;
    bool setupApplied=false;
    OriginalAiEmitterVector previous40{},position170{},velocity180{};
    std::uint32_t flags158=0;
    std::uint8_t flags1D4Low5=0;
    // Constructor does not prove1E8/1EC. Their writes establish known state.
    std::uint32_t word1E8=0,word1EC=0;
    bool word1E8Known=false,word1ECKnown=false;
    float near1B8=0.f,far1BC=0.f,distance1D0=0.f;
    float trigger1DC=0.f,chance1E0=100.f;
    OriginalAiEmitterSound* sound1F0=nullptr;
};

// Genuine native intrusive object list, preserving141ED0 head18/next14 and
// the post-callback next14 read. These are actual owner links, not a projected
// list of 'successful' callbacks. Backing and identities remain stable during
// dispatch; detaching/destroying the currently executing node is not admitted.
struct OriginalAiEmitterChild {
    std::uint64_t objectKey=0;
    OriginalAiEmitterChild* next14=nullptr;
    std::uint32_t mask20=0;
    std::uint8_t flags29=0;
    void* context=nullptr;
    bool (*update38)(void*,float elapsed) noexcept=nullptr;
};
struct OriginalAiEmitterChildPrelude {
    void* context=nullptr;
    // Actual442F70->virtual20(a1=0), immediately before each reached child.
    // Missing is a boundary, not implicit permission to omit this source call.
    bool (*beforeChild)(void*) noexcept=nullptr;
};

struct OriginalAiEngineEmitter {
    OriginalAiEmitterIdentity identity{};
    bool constructed=false,prepared=false,blocked=false,inUpdate=false;
    std::uint64_t completedUpdates=0;
    OriginalAiEmitterSpatial spatial{};
    OriginalAiEmitterSpatial* secondary480=nullptr;
    OriginalAiEmitterSound* engine224=nullptr;
    OriginalAiEmitterSound* boost228=nullptr;
    std::uint32_t childMask24=0;OriginalAiEmitterChild* child18=nullptr;
    // Only source22C bits0..3 are represented. Constructor preserves opaque
    // upper bits; none of this kernel's source paths consumes those bits.
    std::uint8_t flags22CLow4=0;
    OriginalAiEmitterVector vector4A0{},vector4B0{},vector4D0{};
    std::uint32_t word4E8=0,word500=0,word504=0;
    float distance4EC=0;
    // These are written by the real178EA0/178C50 tuning owner, NOT16D8E0.
    // Until that owner has bound them, prepared stays false and none is read.
    const float* idle450=nullptr;const float* range460=nullptr;
    const float* idle458=nullptr;const float* range468=nullptr;
};

// Fresh represented-field projection of290F10->293510->1788E0->16D8E0.
// Does not perform allocation, global object counters/lazy guards, source
// resource construction, registration or173970 setup. Opaque22C upper bits
// are excluded from the typed projection, never guessed zero.
bool projectFreshOriginalAiEngineEmitter(OriginalAiEngineEmitter&,
    const OriginalAiEmitterIdentity&) noexcept;
// Same290F10 represented cold fields for the separately allocated103B80
// secondary emitter. It is NOT a child of D40:103CB4 appends it to AuralScape.
bool projectFreshOriginalAiSecondaryEmitter(OriginalAiEmitterSpatial&,
    std::uint64_t session,std::uint64_t secondaryEmitterKey) noexcept;
// Known numeric portion of the successful cold2911E0 calls: main uses2,
// secondary8, and both consume the real4A0/4B0 packet vectors. This neither
// fabricates141C20/AuralScape success nor resolves/registers any sound.
bool projectOriginalAiEmitterSpatialSetup2911E0(OriginalAiEngineEmitter&,
    OriginalAiEmitterSpatial& actualSecondary480) noexcept;

// Borrow only after the genuine16DA00 setup/resource/backend operations have
// completed:178A50,16DD60 (including16DE30/38), both zero233A80 calls and both
//291A20 dispatches. This is an ownership join, not a success receipt for those
// operations. No factory/sample/start/stop can be synthesized by this function.
// The caller supplies the actual tuning fields and actual secondary emitter.
bool bindOriginalAiEngineEmitterAfterSetup(OriginalAiEngineEmitter&,
    OriginalAiEmitterSound& engine224,OriginalAiEmitterSpatial& secondary480,
    const float& actual450,const float& actual460,const float& actual458,
    const float& actual468,OriginalAiEmitterSound* actualBoost228) noexcept;

struct OriginalAiEmitterRacerInput {
    OriginalAiEmitterIdentity identity{};
    VehicleFramePhase49W phase=VehicleFramePhase49W::Ready;
    OriginalAiEmitterVector localPosition{},bodyVelocity70{};
    float speedRatio70=0,elapsed=0;
};
// Read inside the actual racer's bank lease, at BeforeGraphics for2176E0's
// boost event, or AfterBodyChildren for its later D40 child.22A180/28B180 copy
// body288 LOCAL XYZ; W is the
// original vf0 stack1. Full70.W comes from existing mainSphere, not a zero.
bool readOriginalAiEmitterRacerInput(const OriginalAiEmitterIdentity&,
    const VehicleFrameState49W&,const VehicleFrameBindings49W&,
    float actualRacerSpeedRatio70,OriginalAiEmitterRacerInput&) noexcept;

enum class OriginalAiEmitterStatus : std::uint8_t {
    complete,owner,arithmetic,spatialSound,secondaryOwner,childPrelude,
    childOwner,childUpdate,childCycleOrCapacity,boostAttach,boostStart,blocked,
    soundLifecycle
};
struct OriginalAiEmitterReceipt {
    OriginalAiEmitterStatus status=OriginalAiEmitterStatus::owner;
    std::uint32_t sourcePC=0;
    std::size_t spatialWrites=0,positionSoundCalls=0,velocityNoopCalls=0,
        pitchCalls=0,pitchWrites=0,childCalls=0;
    // Nonzero: the boost sound attach/start at this PC was refused and skipped.
    // The thermal/boost state still advanced; the race is not stopped.
    std::uint32_t skippedBoostSoundPC=0;
};
// Joins actual native sound fields, including its real registered PCM owner.
// Does not claim original resource-factory/AuralScape setup has completed.
bool bindOriginalAiNativeSound(OriginalAiEmitterSound&,std::uint64_t session,
    std::uint64_t soundKey,platform::LogicalAudioOwner&,std::size_t ordinal) noexcept;
// Actual cold16DA00 stop calls occur before the emitter is marked prepared.
// This operates on its already constructed/setup sound ownership directly.
bool stopOriginalAiSpatial291A20(OriginalAiEngineEmitter&,OriginalAiEmitterSpatial&) noexcept;
enum class OriginalAiEmitterLifecycle : std::uint8_t {
    startEngine16DB80,stopEngine16DBC0,startBinder16DC00,stopBinder16DC10
};
// Recovered lifecycle, with exact ordering of291A10/291A20 and22C/504.
//291A10 only enables the emitter; it does NOT start a PCM voice itself.
//291A20->2917A0->233670 stops the same native logical sound's owned playback.
OriginalAiEmitterReceipt applyOriginalAiEmitterLifecycle(OriginalAiEngineEmitter&,
    OriginalAiEmitterLifecycle) noexcept;
struct OriginalAiEmitterBoostBackend {
    void* context=nullptr;
    // Real AuralScape104140 with original event4 arguments. The bool return
    // means native completion; sourceReturned is the actual source V0 test.
    // A genuine zero is allowed and suppresses233410, rather than rejecting
    // or pretending playback began. The backend must operate on this sound.
    bool (*attach104140)(void*,OriginalAiEmitterSound&,const OriginalAiEmitterVector& position,
        const OriginalAiEmitterVector& velocity,float nearDistance,float farDistance,
        bool& sourceReturned) noexcept=nullptr;
    bool (*start233410)(void*,OriginalAiEmitterSound&,std::uint32_t sourceFlags) noexcept=nullptr;
};
// Actual2176E0 with a genuine AI D40: reuse50FK's numeric projection, publish
// source68/64 prefix, invoke16DC50/16DCF0 at its original point, then publish
// the remaining source numeric writes. Existing actual BoostState50FK is the
// sole owner. AI vehicle+A8 is173CB0 (a genuine no-op), not a skipped callback.
// Pure arithmetic admission precedes effects. Reached backend failure leaves
// prefix and prior sound writes committed, blocks D40, and cannot be retried.
OriginalAiEmitterReceipt advanceOriginalAiThermalWithD40(
    OriginalAiEngineEmitter&,const OriginalAiEmitterRacerInput&,
    const BoostInput50FK&,const BoostParameters50FK&,BoostState50FK& actualThermal,
    const OriginalAiEmitterBoostBackend&,BoostDiagnostic50FK&) noexcept;
// Exact ordinary16DA80->16DEE0->291620/233DB0/233DF0, scalar233A80 and
//178B00->141ED0. Scalar MAX has48O signed-zero semantics. Sound pitch range
// rejection is a genuine ignored source return: it preserves54 and continues.
// Copy-only vectors preserve every word and are not numerically normalized.
//
// Stable initialized, disjoint owner/backing storage is required. Sound views
// may intentionally identify the same actual sound only when the source links
// do so; no independent duplicate-field cache may be passed. Calls are serial,
// non-reentrant; callbacks may advance genuine shared application owners/RNG.
// Any unsupported reached operation blocks this owner with earlier effects
// retained. Do not retry/reset the whole update after a partial failure.
// Native finite normal/zero arithmetic, FE_TONEAREST, no contraction required.
OriginalAiEmitterReceipt advanceOriginalAiEngineEmitter16DA80(
    OriginalAiEngineEmitter&,const OriginalAiEmitterRacerInput&,
    const OriginalAiEmitterChildPrelude&,std::size_t maximumChildren=256) noexcept;
}
