#include "pod_sphere_constraint49l.h"
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
using V = PodSphereVector49L;
using M = PodSphereMatrix49L;
using B = PodSphereBody49L;
using J = PodSphereJoint49L;
float fromBits(std::uint32_t u) noexcept { float v; std::memcpy(&v, &u, 4); return v; }
std::uint32_t bits(float v) noexcept { std::uint32_t u; std::memcpy(&u, &v, 4); return u; }
bool nearest() noexcept { return std::fegetround() == FE_TONEAREST; }
bool ready(const B* b) noexcept { return b && b->scenePresent && (b->sceneFlags190 & 1u); }
bool bodies(const J& j, const B* a, const B* b) noexcept {
    return j.bodyAPresent44 && ready(a) && j.bodyBPresent48 == (b != nullptr);
}
struct Math {
    bool valid = true;
    float keep(float x) noexcept { valid = valid && std::isfinite(x); return x; }
    float add(float a, float b) noexcept { volatile float x = a + b; return keep(x); }
    float sub(float a, float b) noexcept { volatile float x = a - b; return keep(x); }
    float mul(float a, float b) noexcept { volatile float x = a * b; return keep(x); }
    float div(float a, float b) noexcept { volatile float x = a / b; return keep(x); }
    float neg(float a) noexcept { return keep(fromBits(bits(a) ^ 0x80000000u)); }
    float root(float a) noexcept {
        // EE SQRT.S interface on this finite, nonnegative domain: flush a
        // subnormal operand to signed zero, otherwise round sqrt(double) once.
        if ((bits(a) & 0x7f800000u) == 0) return fromBits(bits(a) & 0x80000000u);
        volatile float x = static_cast<float>(std::sqrt(static_cast<double>(a)));
        return keep(x);
    }
    V add3(const V& a, const V& b) noexcept {
        V o{{0,0,0,1}}; for (unsigned i=0;i<3;++i) o.v[i]=add(a.v[i],b.v[i]); return o;
    }
    V sub3(const V& a, const V& b) noexcept {
        V o{{0,0,0,1}}; for (unsigned i=0;i<3;++i) o.v[i]=sub(a.v[i],b.v[i]); return o;
    }
    V scale(float s, const V& v) noexcept {
        V o{{0,0,0,1}}; for (unsigned i=0;i<3;++i) o.v[i]=mul(s,v.v[i]); return o;
    }
    float dot(const V& a, const V& b) noexcept {
        const float x=mul(a.v[0],b.v[0]);
        const float y=mul(a.v[1],b.v[1]);
        const float z=mul(a.v[2],b.v[2]);
        return add(add(x,y),mul(1.0f,z));
    }
    V cross(const V& a, const V& b) noexcept {
        // VOPMULA followed by VOPMSUB; W is explicitly subtracted from itself.
        V o{{0,0,0,0}};
        o.v[0]=sub(mul(a.v[1],b.v[2]),mul(b.v[1],a.v[2]));
        o.v[1]=sub(mul(a.v[2],b.v[0]),mul(b.v[2],a.v[0]));
        o.v[2]=sub(mul(a.v[0],b.v[1]),mul(b.v[0],a.v[1]));
        return o;
    }
    M product(const M& a, const M& b) noexcept {
        M o{};
        for(unsigned c=0;c<4;++c) for(unsigned r=0;r<4;++r) {
            float x=mul(a.v[r],b.v[c*4]);
            x=add(x,mul(a.v[4+r],b.v[c*4+1]));
            x=add(x,mul(a.v[8+r],b.v[c*4+2]));
            o.v[c*4+r]=add(x,mul(a.v[12+r],b.v[c*4+3]));
        }
        return o;
    }
    M transposeBasis(const M& m) noexcept {
        // Inline source transpose preserves column3 and each basis W lane.
        // Its explicit +0 operations matter for negative-zero input words.
        M o=m;
        for(unsigned c=0;c<3;++c) for(unsigned r=0;r<4;++r)
            o.v[c*4+r]=r==3 || r==c ? add(m.v[c*4+r],0.0f)
                                                    : add(0.0f,m.v[r*4+c]);
        return o;
    }
    V vector(const M& m, const V& v, float incomingW=0.0f) noexcept {
        // 127790 /28B9B0 mask XYZ, then add vf0 to ALL lanes. incomingW
        // represents the existing vf9.w, not the vector's homogeneous lane.
        V o{{0,0,0,add(incomingW,1.0f)}};
        for(unsigned r=0;r<3;++r) {
            float x=mul(m.v[r],v.v[0]);
            x=add(x,mul(m.v[4+r],v.v[1]));
            x=add(x,mul(m.v[8+r],v.v[2]));
            o.v[r]=add(x,0.0f);
        }
        return o;
    }
    V point(const M& m, const V& v) noexcept {
        V o{{0,0,0,1}};
        for(unsigned r=0;r<3;++r) {
            float x=mul(m.v[r],v.v[0]);
            x=add(x,mul(m.v[4+r],v.v[1]));
            x=add(x,mul(m.v[8+r],v.v[2]));
            o.v[r]=add(x,m.v[12+r]);
        }
        return o;
    }
    M inverseScene(const M& m) noexcept {
        M o=transposeBasis(m);
        // Both inverse scene helpers construct this translation even when
        // their eventual vector multiplication does not use that column.
        for(unsigned r=0;r<3;++r) {
            float x=mul(o.v[r],m.v[12]);
            x=add(x,mul(o.v[4+r],m.v[13]));
            x=add(x,mul(o.v[8+r],m.v[14]));
            o.v[12+r]=mul(x,-1.0f);
        }
        o.v[15]=add(m.v[15],0.0f);
        return o;
    }
    M inverse3(const M& m) noexcept {
        // 14E910 scalar ACC order, including cofactor negation after subtract.
        const float a=m.v[0], b=m.v[4], c=m.v[8];
        const float d=m.v[1], e=m.v[5], f=m.v[9];
        const float g=m.v[2], h=m.v[6], i=m.v[10];
        const float c00=sub(mul(e,i),mul(f,h));
        const float ci=sub(mul(b,i),mul(c,h));
        const float c02=sub(mul(b,f),mul(c,e));
        float determinant=sub(mul(a,c00),mul(d,ci));
        determinant=add(determinant,mul(g,c02));
        M o{};
        if (determinant==0.0f) return o;
        const float u=div(1.0f,determinant);
        o.v[0]=mul(u,c00);
        o.v[1]=mul(u,neg(sub(mul(d,i),mul(f,g))));
        o.v[2]=mul(u,sub(mul(d,h),mul(e,g)));
        o.v[4]=mul(u,neg(ci));
        o.v[5]=mul(u,sub(mul(a,i),mul(c,g)));
        o.v[6]=mul(u,neg(sub(mul(a,h),mul(b,g))));
        o.v[8]=mul(u,c02);
        o.v[9]=mul(u,neg(sub(mul(a,f),mul(c,d))));
        o.v[10]=mul(u,sub(mul(a,e),mul(b,d)));
        // Every remaining lane, including[15], is source +0, not identity.
        return o;
    }
    void pointForce(B& a, const V& worldPoint, const V& force) noexcept {
        const M inverse=inverseScene(a.sceneWorld100);
        const V localForce=vector(inverse,force);
        const V localPoint=point(inverse,worldPoint);
        const V lever=sub3(localPoint,a.localCenter);
        const V torque=cross(lever,localForce);
        for(unsigned i=0;i<3;++i) a.worldForce.v[i]=add(a.worldForce.v[i],force.v[i]);
        for(unsigned i=0;i<3;++i) a.localTorque.v[i]=add(a.localTorque.v[i],torque.v[i]);
    }
};
}

