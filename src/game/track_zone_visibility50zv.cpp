#include "track_zone_visibility50zv.h"
#include "portal_geometry50eo.h"
#include "original_ui_rider_visibility50kg.h"
#include <algorithm>
#include <cfenv>
#include <cstring>
#include <cmath>
#include <limits>

// Arithmetic and order follow original_ra_native_static_draw50la.inl (RA's
// port of the same original routines), which this generalises:
//  27F650/27F7C0 root walk, 27F930 recursive accumulation (incoming polygon,
//  never the accumulated box), 263260 intersection, 230880 projection,
//  2806B0 root exclusions (first hop in 27F650; every depth in 27FEE0),
//  27FEE0 child-first draw walk, 280720 narrowed matrix, 2805B4 scissor.
namespace rr::game {
namespace {
using Status = TrackZoneVisibilityStatus50ZV;
using P = PortalProjectionStatus50EO;
constexpr std::size_t depthLimit = 256, operationLimit = 65536;
}

TrackZoneVisibilityStatus50ZV prepareTrackZoneVisibility50ZV(
    const TrackZoneVisibilityInput50ZV& in, TrackZoneRow50ZV* out, std::size_t capacity) noexcept {
    if(!in.portals||!in.camera||!out||std::fegetround()!=FE_TONEAREST)return Status::invalidInput;
    const auto& w=*in.portals;
    if(!w.zoneCount||w.zoneCount>capacity||w.zoneCount>maxTrackZones50ZV||in.rootZone>=w.zoneCount||
       (in.rootExclusionCount&&!in.rootExclusions)||
       in.viewportX<0||in.viewportY<0||in.viewportWidth<=0||in.viewportHeight<=0)
        return Status::invalidInput;
    std::array<TrackZoneRow50ZV,maxTrackZones50ZV> rows{};
    const auto excluded=[&](std::uint32_t z) noexcept {
        for(std::uint32_t i=0;i<in.rootExclusionCount;++i)if(in.rootExclusions[i]==z)return true;
        return false;
    };
    const auto& camera=*in.camera;
    const float wide=in.widescreenScaleX;
    if(!std::isfinite(wide)||!(wide>0.f)||wide>1.f)return Status::invalidInput;
    BodyMatrix48G combined;std::memcpy(combined.v,camera.combined.data(),sizeof combined.v);
    // Row-vector matrix (clip = world * combined): clip X is column 0, the
    // same column the renderer scales (50DX). 1 leaves the 4:3 source exact.
    if(wide!=1.f)for(unsigned r=0;r<4;++r){volatile float x=combined.v[r*4]*wide;combined.v[r*4]=x;}
    const Vector3_48C eye{{camera.position.x,camera.position.y,camera.position.z}};
    std::size_t operations=0;Status failure=Status::complete;
    const auto edges=[&](std::uint32_t zone,auto&& each)->bool {
        auto link=w.portalHeads[zone];std::size_t walked=0;
        while(link){
            if(link>w.portalLinkCount||++walked>w.portalLinkCount){failure=Status::registration;return false;}
            const auto& edge=w.portalLinks[link-1u];
            if(!edge.zone||edge.zone>w.zoneCount||!edge.portal||edge.portal>w.portalCount)
                {failure=Status::registration;return false;}
            if(!each(edge))return false;
            link=edge.next; // original linked order, no sorting/dedup
        }
        return true;
    };
    const auto project=[&](const LiveZonePortalLink50EN& edge,const PortalProjectionQuad50EO& incoming,
                           PortalProjectionQuad50EO& clipped,bool& hit)->bool {
        PortalProjectionQuad50EO projected;hit=false;
        auto r=projectPortal230880_50EO(w.portals[edge.portal-1u],eye,camera.sceneAfter.nearPlane,
            combined,in.viewportQuad,projected);
        if(r==P::Miss)return true;
        if(r!=P::Hit){failure=Status::arithmetic;return false;}
        r=intersectPortalViewport263260_50EO(incoming,projected,clipped);
        if(r==P::Miss)return true;
        if(r!=P::Hit){failure=Status::arithmetic;return false;}
        hit=true;return true;
    };
    // 27F930: expand this zone's box by the incoming polygon; recurse only if
    // it grew, and intersect each next portal with the INCOMING polygon.
    const auto visit=[&](auto&& self,std::uint32_t zone,const PortalProjectionQuad50EO& incoming,
                         std::size_t depth)->bool {
        if(depth>depthLimit||++operations>operationLimit){failure=Status::bounds;return false;}
        auto& row=rows[zone];
        auto next=row.admitted?row.bounds:std::array<float,4>{{
            std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),-std::numeric_limits<float>::max()}};
        for(const auto& p:incoming.xy){
            if(!std::isfinite(p[0])||!std::isfinite(p[1])){failure=Status::arithmetic;return false;}
            next[0]=std::min(next[0],p[0]);next[1]=std::max(next[1],p[0]);
            next[2]=std::min(next[2],p[1]);next[3]=std::max(next[3],p[1]);
        }
        if(row.admitted&&next==row.bounds)return true;
        row.bounds=next;row.admitted=true;
        return edges(zone,[&](const LiveZonePortalLink50EN& edge){
            PortalProjectionQuad50EO clipped;bool hit=false;
            if(!project(edge,incoming,clipped,hit))return false;
            return !hit||self(self,edge.zone-1u,clipped,depth+1u);
        });
    };
    const auto root=in.rootZone;
    rows[root].admitted=true;rows[root].bounds={{-1.f,1.f,-1.f,1.f}};
    if(!edges(root,[&](const LiveZonePortalLink50EN& edge){
        PortalProjectionQuad50EO clipped;bool hit=false;
        if(!project(edge,in.viewportQuad,clipped,hit))return false;
        // 2806B0 on the first hop only during selection.
        return !hit||excluded(edge.zone-1u)||visit(visit,edge.zone-1u,clipped,1u);
    }))return failure==Status::complete?Status::registration:failure;
    // 27FEE0: child-first draw walk; root exclusions apply at every depth.
    operations=0;
    const auto draw=[&](auto&& self,std::uint32_t zone,std::size_t depth)->bool {
        if(depth>depthLimit||++operations>operationLimit){failure=Status::bounds;return false;}
        auto& row=rows[zone];if(row.drawn)return true;row.drawn=true;
        if(!edges(zone,[&](const LiveZonePortalLink50EN& edge){const auto child=edge.zone-1u;
            return !rows[child].admitted||rows[child].drawn||excluded(child)||self(self,child,depth+1u);}))
            return false;
        // 280720 narrowed matrix and 2805B4 scissor for this drawn zone.
        // Widened bounds: left/right = (halfX/s)*b0/b1 = halfX*(b0/s)/(b1/s); the
        // Y pair and the scissor (below) keep the widened bounds unchanged.
        auto narrowed=row.bounds;
        if(wide!=1.f)for(unsigned i=0;i<2;++i){volatile float x=narrowed[i]/wide;narrowed[i]=x;}
        if(!composePortalZoneView280720(camera,narrowed,row.view)){failure=Status::arithmetic;return false;}
        for(unsigned i=0;i<4;++i){
            const auto& b=row.bounds;
            volatile float shifted=i<2?1.f+b[i]:1.f-b[i==2?3:2];
            volatile float half=shifted/2.f;
            volatile float extent=half*static_cast<float>(i<2?in.viewportWidth:in.viewportHeight);
            volatile float pixel=extent+static_cast<float>(i<2?in.viewportX:in.viewportY);
            if(!std::isfinite(pixel)||pixel<0.f||pixel>2047.f){failure=Status::arithmetic;return false;}
            row.scissor[i]=static_cast<std::uint32_t>(pixel);
        }
        return true;
    };
    if(!draw(draw,root,1u))return failure==Status::complete?Status::registration:failure;
    std::copy_n(rows.begin(),w.zoneCount,out);
    return Status::complete;
}

bool trackZoneBoxOutside50ZV(const std::array<float,16>& matrix60,
    const float minimum[3], const float maximum[3], bool& ok) noexcept {
    ok=false;
    std::array<float,4> center{{0,0,0,1}},half{{0,0,0,0}};
    for(unsigned c=0;c<3;++c){
        if(!std::isfinite(minimum[c])||!std::isfinite(maximum[c])||maximum[c]<minimum[c])return false;
        center[c]=(minimum[c]+maximum[c])*.5f;half[c]=(maximum[c]-minimum[c])*.5f;
    }
    OriginalUiRiderBoxClip50KG clipped;
    if(!projectOriginalUiRiderBoxClip50KG(matrix60,center,half,clipped))return false;
    ok=true;return clipped.outside;
}
}
