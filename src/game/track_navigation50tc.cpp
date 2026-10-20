#include "track_navigation_view50tc.h"
#include "track_navigation_shared50mb.inl"
#include "undertrack_shared50mb.inl"
namespace rr::game {
namespace {
struct GraphReadOnlyTC {const UndertrackNode50ET* nodes;std::uint32_t count;};
struct NavigationReadOnlyTC {
    GraphReadOnlyTC track;const TrackNavigationNode50EU* nodes;const std::uint8_t* nodeIndex78;
    bool gatesPrepared,indicesPrepared;
};
NavigationReadOnlyTC readOnlyTC(const TrackNavigationView50TC& v) noexcept {
    return {{v.track().nodes(),v.count()},v.nodes(),v.nodeIndex78(),v.gatesPrepared(),v.indicesPrepared()};
}
bool joinedTC(const TrackNavigationView50TC& map,const TrackNavigationView50TC& ai) noexcept {
    return map.sameOwner(ai)&&map.role()==NavigationRole50TC::Map&&ai.role()==NavigationRole50TC::Ai;
}
}
bool advancePlayerTrackNavigation50EU(const TrackNavigationView50TC& map,const TrackNavigationView50TC& ai,
    const TrackNavigationInput50EU& in,TrackNavigationProgress50EU& p,TrackNavigationReceipt50EU& r,
    TrackNavigationDiagnostic50EU& d) noexcept {
    if(!joinedTC(map,ai))return navigation_detail50MB::fail(d,TrackNavigationError50EU::NotPrepared,0x1a0da0);
    return navigation_detail50MB::advance<268,293>(readOnlyTC(map),readOnlyTC(ai),in,p,r,d);
}
bool advancePlayerTrackNavigationEffects50GH(const TrackNavigationView50TC& map,const TrackNavigationView50TC& ai,
    const TrackNavigationInput50EU& in,const TrackNavigationMapEffects50GH& effects,
    TrackNavigationProgress50EU& p,TrackNavigationReceipt50EU& r,TrackNavigationDiagnostic50EU& d) noexcept {
    if(!joinedTC(map,ai))return navigation_detail50MB::fail(d,TrackNavigationError50EU::NotPrepared,0x1a0da0);
    return navigation_detail50MB::advanceEffects<268,293>(readOnlyTC(map),readOnlyTC(ai),in,effects,p,r,d);
}
bool undertrackBindings50EU(const TrackNavigationView50TC& view,const std::uint32_t* visibility,
    const std::uint32_t* collision,std::size_t count,UndertrackTableBindings50ET& out) noexcept {
    if(!view.runtimeValid())return false;
    return view.role()==NavigationRole50TC::Map?
        navigation_detail50MB::bindings<268>(readOnlyTC(view),visibility,collision,count,out):
        navigation_detail50MB::bindings<293>(readOnlyTC(view),visibility,collision,count,out);
}
bool selectUndertrackReposition50ET(const UndertrackTrackView50TC& view,const UndertrackProgress50ET& progress,
    const UndertrackInput50ET& in,UndertrackSelection50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    using namespace undertrack_detail50MB;
    if(!view.runtimeValid()||view.role()!=(in.vehicle58?NavigationRole50TC::Ai:NavigationRole50TC::Map))
        return fail(d,E::Track,0x2a2850);
    const GraphReadOnlyTC track{view.nodes(),view.count()};
    const Span arrays[]={{track.nodes,track.count*sizeof(UndertrackNode50ET)},{&view,sizeof view},
        {&progress,sizeof progress},{&in,sizeof in},{&out,sizeof out},{&d,sizeof d}};
    if(!separate(arrays,6))return false;
    return undertrack_detail50MB::select(track,view.capacity(),progress,in,out,d);
}
bool bindUndertrackPlacement50ET(const UndertrackSelection50ET& selection,const TrackNavigationView50TC& view,
    const std::uint32_t* visibility,const std::uint32_t* collision,std::size_t zones,
    UndertrackPlacement50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    UndertrackTableBindings50ET bindings{};
    if(!undertrackBindings50EU(view,visibility,collision,zones,bindings))
        return undertrack_detail50MB::fail(d,UndertrackError50ET::MissingApplicationTable,0x225a38);
    return undertrack_detail50MB::bind(selection,bindings,view.capacity(),out,d);
}
}