bool preparePodSphereCentripetalPrefix49L(const V& separation,const float* angularXYZ,
    const V& anchor,float lastPointVf6W,float& squared,V& tangent,V& centripetal,
    std::uint32_t& stoppedPC) noexcept {
    stoppedPC=0x1b6784u;if(!nearest())return false;Math f;
    const float distance=f.dot(separation,separation);if(!f.valid)return false;
    stoppedPC=0x1b67acu;if(!angularXYZ)return false;
    V angular{};std::memcpy(angular.v,angularXYZ,3*sizeof(float));
    //1B67C0 consumes surviving VF6.W, not point-result W=1. No earlier
    //distance>0 gate: source zero separation still reaches this cross/store.
    stoppedPC=0x1b67c0u;if(!std::isfinite(lastPointVf6W))return false;
    stoppedPC=0x1b67c4u;const V t=f.cross(angular,anchor);if(!f.valid)return false;
    //Finite admitted first cross leaves VF6.W=+0 for1B67E4 self-subtract.
    stoppedPC=0x1b67e8u;const V c=f.cross(angular,t);if(!f.valid)return false;
    squared=distance;tangent=t;centripetal=c;stoppedPC=0x1b67f0u;return true;
}

namespace {
void publishSourcePresence(const PodSphereSourcePresenceHooks49L* hooks,
                           bool bodyB,bool present) noexcept {
    if(hooks && hooks->publish) hooks->publish(hooks->context,bodyB,present);
}
}

