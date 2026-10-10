#pragma once
// Development HOST presentation policy only. This is not recovered game-camera
// behavior, and never moves a scene node, physical body, model, or cable endpoint.
#include "../../platform/linux/native/vehicle_view_camera50dg.h"
#include "../../src/game/vehicle_draw_batch50co.h"
#include <algorithm>
#include <cmath>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Host framing requires strict finite IEEE arithmetic"
#endif

namespace host_chase_camera50fn_fixture {
// Fixed host limits, in the same world units as the submitted mesh. These are
// presentation choices, not original authoring or inferred physical constants.
constexpr double maximumPlaneShift=96.0; // Independently on camera right/up.
constexpr double maximumRetreat=192.0;
constexpr double correctedHorizontalLimit=0.90,correctedVerticalLimit=0.84;

// Caller-owned last successful correction, for telemetry. No accumulated
// offset, filter, pointers, hidden allocation, or frame-rate-dependent history.
struct State {float right=0.f,up=0.f,retreat=0.f;};
enum class Error {none,camera,bounds,range};
struct Diagnostic {
    Error error=Error::none;
    rr::platform::VehicleViewCameraError50DG camera{};
    bool adjusted=false,framed=false,limited=false;
};

namespace detail {
struct Vector {double x,y,z;};
inline Vector cross(Vector a,Vector b) noexcept {
    return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};
}
inline double dot(Vector a,Vector b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline Vector normalize(Vector v) noexcept {
    const double length=std::sqrt(dot(v,v));
    return {v.x/length,v.y/length,v.z/length};
}
inline bool fail(Diagnostic& d,Error error) noexcept {d.error=error;return false;}
inline Vector corner(const rr::assets::Bounds48B& bounds,unsigned bits) noexcept {
    return {(bits&1u)?bounds.maximum[0]:bounds.minimum[0],
            (bits&2u)?bounds.maximum[1]:bounds.minimum[1],
            (bits&4u)?bounds.maximum[2]:bounds.minimum[2]};
}
// Check the actual matrix convention consumed by the renderer, with no
// perspective division or reliance on canonical scene/world cache freshness.
inline bool framed(const rr::game::VehicleDrawBatch50CO& batch,
    const rr::platform::VehicleViewMatrix50DG& view) noexcept {
    const auto* m=view.worldViewProjection;
    for(const auto& part:batch.component)for(unsigned bits=0;bits<8;++bits) {
        const Vector p=corner(part.bounds,bits);
        double clip[4]{};
        for(unsigned c=0;c<4;++c)
            clip[c]=p.x*m[4*c]+p.y*m[4*c+1]+p.z*m[4*c+2]+m[4*c+3];
        if(!(clip[3]>0.0))return false;
        for(unsigned c=0;c<3;++c)
            if(!std::isfinite(clip[c])||std::fabs(clip[c])>clip[3])return false;
    }
    return true;
}
inline double closestShift(double low,double high) noexcept {
    // An impossible interval can occur only at a reported hard limit. Center
    // its remaining error rather than violating std::clamp's ordered bounds.
    const double wanted=low<=high?std::clamp(0.0,low,high):(low+high)*0.5;
    return std::clamp(wanted,-maximumPlaneShift,maximumPlaneShift);
}
}

// Supply this frame's ORIGINAL baseline camera and successfully prepared draw
// batch, never the last corrected camera or a last-frame published batch.
// Uses the 24 world-space corners computed from the actual three rendered
// meshes. It cannot mistake the dormant secondary body for the visible cockpit.
// True means a valid bounded camera was produced; inspect d.framed/d.limited
// for a pose that cannot fit within these limits or the caller's near/far planes.
// False preserves output and state. Diagnostic always describes this call.
// Inputs, output, state and diagnostic must be distinct live objects.
inline bool framePod(const rr::platform::VehicleViewCamera50DG& baseline,
    const rr::game::VehicleDrawBatch50CO& batch,State& state,
    rr::platform::VehicleViewCamera50DG& output,Diagnostic& d) noexcept {
    d={};
    rr::platform::VehicleViewMatrix50DG view{};
    if(!rr::platform::composeVehicleViewCamera50DG(baseline,view,d.camera))
        return detail::fail(d,Error::camera);
    for(const auto& part:batch.component) {
        if(!part.vertexCount)return detail::fail(d,Error::bounds);
        for(unsigned axis=0;axis<3;++axis)
            if(!std::isfinite(part.bounds.minimum[axis])||
               !std::isfinite(part.bounds.maximum[axis])||
               part.bounds.minimum[axis]>part.bounds.maximum[axis])
                return detail::fail(d,Error::bounds);
    }
    // Do not nudge, zoom, or retarget an already fully visible normal chase view.
    if(detail::framed(batch,view)) {
        output=baseline;state={};d.framed=true;return true;
    }

    using detail::Vector;
    const Vector eye{baseline.eye[0],baseline.eye[1],baseline.eye[2]};
    const Vector direction{double(baseline.target[0])-eye.x,
        double(baseline.target[1])-eye.y,double(baseline.target[2])-eye.z};
    const Vector suppliedUp{baseline.up[0],baseline.up[1],baseline.up[2]};
    // Baseline composer already checked nonzero/nonparallel finite axes.
    const Vector forward=detail::normalize(direction);
    const Vector right=detail::normalize(detail::cross(direction,suppliedUp));
    const Vector up=detail::normalize(detail::cross(right,forward));
    const double tangent=std::tan(double(baseline.verticalFovRadians)*0.5);
    const double slopes[2]={tangent*baseline.aspectWidthOverHeight*correctedHorizontalLimit,
                            tangent*correctedVerticalLimit};
    const double infinity=std::numeric_limits<double>::infinity();
    double low[2]={-infinity,-infinity},high[2]={infinity,infinity};
    double minimumDepth=infinity,maximumDepth=-infinity;
    for(const auto& part:batch.component)for(unsigned bits=0;bits<8;++bits) {
        const Vector p=detail::corner(part.bounds,bits);
        const Vector relative{p.x-eye.x,p.y-eye.y,p.z-eye.z};
        const double xy[2]={detail::dot(relative,right),detail::dot(relative,up)};
        const double depth=detail::dot(relative,forward);
        minimumDepth=std::min(minimumDepth,depth);maximumDepth=std::max(maximumDepth,depth);
        for(unsigned axis=0;axis<2;++axis) {
            low[axis]=std::max(low[axis],xy[axis]-depth*slopes[axis]);
            high[axis]=std::min(high[axis],xy[axis]+depth*slopes[axis]);
        }
    }
    // Each corner contributes a legal shift interval. Retreat expands all
    // intervals by the same slope, so their intersection is solved directly.
    // There is no iteration, scene walk, mesh scan, or hot-path allocation.
    const double depthPad=std::min(0.25,(double(baseline.farPlane)-baseline.nearPlane)*0.01);
    double required=std::max(0.0,double(baseline.nearPlane)+depthPad-minimumDepth);
    for(unsigned axis=0;axis<2;++axis) {
        required=std::max(required,(low[axis]-high[axis])/(2.0*slopes[axis]));
        required=std::max(required,(low[axis]-maximumPlaneShift)/slopes[axis]);
        required=std::max(required,(-maximumPlaneShift-high[axis])/slopes[axis]);
    }
    const double allowed=std::max(0.0,std::min(maximumRetreat,
        double(baseline.farPlane)-depthPad-maximumDepth));
    const double retreat=std::min(required,allowed);
    const double shiftRight=detail::closestShift(low[0]-retreat*slopes[0],high[0]+retreat*slopes[0]);
    const double shiftUp=detail::closestShift(low[1]-retreat*slopes[1],high[1]+retreat*slopes[1]);
    const double translation[3]={right.x*shiftRight+up.x*shiftUp-forward.x*retreat,
        right.y*shiftRight+up.y*shiftUp-forward.y*retreat,
        right.z*shiftRight+up.z*shiftUp-forward.z*retreat};
    auto candidate=baseline;
    for(unsigned axis=0;axis<3;++axis) {
        const double e=double(baseline.eye[axis])+translation[axis];
        const double t=double(baseline.target[axis])+translation[axis];
        if(!std::isfinite(e)||!std::isfinite(t)||
           std::fabs(e)>std::numeric_limits<float>::max()||
           std::fabs(t)>std::numeric_limits<float>::max())return detail::fail(d,Error::range);
        candidate.eye[axis]=static_cast<float>(e);candidate.target[axis]=static_cast<float>(t);
    }
    if(!rr::platform::composeVehicleViewCamera50DG(candidate,view,d.camera))
        return detail::fail(d,Error::camera);
    d.adjusted=shiftRight!=0.0||shiftUp!=0.0||retreat!=0.0;
    d.framed=detail::framed(batch,view);
    d.limited=required>allowed||!d.framed;
    output=candidate;
    state={static_cast<float>(shiftRight),static_cast<float>(shiftUp),static_cast<float>(retreat)};
    return true;
}
}
