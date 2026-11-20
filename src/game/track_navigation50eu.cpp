#include "navigation_view50cr.h"
#include "track_navigation_shared50mb.inl"
namespace rr::game {
bool readTrackNavigation50EU(const char* text,std::size_t bytes,SourceLibmState49T& libm,
    TrackNavigationScratch50EU& scratch,TrackNavigation50EU& out,TrackNavigationDiagnostic50EU& d) noexcept {
    return navigation_detail50MB::read<maxUndertrackNodes50ET>(text,bytes,libm,scratch,out,d);
}
bool bindTrackNavigationZones50EU(const LiveZoneWorld50EN& world,TrackNavigation50EU& t,
    TrackNavigationDiagnostic50EU& d) noexcept {
    return navigation_detail50MB::bind<maxUndertrackNodes50ET>(world,t,d);
}
void initializeTrackNavigationProgress50EU(TrackNavigationProgress50EU& p) noexcept {p.reset={1,1,0};p.vehicleC84=0;}
bool advancePlayerTrackNavigation50EU(const TrackNavigation50EU& map,const TrackNavigation50EU& ai,
    const TrackNavigationInput50EU& in,TrackNavigationProgress50EU& p,TrackNavigationReceipt50EU& r,
    TrackNavigationDiagnostic50EU& d) noexcept {
    return navigation_detail50MB::advance<256,256>(map,ai,in,p,r,d);
}
bool advancePlayerTrackNavigationEffects50GH(const TrackNavigation50EU& map,const TrackNavigation50EU& ai,
    const TrackNavigationInput50EU& in,const TrackNavigationMapEffects50GH& effects,
    TrackNavigationProgress50EU& p,TrackNavigationReceipt50EU& r,TrackNavigationDiagnostic50EU& d) noexcept {
    return navigation_detail50MB::advanceEffects<256,256>(map,ai,in,effects,p,r,d);
}
bool undertrackBindings50EU(const TrackNavigation50EU& t,const std::uint32_t* visibility,
    const std::uint32_t* collision,std::size_t count,UndertrackTableBindings50ET& out) noexcept {
    return navigation_detail50MB::bindings<256>(t,visibility,collision,count,out);
}
// ---------------------------------------------------------------- the one view (common rewrite R4)
// Every course: the map/AI views of the one navigation store (common_track50cr.h),
// sized from the spline files' own node counts. The reader guard
// navigationNodeLimit50ET bounds the per-call scratch; the live count is the view's.
namespace {
struct GraphReadOnly {const UndertrackNode50ET* nodes;std::uint32_t count;};
struct NavigationReadOnly {
    GraphReadOnly track;const TrackNavigationNode50EU* nodes;const std::uint8_t* nodeIndex78;
    bool gatesPrepared,indicesPrepared;
};
NavigationReadOnly readOnly(const NavigationView50CR& v) noexcept {
    return {{v.nodes(),v.count()},v.gates(),v.nodeIndex78(),v.gatesPrepared(),v.indicesPrepared()};
}
bool joined(const NavigationView50CR& map,const NavigationView50CR& ai) noexcept {
    return map.sameOwner(ai)&&map.role()==NavigationRole50CR::map&&ai.role()==NavigationRole50CR::ai&&
        map.count()<=navigationNodeLimit50ET&&ai.count()<=navigationNodeLimit50ET;
}
}
bool advancePlayerTrackNavigation50EU(const NavigationView50CR& map,const NavigationView50CR& ai,
    const TrackNavigationInput50EU& in,TrackNavigationProgress50EU& p,TrackNavigationReceipt50EU& r,
    TrackNavigationDiagnostic50EU& d) noexcept {
    if(!joined(map,ai))return navigation_detail50MB::fail(d,TrackNavigationError50EU::NotPrepared,0x1a0da0);
    return navigation_detail50MB::advance<navigationNodeLimit50ET,navigationNodeLimit50ET>(readOnly(map),readOnly(ai),in,p,r,d);
}
bool advancePlayerTrackNavigationEffects50GH(const NavigationView50CR& map,const NavigationView50CR& ai,
    const TrackNavigationInput50EU& in,const TrackNavigationMapEffects50GH& effects,
    TrackNavigationProgress50EU& p,TrackNavigationReceipt50EU& r,TrackNavigationDiagnostic50EU& d) noexcept {
    if(!joined(map,ai))return navigation_detail50MB::fail(d,TrackNavigationError50EU::NotPrepared,0x1a0da0);
    return navigation_detail50MB::advanceEffects<navigationNodeLimit50ET,navigationNodeLimit50ET>(readOnly(map),readOnly(ai),in,effects,p,r,d);
}
bool undertrackBindings50EU(const NavigationView50CR& view,const std::uint32_t* visibility,
    const std::uint32_t* collision,std::size_t count,UndertrackTableBindings50ET& out) noexcept {
    if(!view.runtimeValid()||view.count()>navigationNodeLimit50ET)return false;
    return navigation_detail50MB::bindings<navigationNodeLimit50ET>(readOnly(view),visibility,collision,count,out);
}
}