bool initializePodSphereSourceOrdered49L(J& j,const std::uint32_t* globalWords,
    std::uint32_t& stoppedPC,const PodSphereSourcePresenceHooks49L* hooks) noexcept {
    // Numeric suffix only: caller owns1276E0/base and1B6220 dispatch identity.
    const V vf0{{0,0,0,1}};
    stoppedPC=0x1b622cu;j.localAnchor60=vf0;
    stoppedPC=0x1b6238u;j.localAnchor70=vf0;
    stoppedPC=0x1b6244u;j.worldAnchor80=vf0;
    stoppedPC=0x1b6250u;j.worldAnchor90=vf0;
    stoppedPC=0x1b625cu;j.fixedAnchorA0=vf0;
    stoppedPC=0x1b6268u;j.fieldB0=vf0;
    stoppedPC=0x1b6274u;j.centripetalC0=vf0;
    stoppedPC=0x1b6280u;j.separationD0=vf0;
    stoppedPC=0x1b628cu;j.directionE0=vf0;
    stoppedPC=0x1b6298u;j.preparedForceF0=vf0;
    stoppedPC=0x1b62acu;j.enabled38=1;
    stoppedPC=0x1b62b4u;j.unilateral3C=1;
    stoppedPC=0x1b62bcu;j.suppressed40=0;
    stoppedPC=0x1b62c4u;j.bodyAPresent44=false;publishSourcePresence(hooks,false,false);
    stoppedPC=0x1b62ccu;j.bodyBPresent48=false;publishSourcePresence(hooks,true,false);
    stoppedPC=0x1b62d4u;j.length4C=0;
    stoppedPC=0x1b62dcu;j.lengthSquared50=0;
    stoppedPC=0x1b62e4u;j.speedSquared54=0;
    stoppedPC=0x1b62e8u;if(!globalWords)return false;
    // Each original LQ/SQ remains a word copy.70 retains VF0, not434390.
    stoppedPC=0x1b62f4u;std::memcpy(j.localAnchor60.v,globalWords,16);
    stoppedPC=0x1b6304u;std::memcpy(j.worldAnchor80.v,globalWords,16);
    stoppedPC=0x1b6314u;std::memcpy(j.worldAnchor90.v,globalWords,16);
    stoppedPC=0x1b6324u;std::memcpy(j.fixedAnchorA0.v,globalWords,16);
    stoppedPC=0x1b6334u;std::memcpy(j.fieldB0.v,globalWords,16);
    stoppedPC=0x1b6344u;std::memcpy(j.centripetalC0.v,globalWords,16);
    stoppedPC=0x1b6354u;std::memcpy(j.separationD0.v,globalWords,16);
    stoppedPC=0x1b6364u;std::memcpy(j.directionE0.v,globalWords,16);
    stoppedPC=0x1b6374u;std::memcpy(j.preparedForceF0.v,globalWords,16);
    //14D7F0 constructs all-zero scratch; source copies pairs of scalar words.
    stoppedPC=0x1b6394u;for(unsigned i=0;i<16;++i)j.cached100.v[i]=0;
    stoppedPC=0x1b63ccu;for(unsigned i=0;i<16;++i)j.cached140.v[i]=0;
    stoppedPC=0x1b6404u;for(unsigned i=0;i<16;++i)j.inverse180.v[i]=0;
    stoppedPC=0x1b6434u;if(!nearest())return false;Math f;
    const float period=fromBits(0x3d4ccccdu);
    const float damping=f.div(2.0f,period);if(!f.valid)return false;
    stoppedPC=0x1b643cu;const float squared=f.mul(period,period);if(!f.valid)return false;
    stoppedPC=0x1b6440u;j.damping34=damping;
    stoppedPC=0x1b6450u;const float stiffness=f.div(1.0f,squared);if(!f.valid)return false;
    stoppedPC=0x1b6454u;j.stiffness30=stiffness;
    stoppedPC=0x1b6460u;return true;
}
bool setupPodSphereASourceOrdered49L(J& j,float mass,const M& inertia,const V& anchor,
    std::uint32_t& stoppedPC,const PodSphereSourcePresenceHooks49L* hooks) noexcept {
    //1B64DC loads anchor before44 publication; typed actual A is caller-proven.
    V saved;std::memcpy(saved.v,anchor.v,16);
    stoppedPC=0x1b64ecu;j.bodyAPresent44=true;publishSourcePresence(hooks,false,true);
    stoppedPC=0x1b64f4u;std::memcpy(j.localAnchor60.v,saved.v,16);
    //14E150 performs word copies, not multiplication: even exceptional mass
    //payloads are retained until a later consumer's arithmetic admission.
    M diagonal{};diagonal.v[0]=mass;diagonal.v[5]=mass;diagonal.v[10]=mass;diagonal.v[15]=1;
    stoppedPC=0x1b6520u;for(unsigned i=0;i<16;++i)j.cached140.v[i]=diagonal.v[i];
    stoppedPC=0x1b6538u;if(!nearest())return false;Math f;M s{};
    s.v[1]=f.keep(saved.v[2]);s.v[2]=f.neg(saved.v[1]);
    s.v[4]=f.neg(saved.v[2]);s.v[6]=f.keep(saved.v[0]);
    s.v[8]=f.keep(saved.v[1]);s.v[9]=f.neg(saved.v[0]);
    if(!f.valid)return false;
    stoppedPC=0x1b658cu;const M t=f.product(inertia,s);if(!f.valid)return false;
    stoppedPC=0x1b6600u;const M result=f.product(s,t);if(!f.valid)return false;
    //All four source result columns are computed before the first joint store.
    stoppedPC=0x1b6648u;std::memcpy(j.cached100.v,result.v,16);
    stoppedPC=0x1b664cu;std::memcpy(j.cached100.v+4,result.v+4,16);
    stoppedPC=0x1b6650u;std::memcpy(j.cached100.v+8,result.v+8,16);
    stoppedPC=0x1b6654u;std::memcpy(j.cached100.v+12,result.v+12,16);
    stoppedPC=0x1b6660u;return true;
}
bool setupPodSphereBSourceOrdered49L(J& j,bool bodyB,const V& anchor,float length,
    std::uint32_t& stoppedPC,const PodSphereSourcePresenceHooks49L* hooks) noexcept {
    V saved;std::memcpy(saved.v,anchor.v,16);
    stoppedPC=0x1b668cu;j.bodyBPresent48=bodyB;publishSourcePresence(hooks,true,bodyB);
    stoppedPC=0x1b6698u;std::memcpy(j.localAnchor70.v,saved.v,16);
    stoppedPC=0x1b669cu;if(!nearest() || !std::isfinite(length))return false;
    const float minimum=fromBits(0x3dcccccdu);
    const float clamped=length>minimum ? length : minimum;
    Math f;const float squared=f.mul(clamped,clamped);
    //MUL precedes4C, but its unsupported result is rejected only at50's store:
    //the independent finite length store is not rolled back by square overflow.
    stoppedPC=0x1b66b4u;j.length4C=clamped;
    stoppedPC=0x1b66c0u;if(!f.valid)return false;j.lengthSquared50=squared;
    stoppedPC=0x1b66c4u;return true;
}

