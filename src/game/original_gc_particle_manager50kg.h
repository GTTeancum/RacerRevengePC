#pragma once
#include "shared_random_owner50id.h"
#include "lap_host_clock50id.h"
#include "original_ui_pages50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace rr::game {
constexpr std::size_t originalGcParticleCapacity50KG=5000;
constexpr std::size_t originalGcParticleDepthBuckets50KG=2500;
// Native source fields. ownerId and nextSlot are native registry/slot handles,
// never guest addresses; padding lanes are retained rather than packed away.
struct OriginalGcParticle50KG {
    std::array<float,3> position{}; float positionW=1;
    std::array<float,3> velocity{}; float velocityW=1;
    float depth=0; std::uint32_t reserved24=0;
    std::int32_t frame=0;
    float ageMilliseconds=0,lifeMilliseconds=0,inverseLife=0;
    float size=0,initialSize=0,growth=0;
    std::uint32_t flags=0,ownerId=0;
    std::int32_t nextSlot=-1;
};
static_assert(sizeof(OriginalGcParticle50KG)==0x50,"native source particle field layout");
static_assert(offsetof(OriginalGcParticle50KG,ownerId)==0x48,"native owner handle field");
struct OriginalGcParticleGroup50KG {std::int32_t type=0,first=0,last=0;};
struct OriginalGcParticleFrame50KG {std::array<std::array<float,2>,4> sourceUv{};};
struct OriginalGcParticleEmitterState50KG {
    std::int32_t particleGroup=0;
    float framesPerSet=0,inverseFramesPerSet=0;
    float gravity=0,bouncePlane=0,bounceFactor=0;
    float fogPeriod=-1,fogPhase=0,fogPhaseRatio=0;
    bool fogIncreasing=true;
};
// Source160E24..40 successfully constructs this optional auxiliary owner.
//162F14 initializes its +18 gate to zero. This native slice owns the dormant
// gate explicitly; it never substitutes a null child for the constructed one.
struct OriginalGcParticleAuxiliaryChild50KG {
    bool constructed=true;
    std::uint32_t enabled18=0;
};
enum class OriginalGcParticleStatus50KG : std::uint8_t {
    complete,unprepared,alreadyPrepared,assetIdentity,authoredData,unsupported,
    arithmetic,delta,storage,capacity,clock,sourceRangeAssertion,allocation
};
// Requested registry element payload only. This is neither host heap usage
// nor source Entity/component storage. Peak includes overlapping old/new
// vector requests; after binding it starts with the already retained block.
struct OriginalGcEmitterRegistryReservation50KG {
    std::size_t registeredEmitters=0,reservedEmitters=0;
    std::size_t retainedPayloadBytes=0,peakPayloadBytes=0,reservedPeakPayloadBytes=0;
    bool allocationBound=false,reservationAttempted=false,terminal=false;
};
struct OriginalGcParticleReceipt50KG {
    OriginalGcParticleStatus50KG status=OriginalGcParticleStatus50KG::unprepared;
    std::uint32_t randomDraws=0,spawned=0,expired=0;
    bool poolFull=false;
    bool okay() const noexcept{return status==OriginalGcParticleStatus50KG::complete;}
};
// One native scene+4A0 particle provider shared by authored volumes and race
// effects. This owns the real numeric pool and atlas; it does not certify the
// separate160B10 Entity/component, global counter,160E00 material-reference150,
// renderer scratch or outer scene attachment. Those remain their actual
// callers' obligations. Never substitute atlas readiness for those constructors.
class OriginalGcParticleManager50KG final {
public:
    OriginalGcParticleManager50KG() noexcept;
    ~OriginalGcParticleManager50KG();
    OriginalGcParticleManager50KG(const OriginalGcParticleManager50KG&)=delete;
    OriginalGcParticleManager50KG& operator=(const OriginalGcParticleManager50KG&)=delete;
    OriginalGcParticleStatus50KG prepare(const std::vector<std::uint8_t>& demoprtPrt,
        const std::vector<std::uint8_t>& fogEmt,const std::vector<std::uint8_t>& gcTob) noexcept;
    // The original PodScene1B4E60 uses5000 slots for every admitted track.
    // Caller supplies the exact PRT member from its authenticated track owner.
    // Reads the successful1617A0 atlas route and prepares the SAME numeric
    // pool, without inventing GC fog/fire instances, texture/material readiness,
    // resource-cache chronology or the source fallback61-frame atlas.
    // Nonempty explicit groups/frames are required;4096 is a native storage
    // admission bound, not a recovered original PRT count or scene constant.
    // Transactional and one-shot. No decoder or file lookup occurs here.
    OriginalGcParticleStatus50KG prepareTrackAtlas160E00(
        const std::vector<std::uint8_t>& authenticatedPrt) noexcept;
    bool prepared() const noexcept;
    bool gcFogAuthoringPrepared() const noexcept;
    const std::string* atlasTextureMapName() const noexcept;
    const OriginalGcParticleGroup50KG* group(std::int32_t index) const noexcept;
    const OriginalGcParticleFrame50KG* frame(std::int32_t index) const noexcept;
    const std::array<float,3>* generatorFireAttachmentOffset() const noexcept;
    OriginalGcParticleAuxiliaryChild50KG* auxiliaryChild() noexcept;
    const OriginalGcParticleAuxiliaryChild50KG* auxiliaryChild() const noexcept;
    std::uint32_t registerEmitter(const OriginalGcParticleEmitterState50KG&) noexcept;
    // Additive native storage policy, opt-in and one attempt. Before calling,
    // the actual shared cold ledger must reserve reservedPeakPayloadBytes
    // in total for THIS registry, including any already charged capacity.
    // Keep that quota charged until this manager is destroyed, including on
    // failure. Never charge actual capacity again on top of the quota.
    // The allocator checks its actual request BEFORE upstream allocation;
    // old and new blocks remain charged together until old deallocation.
    // Successful reserve guarantees additionalEmitters allocation-free
    // appends only while no intervening caller consumes that free capacity.
    // Later legacy registerEmitter calls share the SAME lifetime quota.
    // No source row, RNG, particle or component is created by reservation.
    // Reservation/allocation failure after opt-in latches terminal, but
    // existing rows/particles remain available for ordered retirement.
    OriginalGcParticleStatus50KG reserveAdditionalEmitters(
        std::size_t additionalEmitters,std::size_t reservedPeakPayloadBytes,
        OriginalGcEmitterRegistryReservation50KG&) noexcept;
    OriginalGcEmitterRegistryReservation50KG emitterRegistryReservation() const noexcept;
    std::size_t emitterRegistryPayloadReservationBytes() const noexcept;
    // Conservative incremental Storage bookkeeping allowance. Reserve once
    // before prepare/prepareTrackAtlas allocates Storage; it is separate from
    // the element quota. Existing particle/atlas domains remain unchanged.
    static std::size_t emitterRegistryReservationMetadataBytes() noexcept;
    // Actual retained registry allocation; observation only, no reservation.
    std::size_t emitterStoragePayloadBytes() const noexcept;
    // Read-only actual retained data payload. Includes fixed5000 pool/order,
    // atlas vectors/string and emitter capacity, not allocator overhead or
    // a reservation receipt. Do not charge registry capacity atop its quota.
    std::size_t retainedStoragePayloadBytes() const noexcept;
    OriginalGcParticleEmitterState50KG* emitter(std::uint32_t ownerId) noexcept;
    const OriginalGcParticleEmitterState50KG* emitter(std::uint32_t ownerId) const noexcept;
    // Source pointers138[count128], then count128++. Reserve does not publish.
    // Serial callers only. A failed emitter may retain an unpublished record.
    OriginalGcParticle50KG* nextParticle() noexcept;
    bool commitParticle(std::uint32_t ownerId) noexcept;
    std::size_t particleCount() const noexcept;
    OriginalGcParticle50KG* particleAt(std::size_t activeIndex) noexcept;
    const OriginalGcParticle50KG* particleAt(std::size_t activeIndex) const noexcept;
    // Invoke after the six authored attached-fire owners are registered. This
    // constructs and populates fog2, fog3, fog4 in their actual TOB order.
    OriginalGcParticleReceipt50KG populateColdFog(std::uint32_t& sharedRawRandom,
        SharedRandomOwner50ID&,HostMonotonicClock50ID&) noexcept;
    // 161180 pause gate, then1613A8 active-pointer swap removal and flag paths.
    OriginalGcParticleReceipt50KG advance(float elapsedSeconds,bool paused) noexcept;
    // Caller invokes after manager advance in the original scene child order.
    // 160160 has no early pause gate: do not add the manager's pause gate here.
    OriginalGcParticleStatus50KG advanceFogPhases(float elapsedSeconds) noexcept;
    float fogBrightness(const OriginalGcParticle50KG&,
        const std::array<float,3>& activeCameraPosition) const noexcept;
    bool coldFogPopulated() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
// Source308030 numeric operation; caller lends the actual process/session raw
// state. No private seed. Other original consumers may use this same helper.
float drawOriginalGcParticleRandom50KG(std::uint32_t& sharedRawRandom) noexcept;
// Source lazy guard80A8/cache3D6B30. Zero clock values intentionally reseed
// again on the next selection. This uses the existing shared application owner.
bool seedOriginalGcParticleRandom50KG(std::uint32_t& sharedRawRandom,
    SharedRandomOwner50ID&,HostMonotonicClock50ID&) noexcept;
// Source lazy range guard80AC/cache3D6B38, retained across all consumers.
bool checkOriginalGcParticleRandomRange50KG(std::int32_t selected,
    std::int32_t first,std::int32_t last,SharedRandomOwner50ID&) noexcept;
// Cold byte identity and bounded text reader, shared with actual GC effect
// authoring. Source159440 retains bracket/quote whitespace.
bool readOriginalGcParticleText50KG(const std::vector<std::uint8_t>& bytes,
    std::size_t expectedByteCount,const char* expectedSha256,
    std::vector<assets::OriginalUiDatSection50KG>& output) noexcept;
} // namespace rr::game
