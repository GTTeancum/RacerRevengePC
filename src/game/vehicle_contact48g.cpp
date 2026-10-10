#include "vehicle_contact48g.h"
#include <cmath>
#include <cstring>
#include <initializer_list>

namespace rr::game {
namespace {
using V=Vector3_48C;
float fromBits(std::uint32_t x) noexcept { float y;std::memcpy(&y,&x,4);return y; }
std::uint32_t bits(float x) noexcept { std::uint32_t y;std::memcpy(&y,&x,4);return y; }
bool finite(const V& v) noexcept { for(float x:v.v)if(!std::isfinite(x))return false;return true; }
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return keep(x);}
    float div(float a,float b) noexcept {volatile float x=a/b;return keep(x);}
    float root(float a) noexcept {
        if((bits(a)&0x7f800000u)==0)return fromBits(bits(a)&0x80000000u);
        volatile float x=static_cast<float>(std::sqrt(static_cast<double>(a)));return keep(x);
    }
    V add(const V&a,const V&b) noexcept {return {{add(a.v[0],b.v[0]),add(a.v[1],b.v[1]),add(a.v[2],b.v[2])}};}
    V sub(const V&a,const V&b) noexcept {return {{sub(a.v[0],b.v[0]),sub(a.v[1],b.v[1]),sub(a.v[2],b.v[2])}};}
    V scale(const V&a,float b) noexcept {return {{mul(a.v[0],b),mul(a.v[1],b),mul(a.v[2],b)}};}
    float dot(const V&a,const V&b) noexcept {
        return add(add(mul(a.v[0],b.v[0]),mul(a.v[1],b.v[1])),mul(add(0.0f,1.0f),mul(a.v[2],b.v[2])));
    }
    float squaredScalar(const V&a) noexcept {return add(add(mul(a.v[0],a.v[0]),mul(a.v[1],a.v[1])),mul(a.v[2],a.v[2]));}
    V cross(const V&a,const V&b) noexcept {
        return {{sub(mul(a.v[1],b.v[2]),mul(a.v[2],b.v[1])),
                 sub(mul(a.v[2],b.v[0]),mul(a.v[0],b.v[2])),
                 sub(mul(a.v[0],b.v[1]),mul(a.v[1],b.v[0]))}};
    }
    V normalize(const V&a) noexcept {
        const float d=dot(a,a);
        // _vuRSQRT in the supplied PCSX2 VUops.cpp: rounded sqrt then divide.
        // Zero produces signed maximum finite Q (the resulting vector is zero).
        float q;
        if(d==0.0f)q=fromBits((bits(d)&0x80000000u)|0x7f7fffffu);
        else {volatile float r=static_cast<float>(std::sqrt(std::fabs(d)));q=div(1.0f,r);}
        return scale(a,q);
    }
    V rotate(const ReadyBodyBasis48D&b,const V&v) noexcept {
        V o{};for(unsigned i=0;i<3;++i)o.v[i]=add(add(mul(b.columns[0].v[i],v.v[0]),mul(b.columns[1].v[i],v.v[1])),mul(b.columns[2].v[i],v.v[2]));return o;
    }
    V inverse(const ReadyBodyBasis48D&b,const V&v) noexcept {
        // The original transpose helper adds zero to each transposed element.
        V o{};for(unsigned i=0;i<3;++i)o.v[i]=add(add(mul(add(b.columns[i].v[0],0.0f),v.v[0]),mul(add(b.columns[i].v[1],0.0f),v.v[1])),mul(add(b.columns[i].v[2],0.0f),v.v[2]));return o;
    }
    float angle(float x) noexcept {
        if(x>1.0f)return fromBits(0x3fc90fdb);
        if(x< -1.0f)return fromBits(0x4096cbe4);
        const float a=std::fabs(x);
        float t=add(fromBits(0xc372b84f),mul(fromBits(0x42a8a11b),a));
        t=add(fromBits(0x43836812),mul(t,a));
        t=add(fromBits(0xc3031cc3),mul(a,t));
        t=add(fromBits(0x41ed4ad5),mul(a,t));
        t=add(fromBits(0xbfb9d5d7),mul(a,t));
        t=add(fromBits(0x3d06871e),mul(a,t));
        return x<0.0f ? -t:t;
    }
};
bool zero(const V&v) noexcept {return v.v[0]==0.0f&&v.v[1]==0.0f&&v.v[2]==0.0f;}
bool valid(const TerrainFollowState48G&s,const TerrainFollowParameters48G&p,const TerrainSurface48G&t,const TerrainFollowBody48G&b) noexcept {
    for(const V*v:{&s.previousPoint180,&s.previousNormal190,&s.largestVelocityChange1d0,&p.localOffset1c0,
        &t.point150,&t.normal170,&b.position60,&b.worldVelocity70,&b.cachedLocalVelocity80,&b.loads.force,&b.loads.torque,&b.scene.position,
        &b.scene.basis.columns[0],&b.scene.basis.columns[1],&b.scene.basis.columns[2]})if(!finite(*v))return false;
    return std::isfinite(p.airborneTorque204)&&std::isfinite(p.detachAngle208)&&std::isfinite(p.detachDistance20c)&&std::isfinite(b.angleE8);
}
}
bool applyTerrainFollowing48G(const TerrainFollowState48G&previous,const TerrainFollowParameters48G&p,
    const TerrainSurface48G&t,const TerrainFollowBody48G&input,TerrainFollowResult48G&out) noexcept {
    if(!valid(previous,p,t,input))return false;
    TerrainFollowResult48G r{previous,input};auto&s=r.state;auto&b=r.body;Math f;
    const V forward=b.scene.basis.columns[2],up=b.scene.basis.columns[1];
    s.active104=0;
    if(s.detached11c) {
        if(!t.hit110&&!t.hit114)s.detached11c=0;
        else if(f.add(b.position60.v[1],p.localOffset1c0.v[1])<=f.add(3.0f,t.point150.v[1]))s.detached11c=0;
    }
    if(!s.detached11c) {
        if(!t.hit110&&!t.hit114)s.pending10c=1;
        else {
            if(zero(s.previousPoint180))s.previousPoint180=t.point150;
            const float distance=f.dot(f.sub(s.previousPoint180,t.point150),t.normal170);
            if(zero(s.previousNormal190))s.previousNormal190=t.normal170;
            const V side=f.cross(forward,up),normalCross=f.cross(s.previousNormal190,t.normal170);
            const float angle=f.angle(f.root(f.squaredScalar(normalCross)));
            const V normalAxis=f.normalize(normalCross);
            if((!t.forceAngleTest108&&!(distance<=p.detachDistance20c))||
               (f.dot(normalAxis,side)<0.0f&&!(angle<=p.detachAngle208))) {
                s.detached11c=1;s.previousNormal190={{0,0,0}};s.previousPoint180={{0,0,0}};
            } else {s.active104=1;s.previousNormal190=t.normal170;s.previousPoint180=t.point150;}
        }
    }
    if(s.active104&&s.pending10c) {
        if(f.add(b.position60.v[1],p.localOffset1c0.v[1])<=f.add(3.0f,t.point150.v[1]))s.pending10c=0;
        else s.active104=0;
    }
    if(s.active104) {
        const float counter=-f.dot(b.loads.force,t.normal170);
        b.loads.force=f.add(b.loads.force,f.scale(t.normal170,counter));
        const V offsetPoint=f.add(f.rotate(b.scene.basis,p.localOffset1c0),b.scene.position);
        const float gap=f.dot(f.sub(t.point150,offsetPoint),t.normal170);
        const V move=f.scale(t.normal170,f.mul(fromBits(0x3ecccccd),gap));
        b.scene.position=f.add(b.scene.position,move);
        const V oldVelocity=b.worldVelocity70;
        const float normalFraction=std::fabs(f.dot(f.normalize(oldVelocity),t.normal170));
        if(!(normalFraction<=fromBits(0x3f490fd9))) {
            b.worldVelocity70=f.sub(oldVelocity,f.scale(t.normal170,f.dot(oldVelocity,t.normal170)));
        } else {
            const V tangent=f.normalize(f.cross(t.normal170,f.cross(oldVelocity,t.normal170)));
            // Original scalar length multiplies Y before X but adds X+Y+Z.
            const float speed=f.root(f.squaredScalar(oldVelocity));
            b.worldVelocity70=f.scale(tangent,speed);
        }
        const V change=f.sub(b.worldVelocity70,oldVelocity);
        if(!(f.root(f.squaredScalar(change))<=f.root(f.squaredScalar(s.largestVelocityChange1d0))))s.largestVelocityChange1d0=change;
        if(b.cachedLocalVelocity80.v[2]<0.0f) {
            V localForce=f.inverse(b.scene.basis,b.loads.force);
            if(localForce.v[2]<0.0f) {localForce.v[2]=0.0f;b.loads.force=f.rotate(b.scene.basis,localForce);}
        }
        if(b.cachedLocalVelocity80.v[2]<0.0f) {
            b.cachedLocalVelocity80.v[2]=0.0f;b.worldVelocity70=f.rotate(b.scene.basis,b.cachedLocalVelocity80);
        }
        // 28A090(a1=null,t0=t1=0,t2=1) deliberately does not normalize.
        const V newForward=f.cross(t.normal170,f.cross(forward,t.normal170));
        b.scene.basis.columns[0]=f.cross(t.normal170,newForward);
        b.scene.basis.columns[1]=t.normal170;b.scene.basis.columns[2]=newForward;
    } else {
        const float strength=f.mul(b.angleE8,f.mul(fromBits(0x3e99999a),p.airborneTorque204));
        const V extra={{f.mul(strength,0.0f),f.mul(strength,0.0f),f.mul(strength,1.0f)}};
        b.loads.torque=f.add(b.loads.torque,extra);
    }
    if(!f.valid)return false;
    out=r;return true;
}
}
