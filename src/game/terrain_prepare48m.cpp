#include "terrain_prepare48m.h"
#include <cmath>

namespace rr::game {
namespace {
using V=Vector3_48C;
bool finite(const V& v) noexcept {
    for(float x:v.v)if(!std::isfinite(x))return false;
    return true;
}
bool affine(const BodyMatrix48G& m) noexcept {
    for(float x:m.v)if(!std::isfinite(x))return false;
    return m.v[3]==0.f&&m.v[7]==0.f&&m.v[11]==0.f&&m.v[15]==1.f;
}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return keep(x);}
    float root(float a) noexcept {volatile float x=std::sqrt(a);return keep(x);}
    V point(const BodyMatrix48G& m,const V& v) noexcept {
        V r{};
        for(unsigned i=0;i<3;++i) {
            const float xy=add(mul(m.v[i],v.v[0]),mul(m.v[4+i],v.v[1]));
            r.v[i]=add(add(xy,mul(m.v[8+i],v.v[2])),m.v[12+i]);
        }
        return r;
    }
};
}
TerrainPrepareStatus48M prepareTerrainSinglePair48M(
    const TerrainPrepareInput48M& input,TerrainPrepareState48M& output) noexcept {
    if(!input.bodyLinked){output=input.state;return TerrainPrepareStatus48M::NoBody;}
    if(input.auxiliarySource120||input.auxiliarySource124)
        return TerrainPrepareStatus48M::UnsupportedScope;
    if(!affine(input.scene)||!finite(input.lowerEndpoint140))
        return TerrainPrepareStatus48M::InvalidInput;
    auto result=input.state;
    result.terrain.surface.forceAngleTest108=0;
    result.terrain.surface.hit110=0;result.terrain.surface.hit114=0;
    result.auxiliaryAbove118=0;result.terrain.collectedCount=0;
    result.terrain.excludedOwnerIdentity=0;result.follow.largestVelocityChange1d0={};
    Math f;
    // This seed always uses the full scene, independently of active104.
    result.terrain.surface.point150=f.point(input.scene,input.lowerEndpoint140);
    if(!f.valid)return TerrainPrepareStatus48M::InvalidInput;
    if(!input.queryEnabled100){output=result;return TerrainPrepareStatus48M::QueryDisabled;}
    if(input.shapeType88!=2||input.pairCount!=1||input.hasPairTree)
        return TerrainPrepareStatus48M::UnsupportedScope;
    if(!finite(input.state.segment.first)||!finite(input.state.segment.second))
        return TerrainPrepareStatus48M::InvalidInput;
    if(input.state.follow.active104)result.segment.worldMatrix=input.scene;
    else {
        result.segment.worldMatrix={};
        for(unsigned i=0;i<4;++i)result.segment.worldMatrix.v[i*5]=1.f;
        for(unsigned i=0;i<3;++i)result.segment.worldMatrix.v[12+i]=input.scene.v[12+i];
    }
    V midpoint{},half{};
    for(unsigned i=0;i<3;++i) {
        // Preserve the original second+first and second-first operation order.
        midpoint.v[i]=f.mul(.5f,f.add(result.segment.second.v[i],result.segment.first.v[i]));
        half.v[i]=f.mul(.5f,f.sub(result.segment.second.v[i],result.segment.first.v[i]));
    }
    const float xy=f.add(f.mul(half.v[0],half.v[0]),f.mul(half.v[1],half.v[1]));
    result.sphere.radius=f.root(f.add(xy,f.mul(half.v[2],half.v[2])));
    // The original radius remains local even for a nonorthogonal scene basis.
    result.sphere.center=f.point(result.segment.worldMatrix,midpoint);
    if(!f.valid)return TerrainPrepareStatus48M::InvalidInput;
    output=result;return TerrainPrepareStatus48M::QueryReady;
}
}
