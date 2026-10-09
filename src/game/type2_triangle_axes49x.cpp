#include "type2_triangle_axes49x.h"
#include <cmath>
#include <cstring>

namespace rr::game::type2_axis49x {
namespace {
float fp(std::uint32_t u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
std::uint32_t bits(float f) noexcept {std::uint32_t u;std::memcpy(&u,&f,4);return u;}
std::int32_t signedBits(float f) noexcept {std::int32_t n;std::memcpy(&n,&f,4);return n;}
struct Math {
    bool valid=true;
    float read(float a) noexcept {valid=valid&&std::isfinite(a);return a;}
    float add(float a,float b) noexcept {read(a);read(b);volatile float r=a+b;return read(r);}
    float sub(float a,float b) noexcept {read(a);read(b);volatile float r=a-b;return read(r);}
    float mul(float a,float b) noexcept {read(a);read(b);volatile float r=a*b;return read(r);}
    float div(float a,float b) noexcept {read(a);read(b);volatile float r=a/b;return read(r);}
    float neg(float a) noexcept {read(a);return fp(bits(a)^0x80000000u);}
    float abs(float a) noexcept {read(a);return fp(bits(a)&0x7fffffffu);}
    float minimum(float a,float b) noexcept {
        read(a);read(b);const auto x=signedBits(a),y=signedBits(b);
        return fp(static_cast<std::uint32_t>((x<0&&y<0)?(x>y?x:y):(x<y?x:y)));
    }
    float maximum(float a,float b) noexcept {
        read(a);read(b);const auto x=signedBits(a),y=signedBits(b);
        return fp(static_cast<std::uint32_t>((x<0&&y<0)?(x<y?x:y):(x>y?x:y)));
    }
    float dot(const Vector& a,const Vector& b) noexcept {
        const float x=mul(a[0],b[0]),y=mul(a[1],b[1]),z=mul(a[2],b[2]);
        // VMUL.xyz; VADDW.x(0,1); VADDAY.x; VMADDZ.x.
        return add(add(x,y),mul(1.f,z));
    }
    Vector difference(const Vector& a,const Vector& b) noexcept {
        return {{sub(a[0],b[0]),sub(a[1],b[1]),sub(a[2],b[2]),1.f}};
    }
    Vector normalize(const Vector& a) noexcept {
        const float squared=dot(a,a);float q;
        if(squared!=0.f&&(bits(squared)&0x7f800000u)==0)valid=false;
        if(squared==0.f)q=fp((bits(squared)&0x80000000u)|0x7f7fffffu);
        else {volatile float root=std::sqrt(std::fabs(squared));q=div(1.f,read(root));}
        return {{mul(a[0],q),mul(a[1],q),mul(a[2],q),1.f}};
    }
    Vector transform(const std::array<float,16>& m,const Vector& v,bool isPoint) noexcept {
        Vector r{{0.f,0.f,0.f,1.f}};
        for(unsigned i=0;i<3;++i){
            const float p0=mul(m[i],v[0]);const float p1=mul(m[4+i],v[1]);
            const float p2=mul(m[8+i],v[2]);
            r[i]=add(add(add(p0,p1),p2),isPoint?m[12+i]:0.f);
        }
        //107DD0 adds vf0 to all lanes after direction transform.
        if(!isPoint)r[3]=add(r[3],1.f);
        return r;
    }
};
Vector xyz(const std::array<float,3>& a) noexcept {return {{a[0],a[1],a[2],1.f}};}
}
Outcome overlap(const Input& input,Trace* trace) noexcept {
    Math f;
    for(float x:input.center)f.read(x);
    for(float x:input.half)f.read(x);
    for(const auto& v:input.vertex)for(float x:v)f.read(x);
    for(float x:input.targetToQuery)f.read(x);
    if(!f.valid)return Outcome::InvalidNumeric;
    if(trace){*trace=Trace{};for(auto& axis:trace->axes)axis={{0.f,0.f,0.f,1.f}};}
    const float scale=fp(0x38000100u);
    Vector decoded{{0.f,0.f,0.f,1.f}};
    for(unsigned i=0;i<3;++i)decoded[i]=f.mul(scale,static_cast<float>(input.biasedFaceNormal[i]));
    std::array<Vector,3> vertex;
    for(unsigned i=0;i<3;++i)vertex[i]=f.transform(input.targetToQuery,xyz(input.vertex[i]),true);
    const Vector normal=f.transform(input.targetToQuery,decoded,false);
    const Vector relative=f.difference(vertex[0],xyz(input.center));
    if(trace){trace->transformedVertex=vertex;trace->transformedNormal=normal;trace->firstRelativeToCenter=relative;}
    Vector edgeFirst{},edgeSecond{},edgeNormalized{};
    for(unsigned index=0;index<13;++index){
        Vector axis{{0.f,0.f,0.f,1.f}};
        if(index==0)axis=normal;
        else if(index<4)axis[index-1]=1.f;
        else {
            //107F60..107FE0 deliberately uses third-minus-first, NOT B-C.
            //It is normalized only when axis10 is reached.
            if(index==10){
                edgeNormalized=f.normalize(f.difference(vertex[2],vertex[0]));
                if(trace)trace->normalizedThirdMinusFirst=edgeNormalized;
            }
            const Vector& edge=index<7?edgeFirst:(index<10?edgeSecond:edgeNormalized);
            switch((index-4)%3){
                case 0:axis[1]=f.neg(edge[2]);axis[2]=edge[1];break;
                case 1:axis[0]=edge[2];axis[2]=f.neg(edge[0]);break;
                default:axis[0]=f.neg(edge[1]);axis[1]=edge[0];break;
            }
        }
        const float x=f.abs(f.mul(axis[0],input.half[0]));
        const float y=f.abs(f.mul(axis[1],input.half[1]));
        const float z=f.abs(f.mul(axis[2],input.half[2]));
        const float radius=f.add(z,f.add(x,y));
        const float first=f.dot(relative,axis);
        if(trace){trace->axes[index]=axis;trace->radius[index]=radius;trace->projection[index][0]=first;trace->tested=index+1;}
        if(!f.valid)return Outcome::InvalidNumeric;
        bool separate=false;
        if(index==0){
            separate=!(f.abs(first)<=radius); //1080E4/EC; touching is included.
            if(!separate){
                edgeFirst=f.difference(vertex[0],vertex[1]);
                edgeSecond=f.difference(vertex[0],vertex[2]);
                if(trace){trace->firstMinusSecond=edgeFirst;trace->firstMinusThird=edgeSecond;}
            }
        } else {
            //108190..10820C: reuse first projection minus projected edges.
            //Directly projecting B-center and C-center changes binary32 bits.
            const float firstEdge=f.dot(edgeFirst,axis);
            const float secondEdge=f.dot(edgeSecond,axis);
            const float second=f.sub(first,firstEdge);
            const float third=f.sub(first,secondEdge);
            const float low=f.minimum(first,f.minimum(second,third));
            if(trace){trace->projection[index][1]=second;trace->projection[index][2]=third;trace->minMax[index][0]=low;}
            if(!f.valid)return Outcome::InvalidNumeric;
            separate=radius<low; //108234/3C; strict upper-side rejection.
            if(!separate){
                const float high=f.maximum(first,f.maximum(second,third));
                if(trace)trace->minMax[index][1]=high;
                separate=!(f.neg(radius)<=high); //108270/78.
            }
        }
        if(!f.valid)return Outcome::InvalidNumeric;
        if(separate){if(trace)trace->rejectedAxis=static_cast<std::int32_t>(index);return Outcome::Separated;}
    }
    return Outcome::Overlap;
}
}
