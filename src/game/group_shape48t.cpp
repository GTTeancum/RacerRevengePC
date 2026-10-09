#include "group_shape48t.h"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
struct Span {std::uintptr_t begin=0,end=0;};
template<class T>bool span(const T* p,Span& out) noexcept {
    out={};if(!p||reinterpret_cast<std::uintptr_t>(p)%alignof(T))return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(p);
    if(sizeof(T)>std::numeric_limits<std::uintptr_t>::max()-begin)return false;
    out={begin,begin+sizeof(T)};return true;
}
bool overlap(Span a,Span b) noexcept {return a.begin<b.end&&b.begin<a.end;}
struct Math {
    bool valid=true;
    float keep(float f) noexcept {valid=valid&&std::isfinite(f);return f;}
    float add(float a,float b) noexcept {volatile float f=keep(a)+keep(b);return keep(f);}
    float mul(float a,float b) noexcept {volatile float f=keep(a)*keep(b);return keep(f);}
    float root(float a) noexcept {volatile float f=std::sqrt(keep(a));return keep(f);}
};
}
GroupShapeStatus48T setGroupShape0Bounded48T(
    const GroupShapeSource48T& source,const Vector3_48C& translation,
    const GroupShapeState48T& initial,GroupShapeState48T& destination,
    GroupShapeSphere48T& sphere) noexcept {
    using Status=GroupShapeStatus48T;
    Span reads[4]{},writes[2]{};
    if(!span(&source,reads[0])||!span(&translation,reads[1])||!span(&initial,reads[2])||
       !span(&destination,writes[0])||!span(&sphere,writes[1]))return Status::InvalidInput;
    if(source.geometry&&!span(source.geometry,reads[3]))return Status::InvalidInput;
    if(overlap(writes[0],writes[1]))return Status::InvalidInput;
    for(auto write:writes)for(auto read:reads)if(overlap(write,read))return Status::InvalidInput;
    if(!source.geometry)return Status::MissingGeometry;

    GroupShapeState48T result{};GroupShapeSphere48T resultSphere{};Math math;
    //135EC0 copies all sixteen words before135EE0 substitutes XYZ in the
    // stack snapshot.135EF8..135F0C modifies shape+B0/B4/B8 BEFORE11F6D8
    // copies shape+80 to+C0. Both completed matrices thus have new translation.
    std::memcpy(&result.world80,&initial.world80,sizeof(result.world80));
    for(unsigned axis=0;axis<3;++axis)result.world80.v[12+axis]=translation.v[axis];
    std::memcpy(&result.previousC0,&result.world80,sizeof(result.previousC0));

    //11F748..11F78C is four-term VU matrix multiplication, including W.
    // Do not fold sparse/zero terms or replace the final multiply by an add.
    for(unsigned column=0;column<4;++column)for(unsigned lane=0;lane<4;++lane){
        float value=math.mul(result.world80.v[lane],source.local40.v[4*column]);
        value=math.add(value,math.mul(result.world80.v[4+lane],source.local40.v[4*column+1]));
        value=math.add(value,math.mul(result.world80.v[8+lane],source.local40.v[4*column+2]));
        result.composed00.v[4*column+lane]=math.add(value,math.mul(result.world80.v[12+lane],source.local40.v[4*column+3]));
    }
    //11F7B0..11F81C copies geometry center, computes unscaled half length,
    // then11F838..11F848 transforms center XYZ and forces its W to positive1.
    const auto& geometry=*source.geometry;
    const float xx=math.mul(geometry.localHalf.v[0],geometry.localHalf.v[0]);
    const float yy=math.mul(geometry.localHalf.v[1],geometry.localHalf.v[1]);
    const float xy=math.add(xx,yy);
    resultSphere.radius=math.root(math.add(xy,math.mul(geometry.localHalf.v[2],geometry.localHalf.v[2])));
    for(unsigned axis=0;axis<3;++axis){
        float value=math.mul(result.world80.v[axis],geometry.localCenter.v[0]);
        value=math.add(value,math.mul(result.world80.v[4+axis],geometry.localCenter.v[1]));
        value=math.add(value,math.mul(result.world80.v[8+axis],geometry.localCenter.v[2]));
        resultSphere.center.v[axis]=math.add(value,result.world80.v[12+axis]);
    }
    resultSphere.center.v[3]=1.f;
    if(!math.valid)return Status::NonFinite;
    std::memcpy(&destination,&result,sizeof(result));
    std::memcpy(&sphere,&resultSphere,sizeof(resultSphere));return Status::Complete;
}
}
