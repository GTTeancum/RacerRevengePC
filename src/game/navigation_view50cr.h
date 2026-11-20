#pragma once
// Common rewrite (R4, PLAN B-C12 / 2.5): the ONE navigation view family for
// every course. Every consumer (AI, lap, wrong-way, recovery, radar, grid,
// camera, race session) reads the common_track50CR::NavigationView issued over
// the one NavigationStorage that readNavigation2A1260 sizes from the course's
// own m<code>.spl / a<code>.spl node counts (common_navigation50cr.cpp).
// Retires TrackNavigationView50EU/AiTrackMetadataView50IU of the 50MB store and
// their GB/MC/TB/TC clones (track_navigation_view50{mb,gb,mc,tb,tc}.h).
// No member here names a course; counts are the file's.
#include "../../tests/native/common_track50cr.h"
#include <memory>
#include <utility>

namespace rr::game {
using NavigationView50CR=::common_track50CR::NavigationView;
using NavigationRole50CR=::common_track50CR::NavigationRole;

// AI metadata (2A0F30/2A1260 SpeedLimit, DistanceToFinish, BoostScale, flags,
// ProgressNode, track+3C length) of one role of the one navigation store. A
// view, not a store: it borrows the same storage the navigation view holds.
class AiTrackMetadataView50IU final {
    NavigationView50CR navigation_{};
public:
    AiTrackMetadataView50IU()=default;
    explicit AiTrackMetadataView50IU(NavigationView50CR n) noexcept:navigation_(std::move(n)){}
    bool bound() const noexcept {return navigation_.gatesPrepared()&&navigation_.metadata()!=nullptr;}
    bool runtimeValid() const noexcept {return bound()&&navigation_.runtimeValid();}
    NavigationRole50CR role() const noexcept {return navigation_.role();}
    std::uint32_t count() const noexcept {return bound()?navigation_.count():0u;}
    std::size_t capacity() const noexcept {return bound()?navigation_.capacity():0u;}
    const AiTrackNodeMetadata50IU* nodes() const noexcept {return bound()?navigation_.metadata():nullptr;}
    float trackLength3C() const noexcept {return bound()?navigation_.trackLength3C():0.f;}
    bool matches(const NavigationView50CR& n) const noexcept {
        return bound()&&navigation_.sameOwner(n)&&navigation_.role()==n.role()&&count()==n.count();
    }
    const NavigationView50CR& navigation() const noexcept {return navigation_;}
};
inline AiTrackMetadataView50IU metadataView50CR(const NavigationView50CR& v) noexcept {return AiTrackMetadataView50IU{v};}

// 1AAB30 AI cold bindings over the one view pair (map FF0, AI FF4).
struct AiColdTrackBindingsView50CR {
    NavigationView50CR mapFF0,aiFF4;
    AiTrackMetadataView50IU mapMetadata,aiMetadata;
};
inline bool bindAiColdTracks50IU(const NavigationView50CR& map,const NavigationView50CR& ai,
    const TrackNavigationProgress50EU& progress,AiColdTrackBindingsView50CR& out) noexcept {
    const AiTrackMetadataView50IU mapMetadata{map},aiMetadata{ai};
    if(!map.sameOwner(ai)||map.role()!=NavigationRole50CR::map||ai.role()!=NavigationRole50CR::ai||
       !mapMetadata.matches(map)||!aiMetadata.matches(ai)||progress.reset.vehicleC80!=1||
       progress.vehicleC84!=0||progress.reset.vehicleC88!=1||progress.reset.vehicleC8C!=0)return false;
    out={map,ai,mapMetadata,aiMetadata};return true;
}

// Runtime overloads over the one view (definitions: track_navigation50eu.cpp,
// undertrack_reposition50et.cpp). A view that is not runtime-valid is refused.
bool selectUndertrackReposition50ET(const NavigationView50CR&,
    const UndertrackProgress50ET&,const UndertrackInput50ET&,UndertrackSelection50ET&,
    UndertrackDiagnostic50ET&) noexcept;
bool undertrackBindings50EU(const NavigationView50CR&,
    const std::uint32_t*,const std::uint32_t*,std::size_t,UndertrackTableBindings50ET&) noexcept;
bool bindUndertrackPlacement50ET(const UndertrackSelection50ET&,const NavigationView50CR&,
    const std::uint32_t*,const std::uint32_t*,std::size_t,UndertrackPlacement50ET&,
    UndertrackDiagnostic50ET&) noexcept;
bool advancePlayerTrackNavigation50EU(const NavigationView50CR&,const NavigationView50CR&,
    const TrackNavigationInput50EU&,TrackNavigationProgress50EU&,TrackNavigationReceipt50EU&,
    TrackNavigationDiagnostic50EU&) noexcept;
bool advancePlayerTrackNavigationEffects50GH(const NavigationView50CR&,const NavigationView50CR&,
    const TrackNavigationInput50EU&,const TrackNavigationMapEffects50GH&,TrackNavigationProgress50EU&,
    TrackNavigationReceipt50EU&,TrackNavigationDiagnostic50EU&) noexcept;
}
