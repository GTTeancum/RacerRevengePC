#pragma once
#include "live_zone_membership50en.h"
#include "camera_view49f.h"
#include <array>
#include <cstddef>
#include <cstdint>

// Common per-frame zone ("section") visibility for every course: the original
// 114D00 -> 27F650 portal walk and the 27FEE0 child-first draw walk, taken
// out of the RA-only owner (original_ra_native_static_draw50la.inl) so that
// any course's world can use it. Inputs are the course's own data: its
// authored portals (SCB "PortalN" connect1/connect2/vertex1..4, built by
// 272B3C into LiveZoneWorld50EN), the camera zone's ExclusionList (SCB
// "ZoneN", read by 27632C..27641C) and the current camera. No course name,
// zone number or count is known here.
//
// Output per zone (zero-based authored ordinal):
//  - drawn: the zone's static models are submitted this frame (27FEE0).
//  - bounds: accumulated NDC rectangle of the portal polygons that reach it
//    (27F930), min/max x then min/max y; (-1,1,-1,1) for the camera zone.
//  - view: 280720 portal-narrowed matrix; statics are box-tested against
//    view.matrix60 (240C00 with the retail 3833D8 = 1 membership gate).
//  - scissor: 2805B4 inclusive Xmin,Xmax,Ymin,Ymax in source viewport pixels.
// A numeric failure is a failure, never a cull: the caller then draws every
// zone (the previous behaviour) and logs it.
namespace rr::game {
constexpr std::size_t maxTrackZones50ZV = 64;
struct TrackZoneRow50ZV {
    bool admitted = false, drawn = false;
    std::array<float,4> bounds{};
    PortalZoneViewOutput49F view{};
    std::array<std::uint32_t,4> scissor{};
};
struct TrackZoneVisibilityInput50ZV {
    const LiveZoneWorld50EN* portals = nullptr;
    // Camera scene +38 parent, projected to zero-based (readCameraPrimaryZone48).
    std::uint32_t rootZone = 0;
    // The ROOT zone's authored exclusions (zero-based, source order, may hold
    // values >= zone count, which never match: 27632C stores atoi-1 unchecked).
    const std::uint32_t* rootExclusions = nullptr;
    std::uint32_t rootExclusionCount = 0;
    const CameraViewOutput49F* camera = nullptr;
    // 230450 constant quad, built by constructPortalViewport230450_50EO.
    PortalProjectionQuad50EO viewportQuad{};
    // Source viewport 1E0/1E4/1E8/1EC.
    std::int32_t viewportX = 0, viewportY = 0, viewportWidth = 0, viewportHeight = 0;
    // Native widescreen (Hor+, nativeWidescreenScaleX50DX of the output; 1 at
    // 4:3). PC enhancement, not original: the port draws the world with the
    // camera's clip-X column times this factor, so the zone walk must see the
    // same wide screen or zones at the wide edges are hidden (S0-widescreen 1e):
    // portals project through the widened combined matrix, the viewport quad
    // stays (-1..1) in widened NDC, and 280720's halfX is divided by it.
    float widescreenScaleX = 1.f;
};
enum class TrackZoneVisibilityStatus50ZV : std::uint8_t {
    complete, invalidInput, bounds, arithmetic, registration
};
// rows[0..zoneCount) are fully rewritten on success; untouched on failure.
TrackZoneVisibilityStatus50ZV prepareTrackZoneVisibility50ZV(
    const TrackZoneVisibilityInput50ZV& input, TrackZoneRow50ZV* rows, std::size_t rowCapacity) noexcept;
// 2B0580 eight-corner CLIPw.xyz test of a world AABB against a zone's
// matrix60 (identity model). true = AND of outcodes nonzero (rejected).
// false on numeric failure is reported through ok=false (caller keeps the draw).
bool trackZoneBoxOutside50ZV(const std::array<float,16>& matrix60,
    const float minimum[3], const float maximum[3], bool& ok) noexcept;
}
