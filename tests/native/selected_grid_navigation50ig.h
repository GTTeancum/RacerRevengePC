#pragma once
#include "source_grid50ig_fixture.h"

namespace source_grid50IG {
// Cold-only non-owning capability. The selected map/view and its authentic
// source/world owners must outlive this object. No allocation, copied topology,
// fallback map, or new lifetime lease. Invalidated views fail before access.
class SelectedMapGrid final {
    const void* map_=nullptr;
    bool (*prepare_)(const void*,std::uint32_t,GridCenterResult50AC&) noexcept=nullptr;
public:
    // The one common map view (R4). The bound view object must outlive this.
    bool bindView(const NavigationView50CR& view) noexcept {
        if(map_||!view.gatesPrepared()||view.role()!=NavigationRole50CR::map||
            view.count()<2||view.count()>view.capacity())return false;
        map_=&view;
        prepare_=[](const void* p,std::uint32_t rank,GridCenterResult50AC& out) noexcept {
            const auto& v=*static_cast<const NavigationView50CR*>(p);
            if(v.count()<2)return false;
            return source_grid50IG::prepare(v,rank,out);
        };
        return true;
    }
    bool bindLegacy(const TrackNavigation50EU* map) noexcept {
        if(map_||!map||!map->gatesPrepared||map->track.count<2||
            map->track.count>maxUndertrackNodes50ET)return false;
        map_=map;
        prepare_=[](const void* p,std::uint32_t rank,GridCenterResult50AC& out) noexcept {
            return source_grid50IG::prepare(*static_cast<const TrackNavigation50EU*>(p),rank,out);
        };
        return true;
    }
    bool prepare(std::uint32_t rank,GridCenterResult50AC& out) const noexcept {
        return map_&&prepare_&&prepare_(map_,rank,out);
    }
};
} // namespace source_grid50IG
