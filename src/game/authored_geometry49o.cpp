#include "authored_geometry49o.h"
#include <cfenv>
#include <cmath>
#include <cstring>
namespace rr::game {
namespace {
inline float fp(unsigned u) noexcept {float f;std::memcpy(&f,&u,4);return f;}
inline unsigned bits(float f) noexcept {unsigned u;std::memcpy(&u,&f,4);return u;}
struct Math {
 bool valid=true;
 float keep(float f)noexcept{valid=valid&&std::isfinite(f);return f;}
 float add(float a,float b)noexcept{volatile float f=a+b;return keep(f);}
 float sub(float a,float b)noexcept{volatile float f=a-b;return keep(f);}
 float mul(float a,float b)noexcept{volatile float f=a*b;return keep(f);}
 float div(float a,float b)noexcept{volatile float f=a/b;return keep(f);}
 float neg(float a)noexcept{return fp(bits(a)^0x80000000u);}
 float root(float a)noexcept{if((bits(a)&0x7f800000u)==0)return fp(bits(a)&0x80000000u);volatile float f=static_cast<float>(std::sqrt(static_cast<double>(a)));return keep(f);}
 BodyMatrix48G inverse(const BodyMatrix48G&m)noexcept{
  const float a=m.v[0],b=m.v[4],c=m.v[8],d=m.v[1],e=m.v[5],f=m.v[9],g=m.v[2],h=m.v[6],i=m.v[10];
  const float c00=sub(mul(e,i),mul(f,h)),ci=sub(mul(b,i),mul(c,h)),c02=sub(mul(b,f),mul(c,e));
  float det=sub(mul(a,c00),mul(d,ci));det=add(det,mul(g,c02));BodyMatrix48G out{};
  if(det==0)return out;
  const float u=div(1,det);
  out.v[0]=mul(u,c00);out.v[1]=mul(u,neg(sub(mul(d,i),mul(f,g))));out.v[2]=mul(u,sub(mul(d,h),mul(e,g)));
  out.v[4]=mul(u,neg(ci));out.v[5]=mul(u,sub(mul(a,i),mul(c,g)));out.v[6]=mul(u,neg(sub(mul(a,h),mul(b,g))));
  out.v[8]=mul(u,c02);out.v[9]=mul(u,neg(sub(mul(a,f),mul(c,d))));out.v[10]=mul(u,sub(mul(a,e),mul(b,d)));return out;
 }
};
}
// Input geometry is the source-ready typed boundary. Fresh original vehicle
// vector initialization proves the W lanes written here, not a body initializer.
bool bindAuthoredGeometry49O(const AuthoredGeometryInputs49O&in,AuthoredGeometryBinding49O&out)noexcept{
 if(std::fegetround()!=FE_TONEAREST||!std::isfinite(in.postMotionRadius198)||in.postMotionRadius198<0)return false;
 Math f;AuthoredGeometryBinding49O b{};b.radius198=in.postMotionRadius198;b.mass26C=fp(0x40c6c1fdu);
 const float squared=f.mul(b.radius198,b.radius198);const float inertia=f.mul(f.mul(.5f,b.mass26C),squared);
 b.inertia100.v[0]=b.inertia100.v[5]=b.inertia100.v[10]=inertia;b.inertia100.v[15]=1;
 b.inverseMass270=f.div(1,b.mass26C);b.inverseInertia140=f.inverse(b.inertia100);
 b.targetRelative2F0={{f.sub(in.preAnimationChariotNullCgLocal.v[0],0),f.sub(in.preAnimationChariotNullCgLocal.v[1],0),f.sub(in.preAnimationChariotNullCgLocal.v[2],in.preAnimationRightEngineNullCgWorldZ),1}};
 const auto&v=b.targetRelative2F0.v;const float q=f.add(f.add(f.mul(v[0],v[0]),f.mul(v[1],v[1])),f.mul(v[2],v[2]));b.length854=f.root(q);
 b.secondaryAnchor300={{0,0,3,1}};b.mainAnchor={{0,0,0,1}};
 // Source2126D0..2126E0 still executes the anchor length arithmetic.
 const auto&a=b.secondaryAnchor300.v;const float aq=f.add(f.add(f.mul(a[0],a[0]),f.mul(a[1],a[1])),f.mul(a[2],a[2]));b.unclampedLength858=f.sub(b.length854,f.root(aq));
 if(!f.valid)return false;
 out=b;return true;
}
// Only mass, inverse matrix, source four limits, and target are projected.
// Main/body axes/center/pose/history/scene registration/noise remain supplied.
bool projectAuthoredGeometry49O(const AuthoredGeometryBinding49O&b,const CoupledBodyParameters49M&previous,const SecondaryInputs49M&previousInputs,CoupledBodyParameters49M&out,SecondaryInputs49M&outInputs)noexcept{
 if(std::fegetround()!=FE_TONEAREST||!std::isfinite(b.mass26C)||!std::isfinite(b.inverseMass270))return false;
 for(float x:b.inverseInertia140.v)if(!std::isfinite(x))return false;
 for(float x:b.targetRelative2F0.v)if(!std::isfinite(x))return false;
 auto p=previous;auto i=previousInputs;p.secondary.body.mass=b.mass26C;p.secondary.body.inverseMass=b.inverseMass270;
 for(unsigned c=0;c<3;++c)for(unsigned r=0;r<3;++r)p.secondary.body.inverseInertiaColumns[c].v[r]=b.inverseInertia140.v[c*4+r];
 constexpr unsigned lanes[]={3,7,11,12,13,14,15};for(unsigned j=0;j<7;++j)p.secondary.inverseInertiaExtra[j]=b.inverseInertia140.v[lanes[j]];
 const float limit=fp(0x501502f9u);p.secondary.body.linearAccelerationLimitSquared=limit;p.secondary.body.angularAccelerationLimitSquared=limit;p.secondary.body.linearVelocityLimitSquared=limit;p.secondary.body.angularVelocityLimitSquared=limit;
 i.targetRelative2F0=b.targetRelative2F0;out=p;outInputs=i;return true;
}
// Original suffix + setup caches, after successful body parameter binding.
// The explicit main body supplies its presence identity;49L B setup does not
// inspect main parameters/pose. No pose is manufactured or moved to fit rest.
bool setupAuthoredPodSphere49O(const AuthoredGeometryBinding49O&b,const PodSphereBody49L&main,const PodSphereJoint49L&previous,PodSphereJoint49L&out)noexcept{
 if(!std::isfinite(b.inverseMass270)||!std::isfinite(b.unclampedLength858))return false;
 for(float x:b.inverseInertia140.v)if(!std::isfinite(x))return false;
 for(float x:b.secondaryAnchor300.v)if(!std::isfinite(x))return false;
 for(float x:b.mainAnchor.v)if(!std::isfinite(x))return false;
 PodSphereBody49L secondary{};secondary.inverseMass=b.inverseMass270;for(unsigned i=0;i<16;++i)secondary.inverseInertia.v[i]=b.inverseInertia140.v[i];
 auto j=previous;if(!initializePodSphere49L(j)||!setupPodSphereA49L(j,secondary,b.secondaryAnchor300)||!setupPodSphereB49L(j,&main,b.mainAnchor,b.unclampedLength858))return false;out=j;return true;
}
}
