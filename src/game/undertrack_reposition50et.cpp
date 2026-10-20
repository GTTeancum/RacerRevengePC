#include "track_navigation_view50mb.h"
#include "undertrack_shared50mb.inl"
namespace rr::game {
using namespace undertrack_detail50MB;
bool readUndertrackTrack50ET(const char* text,std::size_t bytes,UndertrackReadScratch50ET& scratch,
    UndertrackTrack50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    return undertrack_detail50MB::read<maxUndertrackNodes50ET>(text,bytes,scratch,out,d);
}
bool selectUndertrackReposition50ET(const UndertrackTrack50ET& track,const UndertrackProgress50ET& progress,
    const UndertrackInput50ET& in,UndertrackSelection50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    return undertrack_detail50MB::select(track,maxUndertrackNodes50ET,progress,in,out,d);
}
bool bindUndertrackPlacement50ET(const UndertrackSelection50ET& selection,const UndertrackTableBindings50ET& b,
    UndertrackPlacement50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    return undertrack_detail50MB::bind(selection,b,maxUndertrackNodes50ET,out,d);
}
bool selectUndertrackReposition50ET(const UndertrackTrackView50ET& view,const UndertrackProgress50ET& progress,
    const UndertrackInput50ET& in,UndertrackSelection50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    if(!view.runtimeValid()||view.role()!=(in.vehicle58?NavigationRole50MB::Ai:NavigationRole50MB::Map))
        return fail(d,E::Track,0x2a2850);
    struct ReadOnly {const UndertrackNode50ET* nodes;std::uint32_t count;};
    const ReadOnly track{view.nodes(),view.count()};
    const Span arrays[]={{track.nodes,track.count*sizeof(UndertrackNode50ET)},{&view,sizeof view},
        {&progress,sizeof progress},{&in,sizeof in},{&out,sizeof out},{&d,sizeof d}};
    if(!separate(arrays,6))return false;
    return undertrack_detail50MB::select(track,view.capacity(),progress,in,out,d);
}
bool bindUndertrackPlacement50ET(const UndertrackSelection50ET& selection,const TrackNavigationView50EU& view,
    const std::uint32_t* visibility,const std::uint32_t* collision,std::size_t zones,
    UndertrackPlacement50ET& out,UndertrackDiagnostic50ET& d) noexcept {
    UndertrackTableBindings50ET bindings{};
    if(!undertrackBindings50EU(view,visibility,collision,zones,bindings))
        return fail(d,E::MissingApplicationTable,0x225a38);
    return undertrack_detail50MB::bind(selection,bindings,view.capacity(),out,d);
}
void resetUndertrackPlayerPrefix50ET(UndertrackPlayerPrefix50ET& s) noexcept {
    s.counter109C=0;
    s.flags10CC=static_cast<std::uint8_t>(s.flags10CC&0xfbu);
    s.flags10CC=static_cast<std::uint8_t>(s.flags10CC&0xefu);
    s.flags10CC=static_cast<std::uint8_t>(s.flags10CC&0xfeu);
    s.flags10CC=static_cast<std::uint8_t>(s.flags10CC&0xfdu);
}

void resetUndertrackController50ET(UndertrackController50ET& s) noexcept {
    s.field474=0;s.field4AC=0;
    s.flags4A4=static_cast<std::uint8_t>(s.flags4A4&0xfeu);
    s.flags4A4=static_cast<std::uint8_t>(s.flags4A4&0xfdu);
    s.field484=0x3f800000u;s.field490=0x3f800000u;
    s.field494=0;s.field498=0;s.field49C=0;s.field4A0=0;
    s.field4A5=3;s.field4A8=0;s.flags22C=static_cast<std::uint8_t>(s.flags22C&0xfbu);
}

bool resetUndertrackBasePrefix50ET(std::uint8_t* flags,std::uint32_t* field24C) noexcept {
    if(!flags)return true;
    if(field24C&&overlap(flags,1,field24C,sizeof(*field24C)))return false;
    //216A78 commits before216A7C reads D44 and216A80 calls2C2810.
    // A missing later owner stops that call; it cannot undo the D40 store.
    *flags=static_cast<std::uint8_t>(*flags&0xfbu);
    if(!field24C)return false;
    *field24C=0;return true;
}

void resetUndertrackCameraMotion50ET(UndertrackCameraMotion50ET& s,
    const std::uint32_t shared433DB0[4]) noexcept {
    std::memcpy(s.field180,shared433DB0,16);std::memcpy(s.field1A0,shared433DB0,16);
}

bool prepareUndertrackResetPose50ET(const float positionBC0[4],const float headingBD0[4],
    const float up434650[4],BodyMatrix48G& out,UndertrackDiagnostic50ET& d) noexcept {
    const Span spans[]={{positionBC0,16},{headingBD0,16},{up434650,16},{&out,sizeof out},{&d,sizeof d}};
    if(!separate(spans,5))return false;
    if(std::fegetround()!=FE_TONEAREST)return fail(d,E::RoundingMode,0x216af0);
    Math f;Vec heading{},up{};std::memcpy(heading.v,headingBD0,16);std::memcpy(up.v,up434650,16);
    const Vec initialRight=f.cross(up,heading),correctedUp=f.cross(heading,initialRight);
    const Vec normalizedUp=f.normalize(correctedUp),normalizedHeading=f.normalize(heading);
    const Vec right=f.cross(normalizedUp,normalizedHeading);BodyMatrix48G result{};
    for(unsigned i=0;i<3;++i){result.v[i]=right.v[i];result.v[4+i]=normalizedUp.v[i];
        result.v[8+i]=normalizedHeading.v[i];result.v[12+i]=f.read(positionBC0[i]);}
    result.v[15]=1.f;
    if(!f.valid)return fail(d,E::Arithmetic,0x28a090);
    out=result;d={};return true;
}

bool setUndertrackResetScene50ET(const SceneGraph49P& graph,SceneNodeIndex49P index,
    const BodyMatrix48G& pose,UndertrackSceneWorkspace50ET& w,UndertrackDiagnostic50ET& d,
    UndertrackSceneOrder50ET order) noexcept {
    if(!validateSceneGraph49P(graph))return fail(d,E::Scene,0x28a090);
    const auto i=static_cast<std::int32_t>(index);
    if(i<0||static_cast<std::uint32_t>(i)>=graph.count||graph.nodes[i].parent180!=SceneNodeIndex49P::none)
        return fail(d,E::Scene,0x28a090);
    const Span spans[]={{&graph,sizeof graph},{graph.nodes,graph.count*sizeof(SceneNode49P)},
        {&pose,sizeof pose},{&w,sizeof w},{&d,sizeof d}};
    if(!separate(spans,5))return false;
    if(std::fegetround()!=FE_TONEAREST)return fail(d,E::RoundingMode,0x28a090);
    if(order!=UndertrackSceneOrder50ET::OrientationThenPosition&&order!=UndertrackSceneOrder50ET::PositionThenOrientation)
        return fail(d,E::Input,0x28a090);
    // Only admit the actual matrix domain produced by the source basis helper.
    for(float x:pose.v)if(!std::isfinite(x))return fail(d,E::Arithmetic,0x28a090);
    if(bits(pose.v[3])||bits(pose.v[7])||bits(pose.v[11])||bits(pose.v[15])!=0x3f800000u)
        return fail(d,E::Input,0x28a090);
    std::memcpy(w.nodes,graph.nodes,graph.count*sizeof(SceneNode49P));const SceneGraph49P staged{w.nodes,graph.count};
    if(order==UndertrackSceneOrder50ET::PositionThenOrientation){
        BodyMatrix48G translated=w.nodes[i].localC0;std::memcpy(translated.v+12,pose.v+12,12);
        if(!setLocalMatrix49P(staged,index,translated))return fail(d,E::Scene,0x28add0);
    }
    BodyMatrix48G orientation=pose;std::memcpy(orientation.v+12,w.nodes[i].localC0.v+12,12);
    if(!setLocalMatrix49P(staged,index,orientation))return fail(d,E::Scene,0x28a090);
    if(order==UndertrackSceneOrder50ET::OrientationThenPosition){
        BodyMatrix48G translated=w.nodes[i].localC0;std::memcpy(translated.v+12,pose.v+12,12);
        if(!setLocalMatrix49P(staged,index,translated))return fail(d,E::Scene,0x28add0);
    }
    std::memcpy(graph.nodes,w.nodes,graph.count*sizeof(SceneNode49P));d={};return true;
}

bool resetUndertrackMainBlock50ET(MainBodyState49K& main,MainSphereLanes49M& lanes,
    SecondaryActivationSupplement49V& supplement,std::uint32_t& active,
    ChariotLifecycleState49V& lifecycle,const SecondaryActivationGlobals49V& bodyGlobals,
    const ResetLifecycleGlobals50AT& lifecycleGlobals) noexcept {
    const Span spans[]={{&main,sizeof main},{&lanes,sizeof lanes},{&supplement,sizeof supplement},
        {&active,sizeof active},{&lifecycle,sizeof lifecycle},{&bodyGlobals,sizeof bodyGlobals},
        {&lifecycleGlobals,sizeof lifecycleGlobals}};
    if(!separate(spans,7))return false;
    auto stagedLifecycle=lifecycle;auto stagedActive=active;
    if(!resetLifecycleRecords50AT(stagedActive,stagedLifecycle,lifecycleGlobals))return false;
    resetMainRigidBody50AR(main,lanes,supplement,bodyGlobals);
    lifecycle=stagedLifecycle;active=stagedActive;return true;
}

void resetUndertrackPriorPoint50ET(MainBodyState49K& main,std::uint32_t& vehicle850,
    const std::uint32_t shared434630[4]) noexcept {
    std::memcpy(main.priorPoint7B0,shared434630,16);vehicle850=0;
}

void finishUndertrackControlHistory50ET(MainBodyState49K& main) noexcept {
    const std::uint32_t zero=0;
    std::memcpy(&main.controls.bound.control.shaping.smoothedChannel0D28,&zero,4);
}
}
