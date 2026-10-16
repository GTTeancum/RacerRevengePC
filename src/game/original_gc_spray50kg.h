#pragma once
#include "effect_control50bc.h"
#include "scene_graph49p.h"
#include "original_gc_particle_manager50kg.h"
#include "../assets/original_ui_pages50kg.h"
#include <array>
#include <cstdint>
#include <vector>

namespace rr::game {
//19DA30 then15AEA0. Values below are source reader defaults, not a substitute
// for the authored member. The raw authenticated member must be parsed first.
struct OriginalGcSprayDefinition50KG {
    std::array<float,3> minRange{},maxRange{},positionOffset{},velocity{};
    float minStreamHeight=1,maxStreamHeight=3,sizeModifier=.5f;
    float fillerLifeModifier=.4f,fillerSizeModifier=.8f;
    float fillerVelocityModifier=.5f,sideVelocityModifier=2.5f;
    std::int32_t particleGroup=-1,emitMode=0;
    float particleLife=1000,initialSize=1,growth=0,emitRate=500,ageAtBirth=0;
    float delayTime=0,groundLevel=0,gravityMultiplier=3,restitution=.55f;
    float minTimeOn=0,maxTimeOn=0,minTimeOff=0,maxTimeOff=0;
    std::uint32_t particleFlags=0;
    bool randomMirrorX=false,randomMirrorY=false;
    float framesPerSet=-1,inverseFramesPerSet=-1;
    std::vector<std::int32_t> forcedSets;
};
enum class OriginalGcSprayStatus50KG : std::uint8_t {
    complete,unprepared,alreadyPrepared,assetIdentity,authoredData,unsupported,
    arithmetic,storage,capacity,clock,sourceRangeAssertion,allocation
};
struct OriginalGcSprayReceipt50KG {
    OriginalGcSprayStatus50KG status=OriginalGcSprayStatus50KG::unprepared;
    std::uint32_t sourcePC=0,randomDraws=0,spawned=0;
    bool commonSkipped=false,poolFull=false,deadZeroCountReciprocal=false;
    bool okay() const noexcept{return status==OriginalGcSprayStatus50KG::complete;}
};
// Common15A7A0 source timing fields. Cold parser takes caller-authenticated
// EMT bytes; it does not authenticate an arbitrary subclass on the caller's
// behalf. Ranges are immutable authoring. Controls/delay/duration are BORROWED
// canonical mutable words, including an existing EffectOwner50BE duration.
struct OriginalGcCommonEmitterTiming50KG {
    std::uint32_t mode1D8=0;
    float delayTime1E0=0,minTimeOn1E8=0,maxTimeOn1EC=0,minTimeOff1F0=0,maxTimeOff1F4=0;
};
OriginalGcSprayStatus50KG readOriginalGcCommonEmitterTiming50KG(
    const std::vector<std::uint8_t>& callerAuthenticatedEmt,
    OriginalGcCommonEmitterTiming50KG&,assets::OriginalUiPageDiagnostic50KG&) noexcept;
struct OriginalGcCommonEmitterDispatch50KG {
    void* context=nullptr;
    bool (*start80)(void*,EffectControlState50BC&)=nullptr;
    bool (*stop84)(void*,EffectControlState50BC&)=nullptr;
};
// Actual common+74 scene owner and source+50 offset. Borrow the live graph;
// no cached position snapshot or transform-completion callback is accepted.
struct OriginalGcCommonAttachment50KG {
    const SceneGraph49P* graph=nullptr;
    SceneNodeIndex49P node=SceneNodeIndex49P::none;
    std::array<float,4> offset50{{0,0,0,1}};
};
// False commonSkipped on success means the concrete derived update is due.
// A present attachment74 requires its actual live graph/offset below. It
// performs native28B1A0/28B9B0 before delay/mode checks, after pause/first-update.
// Virtual callbacks bind the actual subtype. Missing callbacks fail only when
// that call is reached; generic stopped/paused/first-update paths need none.
OriginalGcSprayReceipt50KG advanceOriginalGcCommonEmitter50KG(
    EffectControlState50BC& canonicalControls,const OriginalGcCommonEmitterTiming50KG&,
    float& canonicalDelay1DC,std::uint32_t& canonicalDuration1E4,
    bool attachment74Present,float elapsedSeconds,bool applicationPaused,
    std::uint32_t& sharedRawRandom,
    const OriginalGcCommonEmitterDispatch50KG* virtualCalls=nullptr,
    const OriginalGcCommonAttachment50KG* attachment=nullptr) noexcept;
// This parser authenticates the original1027-byte podSpray.emt member and
// covers common/derived source defaults before owner publication. No file
// IO, archive extraction, asset decoding, guest execution or isolated RNG.
OriginalGcSprayStatus50KG readOriginalGcSprayDefinition50KG(
    const std::vector<std::uint8_t>& originalEmt,OriginalGcSprayDefinition50KG&,
    assets::OriginalUiPageDiagnostic50KG&) noexcept;
class OriginalGcSprayOwner50KG final {
public:
    OriginalGcSprayOwner50KG() noexcept=default;
    OriginalGcSprayOwner50KG(const OriginalGcSprayOwner50KG&)=delete;
    OriginalGcSprayOwner50KG& operator=(const OriginalGcSprayOwner50KG&)=delete;
    // Actual vehicle211E60/211F00 supplies opposite Y side directions and
    // null attachment74/relative offset. This bounded loader admits precisely
    // that unattached subtype; it does not manufacture a null spray on failure.
    OriginalGcSprayReceipt50KG load(OriginalGcParticleManager50KG&,
        const std::vector<std::uint8_t>& originalEmt,
        const std::array<float,3>& direction,std::uint32_t& sharedRawRandom) noexcept;
    // Canonical aliases: +204 is the source fractional accumulator, not an
    // independent visual intensity. All22A620 writes and Start/Stop calls must
    // use this same controls() record. No mirrored control bank is maintained.
    EffectControlState50BC& controls() noexcept{return controls_;}
    const EffectControlState50BC& controls() const noexcept{return controls_;}
    std::uint32_t ownerId() const noexcept{return ownerId_;}
    bool loaded() const noexcept{return manager_&&ownerId_;}
    const OriginalGcSprayDefinition50KG& definition() const noexcept{return definition_;}
    // Executes source15A7A0 common and19CED0 derived update in caller order.
    // Failed receipts retain the completed source mutation/RNG/particle prefix;
    // never retry an update after failure without the enclosing owner policy.
    OriginalGcSprayReceipt50KG update(float elapsedSeconds,bool applicationPaused,
        std::uint32_t& sharedRawRandom,SharedRandomOwner50ID&,
        HostMonotonicClock50ID&) noexcept;
private:
    EffectControlState50BC controls_{};
    OriginalGcSprayDefinition50KG definition_{};
    OriginalGcParticleManager50KG* manager_=nullptr;
    std::uint32_t ownerId_=0;
    std::array<float,3> sideDirection_{};
    std::int32_t firstFrame_=0,lastFrame_=0,currentFrame_=0;
    float delay1DC_=0,carriedSize18C_=0;
    std::uint32_t duration1E4_=0;
};
} // namespace rr::game
