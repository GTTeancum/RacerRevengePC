#include "body_scene48g.h"
#include <cmath>
#include <cstdint>
#include <cstring>

namespace rr::game {
namespace {
float fp(std::uint32_t b) noexcept {float f;std::memcpy(&f,&b,4);return f;}
std::uint32_t bits(float f) noexcept {std::uint32_t b;std::memcpy(&b,&f,4);return b;}
struct Arithmetic {
    bool valid=true;
    float keep(float f) noexcept {valid=valid&&std::isfinite(f);return f;}
    float add(float a,float b) noexcept {volatile float f=a+b;return keep(f);}
    float sub(float a,float b) noexcept {volatile float f=a-b;return keep(f);}
    float mul(float a,float b) noexcept {volatile float f=a*b;return keep(f);}
    float div(float a,float b) noexcept {volatile float f=a/b;return keep(f);}
    float root(float x) noexcept {
        // Same finite square-root convention as the recovered body core.
        if((bits(x)&0x7f800000u)==0)return fp(bits(x)&0x80000000u);
        volatile float f=static_cast<float>(std::sqrt(static_cast<double>(x)));return keep(f);
    }
};
bool finite(const Quaternion48C& q) noexcept {for(float x:q.v)if(!std::isfinite(x))return false;return true;}
float angle(float y,float x,Arithmetic& f) noexcept {
    // ELF 13C1F0..13C550: axis behavior differs from the C library for -0.
    const float halfPi=fp(0x3fc90fdbu),pi=fp(0x40490fdbu),c=fp(0x3e8f5c29u);
    if(x==0)return y>0 ? halfPi:(y<0 ? -halfPi:0.0f);
    if(y==0)return x>0 ? 0.0f:pi;
    const float a=std::fabs(f.div(y,x));
    if(a<=1) {
        const float denominator=f.add(1,f.mul(f.mul(c,a),a));
        if(y>0)return x>0 ? f.div(a,denominator):f.sub(pi,f.div(a,denominator));
        return x>0 ? f.div(-a,denominator):f.add(-pi,f.div(a,denominator));
    }
    const float ratio=f.div(a,f.add(c,f.mul(a,a)));
    if(y>0)return x>0 ? f.sub(halfPi,ratio):f.add(halfPi,ratio);
    return x>0 ? f.add(-halfPi,ratio):f.sub(-halfPi,ratio);
}
}
bool quaternionToMatrix48G(const Quaternion48C& q,BodyMatrix48G& output) noexcept {
    if(!finite(q))return false;
    Arithmetic f;const float w=q.v[0],x=q.v[1],y=q.v[2],z=q.v[3];
    const float norm=f.add(f.add(f.mul(y,y),f.add(f.mul(w,w),f.mul(x,x))),f.mul(z,z));
    const float scale=norm<=0 ? 0.0f:f.div(2,norm);
    const float ty=f.mul(y,scale),tx=f.mul(x,scale),tz=f.mul(z,scale);
    const float zz=f.mul(z,tz),wx=f.mul(w,tx),wy=f.mul(w,ty),xx=f.mul(x,tx);
    const float xy=f.mul(x,ty),wz=f.mul(w,tz),xz=f.mul(x,tz),yz=f.mul(y,tz),yy=f.mul(y,ty);
    BodyMatrix48G result{{f.sub(1,f.add(yy,zz)),f.add(xy,wz),f.sub(xz,wy),0,
                         f.sub(xy,wz),f.sub(1,f.add(xx,zz)),f.add(yz,wx),0,
                         f.add(xz,wy),f.sub(yz,wx),f.sub(1,f.add(xx,yy)),0,
                         0,0,0,1}};
    if(!f.valid)return false;output=result;return true;
}
bool matrixToQuaternion48G(const BodyMatrix48G& matrix,Quaternion48C& output) noexcept {
    const auto& m=matrix.v;
    for(unsigned r=0;r<3;++r)for(unsigned c=0;c<3;++c)if(!std::isfinite(m[r*4+c]))return false;
    Arithmetic f;
    const float plus=f.add(1,m[0]),minus=f.sub(1,m[0]);
    const float t0=f.mul(.25f,f.add(m[10],f.add(plus,m[5])));
    const float t1=f.mul(.25f,f.sub(f.sub(plus,m[5]),m[10]));
    const float t2=f.mul(.25f,f.sub(f.add(minus,m[5]),m[10]));
    const float t3=f.mul(.25f,f.add(m[10],f.sub(minus,m[5])));
    Quaternion48C q{};
    if(t0>t1&&t0>t2&&t0>t3) {
        q.v[0]=f.root(t0);const float s=f.div(.25f,q.v[0]);
        q.v[1]=f.mul(s,f.sub(m[6],m[9]));q.v[2]=f.mul(s,f.sub(m[8],m[2]));q.v[3]=f.mul(s,f.sub(m[1],m[4]));
    } else if(t1>t2&&t1>t3) {
        q.v[1]=f.root(t1);const float s=f.div(.25f,q.v[1]);
        q.v[0]=f.mul(s,f.sub(m[6],m[9]));q.v[2]=f.mul(s,f.add(m[1],m[4]));q.v[3]=f.mul(s,f.add(m[2],m[8]));
    } else if(t2>t3) {
        q.v[2]=f.root(t2);const float s=f.div(.25f,q.v[2]);
        q.v[0]=f.mul(s,f.sub(m[8],m[2]));q.v[1]=f.mul(s,f.add(m[1],m[4]));q.v[3]=f.mul(s,f.add(m[6],m[9]));
    } else {
        q.v[3]=f.root(t3);const float s=f.div(.25f,q.v[3]);
        q.v[0]=f.mul(s,f.sub(m[1],m[4]));q.v[1]=f.mul(s,f.add(m[2],m[8]));q.v[2]=f.mul(s,f.add(m[6],m[9]));
    }
    if(!f.valid)return false;output=q;return true;
}
bool originalAngle48G(float y,float x,float& output) noexcept {
    if(!std::isfinite(y)||!std::isfinite(x))return false;
    Arithmetic f;const float a=angle(y,x,f);if(!f.valid)return false;output=a;return true;
}
bool quaternionToAngles48G(const Quaternion48C& q,Vector3_48C& output) noexcept {
    if(!finite(q))return false;
    Arithmetic f;const float w=q.v[0],x=q.v[1],y=q.v[2],z=q.v[3];
    const float a=f.mul(2,f.add(f.mul(x,y),f.mul(w,z)));
    const float b=f.sub(1,f.mul(2,f.add(f.mul(x,x),f.mul(z,z))));
    const float c=f.mul(2,f.add(f.mul(x,z),f.mul(w,y)));
    const float e=f.mul(2,f.sub(f.mul(y,z),f.mul(w,x)));
    const float d=f.sub(1,f.mul(2,f.add(f.mul(x,x),f.mul(y,y))));
    Vector3_48C result{};result.v[1]=-angle(c,d,f);result.v[2]=-angle(a,b,f);
    result.v[0]=-angle(-e,f.root(f.add(f.mul(a,a),f.mul(b,b))),f);
    if(!f.valid)return false;output=result;return true;
}
}
