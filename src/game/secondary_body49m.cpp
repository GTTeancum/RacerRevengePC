#include "secondary_body49m.h"
#include "vehicle_aero48e.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using V=PodSphereVector49L;using M=PodSphereMatrix49L;
float fp(std::uint32_t u) noexcept {float x;std::memcpy(&x,&u,4);return x;}
std::uint32_t bits(float x) noexcept {std::uint32_t u;std::memcpy(&u,&x,4);return u;}
bool nearest() noexcept {return std::fegetround()==FE_TONEAREST;}
struct Math {
    bool valid=true;
    float keep(float x) noexcept {valid=valid&&std::isfinite(x);return x;}
    float add(float a,float b) noexcept {volatile float x=a+b;return keep(x);}
    float sub(float a,float b) noexcept {volatile float x=a-b;return keep(x);}
    float mul(float a,float b) noexcept {volatile float x=a*b;return keep(x);}
    float div(float a,float b) noexcept {volatile float x=a/b;return keep(x);}
    float neg(float x) noexcept {return keep(fp(bits(x)^0x80000000u));}
    float scalarRoot(float x) noexcept {
        if((bits(x)&0x7f800000u)==0)return fp(bits(x)&0x80000000u);
        volatile float r=static_cast<float>(std::sqrt(static_cast<double>(x)));return keep(r);
    }
    float squared(const V&v,bool vu=false) noexcept {
        const float x=mul(v.v[0],v.v[0]),y=mul(v.v[1],v.v[1]),z=mul(v.v[2],v.v[2]);
        return add(add(x,y),vu?mul(1.0f,z):z);
    }
    V rotate(const M&m,const V&v,bool point=false) noexcept {
        V o{{0,0,0,1}};
        for(unsigned r=0;r<3;++r){float x=mul(m.v[r],v.v[0]);x=add(x,mul(m.v[r+4],v.v[1]));x=add(x,mul(m.v[r+8],v.v[2]));o.v[r]=add(x,point?m.v[r+12]:0.0f);}return o;
    }
    M inverse(const M&m) noexcept {
        M o=m;
        for(unsigned c=0;c<3;++c)for(unsigned r=0;r<4;++r)
            o.v[c*4+r]=(r==3||r==c)?add(m.v[c*4+r],0.0f):add(0.0f,m.v[r*4+c]);
        for(unsigned r=0;r<3;++r){float x=mul(o.v[r],m.v[12]);x=add(x,mul(o.v[4+r],m.v[13]));x=add(x,mul(o.v[8+r],m.v[14]));o.v[12+r]=mul(x,-1.0f);}
        o.v[15]=add(m.v[15],0.0f);return o;
    }
    V scale(float s,const V&v) noexcept {V o{{0,0,0,1}};for(unsigned i=0;i<3;++i)o.v[i]=mul(s,v.v[i]);return o;}
    V cross(const V&a,const V&b) noexcept {V o{{0,0,0,0}};for(unsigned i=0;i<3;++i){unsigned j=(i+1)%3,k=(i+2)%3;o.v[i]=sub(mul(a.v[j],b.v[k]),mul(b.v[j],a.v[k]));}return o;}
    V normalize(const V&v) noexcept {
        const float d=squared(v,true);float q;
        // Original VRSQRT numerator is vf0.w=1; zero denominator saturatesQ.
        if(d==0.0f)q=fp((bits(d)&0x80000000u)|0x7f7fffffu);
        else {volatile float root=std::sqrt(std::fabs(d));q=div(1.0f,root);}
        return scale(q,v);
    }
    void addLoad(Vector3_48C&to,const V&v) noexcept {for(unsigned i=0;i<3;++i)to.v[i]=add(to.v[i],v.v[i]);}
};
M matrix(const BodyMatrix48G&m) noexcept {M o{};for(unsigned i=0;i<16;++i)o.v[i]=m.v[i];return o;}
V vec(const Vector3_48C&v,float w=1.0f) noexcept {return V{{v.v[0],v.v[1],v.v[2],w}};}
bool finiteMatrix(const M&m) noexcept {for(float x:m.v)if(!std::isfinite(x))return false;return true;}
}
void initializedSecondaryGlobals49M(SecondaryForceContext49M&p) noexcept {
    p.effective4346C8=fp(0x42820000);p.effective4346D0=fp(0x43af0000);
    p.effective4346D8=fp(0x44bb8000);p.effective4346E0=fp(0x43960000);
    p.effective4346E8=fp(0x457a0000);p.effective4346F0=fp(0x42480000);
    p.effective4346F8=fp(0x42f00000);p.effective434700=0.0f;p.effective434708=fp(0x40a00000);
}
bool refreshSecondaryFrameCache49M(const SecondaryBodyState49M&in,SecondaryBodyState49M&out) noexcept {
    if(!nearest()||in.phase!=SecondaryPhase49M::Ready)return false;
    auto r=in;
    if(!quaternionToAngles48G(r.root.body.orientation,r.cachedAnglesE0)||
       !refreshCachedSpeed48E(r.root.body.velocity,r.cachedSpeed34))return false;
    out=r;return true;
}
void makeSecondarySphereBody49M(const SecondaryBodyState49M&s,const SecondaryBodyParameters49M&p,PodSphereBody49L&out) noexcept {
    PodSphereBody49L b{};b.scenePresent=s.scenePresent;b.sceneFlags190=s.sceneFlags190;
    b.sceneWorld100=matrix(s.root.scene);b.inverseMass=p.body.inverseMass;
    for(unsigned c=0;c<3;++c)for(unsigned r=0;r<3;++r)b.inverseInertia.v[4*c+r]=p.body.inverseInertiaColumns[c].v[r];
    constexpr unsigned lanes[]={3,7,11,12,13,14,15};
    for(unsigned i=0;i<7;++i)b.inverseInertia.v[lanes[i]]=p.inverseInertiaExtra[i];
    b.linearVelocity=vec(s.root.body.velocity,s.velocity70W);b.angularVelocity=vec(s.root.body.angularVelocity,s.angularVelocityB0W);
    b.worldForce=vec(s.root.loads.force,s.force180W);b.localTorque=vec(s.root.loads.torque,s.torque190W);
    b.localCenter=vec(p.centerOffset,p.centerOffsetW);out=b;
}
bool prepareSecondaryDampingPrefix49M(float speed,float low,float high,
    const Vector3_48C& angular,Vector3_48C& output) noexcept {
    if(!nearest()||!std::isfinite(speed)||!std::isfinite(low)||!std::isfinite(high))return false;
    for(float lane:angular.v)if(!std::isfinite(lane))return false;
    Math f;const float range=f.sub(high,low),position=f.sub(speed,fp(0x422fffcc));
    const float product=f.mul(position,range),scaled=f.div(product,fp(0x41eaaa64));
    float damping=f.add(low,scaled);
    if(high<=low){if(damping<high)damping=high;else if(!(damping<=low))damping=low;}
    else{if(!(damping<=high))damping=high;else if(damping<low)damping=low;}
    const V torque=f.scale(f.neg(damping),vec(angular));
    if(!f.valid)return false;std::memcpy(output.v,torque.v,sizeof(output.v));return true;
}
bool produceSecondaryForces49M(const SecondaryBodyState49M&in,const SecondaryBodyParameters49M&p,
    const SecondaryForceContext49M&c,float dt,GradientNoiseState49M&noise,std::uint32_t&random,
    SecondaryBodyState49M&out) noexcept {
    if(!nearest()||in.phase!=SecondaryPhase49M::Ready||!std::isfinite(dt)||dt<0.0f||
       !in.scenePresent||!(in.sceneFlags190&1u)||!(c.mainSceneFlags190&1u))return false;
    const M secondary=matrix(in.root.scene);
    if(!finiteMatrix(secondary)||!finiteMatrix(c.mainSceneWorld100))return false;
    Math f;auto r=in;
    // 218A3C: this reads the CURRENT probe104 after main response219CF4.
    const float gravity=c.probe104?c.gravity378:c.gravity37C;
    const V g{{0.0f,f.mul(f.neg(p.body.mass),gravity),0.0f,1.0f}};
    f.addLoad(r.root.loads.force,g);
    const float low=c.effective4346D0,high=c.effective4346D8;
    float damping=f.add(low,f.div(f.mul(f.sub(in.cachedSpeed34,fp(0x422fffcc)),f.sub(high,low)),fp(0x41eaaa64)));
    // Source conditional ordering also handles reversed/equal endpoint values.
    if(high<=low){if(damping<high)damping=high;else if(!(damping<=low))damping=low;}
    else {if(!(damping<=high))damping=high;else if(damping<low)damping=low;}
    f.addLoad(r.root.loads.torque,f.scale(f.neg(damping),vec(in.root.body.angularVelocity)));
    // 28B1A0 obtains secondary scene translation in main scene coordinates.
    const M inverseMain=f.inverse(c.mainSceneWorld100);
    const V position{{secondary.v[12],secondary.v[13],secondary.v[14],1.0f}};
    const V relative=f.rotate(inverseMain,position,true);
    V error{{f.sub(c.targetRelative2F0.v[0],relative.v[0]),
             f.sub(c.targetRelative2F0.v[1],relative.v[1]),
             f.sub(c.targetRelative2F0.v[2],relative.v[2]),1.0f}};
    error.v[1]=0.0f; // AFTER the original subtraction, exactly218CAC.
    const float length=f.scalarRoot(f.squared(error));
    const V worldError=f.rotate(c.mainSceneWorld100,error);
    const V direction=f.normalize(worldError);
    f.addLoad(r.root.loads.force,f.scale(f.mul(c.effective4346E0,length),direction));
    GradientNoiseState49M nextNoise{};std::uint32_t nextRandom=random;
    if(c.vehicleState40==0){
        f.addLoad(r.root.loads.force,f.scale(f.neg(c.effective4346C8),vec(in.root.body.velocity)));
        const V upA{{secondary.v[4],secondary.v[5],secondary.v[6],1.0f}};
        const V upB{{c.mainSceneWorld100.v[4],c.mainSceneWorld100.v[5],c.mainSceneWorld100.v[6],1.0f}};
        const V worldTorque=f.scale(c.effective4346E8,f.cross(upA,upB));
        f.addLoad(r.root.loads.torque,f.rotate(f.inverse(secondary),worldTorque));
    }else{
        nextNoise=noise;float value=0.0f;
        const float coordinate=f.mul(c.effective434708,in.time864);
        if(!f.valid||!evaluateGradientNoise49M(nextNoise,nextRandom,coordinate,value))return false;
        const float h=f.add(0.5f,f.mul(0.5f,value));
        const V torque{{0.0f,0.0f,f.mul(c.mainCachedSpeed34,f.mul(h,c.effective4346F8)),1.0f}};
        f.addLoad(r.root.loads.torque,torque);
        const V force{{f.mul(c.mainCachedSpeed34,f.mul(h,c.effective434700)),0.0f,0.0f,1.0f}};
        r.time864=f.add(in.time864,dt);
        f.addLoad(r.root.loads.force,f.rotate(secondary,force));
        f.addLoad(r.root.loads.force,f.scale(f.neg(c.effective4346F0),vec(in.root.body.velocity)));
    }
    if(!f.valid)return false;
    r.phase=SecondaryPhase49M::ConstraintPending;
    out=r;if(c.vehicleState40!=0){noise=nextNoise;random=nextRandom;}return true;
}
bool applySecondaryConstraint49M(const SecondaryBodyState49M&in,const SecondaryBodyParameters49M&p,
    const PodSphereBody49L&main,const PodSphereJoint49L&joint,SecondaryBodyState49M&out,
    PodSphereJoint49L&outJoint,float&magnitude) noexcept {
    if(!nearest()||in.phase!=SecondaryPhase49M::ConstraintPending||!joint.bodyBPresent48)return false;
    PodSphereBody49L secondary{};makeSecondarySphereBody49M(in,p,secondary);
    auto j=joint;float m=0.0f;
    if(!preparePodSphere49L(j,&secondary,&main)||!correctPodSphere49L(j,&secondary,&main,m)||
       !applyPodSphere49L(j,&secondary))return false;
    auto r=in;for(unsigned i=0;i<3;++i){r.root.loads.force.v[i]=secondary.worldForce.v[i];r.root.loads.torque.v[i]=secondary.localTorque.v[i];}
    r.force180W=secondary.worldForce.v[3];r.torque190W=secondary.localTorque.v[3];
    r.phase=SecondaryPhase49M::Response368Pending;out=r;outJoint=j;magnitude=m;return true;
}
bool completeSecondaryResponse49M(const SecondaryBodyState49M&supplied,const SecondaryBodyParameters49M&p,
    float dt,SecondaryBodyState49M&out) noexcept {
    if(!nearest()||supplied.phase!=SecondaryPhase49M::Response368Pending||!std::isfinite(dt)||dt<0.0f||
       (supplied.scenePresent&&supplied.enabled&&!(supplied.sceneFlags190&1u)))return false;
    auto r=supplied;
    if(!updateRootBody48H(r.root,p.body,p.centerOffset,r.scenePresent,r.enabled,dt,r.root))return false;
    // Source3C9290..3C9310 initializes433B80 to(0,0,0,1);1682FC clears loads.
    r.force180W=1.0f;r.torque190W=1.0f;
    // Root transform setters mark bit7 through28C4F0; final48H direction
    // transforms refresh bit0. Disabled/missing-scene paths do neither.
    if(r.scenePresent&&r.enabled)r.sceneFlags190|=0x80u;
    r.phase=SecondaryPhase49M::Ready;out=r;return true;
}
}