bool initializePodSphere49L(J& joint) noexcept {
    if(!nearest()) return false;
    J j=joint; Math f;
    // Initialized source global434390 is (0,0,0,1); original3C9CF0..3C9D70.
    const V zero{{0,0,0,1}};
    j.localAnchor60=j.localAnchor70=j.worldAnchor80=j.worldAnchor90=zero;
    j.fixedAnchorA0=j.fieldB0=j.centripetalC0=j.separationD0=zero;
    j.directionE0=j.preparedForceF0=zero;
    j.cached100={}; j.cached140={}; j.inverse180={};
    j.enabled38=1; j.unilateral3C=1; j.suppressed40=0;
    j.bodyAPresent44=false; j.bodyBPresent48=false;
    j.length4C=j.lengthSquared50=j.speedSquared54=0.0f;
    const float period=fromBits(0x3d4ccccdu);
    j.damping34=f.div(2.0f,period);
    j.stiffness30=f.div(1.0f,f.mul(period,period));
    if(!f.valid) return false;
    joint=j; return true;
}
bool setupPodSphereA49L(J& joint, const B& body, const V& localAnchor) noexcept {
    if(!nearest() || !std::isfinite(body.inverseMass)) return false;
    J j=joint; Math f;
    j.bodyAPresent44=true; j.localAnchor60=localAnchor;
    j.cached140={}; j.cached140.v[0]=j.cached140.v[5]=j.cached140.v[10]=body.inverseMass;
    j.cached140.v[15]=1.0f;
    M s{};
    s.v[1]=f.keep(localAnchor.v[2]); s.v[2]=f.neg(localAnchor.v[1]);
    s.v[4]=f.neg(localAnchor.v[2]); s.v[6]=f.keep(localAnchor.v[0]);
    s.v[8]=f.keep(localAnchor.v[1]); s.v[9]=f.neg(localAnchor.v[0]);
    const M t=f.product(body.inverseInertia,s);
    j.cached100=f.product(s,t);
    if(!f.valid) return false;
    joint=j; return true;
}
bool setupPodSphereB49L(J& joint, const B* bodyB, const V& localAnchor, float length) noexcept {
    if(!nearest() || !std::isfinite(length)) return false;
    J j=joint; Math f;
    j.bodyBPresent48=bodyB!=nullptr; j.localAnchor70=localAnchor;
    const float minimum=fromBits(0x3dcccccdu);
    j.length4C=length>minimum ? length : minimum;
    j.lengthSquared50=f.mul(j.length4C,j.length4C);
    if(!f.valid) return false;
    joint=j; return true;
}
bool preparePodSphere49L(J& joint, const B* a, const B* b) noexcept {
    if(!nearest()) return false;
    if(joint.enabled38==0) return true;
    if(!bodies(joint,a,b) || (b && !ready(b))) return false;
    // The final point helper loads scene column2 into vf6. Its W lane is
    // consumed by the subsequent VSub.w self-clear (1B67C0), even though
    // point XYZ arithmetic ignores it. Nonfinite would persist into JC0.w.
    if(b && !std::isfinite(b->sceneWorld100.v[11])) return false;
    J j=joint; Math f;
    j.worldAnchor80=f.point(a->sceneWorld100,j.localAnchor60);
    j.worldAnchor90=b ? f.point(b->sceneWorld100,j.localAnchor70) : j.fixedAnchorA0;
    j.separationD0=f.sub3(j.worldAnchor80,j.worldAnchor90);
    const float distanceSquared=f.dot(j.separationD0,j.separationD0);
    if(!f.valid || !(distanceSquared>0.0f)) return false;
    const V tangential=f.cross(a->angularVelocity,j.localAnchor60);
    j.centripetalC0=f.cross(a->angularVelocity,tangential);
    const M transposed=f.transposeBasis(a->sceneWorld100);
    const M cachedTimesTranspose=f.product(j.cached100,transposed);
    const M worldCached=f.product(a->sceneWorld100,cachedTimesTranspose);
    // vmaddw stores first result column into vf9, whose W remains live through
    // the rotate and final matrix-vector helpers below.
    const V worldTangential=f.vector(a->sceneWorld100,tangential,worldCached.v[3]);
    const V velocityA=f.add3(a->linearVelocity,worldTangential);
    const V velocityB=b ? b->linearVelocity : V{{0,0,0,1}};
    const V relativeVelocity=f.sub3(velocityA,velocityB);
    const float lengthError=f.sub(j.lengthSquared50,distanceSquared);
    j.speedSquared54=f.dot(relativeVelocity,relativeVelocity);
    const float radialVelocity=f.dot(j.separationD0,relativeVelocity);
    M effective{};
    for(unsigned i=0;i<16;++i) effective.v[i]=f.sub(j.cached140.v[i],worldCached.v[i]);
    j.inverse180=f.inverse3(effective);
    const float inverseDistanceSquared=f.div(1.0f,distanceSquared);
    // Source scales as d*(1/D), not componentwise division by D.
    j.directionE0={{f.mul(j.separationD0.v[0],inverseDistanceSquared),
                    f.mul(j.separationD0.v[1],inverseDistanceSquared),
                    f.mul(j.separationD0.v[2],inverseDistanceSquared),1}};
    const float q=f.sub(f.mul(j.stiffness30,lengthError),f.mul(j.damping34,radialVelocity));
    const V projected=f.scale(q,j.directionE0);
    j.preparedForceF0=f.vector(j.inverse180,projected,worldTangential.v[3]);
    if(!f.valid) return false;
    joint=j; return true;
}
bool correctPodSphere49L(J& joint, B* a, const B* b, float& magnitude) noexcept {
    if(!nearest()) return false;
    if(joint.enabled38==0) { magnitude=0.0f; return true; }
    if(!bodies(joint,a,b)) return false;
    // 127790 loads current inertia column2 into vf6; the cross product
    // at1B6C3C consumes vf6.w when clearing its temporary W lane.
    if(!std::isfinite(a->inverseInertia.v[11])) return false;
    J j=joint; B body=*a; Math f;
    const V linearA=f.scale(body.inverseMass,body.worldForce);
    const V angularAcceleration=f.vector(body.inverseInertia,body.localTorque);
    const V anchorAcceleration=f.cross(angularAcceleration,j.localAnchor60);
    const V rotational=f.add3(j.centripetalC0,anchorAcceleration);
    const V worldRotational=f.vector(body.sceneWorld100,rotational);
    const V accelerationA=f.add3(linearA,worldRotational);
    const V accelerationB=b ? f.scale(b->inverseMass,b->worldForce) : V{{0,0,0,1}};
    const V relativeAcceleration=f.sub3(accelerationA,accelerationB);
    const float radialAcceleration=f.dot(j.separationD0,relativeAcceleration);
    const float coefficient=f.sub(f.neg(j.speedSquared54),radialAcceleration);
    const V correction=f.vector(j.inverse180,f.scale(coefficient,j.directionE0));
    if(j.unilateral3C!=0) {
        const V total=f.add3(j.preparedForceF0,correction);
        const float direction=f.dot(total,j.separationD0);
        if(!f.valid) return false;
        if(direction>0.0f) { j.suppressed40=1; joint=j; magnitude=0.0f; return true; }
    }
    j.suppressed40=0;
    f.pointForce(body,j.worldAnchor80,correction);
    const float x=f.mul(correction.v[0],correction.v[0]);
    const float y=f.mul(correction.v[1],correction.v[1]);
    const float squared=f.add(f.add(x,y),f.mul(correction.v[2],correction.v[2]));
    const float result=f.root(squared);
    if(!f.valid) return false;
    joint=j; *a=body; magnitude=result; return true;
}
bool applyPodSphere49L(const J& joint, B* a) noexcept {
    if(!nearest()) return false;
    if(joint.enabled38==0 || joint.suppressed40!=0) return true;
    if(!joint.bodyAPresent44 || !ready(a)) return false;
    B body=*a; Math f;
    f.pointForce(body,joint.worldAnchor80,joint.preparedForceF0);
    if(!f.valid) return false;
    *a=body; return true;
}
}
