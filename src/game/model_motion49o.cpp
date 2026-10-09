#include "model_motion49o.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <limits>

namespace rr::game {
namespace {
static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559,
              "49O motion requires IEEE binary32");
#include "model_motion49o_tables.inc"
float value(std::uint32_t u) noexcept { float f;std::memcpy(&f,&u,4);return f; }
using Q=std::array<float,4>;
using V=std::array<float,3>;
struct Eval {
    bool valid=true;
    float finite(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return finite(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return finite(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return finite(x);}
    float div(float a,float b) noexcept {if(b==0){valid=false;return 0;}volatile float x=a/b;return finite(x);}
    float sine(float x) noexcept {
        if(x>value(0x3fc90fdb))x=sub(value(0x40490fdb),x);
        else if(x<value(0xbfc90fdb))x=sub(value(0xc0490fdb),x);
        float t=mul(x,x),c=add(value(0xb9500d01),mul(value(0x3638ef1d),t));
        c=add(value(0x3c088889),mul(t,c));c=add(value(0xbe2aaaab),mul(t,c));
        return add(x,mul(mul(x,t),c));
    }
    float acosPositive(float x) noexcept {
        //14FD00 supplies the nonnegative absolute quaternion dot product.
        if(!(x>=0&&x<1)){valid=false;return 0;}
        float scaled=mul(2048,x);auto i=static_cast<std::uint32_t>(scaled);
        if(i>=2048){valid=false;return 0;}
        float u=sub(scaled,static_cast<float>(i));
        return add(mul(sub(1,u),value(acosWords[i])),mul(u,value(acosWords[i+1])));
    }
    float dot(const Q&a,const Q&b) noexcept {
        return add(add(mul(a[3],b[3]),add(mul(a[1],b[1]),mul(a[2],b[2]))),mul(a[0],b[0]));
    }
    Q blend(const Q&a,const Q&b,float t) noexcept {
        float c=dot(a,b),sign=1;if(c<0){sign=-1;c=-c;}if(!(c<1))return a;
        float theta=acosPositive(c),inverse=div(1,sine(theta)),tt=mul(t,theta);
        float bWeight=mul(sign,mul(inverse,sine(tt))),aWeight=mul(inverse,sine(sub(theta,tt)));
        Q q{};for(unsigned i=0;i<4;++i)q[i]=add(mul(aWeight,a[i]),mul(bWeight,b[i]));return q;
    }
    Q decode(const assets::ByteAxisKey49O& k) noexcept {
        float s=value(sineWords[k.angle]);Q q{value(cosineWords[k.angle]),0,0,0};
        for(unsigned i=0;i<3;++i)q[i+1]=mul(mul(value(0x3c010204),static_cast<float>(k.axis[i])),s);
        return q;
    }
    bool rotation(const assets::MotionView49O& v,std::uint32_t ti,float time,Q& result) noexcept {
        const auto n=v.rotation[ti].keyCount;assets::ByteAxisKey49O a{},b{};
        if(n<2||n>assets::maxMotKeysPerTrack49O||!assets::readRotationKey49O(v,ti,0,a)||
           !assets::readRotationKey49O(v,ti,n-1,b)||time<a.time||time>b.time)return false;
        if(time==b.time){result=decode(b);return true;}
        // Same strict upper interval as145850, with checked finite asset view.
        std::uint32_t lo=0,hi=n-1;
        while(hi-lo>1){auto mid=(lo+hi)/2;assets::ByteAxisKey49O k{};
            if(!assets::readRotationKey49O(v,ti,mid,k))return false;
            if(time<k.time)hi=mid;else lo=mid;
        }
        if(!assets::readRotationKey49O(v,ti,lo,a)||!assets::readRotationKey49O(v,ti,hi,b))return false;
        float u=div(sub(time,a.time),sub(b.time,a.time));Q later=decode(b);
        result=blend(decode(a),later,u);
        if(dot(result,later)<0)for(auto& x:result)x=-x;
        return true;
    }
    bool position(const assets::MotionView49O& v,std::uint32_t ti,float time,V& p) noexcept {
        const auto n=v.position[ti].keyCount;assets::BezierKey49O a{},b{};
        if(n<2||n>assets::maxMotKeysPerTrack49O||!assets::readPositionKey49O(v,ti,0,a)||
           !assets::readPositionKey49O(v,ti,n-1,b)||time<a.time||time>b.time)return false;
        const bool interpolate=time!=b.time;std::uint32_t lo=0,hi=n-1;
        if(interpolate){while(hi-lo>1){auto mid=(lo+hi)/2;assets::BezierKey49O k{};
            if(!assets::readPositionKey49O(v,ti,mid,k))return false;
            if(time<k.time)hi=mid;else lo=mid;
        }}else lo=n-2;
        if(!assets::readPositionKey49O(v,ti,lo,a)||!assets::readPositionKey49O(v,ti,hi,b))return false;
        float u=div(sub(time,interpolate?a.time:b.time),sub(b.time,a.time));
        if(!interpolate){for(unsigned i=0;i<3;++i)p[i]=add(b.position[i],mul(u,sub(b.position[i],a.position[i])));return true;}
        float u2=mul(u,u),u3=mul(u,u2),s=sub(mul(3,u2),mul(2,u3)),aWeight=sub(1,s),t2=sub(u3,u2),t1=add(u,sub(t2,u2));
        for(unsigned i=0;i<3;++i)p[i]=add(add(mul(a.outgoing[i],t1),add(mul(a.position[i],aWeight),mul(b.position[i],s))),mul(b.incoming[i],t2));
        return true;
    }
    void rotationMatrix(const Q&q,ModelLocalMatrix49O& local) noexcept {
        float* m=local.m;float w=q[0],x=q[1],y=q[2],z=q[3];
        float norm=add(add(mul(y,y),add(mul(w,w),mul(x,x))),mul(z,z));
        if(norm<=0){for(unsigned r=0;r<3;++r)for(unsigned c=0;c<4;++c)m[r*4+c]=float(r==c);return;}
        float s=div(2,norm),ys=mul(y,s),xs=mul(x,s),zs=mul(z,s);
        float zz=mul(z,zs),wx=mul(w,xs),xx=mul(x,xs),wy=mul(w,ys),xy=mul(x,ys),wz=mul(w,zs),xz=mul(x,zs),yz=mul(y,zs),yy=mul(y,ys);
        m[0]=sub(1,add(yy,zz));m[1]=add(xy,wz);m[2]=sub(xz,wy);m[3]=0;
        m[4]=sub(xy,wz);m[5]=sub(1,add(xx,zz));m[6]=add(yz,wx);m[7]=0;
        m[8]=add(xz,wy);m[9]=sub(yz,wx);m[10]=sub(1,add(xx,yy));m[11]=0;
    }
};
bool reject(ModelMotionDiagnostic49O& d,ModelMotionError49O e,std::uint32_t i=0) noexcept {d={e,i};return false;}
}
bool sampleModelMotionBlend49O(const assets::MotionView49O& first,
    const assets::MotionView49O& second,const assets::MotionNodeBindings49O& bindings,
    float fraction,float weight,ModelLocalMatrix49O* locals,std::uint32_t nodes,
    ModelMotionDiagnostic49O& d) noexcept {
    d={};
    if(std::fegetround()!=FE_TONEAREST)return reject(d,ModelMotionError49O::rounding_mode);
    if(!locals||!nodes||nodes>64||!first.data||!second.data||!std::isfinite(fraction)||
       !std::isfinite(weight)||fraction<0||fraction>1||weight<0||weight>1||
       !std::isfinite(first.duration)||!(first.duration>0))return reject(d,ModelMotionError49O::input);
    if(first.rotationCount>64||first.positionCount>64||first.rotationCount!=second.rotationCount||
       first.positionCount!=second.positionCount||!bindings.count||bindings.count>64)
        return reject(d,ModelMotionError49O::topology);
    ModelLocalMatrix49O staged[64];
    for(std::uint32_t i=0;i<nodes;++i){for(unsigned j=0;j<16;++j)if(!std::isfinite(locals[i].m[j]))return reject(d,ModelMotionError49O::input,i);
        if(locals[i].m[3]!=0||locals[i].m[7]!=0||locals[i].m[11]!=0||locals[i].m[15]!=1)return reject(d,ModelMotionError49O::input,i);
        staged[i]=locals[i];}
    Eval e;float time=e.mul(first.duration,fraction);
    if(!e.valid)return reject(d,ModelMotionError49O::arithmetic);
    for(unsigned channel=0;channel<2;++channel){auto count=channel?first.positionCount:first.rotationCount;
        for(std::uint32_t i=0;i<count;++i){const auto& a=channel?first.position[i]:first.rotation[i];const auto& b=channel?second.position[i]:second.rotation[i];
            if(a.rowIndex!=b.rowIndex)return reject(d,ModelMotionError49O::topology,i);
            if(a.rowIndex>=bindings.count||bindings.rows[a.rowIndex].sceneNode>=nodes)return reject(d,ModelMotionError49O::binding,i);
            auto& local=staged[bindings.rows[a.rowIndex].sceneNode];
            if(!channel){Q qa{},qb{};if(!e.rotation(first,i,time,qa)||!e.rotation(second,i,time,qb))return reject(d,ModelMotionError49O::sample_time,i);
                e.rotationMatrix(e.blend(qb,qa,weight),local);
            }else{V pa{},pb{};if(!e.position(first,i,time,pa)||!e.position(second,i,time,pb))return reject(d,ModelMotionError49O::sample_time,i);
                for(unsigned j=0;j<3;++j)local.m[12+j]=e.sub(pb[j],e.mul(weight,e.sub(pb[j],pa[j])));}
            if(!e.valid)return reject(d,ModelMotionError49O::arithmetic,i);
        }
    }
    for(std::uint32_t i=0;i<nodes;++i)locals[i]=staged[i];
    return true;
}
}
