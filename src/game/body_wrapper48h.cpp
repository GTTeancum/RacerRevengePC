#include "body_wrapper48h.h"
#include <cmath>
#include <cstring>
#include <initializer_list>

namespace rr::game {
namespace {
using V=Vector3_48C;
float fp(std::uint32_t u) noexcept {float x;std::memcpy(&x,&u,4);return x;}
std::uint32_t bits(float x) noexcept {std::uint32_t u;std::memcpy(&u,&x,4);return u;}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return keep(x);}
    float div(float a,float b) noexcept {volatile float x=a/b;return keep(x);}
    float root(float a) noexcept {
        if((bits(a)&0x7f800000u)==0)return fp(bits(a)&0x80000000u);
        volatile float x=static_cast<float>(std::sqrt(static_cast<double>(a)));return keep(x);
    }
    float squared(const V&v) noexcept {return add(add(mul(v.v[0],v.v[0]),mul(v.v[1],v.v[1])),mul(v.v[2],v.v[2]));}
    V rotate(const BodyMatrix48G&m,const V&v,bool point=false) noexcept {
        V out{};for(unsigned i=0;i<3;++i){out.v[i]=add(add(mul(m.v[i],v.v[0]),mul(m.v[4+i],v.v[1])),mul(m.v[8+i],v.v[2]));out.v[i]=add(out.v[i],point?m.v[12+i]:0.0f);}return out;
    }
    V inverse(const BodyMatrix48G&m,const V&v) noexcept {
        V out{};const float zero=sub(0,0);
        for(unsigned i=0;i<3;++i){float a[3];for(unsigned j=0;j<3;++j)a[j]=i==j?add(m.v[4*i+j],zero):add(0,m.v[4*i+j]);
            out.v[i]=add(add(add(mul(a[0],v.v[0]),mul(a[1],v.v[1])),mul(a[2],v.v[2])),0);}
        return out;
    }
    float sine(float x) noexcept {
        if(x>fp(0x3fc90fdb))x=sub(fp(0x40490fdb),x);
        else if(x<fp(0xbfc90fdb))x=sub(fp(0xc0490fdb),x);
        const float d=mul(x,x);
        float t=add(fp(0xb9500d01),mul(fp(0x3638ef1d),d));
        t=add(fp(0x3c088889),mul(d,t));t=add(fp(0xbe2aaaab),mul(d,t));
        return add(x,mul(mul(x,d),t));
    }
    float cosine(float x) noexcept {
        float sign=1;
        if(x>fp(0x3fc90fdb)){x=sub(fp(0x40490fdb),x);sign=-1;}
        else if(x<fp(0xbfc90fdb)){x=sub(fp(0xc0490fdb),x);sign=-1;}
        const float d=mul(x,x);
        float t=add(fp(0x37d00d01),mul(fp(0xb493f27e),d));
        t=add(fp(0xbab60b61),mul(d,t));t=add(fp(0x3d2aaaab),mul(d,t));
        t=add(-.5f,mul(d,t));return mul(sign,add(1,mul(d,t)));
    }
    void correction(BodyMatrix48G&m,const V&local) noexcept {
        V axis=rotate(m,local);const float length=root(squared(axis));
        if(length!=0){const float inv=div(1,length);for(float&x:axis.v)x=mul(x,inv);}
        const float c=cosine(length),s=sine(length),t=sub(1,c);
        // 112B20 normalizes again using VU sqrt-then-divide semantics.
        const float d=squared(axis);float q;
        if(d==0)q=fp((bits(d)&0x80000000u)|0x7f7fffffu);
        else {volatile float r=static_cast<float>(std::sqrt(std::fabs(d)));q=div(1,r);}
        for(float&x:axis.v)x=mul(x,q);
        BodyMatrix48G r{};V a{};for(unsigned i=0;i<3;++i)a.v[i]=mul(axis.v[i],s);
        for(unsigned col=0;col<3;++col)for(unsigned row=0;row<3;++row)r.v[col*4+row]=mul(mul(axis.v[row],axis.v[col]),t);
        r.v[0]=add(r.v[0],c);r.v[1]=add(r.v[1],a.v[2]);r.v[2]=sub(r.v[2],a.v[1]);
        r.v[4]=sub(r.v[4],a.v[2]);r.v[5]=add(r.v[5],c);r.v[6]=add(r.v[6],a.v[0]);
        r.v[8]=add(r.v[8],a.v[1]);r.v[9]=sub(r.v[9],a.v[0]);r.v[10]=add(r.v[10],c);
        r.v[3]=r.v[7]=r.v[11]=sub(0,0);r.v[15]=1;
        const auto old=m;
        for(unsigned col=0;col<3;++col)for(unsigned row=0;row<4;++row)
            m.v[col*4+row]=add(add(add(mul(old.v[row],r.v[col*4]),mul(old.v[4+row],r.v[col*4+1])),mul(old.v[8+row],r.v[col*4+2])),mul(old.v[12+row],r.v[col*4+3]));
    }
};
bool finite(const V&v) noexcept {for(float x:v.v)if(!std::isfinite(x))return false;return true;}
void clear(RootBodyState48H&r) noexcept {
    r.loads={};r.accumulator1c0={};r.accumulator1d0={};r.positionCorrection1a0={};r.rotationCorrection1b0={};r.callbackState238=0;
}
}
bool updateRootBody48H(const RootBodyState48H&input,const BodyParameters48C&p,
    const V&center,bool attached,bool enabled,float dt,RootBodyState48H&output) noexcept {
    if(!attached||!enabled){auto r=input;clear(r);output=r;return true;}
    for(float x:input.scene.v)if(!std::isfinite(x))return false;
    for(const V*v:{&center,&input.positionCorrection1a0,&input.rotationCorrection1b0})if(!finite(*v))return false;
    Math f;auto r=input;r.previousVelocity90=r.body.velocity;
    r.body.position=f.rotate(r.scene,center,true);
    if(!matrixToQuaternion48G(r.scene,r.body.orientation)||!f.valid)return false;
    if(!integrateBody48C(r.body,p,r.loads,dt,r.body)||!quaternionToMatrix48G(r.body.orientation,r.scene))return false;
    const auto& a=r.rotationCorrection1b0.v;
    if(a[0]!=0||a[1]!=0||a[2]!=0)f.correction(r.scene,r.rotationCorrection1b0);
    const V point=f.rotate(r.scene,center,true);
    for(unsigned i=0;i<3;++i){
        r.body.position.v[i]=f.add(r.body.position.v[i],f.sub(r.positionCorrection1a0.v[i],point.v[i]));
        r.scene.v[12+i]=f.add(r.scene.v[12+i],r.body.position.v[i]);
    }
    r.localVelocity80=f.inverse(r.scene,r.body.velocity);
    r.worldAngularVelocityA0=f.rotate(r.scene,r.body.angularVelocity);
    clear(r);
    if(!f.valid)return false;output=r;return true;
}
}
