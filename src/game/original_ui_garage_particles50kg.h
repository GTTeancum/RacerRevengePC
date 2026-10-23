#pragma once
#include "../assets/original_ui_garage_scene_assets50kg.h"
#include "original_gc_spray50kg.h"
#include "original_wreck_explosion50la.h"
#include <array>
#include <memory>

namespace rr::platform {class OriginalUiGaragePodScene50KG;}
namespace rr::game {
struct OriginalRacerSceneObject50LA;
inline constexpr std::size_t originalUiGarageEmitterCount50KG=7;
enum class OriginalUiGarageEmitter50KG : std::uint8_t {
    garageSparks,pdFire,pdSmoke,garageExplosion,garageExplSparks,pdExt,pdFall
};
// Authenticated original Garage definitions. These are neither pod dust/sparks
// identities nor live Entity/material/observer/render admission receipts.
struct OriginalUiGarageEmitterDefinition50KG {
    assets::OriginalUiMemberView50KG source{};
    std::string filename,attachMcf,childName;
    std::int32_t type=0,zone=0;
    std::array<float,3> position{},offset{};
    bool startOn=false,additiveToy=false;

    OriginalGcCommonEmitterTiming50KG timing{};
    std::array<float,3> velocity60{},minimumVelocityNoise240{},maximumVelocityNoise250{};
    std::int32_t particleGroup1B8=-1;
    float particleLife17C=1000,growth180=0,initialSize184=1,emitRate188=500,ageAtBirth1A0=0;
    float gravityMultiplier=3,groundLevel=0,restitution=.55f;
    float framesPerSet=-1,inverseFramesPerSet=-1;
    std::uint32_t particleFlags198=0;
    bool randomMirrorX=false,randomMirrorY=false;
    float minimumInitialSize208=0,maximumInitialSize20C=5,initialSizeToSpeedRatio210=.01f;
    float minimumLife214=500,maximumLife218=3500,minimumPercentLife21C=.5f;
    float minimumPercentLifeConvergance220=1,lifeToSpeedRatio224=5,initialPercentAge228=0;
    float minimumEmitRate22C=0,maximumEmitRate230=10,particlesToDistanceRatio234=.01f;
    float particlesToSpeedRatio238=0,minimumEmissionSpeed23C=0;
    std::array<float,3> minRange{},maxRange{};
    float spreadVelocityScale220=0;
    bool applySpreadToVelocity=true;
    float minStreamLength=3,maxStreamLength=5,separation=1.5f,tailLife=.6f;
    float tailThickness=1,headThickness=2;
    bool superbrightHead=true,moving=false,parsed=false;
};
class OriginalUiGarageParticleAssets50KG final {
public:
    OriginalGcSprayStatus50KG load(std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG>,
        assets::OriginalUiPageDiagnostic50KG&) noexcept;
    bool loaded() const noexcept {return bool(scene_);}
    const OriginalUiGarageEmitterDefinition50KG* emitter(OriginalUiGarageEmitter50KG) const noexcept;
    assets::OriginalUiMemberView50KG atlas() const noexcept{return atlas_;}
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> scene() const noexcept{return scene_;}
private:
    std::shared_ptr<const assets::OriginalUiGarageSceneAssets50KG> scene_;
    assets::OriginalUiMemberView50KG atlas_{};
    std::array<OriginalUiGarageEmitterDefinition50KG,7> definitions_{};
};
// Borrow the actual application words, never a per-Garage serial or RNG.
// Existing Main/GC callers already receive initialized433970; use that SAME
// counter and its real lazy84C8 lifetime flag here. Explosion globals also
// remain shared with every existing15E800 emitter.
struct OriginalUiGarageParticleShared50KG {
    std::uint32_t& raw308030;
    bool& fireLazy84C8;
    std::uint32_t& fireSerial433970;
    OriginalWreckExplosionGlobals50LA& explosion;
    SharedRandomOwner50ID& lazyRandom;
    HostMonotonicClock50ID& clock;
};
// Actual renderer-linked +19C state, borrowed by pointer, not a tick snapshot.
// A genuine null19C needs an explicit binding too. When present, child04 and
// its A4 stamp must exist. The current8BE0 pointer is application-owned.
struct OriginalUiGarageEmitterRenderBinding50KG {
    const bool* attachment19CPresent=nullptr;
    const std::uint16_t* child04TagA4=nullptr;
    const std::uint16_t* currentTag8BE0=nullptr;
};
struct OriginalUiGarageParticleVisit50KG : OriginalGcSprayReceipt50KG {
    std::uint32_t emitterOrdinal=0;
    bool callerCarryUnavailable=false,renderGateSkipped=false;
};
struct OriginalUiGarageEmitterSnapshot50KG {
    EffectControlState50BC controls{};
    float delay1DC=0,fractional224=0,remainder200=0,carriedSize18C=0;
    std::uint32_t duration1E4=0,managerOwnerId=0;
    bool enabled=false,attachmentBound=false;
};
// The numeric portion of the real page+4A0 pool and seven TOY owners. The
// enclosing Entity list, observer links, material-cache reference150, render
// provider and reverse sibling retirement are still caller obligations.
class OriginalUiGarageParticles50KG final {
public:
    OriginalUiGarageParticles50KG() noexcept;
    ~OriginalUiGarageParticles50KG();
    OriginalUiGarageParticles50KG(const OriginalUiGarageParticles50KG&)=delete;
    OriginalUiGarageParticles50KG& operator=(const OriginalUiGarageParticles50KG&)=delete;
    // Actual early1B4DC0 owner calls this with bytes read through its live
    //157320 Config. This allocates the ONE pool/atlas, with no emitter or RNG.
    OriginalUiGarageParticleVisit50KG prepareManager160E00(
        std::shared_ptr<const OriginalUiGarageParticleAssets50KG>,
        const std::vector<std::uint8_t>& livePrtReadBuffer) noexcept;
    bool managerPrepared160E00()const noexcept;
    // Legacy numeric-only late composition. Reuses the same pool but does
    //not construct/interleave Entities. The authentic Scene uses phases below.
    OriginalUiGarageParticleVisit50KG prepareEmitters274370(std::uint32_t& sharedRawRandom)noexcept;
    // Genuine Scene loader uses these bounded phases, interleaved with the
    // actual Entity constructor, live EMT Config, activity and141960 attach.
    // The ordinal is source TOY order; only one unfinished row may exist.
    OriginalUiGarageParticleVisit50KG constructEmitter15A5B0(OriginalUiGarageEmitter50KG)noexcept;
    bool bindSourceAttachment15DEF0(OriginalUiGarageEmitter50KG,const SceneGraph49P&,
        SceneNodeIndex49P,std::uint32_t expectedSourceRow,const OriginalRacerSceneObject50LA&)noexcept;
    OriginalUiGarageParticleVisit50KG readEmitter15AEA0(OriginalUiGarageEmitter50KG,
        const std::vector<std::uint8_t>& liveEmtReadBuffer,std::uint32_t& sharedRawRandom)noexcept;
    OriginalUiGarageParticleVisit50KG activateEmitter27D7A0(OriginalUiGarageEmitter50KG)noexcept;
    bool finishEmitter27D75C(OriginalUiGarageEmitter50KG)noexcept;
    bool retireEmitter15A720(OriginalUiGarageEmitter50KG)noexcept;
    bool emitterRowsRetired15A720()const noexcept;
    // Bounded numeric compatibility composition, NOT the authentic Scene
    //constructor sequence. Cold preparation consumes exactly the garageSparks15B148 random draw.
    // Invoke BEFORE the page's two activation timer draws. Failure is latched
    // after publication starts; retain its mutation/RNG prefix, never retry.
    OriginalUiGarageParticleVisit50KG prepare(std::shared_ptr<const OriginalUiGarageParticleAssets50KG>,
        std::uint32_t& sharedRawRandom) noexcept;
    bool prepared() const noexcept;
    const OriginalGcParticleManager50KG* manager() const noexcept;
    bool snapshot(OriginalUiGarageEmitter50KG,OriginalUiGarageEmitterSnapshot50KG&) const noexcept;
    // Bind ONLY the authentic actor's actual source-row remap in its live
    // graph. expectedSourceRow is checked against pd_fire Plane01 or nozzle
    // root; graph index need not equal PSG index. All pointers outlive bank.
    bool bindAttachment(OriginalUiGarageEmitter50KG,const SceneGraph49P&,
        SceneNodeIndex49P,std::uint32_t expectedSourceRow,
        const OriginalUiGarageEmitterRenderBinding50KG&) noexcept;
    OriginalUiGarageParticleVisit50KG reset88(OriginalUiGarageEmitter50KG,std::uint32_t&) noexcept;
    bool enable20(OriginalUiGarageEmitter50KG) noexcept;
    bool disable18(OriginalUiGarageEmitter50KG) noexcept;
    bool start80(OriginalUiGarageEmitter50KG) noexcept;
    bool stop84(OriginalUiGarageEmitter50KG) noexcept;
    // Source order: page controls, animation manager/actors, existing pool,
    // then emitter list. No internal actor tick or position snapshot exists.
    OriginalGcParticleReceipt50KG stepPool(float elapsedSeconds,bool paused) noexcept;
    // Actual Garage page child-list numeric order. Fire's reached small-dt
    // branch (0 < accumulated carry <= 1) uses caller s1. Until that outer
    // source register is supplied by a proven application caller, this branch
    // returns unsupported after its original carry-write and lazy-global prefix. No dt clamp,
    // guessed argc/pointer, or undefined-register compatibility is provided.
    OriginalUiGarageParticleVisit50KG stepPageEmitters(float elapsedSeconds,
        bool paused,OriginalUiGarageParticleShared50KG&) noexcept;
private:
    friend class rr::platform::OriginalUiGaragePodScene50KG;
    // Only the actual detached-manager Page tail may retire an emitter while
    // its numeric pool/registry remain retained for the later160C70 callback.
    bool retainedPoolCurrentForEmitterRetirement15A720()const noexcept;
    bool retireRetainedPoolEmitter15A720(OriginalUiGarageEmitter50KG)noexcept;
    OriginalGcParticleStatus50KG normalizeAtlas161234(float,float)noexcept;
    //Actual Entity traversal selects the row; Runtime.enabled is not an
    //authority. A complete common prefix is not a derived callback return.
    OriginalUiGarageParticleVisit50KG updateSourceCommon15A7A0(
        OriginalUiGarageEmitter50KG,float,bool,std::uint32_t&)noexcept;
    // Only the real Scene Entity walk, immediately after its successful
    // non-skipped common call. Never repeat common or select Runtime.enabled.
    // Type11 uses raw alone; type0 borrows the process selector/clock owners.
    // Explosion borrows the single process globals; fire reads live Visibility below.
    OriginalUiGarageParticleVisit50KG continueSourceEmitterAfter15A7A0(
        OriginalUiGarageEmitter50KG,float,std::uint32_t&,SharedRandomOwner50ID&,
        HostMonotonicClock50ID*,OriginalWreckExplosionGlobals50LA&)noexcept;
    //Velocity stores precede the live attachment/observer/tag read. The
    //actual Scene owns that read between these two private numerical parts.
    OriginalUiGarageParticleVisit50KG beginSourceFire15E10C(OriginalUiGarageEmitter50KG,
        float,const OriginalRacerSceneObject50LA*&)noexcept;
    OriginalUiGarageParticleVisit50KG finishSourceFire15E1D0(OriginalUiGarageEmitter50KG,
        float,std::uint32_t&,std::uint32_t&,bool&)noexcept;

    struct Storage;std::unique_ptr<Storage> storage_;
};
} // namespace rr::game
