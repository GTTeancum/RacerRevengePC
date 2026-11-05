#include "../../src/game/original_water_topology_numeric50la.h"
#include <cassert>
#include <cstdio>
namespace w=rr::game::water_topology_numeric50LA;
int main(){
    const auto oldRound=std::fegetround();assert(std::fesetround(FE_TONEAREST)==0);
    std::array<w::Vertex,4> vertices{{{{0,0,0},{0,0}},{{1,0,0},{1,0}},{{0,0,1},{0,1}},{{1,0,1},{1,1}}}};
    std::array<w::Vec3,4> normals{{{0,1,0},{0,2,0},{0,1,0},{0,1,0}}},out{};
    auto r=w::prepare(vertices,normals,out);assert(r.prepared&&r.pc==0x299780u);
    assert((out[0]==w::Vec3{1,0,0})&&(out[1]==w::Vec3{2,0,0}));
    assert((out[2]==w::Vec3{})&&(out[3]==w::Vec3{}));
    normals[0]={1,1,0};
    assert(w::prepare(vertices,normals,out).prepared);
    assert((out[0]==w::Vec3{1,-1,0})); //Ordered double cross projects off input.
    normals[0]={0,1,0};vertices[1].position={1,0,1};
    assert(w::prepare(std::span(vertices).first(3),std::span(normals).first(3),std::span(out).first(3)).prepared);
    const float diagonal=1.f/std::sqrt(2.f);
    assert(std::abs(out[0][0]-diagonal)<0.000001f&&std::abs(out[0][2]-diagonal)<0.000001f);
    vertices[1].position={1,0,0};
    //Alternate UV denominator branch, then mirrored U sign correction.
    std::swap(vertices[1],vertices[2]);
    assert(w::prepare(std::span(vertices).first(3),std::span(normals).first(3),std::span(out).first(3)).prepared);
    assert((out[0]==w::Vec3{1,0,0}));
    for(auto& v:vertices)v.uv[0]=-v.uv[0];
    assert(w::prepare(std::span(vertices).first(3),std::span(normals).first(3),std::span(out).first(3)).prepared);
    assert((out[0]==w::Vec3{-1,0,0}));
    //Degenerate UVs refuse before publishing any prepared output.
    for(auto& v:vertices)v.uv={0,0};
    out.fill({7,8,9});const auto sentinel=out;
    assert(!w::prepare(vertices,normals,out).prepared&&out==sentinel);
    //Exactly two records execute only tails: vertex XYZ/UV are not consumed.
    vertices[0].position[0]=std::numeric_limits<float>::quiet_NaN();
    assert(w::prepare(std::span(vertices).first(2),std::span(normals).first(2),std::span(out).first(2)).prepared);
    assert((out[0]==w::Vec3{})&&(out[1]==w::Vec3{}));
    out=sentinel;normals[0][0]=std::numeric_limits<float>::infinity();
    assert(!w::prepare(std::span(vertices).first(2),std::span(normals).first(2),std::span(out).first(2)).prepared&&out==sentinel);
    assert(!w::prepare(std::span(vertices).first(1),std::span(normals).first(1),std::span(out).first(1)).prepared&&out==sentinel);
    assert(!w::prepare(vertices,std::span(normals).first(3),out).prepared&&out==sentinel);
    assert(std::fesetround(FE_DOWNWARD)==0);
    assert(!w::prepare(vertices,normals,out).prepared&&out==sentinel);
    assert(std::fesetround(oldRound)==0);
    std::puts("Water topology numeric: UV branches/sign, normal scale, zero tails, refusal atomicity passed");
}
