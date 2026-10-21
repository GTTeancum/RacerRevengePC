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
    template<class View> bool bindView(const View& view) noexcept {
        using Role=decltype(view.role());
        if(map_||!view.gatesPrepared()||view.role()!=Role::Map||
            view.count()<2||view.count()>view.capacity())return false;
        map_=&view;
        prepare_=[](const void* p,std::uint32_t rank,GridCenterResult50AC& out) noexcept {
            const auto& v=*static_cast<const View*>(p);
            if(!v.gatesPrepared()||v.role()!=Role::Map||v.count()<2||v.count()>v.capacity())return false;
            struct Graph {const UndertrackNode50ET* nodes;std::uint32_t count;};
            struct Navigation {Graph track;const TrackNavigationNode50EU* nodes;bool gatesPrepared;};
            const Navigation map{{v.track().nodes(),v.count()},v.nodes(),true};
            return detail::prepare(map,v.capacity(),rank,out);
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
