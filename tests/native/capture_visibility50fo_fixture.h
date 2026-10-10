#pragma once
// Capture-only HOST debugging evidence. Never changes a camera, scene, physics,
// mesh, or rendering state. The caller must not invoke this on ordinary frames.
#include "../../src/game/vehicle_draw_batch50co.h"
#include "../../platform/linux/native/vehicle_view_camera50dg.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "Capture visibility evidence requires strict finite IEEE arithmetic"
#endif

namespace capture_visibility50fo_fixture {
struct Point { double x=0.0,y=0.0,z=0.0; };
struct Hit {
    bool hit=false;
    // Segment fraction for cameraToCockpit; world distance for vertical hits.
    double t=0.0;
    std::uint32_t triangle=std::numeric_limits<std::uint32_t>::max();
    Point point{};
};
struct Vertical { Hit above{},below{}; };
struct Report {
    Point cockpitCenter{},cameraEye{};
    Hit cameraToCockpit{};
    Vertical cockpitVertical{},cameraVertical{};
    std::uint32_t trianglesScanned=0;
};
enum class Error { none,mesh,bounds,camera,arithmetic };
struct Diagnostic { Error error=Error::none;std::uint32_t triangle=0; };

namespace detail {
inline Point subtract(Point a,Point b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline Point cross(Point a,Point b) noexcept {
    return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};
}
inline double dot(Point a,Point b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline double largest(Point p) noexcept {return std::max({std::fabs(p.x),std::fabs(p.y),std::fabs(p.z)});}
inline bool finite(Point p) noexcept {return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
inline Point point(const rr::assets::Vertex48B& v) noexcept {
    return {v.position[0],v.position[1],v.position[2]};
}
enum class Intersection { miss,hit,arithmetic };
// Two-sided Moller-Trumbore line intersection. Parallel/coplanar and degenerate
// triangles do not give a unique surface crossing and are deliberately skipped.
inline Intersection intersect(Point origin,Point direction,Point a,Point b,Point c,
    double& t) noexcept {
    const Point first=subtract(b,a),second=subtract(c,a);
    const Point p=cross(direction,second);
    const double determinant=dot(first,p);
    const double scale=largest(first)*largest(second)*largest(direction);
    if(!std::isfinite(determinant)||!std::isfinite(scale))return Intersection::arithmetic;
    constexpr double relativeTolerance=64.0*std::numeric_limits<double>::epsilon();
    if(std::fabs(determinant)<=relativeTolerance*scale)return Intersection::miss;
    const Point offset=subtract(origin,a),q=cross(offset,first);
    const double u=dot(offset,p)/determinant,v=dot(direction,q)/determinant;
    const double candidate=dot(second,q)/determinant;
    if(!std::isfinite(u)||!std::isfinite(v)||!std::isfinite(candidate))return Intersection::arithmetic;
    // A small barycentric tolerance keeps a shared edge from becoming a crack.
    constexpr double edgeTolerance=1e-10;
    if(u < -edgeTolerance||v < -edgeTolerance||u+v > 1.0+edgeTolerance)return Intersection::miss;
    t=candidate;return Intersection::hit;
}
inline bool remember(Hit& hit,double distance,std::uint32_t triangle,Point p) noexcept {
    if(!std::isfinite(distance)||!finite(p))return false;
    // Exact ties retain original triangle order.
    if(!hit.hit||distance<hit.t)hit={true,distance,triangle,p};
    return true;
}
inline bool vertical(Vertical& result,Point origin,Point a,Point b,Point c,
    std::uint32_t triangle) noexcept {
    double t=0.0;
    const auto status=intersect(origin,{0.0,1.0,0.0},a,b,c,t);
    if(status==Intersection::arithmetic)return false;
    if(status==Intersection::miss)return true;
    const Point p{origin.x,origin.y+t,origin.z};
    if(t>=0.0&&!remember(result.above,t,triangle,p))return false;
    if(t<=0.0&&!remember(result.below,-t,triangle,p))return false;
    return true;
}
}

// Read-only bounded scan of the actual retained track triangles. Output is
// unchanged on false; diagnostic describes rejection. Inputs/output must not
// overlap. No pointers are retained and no heap storage is allocated.
inline bool inspect(const rr::assets::Mesh48B& track,
    const rr::game::VehicleDrawBatch50CO& batch,
    const rr::platform::VehicleViewCamera50DG& camera,Report& output,
    Diagnostic& diagnostic) noexcept {
    diagnostic={};
    const auto fail=[&](Error error,std::uint32_t triangle=0) noexcept {
        diagnostic={error,triangle};return false;
    };
    if(track.vertices.empty()||track.indices.empty()||
       track.indices.size()>rr::assets::max_mesh_indices||track.indices.size()%3u)
        return fail(Error::mesh);
    rr::platform::VehicleViewMatrix50DG unused{};
    rr::platform::VehicleViewCameraError50DG cameraError{};
    if(!rr::platform::composeVehicleViewCamera50DG(camera,unused,cameraError))return fail(Error::camera);
    const auto& cockpit=batch.component[0];
    if(!cockpit.vertexCount)return fail(Error::bounds);
    double center[3]{};
    for(unsigned axis=0;axis<3;++axis) {
        const double lo=cockpit.bounds.minimum[axis],hi=cockpit.bounds.maximum[axis];
        if(!std::isfinite(lo)||!std::isfinite(hi)||lo>hi)return fail(Error::bounds);
        center[axis]=(lo+hi)*0.5;
    }
    Report next{};
    next.cockpitCenter={center[0],center[1],center[2]};
    next.cameraEye={camera.eye[0],camera.eye[1],camera.eye[2]};
    const Point direction=detail::subtract(next.cockpitCenter,next.cameraEye);
    const auto count=static_cast<std::uint32_t>(track.indices.size()/3u);
    for(std::uint32_t triangle=0;triangle<count;++triangle) {
        const std::size_t at=std::size_t(triangle)*3u;
        const auto ia=track.indices[at],ib=track.indices[at+1u],ic=track.indices[at+2u];
        if(ia>=track.vertices.size()||ib>=track.vertices.size()||ic>=track.vertices.size())
            return fail(Error::mesh,triangle);
        const Point a=detail::point(track.vertices[ia]),b=detail::point(track.vertices[ib]),
                    c=detail::point(track.vertices[ic]);
        if(!detail::finite(a)||!detail::finite(b)||!detail::finite(c))return fail(Error::mesh,triangle);
        double t=0.0;
        const auto status=detail::intersect(next.cameraEye,direction,a,b,c,t);
        if(status==detail::Intersection::arithmetic)return fail(Error::arithmetic,triangle);
        if(status==detail::Intersection::hit&&t>=0.0&&t<=1.0) {
            const Point p{next.cameraEye.x+t*direction.x,next.cameraEye.y+t*direction.y,
                          next.cameraEye.z+t*direction.z};
            if(!detail::remember(next.cameraToCockpit,t,triangle,p))return fail(Error::arithmetic,triangle);
        }
        if(!detail::vertical(next.cockpitVertical,next.cockpitCenter,a,b,c,triangle)||
           !detail::vertical(next.cameraVertical,next.cameraEye,a,b,c,triangle))
            return fail(Error::arithmetic,triangle);
        ++next.trianglesScanned;
    }
    output=next;return true;
}
}
