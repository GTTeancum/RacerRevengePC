#include "track_navigation_view50tb.h"
#include "track_navigation_shared50mb.inl"
#include "undertrack_shared50mb.inl"
namespace rr::game {
namespace {
struct GraphReadOnlyTB {const UndertrackNode50ET* nodes;std::uint32_t count;};
struct NavigationReadOnlyTB {
    GraphReadOnlyTB track;const TrackNavigationNode50EU* nodes;const std::uint8_t* nodeIndex78;
    bool gatesPrepared,indicesPrepared;
};
NavigationReadOnlyTB readOnlyTB(const TrackNavigationView50TB& v) noexcept {
    return {{v.track().nodes(),v.count()},v.nodes(),v.nodeIndex78(),v.gatesPrepared(),v.indicesPrepared()};
}
bool joinedTB(const TrackNavigationView50TB& map,const TrackNavigationView50TB& ai) noexcept {
    return map.sameOwner(ai)&&map.role()==NavigationRole50TB::Map&&ai.role()==NavigationRole50TB::Ai;
}
}
bool advancePlayerTrackNavigation50EU(const TrackNavigationView50TB& map,const TrackNavigationView50TB& ai,
    const TrackNavigationInput50EU& in,TrackNavigationProgress50EU& p,TrackNavigationReceipt50EU& r,
    TrackNavigationDiagnostic50EU& d) noexcept {
    if(!joinedTB(map,ai))return navigation_detail50MB::fail(d,TrackNavigationError50EU::NotPrepared,0x1a0da0);
    return navigation_detail50MB::advance<270,269>(readOnlyTB(map),readOnlyTB(ai),in,p,r,d);
}
bool advancePlayerTrackNavigationEffects50GH(const TrackNavigationView50TB& map,const TrackNavigationView50TB& ai,
    const TrackNavigationInput50EU& in,const TrackNavigationMapEffects50GH& effects,
    TrackNavigationProgress50EU& p,TrackNavigationReceipt50EU& r,TrackNavigationDiagnostic50EU& d) noexcept {
    if(!joinedTB(map,ai))return navigation_detail50MB::fail(d,TrackNavigationError50EU::NotPrepared,0x1a0da0);
    return navigation_detail50MB::advanceEffects<270,269>(readOnlyTB(map),readOnlyTB(ai),in,effects,p,r,d);
}
bool undertrackBindings50EU(const TrackNavigationView50TB& view,const std::uint32_t* visibility,
    const std::uint32_t* collision,std::size_t count,UndertrackTableBindings50ET& out) noexcept {
    if(!view.runtimeValid())return false;
    return view.role()==NavigationRole50TB::Map?
        navigation_detail50MB::bindings<270>(readOnlyTB(view),visibility,collision,count,out):
        navigation_detail50MB::bindings<269>(readOnlyTB(view),visibility,collision,count,out);
}
bool selectUndertrackReposition50ET(const UndertrackTrackView50TB& view,const UndertrackProgress50ET& progress,
    const UndertrackInput50ET& in,UndertrackSelection50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    using namespace undertrack_detail50MB;
    if(!view.runtimeValid()||view.role()!=(in.vehicle58?NavigationRole50TB::Ai:NavigationRole50TB::Map))
        return fail(d,E::Track,0x2a2850);
    const GraphReadOnlyTB track{view.nodes(),view.count()};
    const Span arrays[]={{track.nodes,track.count*sizeof(UndertrackNode50ET)},{&view,sizeof view},
        {&progress,sizeof progress},{&in,sizeof in},{&out,sizeof out},{&d,sizeof d}};
    if(!separate(arrays,6))return false;
    return undertrack_detail50MB::select(track,view.capacity(),progress,in,out,d);
}
bool bindUndertrackPlacement50ET(const UndertrackSelection50ET& selection,const TrackNavigationView50TB& view,
    const std::uint32_t* visibility,const std::uint32_t* collision,std::size_t zones,
    UndertrackPlacement50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    UndertrackTableBindings50ET bindings{};
    if(!undertrackBindings50EU(view,visibility,collision,zones,bindings))
        return undertrack_detail50MB::fail(d,UndertrackError50ET::MissingApplicationTable,0x225a38);
    return undertrack_detail50MB::bind(selection,bindings,view.capacity(),out,d);
}
}
