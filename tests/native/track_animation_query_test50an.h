#pragma once
#include "../../src/game/compound_query50ij.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>


namespace track_animation_query_test50AN {
// A test ray intersects the middle of a native moving face after publication to
// the live world. Query in world space so stale owner bounds or child transforms
// can reject the ray. The production collision dispatcher handles the intersection.
inline bool contact(const rr::game::CompoundCollisionView50IJ& view, unsigned owner,
                    std::size_t row, float time, unsigned sample) {
    namespace g=rr::game;
    if(!view.count||!view.parts||!view.parts[0].mesh)return false;
    const auto& part=view.parts[0];
    const auto& mesh=*part.mesh;
    if(mesh.faces.empty()||!part.active)return false;
    const auto& face=mesh.faces.front();
    g::compound50ij_detail::Math math;
    g::Vector3_48C midpoint{},normal{};
    for(unsigned axis=0;axis<3;++axis){
        for(auto id:face.vertices)midpoint.v[axis]+=mesh.vertices.at(id)[axis]/3.f;
        normal.v[axis]=static_cast<float>(face.normal[axis]);
    }
    const auto centre=math.transform(part.worldMatrix,midpoint,true);
    auto direction=math.transform(part.worldMatrix,normal,false);
    const float length=std::hypot(direction.v[0],direction.v[1],direction.v[2]);
    if(!math.valid||!(length>0.f))return false;
    g::LocalSegment48I ray{};
    for(unsigned i=0;i<4;++i)ray.worldMatrix.v[5*i]=1.f;
    for(unsigned axis=0;axis<3;++axis){
        const float delta=2.f*direction.v[axis]/length;
        ray.first.v[axis]=centre.v[axis]+delta;ray.second.v[axis]=centre.v[axis]-delta;
    }
    std::array<std::uint32_t,256> stack{};
    g::CollisionResult48I hit{};
    auto status=g::queryCompoundPair50IJ(ray,view,owner,{stack.data(),stack.size()},hit);
    if(status==g::CollisionQueryStatus48I::Miss){
        std::swap(ray.first,ray.second);
        status=g::queryCompoundPair50IJ(ray,view,owner,{stack.data(),stack.size()},hit);
    }
    std::fprintf(stderr,"moving_collision_probe row=%zu sample=%u time=%g centre=%g,%g,%g status=%u owner=%u hit_owner=%u face=%u fraction=%g\n",
        row,sample,double(time),double(centre.v[0]),double(centre.v[1]),double(centre.v[2]),unsigned(status),
        owner,hit.selected.candidateId,hit.selected.face,double(hit.selected.fraction));
    return status==g::CollisionQueryStatus48I::Hit&&hit.selected.candidateId==owner&&
        std::abs(hit.selected.fraction-.5f)<.02f;
}
}
