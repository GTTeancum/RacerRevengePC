#pragma once
#include "original_ui_main_scene50kg.h"
#include <array>
#include <memory>

namespace rr::game {
constexpr std::size_t originalUiMainParticleCapacity50KG=5000; //1B4E60
constexpr std::size_t originalUiMainDepthBuckets50KG=2500;    //160E44
struct OriginalUiMainParticle50KG {
    std::array<float,3> position{},velocity{};
    std::int32_t frame=0;
    float ageMilliseconds=0,lifeMilliseconds=0,inverseLife=0;
    float size=0,initialSize=0,growth=0;
    std::uint32_t flags=0;
};
struct OriginalUiMainParticlePacket50KG {
    std::array<float,3> position{};
    // Source162248 packs runs of <=12. Only the first particle of a run has
    // nonzero signedRunCount; negative selects the additive material state.
    std::int32_t signedRunCount=0;
    float sizeTimesCamera1D8=0,quantizedSourceDepth=0,sourceBrightness=0;
    std::array<std::array<float,2>,4> normalizedUv{};
    std::uint16_t sourceParticleIndex=0,sourceFrame=0,sourceDepthBucket=0;
};
struct OriginalUiMainParticleView50KG {
    // Third column of the actual original camera+250 transform, NOT a
    // conventional normalized clip-Z matrix or a guessed camera distance.
    std::array<float,4> sourceDepthColumn{};
    float camera1D8=0;
};
enum class OriginalUiMainFireStatus50KG : std::uint8_t {
    complete,unprepared,alreadyInitialized,definition,arithmetic,delta,
    storage,allocation
};
struct OriginalUiMainFireReceipt50KG {
    OriginalUiMainFireStatus50KG status=OriginalUiMainFireStatus50KG::unprepared;
    std::uint32_t randomDraws=0,spawned=0,expired=0;
    bool skippedFirstUpdate=false,poolFull=false,nativeUndefinedCarryPolicy=false;
    bool okay() const noexcept{return status==OriginalUiMainFireStatus50KG::complete;}
};
// Bounded actual Main stationary/unattached fire owner. Native scalar data,
// no guest execution or generic substitute particles. Serial use only.
class OriginalUiMainFireOwner50KG final {
public:
    OriginalUiMainFireOwner50KG() noexcept;
    ~OriginalUiMainFireOwner50KG();
    OriginalUiMainFireOwner50KG(const OriginalUiMainFireOwner50KG&)=delete;
    OriginalUiMainFireOwner50KG& operator=(const OriginalUiMainFireOwner50KG&)=delete;
    OriginalUiMainFireStatus50KG initialize(const OriginalUiMainSceneOwner50KG&) noexcept;
    // Same source emitter equations, explicitly admitted actual Options owner.
    OriginalUiMainFireStatus50KG initializeOptionsMenu(const OriginalUiMainSceneOwner50KG&) noexcept;
    // Separate source subscribers: caller must preserve actual manager order.
    // No manufactured fixed tick, elapsed clamp, private seed, or reseeding.
    // Shared RNG is the SAME application raw308030 state as other consumers.
    // sharedFireSerial is original433970 process-wide counter, not per emitter.
    OriginalUiMainFireReceipt50KG emit(float elapsedSeconds,bool paused,
        std::uint32_t& sharedRawRandom,std::uint32_t& sharedFireSerial) noexcept;
    OriginalUiMainFireReceipt50KG advance(float elapsedSeconds,bool paused) noexcept;
    // Source162870 depth buckets followed by162230 packet semantics. This
    // produces actual ParticleCode inputs, NOT completed native GPU billboards.
    // Native ParticleCode expansion, lighting, blend/depth state and traversal
    // remain separate joins. No guessed shader state is hidden in this owner.
    OriginalUiMainFireStatus50KG preparePackets(const OriginalUiMainParticleView50KG&) noexcept;
    std::size_t particleCount() const noexcept;
    const OriginalUiMainParticle50KG* particles() const noexcept;
    std::size_t packetCount() const noexcept;
    const OriginalUiMainParticlePacket50KG* packets() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
    OriginalUiMainFireStatus50KG initializeScene(const OriginalUiMainSceneOwner50KG&,bool options) noexcept;
};
}
