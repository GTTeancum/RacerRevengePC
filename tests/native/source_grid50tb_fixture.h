#pragma once
#include "source_grid50ig_fixture.h"
#include "../../src/game/track_navigation_view50tb.h"
namespace source_grid50IG {
inline bool prepare(const rr::game::TrackNavigationView50TB& view,std::uint32_t rank,
    rr::game::GridCenterResult50AC& out) noexcept {
    using namespace rr::game;
    if(!view.gatesPrepared()||view.role()!=NavigationRole50TB::Map||!view.count()||view.count()>view.capacity())return false;
    struct Graph {const UndertrackNode50ET* nodes;std::uint32_t count;};
    struct Navigation {Graph track;const TrackNavigationNode50EU* nodes;bool gatesPrepared;};
    const Navigation map{{view.track().nodes(),view.count()},view.nodes(),true};
    return detail::prepare(map,view.capacity(),rank,out);
}
}
