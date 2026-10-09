#pragma once
#include "undertrack_reposition50et.h"
#include "live_zone_membership50en.h"
#include "source_acos49t.h"

namespace rr::game {
// All APIs require backed/aligned, mutually disjoint records and borrowed spans,
// except the explicitly in-place state argument. Calls are serial. Output views
// retain caller-owned lifetimes; no allocation, I/O, guest memory or callbacks.

struct TrackNavigationNode50EU {
    float gate10[4]{},gate20[4]{},wideGate30[4]{},wideGate40[4]{};
    float height50=300.f,left54=0.f,right58=0.f;
    std::int32_t branch68=0;
    std::uint8_t flags79=0;
};
struct TrackNavigation50EU {
    UndertrackTrack50ET track{};
    TrackNavigationNode50EU nodes[maxUndertrackNodes50ET]{};
    std::uint8_t nodeIndex78[maxUndertrackNodes50ET]{};
    bool gatesPrepared=false,indicesPrepared=false;
};
struct TrackNavigationScratch50EU {
    UndertrackReadScratch50ET reader{};
    TrackNavigation50EU candidate{};
    std::uint32_t fieldMask[maxUndertrackNodes50ET]{};
};
enum class TrackNavigationError50EU : std::uint8_t {
    None,Input,Text,Track,Arithmetic,Traversal,Portal,MissingPredecessor,NotPrepared,EffectBoundary
};
struct TrackNavigationDiagnostic50EU {
    TrackNavigationError50EU error=TrackNavigationError50EU::None;
    std::uint32_t sourcePC=0,node=0;
};
// Original2A1260/2A1D80 cold projection, including NodeHeight, authored widths,
// BranchID, SkipPlayerTest/Pretty flags and ordered predecessor overwrites of
// gates. Reuses50ET's graph reader and exact49T acos/49U rotation. Node0-based
// documents only, same bounded lexical domain as50ET. No host acos/sin/cos.
// Caller supplies the actual serial libm owner. Failure preserves it/output.
bool readTrackNavigation50EU(const char*,std::size_t,SourceLibmState49T&,
    TrackNavigationScratch50EU&,TrackNavigation50EU&,
    TrackNavigationDiagnostic50EU&) noexcept;

// Concrete1ACF70 index producer. Uses50EN's source-ordered collision portals,
// actual2A26E0 predecessor selection, and two280D40 primary-zone walks per
// node. Node0 retains the explicit2A0F30 constructor byte0. Both L-shaped
// segments retain230BD0 sign-bit/tie rules. No point-in-zone/nearest fallback.
// Pure projection of final index effects: temporary radius1 registry entries
// created/removed by1ACF70 cannot change primary-zone selection and are omitted.
// Failure preserves track; world topology and portal caches are read-only.
bool bindTrackNavigationZones50EU(const LiveZoneWorld50EN&,
    TrackNavigation50EU&,TrackNavigationDiagnostic50EU&) noexcept;

struct TrackNavigationProgress50EU {
    UndertrackProgress50ET reset{};
    std::uint32_t vehicleC84=0;
};
struct TrackNavigationInput50EU {
    float previousCA0[4]{},currentLocal190[4]{};
    std::uint32_t mainBody104=0;
};
struct TrackNavigationReceipt50EU {
    std::uint32_t aiSequential=0,mapSequential=0,aiScan=0,mapScan=0;
};
// Source217394..2173B8 full-initialization words. Never call during undertrack
// reset(a1=0): that path deliberately preserves live C80/C84/C88/C8C.
void initializeTrackNavigationProgress50EU(TrackNavigationProgress50EU&) noexcept;
// Player(vehicle58==0)1A0DA0 progression-word projection. CA0 is228724..730's
// copy of vehicle340->288 LOCAL translation before simulation; current is190's
// later LOCAL translation. Source1A08A0/1A0AA0 use XZ segment intersections and
// height limits, NOT nearest waypoints or frame displacement guesses.
// Advances ordered Next0, then only if no sequential advance scans all nodes
// in ascending authored order with original branch/SkipPlayerTest gating.
// This owns ONLY progression words. Lap/audio/shortcut-flag effects are not
// claimed; their fields do not feed the represented index stores. AI vehicle
//170000/1B282C behavior is not represented by this player-specific API.
// Failure preserves progress/receipt; finite strict binary32/FE_TONEAREST.
bool advancePlayerTrackNavigation50EU(const TrackNavigation50EU& map,
    const TrackNavigation50EU& ai,const TrackNavigationInput50EU&,
    TrackNavigationProgress50EU&,TrackNavigationReceipt50EU&,
    TrackNavigationDiagnostic50EU&) noexcept;
// Optional integrated map effects at original1A0E60/1A10D0. The provider
// must complete all native effects synchronously before returning true; false
// suspends before this crossing's C8C/C88 writes. It must not mutate the track,
// navigation indices, input positions or receipt, and must not throw/reenter.
struct TrackNavigationMapEffects50GH {
    void* context=nullptr;
    bool (*accepted)(void*,bool sequential,std::uint32_t crossedNode,
                     std::uint32_t currentC88) noexcept=nullptr;
};
// Source order: AI sequential, map sequential, AI fallback, map fallback.
// Unlike the projection API above, failure may leave a source-committed prefix
// in progress/receipt and in the effect provider. Never retry it as a fresh
// frame or fabricate completion; the caller owns the exact pending boundary.
bool advancePlayerTrackNavigationEffects50GH(const TrackNavigation50EU& map,
    const TrackNavigation50EU& ai,const TrackNavigationInput50EU&,
    const TrackNavigationMapEffects50GH&,TrackNavigationProgress50EU&,
    TrackNavigationReceipt50EU&,TrackNavigationDiagnostic50EU&) noexcept;
// Source272A78..272AE4 table order is authored zone ordinal, each zone+48=i.
// Actual native refs in the two table namespaces must be supplied by their
// world owners; do not pass guest pointer words or fabricate process addresses.
// The returned bindings borrow stable prepared indices and supplied tables.
bool undertrackBindings50EU(const TrackNavigation50EU&,
    const std::uint32_t* visibility4C0,const std::uint32_t* collision4C4,
    std::size_t zoneCount,UndertrackTableBindings50ET&) noexcept;
}
