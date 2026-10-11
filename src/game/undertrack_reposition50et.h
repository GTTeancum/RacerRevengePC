#pragma once
#include "scene_graph49p.h"
#include "main_reset50ar.h"
#include "reset_lifecycle50at.h"
#include <cstddef>
#include <cstdint>

namespace rr::game {
// These are native storage bounds, not claimed limits of TunnelTrack.
constexpr std::size_t maxUndertrackNodes50ET=256;
constexpr std::size_t maxUndertrackNext50ET=8;
constexpr std::size_t maxUndertrackText50ET=65536;
struct UndertrackNode50ET {
    float position[4]{};
    std::uint32_t next[maxUndertrackNext50ET]{};
    std::uint32_t nextCount=0;
};
struct UndertrackTrack50ET {
    UndertrackNode50ET nodes[maxUndertrackNodes50ET]{};
    std::uint32_t count=0;
};
struct UndertrackReadScratch50ET {
    UndertrackTrack50ET track{};
    std::uint8_t seen[maxUndertrackNodes50ET]{},fields[maxUndertrackNodes50ET]{},
        nextFields[maxUndertrackNodes50ET]{};
};
enum class UndertrackError50ET : std::uint32_t {
    None,Storage,RoundingMode,Input,Limit,Syntax,Duplicate,Number,Missing,
    Track,Arithmetic,MissingNodeIndex,MissingApplicationTable,Scene
};
struct UndertrackDiagnostic50ET {
    UndertrackError50ET error=UndertrackError50ET::None;
    std::uint32_t sourcePC=0;
    std::size_t textOffset=0;
};
// Cold projection of original2A1260 Position,NumNextNodes,ordered NextN.
// Uses50AD's original decimal conversion. No StartNode restriction: reset uses
// live progression, never the grid. Reverb/width/unconsumed authoring is ignored.
// No node+78 index is invented:1ACF70 supplies it AFTER the authored loader.
// Distinct backed records; false preserves output, scratch may change.
bool readUndertrackTrack50ET(const char*,std::size_t,UndertrackReadScratch50ET&,
    UndertrackTrack50ET&,UndertrackDiagnostic50ET&) noexcept;

// Sole caller-owned projection of the three source progression words needed
// here. Constructor/grid values are not a substitute for their live producer.
struct UndertrackProgress50ET {
    std::uint32_t vehicleC80=0,vehicleC88=0,vehicleC8C=0;
};
struct UndertrackInput50ET {
    std::uint32_t vehicle58=0;
    float worldPosition[4]{},localPosition[4]{},lift7EC=0;
};
struct UndertrackSelection50ET {
    std::uint32_t anchorNode=0,selectedNextNode=0;
    bool writesC80=false;
    std::uint32_t nextC80=0;
    float anchorPosition[4]{},positionBC0[4]{},headingBD0[4]{};
    // Source225990..225A08 computes this, then never uses it for placement.
    // Keeping it makes the source's arithmetic domain explicit.
    float unusedInterpolatedPosition[4]{};
};
// Original2255B0 geometry through225AD0, before persistent stores/callbacks.
// vehicle58==0 uses C88/C8C and the map track; otherwise C80 and AI Next0.
// Candidate order and strict-greater ties match225758..225898. Reset position
// is anchor+lift, NOT a nearest-point projection or a start-grid teleport.
// worldPosition is28B2B0 AFTER its required cache refresh; localPosition is the
// later28B180 copy. The frame caller must derive both from its canonical scene.
// FE_TONEAREST/strict separate binary32, finite operands/intermediates; positive
// subnormal VU norms and zero scalar divisor are explicit unsupported domains.
// VU normalization of a true zero retains its original saturated-Q behavior.
// All input/output/diagnostic records distinct; no heap or scene mutation.
bool selectUndertrackReposition50ET(const UndertrackTrack50ET&,
    const UndertrackProgress50ET&,const UndertrackInput50ET&,
    UndertrackSelection50ET&,UndertrackDiagnostic50ET&) noexcept;

struct UndertrackTableBindings50ET {
    // Actual post1ACF70 per-node bytes, not NodeReverb or zone guesses.
    const std::uint8_t* nodeIndex78=nullptr;
    std::size_t nodeCount=0;
    const std::uint32_t* application4C0=nullptr;
    const std::uint32_t* application4C4=nullptr;
    std::size_t tableCount=0;
};
struct UndertrackPlacement50ET {
    float positionBC0[4]{},headingBD0[4]{};
    std::uint32_t vehicleC30=0,vehicleC34=0;
    std::uint8_t index78=0;
};
// Exact225A38..225AEC values; unavailable bindings reject without output stores.
// This is preparation, NEVER completion of virtual78/1A24F0/216A30 or camera.
bool bindUndertrackPlacement50ET(const UndertrackSelection50ET&,
    const UndertrackTableBindings50ET&,UndertrackPlacement50ET&,
    UndertrackDiagnostic50ET&) noexcept;

struct UndertrackPlayerPrefix50ET {
    std::uint32_t counter109C=0;
    std::uint8_t flags10CC=0;
};
// Exact1A2508..1A2574 for this action's a1=0. Caller must next execute1A0240
// when vehicle3C==0, then216A30(a1=0,a2=1); this leaf never claims that work.
void resetUndertrackPlayerPrefix50ET(UndertrackPlayerPrefix50ET&) noexcept;

struct UndertrackController50ET {
    std::uint32_t field474=0,field4AC=0,field484=0,field490=0,
        field494=0,field498=0,field49C=0,field4A0=0,field4A8=0;
    std::uint8_t flags4A4=0,field4A5=0,flags22C=0;
};
// Complete1A0240 (no calls), reached with the actual D40 owner when vehicle3C
// is zero. This source-owned projection must be retained by that controller.
void resetUndertrackController50ET(UndertrackController50ET&) noexcept;
//216A50..216A84: present D40 clears22C bit2 and its associated D44 receives
//2C2810(a1=0), which stores24C=0 and skips its virtualA8. Missing D44 for a
//present D40 returns false AFTER the216A78 flags store; retain that prefix.
//Overlapping fields reject before entry. Null D40 skips the entire branch
//without reading/writing D44. Caller must establish actual owner presence;
//an unbound owner is not source-proved absence.
bool resetUndertrackBasePrefix50ET(std::uint8_t* d40Flags22C,
    std::uint32_t* d44Field24C) noexcept;

struct UndertrackCameraMotion50ET {std::uint32_t field180[4]{},field1A0[4]{};};
// Complete17BCE0, not178860. Copies the caller's actual shared433DB0 packet;
// the following camera scene registration and virtual18 are still required.
void resetUndertrackCameraMotion50ET(UndertrackCameraMotion50ET&,
    const std::uint32_t shared433DB0[4]) noexcept;

// Basis216AF0..216B34 and parentless28A090(t0=0,t1=1,t2=0). Position must be
// the actual BC0 for this source point (including any earlier terrain/water
// adjustment, or the completed ground-probe height for the second placement).
// This is independent of the starting grid and adds no lift a second time.
bool prepareUndertrackResetPose50ET(const float positionBC0[4],
    const float headingBD0[4],const float sharedUp434650[4],BodyMatrix48G&,
    UndertrackDiagnostic50ET&) noexcept;
struct UndertrackSceneWorkspace50ET {SceneNode49P nodes[maxSceneNodes49P];};
enum class UndertrackSceneOrder50ET {OrientationThenPosition,PositionThenOrientation};
// Concrete adjacent28A090/28ADD0 calls in the specified source order, retaining
// both invalidations and the current translation during orientation. No bounds,
// body synchronization, eager world refresh, observer, or frame advancement.
// Parentless target only; graph/workspace/pose/diagnostic are distinct.
bool setUndertrackResetScene50ET(const SceneGraph49P&,SceneNodeIndex49P,
    const BodyMatrix48G& preparedPose,UndertrackSceneWorkspace50ET&,
    UndertrackDiagnostic50ET&,
    UndertrackSceneOrder50ET=UndertrackSceneOrder50ET::OrientationThenPosition) noexcept;

// Contiguous216C38..216CE4 after the initial190/198 scene operations. Resets the
// real main body and lifecycle owners using50AR/50AT. Does NOT reset secondary
// before its intervening scene placement, nor overwrite body poses/phase tags.
// Records are disjoint initialized storage; globals are the actual shared ones.
// False (invalid lifecycle globals) preserves all mutable records.
bool resetUndertrackMainBlock50ET(MainBodyState49K&,MainSphereLanes49M&,
    SecondaryActivationSupplement49V&,std::uint32_t& vehicle838,
    ChariotLifecycleState49V&,const SecondaryActivationGlobals49V&,
    const ResetLifecycleGlobals50AT&) noexcept;
// Exact217250..21725C immediately AFTER the second main-body reset and BEFORE
//effect Stop84 calls. Clear only source850; copy all four shared zero words.
void resetUndertrackPriorPoint50ET(MainBodyState49K&,std::uint32_t& vehicle850,
    const std::uint32_t shared434630[4]) noexcept;
// Exact2173F8, AFTER effect stops. This action's a1=0 skips the preceding
//damage/player/grid/engine-timer reset block; D28 alone is always cleared.
void finishUndertrackControlHistory50ET(MainBodyState49K&) noexcept;
}
