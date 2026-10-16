#pragma once
#include "original_gc_particle_manager50kg.h"
#include "body_scene48g.h"
#include <array>
#include <memory>

namespace rr::game {
struct OriginalGcFireAttachment50KG {
    BodyMatrix48G actualWorldMatrix{};
    // Actual shade SoulTree+19C presence and linked render owner+4+A4 stamp.
    // Nonnull mismatched stamp returns before emission/RNG at15E1C8.
    bool renderOwnerPresent=false;
    std::uint16_t renderStamp=0;
};
struct OriginalGcFireReceipt50KG {
    OriginalGcParticleStatus50KG status=OriginalGcParticleStatus50KG::unprepared;
    std::uint32_t spawned=0,randomDraws=0,firstUpdatesSkipped=0,renderGateSkips=0;
    bool poolFull=false,nativeUndefinedCarryPolicy=false;
    bool okay() const noexcept{return status==OriginalGcParticleStatus50KG::complete;}
};
// Six actual attached genratorfire.emt owners in GC TOB emitter1 order. No
// private pool, raw RNG, fire serial, synthetic placement or Main UI owner.
class OriginalGcGeneratorFireOwner50KG final {
public:
    OriginalGcGeneratorFireOwner50KG() noexcept;
    ~OriginalGcGeneratorFireOwner50KG();
    OriginalGcGeneratorFireOwner50KG(const OriginalGcGeneratorFireOwner50KG&)=delete;
    OriginalGcGeneratorFireOwner50KG& operator=(const OriginalGcGeneratorFireOwner50KG&)=delete;
    // Register these six owners BEFORE manager.populateColdFog().
    OriginalGcParticleStatus50KG prepare(OriginalGcParticleManager50KG&,
        const std::vector<std::uint8_t>& originalGeneratorFireEmt) noexcept;
    // Attached matrices and rendering stamps come from genuine scene owners.
    // sharedFireSerial is the application433970 value, already initialized by
    // its application lifetime owner (shared with any preceding Main fires).
    // Explicit compatibility policy only for undefined15E288..298 carry:
    // false rejects; true uses the existing Main integral/fraction policy and
    // reports it. Neither choice claims recovered original-register contents.
    OriginalGcFireReceipt50KG update(float elapsedSeconds,bool paused,
        const std::array<OriginalGcFireAttachment50KG,6>& attachments,
        std::uint16_t actualSceneFrameStamp,std::uint32_t& sharedRawRandom,
        std::uint32_t& sharedFireSerial,bool allowNativeUndefinedCarryPolicy=false) noexcept;
    bool prepared() const noexcept;
private:
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
