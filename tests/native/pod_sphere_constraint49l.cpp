#include "../../src/game/pod_sphere_constraint49l.h"
#include "../../tools/gameplay/pod_sphere_constraint_oracle49l.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>

// Successful native hot-call allocations only; no failure injection, ownership
// experiment, process RAM measurement or diagnostic investigation.
static std::uint64_t allocations=0;
void* operator new(std::size_t n){if(void* p=std::malloc(n?n:1)){++allocations;return p;}throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,std::size_t)noexcept{std::free(p);}
void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
using namespace rr::game;
namespace original=sphere49l_oracle;
using U=std::uint32_t;using V=PodSphereVector49L;using M=PodSphereMatrix49L;using B=PodSphereBody49L;using J=PodSphereJoint49L;using O=original::Machine;
static std::uint64_t checks=0,cases=0,hotCalls=0,instructions=0,suppressedCases=0,acceptedCases=0;
static std::array<std::uint64_t,sphere49l_words::total_words> hits{},taken{},notTaken{};
static const char* current="initial";
static U bits(float x){return original::bits(x);}
static float fp(U x){return original::fp(x);}
static void require(bool ok,const char* msg){++checks;if(!ok)throw std::runtime_error(std::string(current)+": "+msg);}
static void same(float x,float y,const std::string& label){++checks;if(bits(x)!=bits(y)){char z[160];std::snprintf(z,sizeof(z)," %08x != %08x (%g != %g)",bits(x),bits(y),double(x),double(y));throw std::runtime_error(std::string(current)+": "+label+z);}}
static void collect(const O& o){instructions+=o.steps;for(std::size_t i=0;i<hits.size();++i){hits[i]+=o.hits[i];taken[i]+=o.taken[i];notTaken[i]+=o.notTaken[i];}}
static V vec(float x,float y,float z,float w=1){return {{x,y,z,w}};}
static M identity(float x=0,float y=0,float z=0){M m{};m.v[0]=m.v[5]=m.v[10]=m.v[15]=1;m.v[12]=x;m.v[13]=y;m.v[14]=z;return m;}
static B body(float x=0,float y=0,float z=0){B b{};b.scenePresent=true;b.sceneFlags190=1;b.sceneWorld100=identity(x,y,z);b.inverseMass=.5f;b.inverseInertia=identity();b.linearVelocity=vec(0,0,0,17);b.angularVelocity=vec(0,0,0,19);b.worldForce=vec(0,0,0,23);b.localTorque=vec(0,0,0,29);b.localCenter=vec(0,0,0,31);return b;}
static J seeded(){J j{};j.stiffness30=3;j.damping34=5;j.enabled38=7;j.unilateral3C=9;j.suppressed40=11;j.bodyAPresent44=true;j.bodyBPresent48=true;j.length4C=13;j.lengthSquared50=17;j.speedSquared54=19;unsigned n=0;for(V* p:{&j.localAnchor60,&j.localAnchor70,&j.worldAnchor80,&j.worldAnchor90,&j.fixedAnchorA0,&j.fieldB0,&j.centripetalC0,&j.separationD0,&j.directionE0,&j.preparedForceF0})for(float& f:p->v)f=float(++n)/8;for(M* p:{&j.cached100,&j.cached140,&j.inverse180})for(float& f:p->v)f=float(++n)/8;return j;}
static void put(O& o,U a,const V& v){for(unsigned i=0;i<4;++i)o.store(a+4*i,v.v[i]);}
static void put(O& o,U a,const M& m){for(unsigned i=0;i<16;++i)o.store(a+4*i,m.v[i]);}
static void compare(const O& o,U a,const V& v,const char* name){for(unsigned i=0;i<4;++i)same(v.v[i],o.load(a+4*i),std::string(name)+" lane"+std::to_string(i));}
static void compare(const O& o,U a,const M& m,const char* name){for(unsigned i=0;i<16;++i)same(m.v[i],o.load(a+4*i),std::string(name)+" lane"+std::to_string(i));}
static void putJoint(O& o,const J& j){const U a=O::joint;
#define F(off,field) o.store(a+off,j.field)
 F(0x30,stiffness30);F(0x34,damping34);F(0x4c,length4C);F(0x50,lengthSquared50);F(0x54,speedSquared54);
#undef F
 o.write(a+0x38,j.enabled38);o.write(a+0x3c,j.unilateral3C);o.write(a+0x40,j.suppressed40);o.write(a+0x44,j.bodyAPresent44?O::bodyA:0);o.write(a+0x48,j.bodyBPresent48?O::bodyB:0);
#define VEC(off,field) put(o,a+off,j.field)
 VEC(0x60,localAnchor60);VEC(0x70,localAnchor70);VEC(0x80,worldAnchor80);VEC(0x90,worldAnchor90);VEC(0xa0,fixedAnchorA0);VEC(0xb0,fieldB0);VEC(0xc0,centripetalC0);VEC(0xd0,separationD0);VEC(0xe0,directionE0);VEC(0xf0,preparedForceF0);VEC(0x100,cached100);VEC(0x140,cached140);VEC(0x180,inverse180);
#undef VEC
}
static void compareJoint(const O& o,const J& j){const U a=O::joint;
#define F(off,field) same(j.field,o.load(a+off),#field)
 F(0x30,stiffness30);F(0x34,damping34);F(0x4c,length4C);F(0x50,lengthSquared50);F(0x54,speedSquared54);
#undef F
 require(j.enabled38==o.read(a+0x38),"enabled");require(j.unilateral3C==o.read(a+0x3c),"unilateral");require(j.suppressed40==o.read(a+0x40),"suppressed");require(j.bodyAPresent44==(o.read(a+0x44)!=0),"A presence");require(j.bodyBPresent48==(o.read(a+0x48)!=0),"B presence");
#define VEC(off,field) compare(o,a+off,j.field,#field)
 VEC(0x60,localAnchor60);VEC(0x70,localAnchor70);VEC(0x80,worldAnchor80);VEC(0x90,worldAnchor90);VEC(0xa0,fixedAnchorA0);VEC(0xb0,fieldB0);VEC(0xc0,centripetalC0);VEC(0xd0,separationD0);VEC(0xe0,directionE0);VEC(0xf0,preparedForceF0);VEC(0x100,cached100);VEC(0x140,cached140);VEC(0x180,inverse180);
#undef VEC
}
static void putBody(O& o,U a,U scene,const B& b){
 o.write(a+0x288,b.scenePresent?scene:0);o.write(scene+0x190,b.sceneFlags190);put(o,scene+0x100,b.sceneWorld100);o.store(a+0x270,b.inverseMass);put(o,a+0x140,b.inverseInertia);put(o,a+0x70,b.linearVelocity);put(o,a+0xb0,b.angularVelocity);put(o,a+0x180,b.worldForce);put(o,a+0x190,b.localTorque);put(o,a+0x1e0,b.localCenter);
}
static void compareBody(const O& o,U a,U scene,const B& b){
 require(o.read(a+0x288)==(b.scenePresent?scene:0),"body scene presence");require(o.read(scene+0x190)==b.sceneFlags190,"body scene flags");compare(o,scene+0x100,b.sceneWorld100,"scene matrix");same(b.inverseMass,o.load(a+0x270),"body inverse mass");compare(o,a+0x140,b.inverseInertia,"body inverse inertia");compare(o,a+0x70,b.linearVelocity,"body linear velocity");compare(o,a+0xb0,b.angularVelocity,"body angular velocity");compare(o,a+0x180,b.worldForce,"body force");compare(o,a+0x190,b.localTorque,"body torque");compare(o,a+0x1e0,b.localCenter,"body local center");
}
template<class F>static void native(F f,const char* name){auto n=allocations;bool ok=f();auto z=allocations;require(ok,name);require(n==z,"native hot call allocated");++hotCalls;}
static void seed(O& o,J& j,B& a,B& b){j=seeded();putJoint(o,j);putBody(o,O::bodyA,O::sceneA,a);putBody(o,O::bodyB,O::sceneB,b);o.constructor();native([&]{return initializePodSphere49L(j);},"initialize");compareJoint(o,j);}
static void setup(O& o,J& j,const B& a,const B* b,const V& va,const V& vb,float length){
 put(o,O::input,va);o.call(0x1b64d0,O::joint,O::bodyA,O::input);native([&]{return setupPodSphereA49L(j,a,va);},"setup A");compareJoint(o,j);
 put(o,O::input,vb);o.f[12]=length;o.call(0x1b6670,O::joint,b?O::bodyB:0,O::input);native([&]{return setupPodSphereB49L(j,b,vb,length);},"setup B");compareJoint(o,j);
}
struct Result{J j;B a;float magnitude;};
static void noBForbiddenReads(const O& o){
 for(U base:{O::sceneA,O::sceneB})for(U x=base;x<base+0x200;x+=4){const U k=x-base;if(!(k>=0x100&&k<0x140)&&k!=0x190)require(o.reads[x/4]==0,"original read outside ready scene fields");require(o.writes[x/4]==0,"original wrote initialized ready scene");}
 for(U x=O::bodyA;x<O::bodyA+0x300;x+=4){const U k=x-O::bodyA;const bool allowed=(k>=0x70&&k<0x80)||(k>=0xb0&&k<0xc0)||(k>=0x140&&k<0x180)||(k>=0x180&&k<0x1a0)||(k>=0x1e0&&k<0x1ec)||k==0x270||k==0x288;if(!allowed)require(o.reads[x/4]==0,"original read outside A numeric fields");}
 for(U x=O::bodyB;x<O::bodyB+0x300;x+=4){const U k=x-O::bodyB;const bool allowed=(k>=0x70&&k<0x80)||(k>=0x180&&k<0x18c)||k==0x270||k==0x288; if(!allowed)require(o.reads[x/4]==0,"original read forbidden B angular/inertia/torque/center state");}
}
static Result ordered(const char* label,B a,B b,bool hasB,V va,V vb,float length,bool unilateral=true,bool mutateCache=false,bool weird=false){current=label;O o;J j;seed(o,j,a,b);setup(o,j,a,hasB?&b:nullptr,va,vb,length);
 j.unilateral3C=unilateral?1:0;j.fixedAnchorA0=vec(-.5f,.25f,-.125f,3);o.write(O::joint+0x3c,j.unilateral3C);put(o,O::joint+0xa0,j.fixedAnchorA0);
 if(mutateCache){a.inverseMass=.125f;a.inverseInertia.v[0]=2.5f;a.inverseInertia.v[5]=1.25f;a.inverseInertia.v[10]=.75f;putBody(o,O::bodyA,O::sceneA,a);}
 if(weird){j.cached140.v[3]=.125f;j.cached140.v[7]=-.25f;j.cached140.v[11]=.5f;j.cached140.v[15]=3;put(o,O::joint+0x140,j.cached140);}
 o.reads.fill(0);o.writes.fill(0);o.call(0x1b66d0);noBForbiddenReads(o);native([&]{return preparePodSphere49L(j,&a,hasB?&b:nullptr);},"prepare");compareJoint(o,j);compareBody(o,O::bodyA,O::sceneA,a);compareBody(o,O::bodyB,O::sceneB,b);
 // Only correction/application may touch A's six XYZ load words.
 for(U x=O::bodyA;x<O::bodyA+0x300;x+=4)require(o.writes[x/4]==0,"prepare wrote body A");
 if(mutateCache){a.worldForce=vec(.125f,-.75f,.5f,23);a.localTorque=vec(.375f,.25f,-.125f,29);a.sceneWorld100.v[12]+=.25f;a.localCenter.v[0]+=.125f;putBody(o,O::bodyA,O::sceneA,a);}
 o.writes.fill(0);o.reads.fill(0);float magnitude=fp(0x42f60000);o.call(0x1b6ba0);noBForbiddenReads(o);native([&]{return correctPodSphere49L(j,&a,hasB?&b:nullptr,magnitude);},"correct");same(magnitude,o.f[0],"correction magnitude");compareJoint(o,j);compareBody(o,O::bodyA,O::sceneA,a);compareBody(o,O::bodyB,O::sceneB,b);
 if(j.suppressed40)++suppressedCases;else ++acceptedCases;
 o.reads.fill(0);o.call(0x1b6ff0);noBForbiddenReads(o);native([&]{return applyPodSphere49L(j,&a);},"apply");compareJoint(o,j);compareBody(o,O::bodyA,O::sceneA,a);compareBody(o,O::bodyB,O::sceneB,b);
 for(U x=O::bodyA;x<O::bodyA+0x300;x+=4)if(x< O::bodyA+0x180||x>O::bodyA+0x198||x==O::bodyA+0x18c)require(o.writes[x/4]==0,"unexpected A write footprint");
 for(U x=O::bodyB;x<O::bodyB+0x300;x+=4)require(o.writes[x/4]==0,"original wrote B");
 
 collect(o);++cases;return {j,a,magnitude};
}
static void fixtures(){
 B a=body(3,0,0),b=body();
 auto neg=ordered("stretched negative radial force",a,b,true,vec(0,0,0),vec(0,0,0),1);
 require(neg.j.preparedForceF0.v[0]<0&&neg.j.suppressed40==0,"negative force case missing");
 auto pos=ordered("compressed positive unilateral suppressed",a,b,true,vec(0,0,0),vec(0,0,0),4);
 require(pos.j.preparedForceF0.v[0]>0&&pos.j.suppressed40==1&&bits(pos.magnitude)==0,"positive suppression missing");
 auto eq=ordered("exact equality accepted",a,b,true,vec(0,0,0),vec(0,0,0),3);
 require(eq.j.suppressed40==0&&eq.magnitude==0&&eq.j.preparedForceF0.v[0]==0,"unilateral equality branch missing");
 auto bilateral=ordered("positive force bilateral",a,b,true,vec(0,0,0),vec(0,0,0),4,false);
 require(bilateral.j.suppressed40==0&&bilateral.a.worldForce.v[0]>0,"bilateral positive path missing");
 ordered("null B fixed point",a,b,false,vec(.25f,-.5f,.75f,7),vec(11,13,17,19),1.5f);
 for(float n:{-3.0f,-0.0f,0.0f,fp(0x3dcccccbu),fp(0x3dccccccu),fp(0x3dcccccdu),fp(0x3dccccceu),.125f,3.0f})ordered("source length max boundary",a,b,true,vec(0,0,0),vec(0,0,0),n);
 a=body(3,.5f,-.75f);b=body(-.25f,.5f,.125f);a.inverseInertia.v[0]=.25f;a.inverseInertia.v[5]=.5f;a.inverseInertia.v[10]=.75f;
 a.linearVelocity=vec(.25f,-.5f,.125f,17);b.linearVelocity=vec(-.25f,.125f,.5f,19);a.angularVelocity=vec(.125f,.25f,-.375f,23);a.worldForce=vec(.75f,-1.25f,.5f,29);a.localTorque=vec(.125f,-.25f,.5f,31);a.localCenter=vec(.25f,-.125f,.375f,37);b.worldForce=vec(-.125f,.25f,-.5f,41);
 a.sceneWorld100.v[0]=0;a.sceneWorld100.v[1]=1;a.sceneWorld100.v[4]=-1;a.sceneWorld100.v[5]=0;
 b.sceneWorld100.v[5]=0;b.sceneWorld100.v[6]=1;b.sceneWorld100.v[9]=-1;b.sceneWorld100.v[10]=0;
 ordered("rotated torque angular velocity two bodies",a,b,true,vec(.25f,-.5f,.75f,7),vec(-.125f,.25f,-.375f,11),1.5f,false);
 ordered("cached setup versus live correction",a,b,true,vec(.25f,-.5f,.75f,7),vec(-.125f,.25f,-.375f,11),1.5f,false,true);
 a.sceneWorld100.v[3]=.125f;a.sceneWorld100.v[7]=-.25f;a.sceneWorld100.v[11]=.375f;a.sceneWorld100.v[15]=1.5f;
 a.inverseInertia.v[3]=.0625f;a.inverseInertia.v[7]=-.125f;a.inverseInertia.v[11]=.25f;a.inverseInertia.v[12]=.125f;a.inverseInertia.v[13]=-.25f;a.inverseInertia.v[14]=.5f;a.inverseInertia.v[15]=2;
 ordered("full fourth lanes and nonaffine cached product",a,b,true,vec(.25f,-.5f,.75f,-3),vec(-.125f,.25f,-.375f,11),1.5f,false,false,true);
 a=body(3,1,-1);a.inverseMass=0;a.inverseInertia=M{};
 auto singular=ordered("exact singular inverse",a,body(),true,vec(0,0,0),vec(0,0,0),1,false);
 for(float x:singular.j.inverse180.v)require(bits(x)==0,"singular inverse nonzero lane");
 // B's angular, inertia, torque, local center are deliberately varied. They
 // are initialized but never consumed; no reciprocity is silently introduced.
 b=body();b.angularVelocity=vec(99,-91,87,71);for(float& x:b.inverseInertia.v)x=83;b.localTorque=vec(-77,69,61,53);b.localCenter=vec(43,37,31,29);
 auto unused=ordered("unconsumed B rotational fields",body(3,0,0),b,true,vec(0,0,0),vec(0,0,0),1);
 for(unsigned i=0;i<4;++i)same(unused.a.worldForce.v[i],neg.a.worldForce.v[i],"B rotational invariance force");
 // A deterministic finite normal grid, with independent source execution for
 // every fixture. No expected number is generated from a native formula.
 U rng=0x49c01a5u;auto rand=[&](){rng=rng*1664525u+1013904223u;return (float((rng>>24)&255u)-127.0f)/128.0f;};
 for(unsigned n=0;n<512;++n){a=body(3+rand(),rand(),rand());b=body(rand(),rand(),rand());a.inverseMass=.25f+std::fabs(rand());b.inverseMass=.25f+std::fabs(rand());
  a.linearVelocity=vec(rand(),rand(),rand(),13);b.linearVelocity=vec(rand(),rand(),rand(),17);a.angularVelocity=vec(rand()*.25f,rand()*.25f,rand()*.25f,19);a.worldForce=vec(rand(),rand(),rand(),23);a.localTorque=vec(rand(),rand(),rand(),29);b.worldForce=vec(rand(),rand(),rand(),31);a.localCenter=vec(rand()*.25f,rand()*.25f,rand()*.25f,37);
  a.inverseInertia.v[0]=.25f+std::fabs(rand());a.inverseInertia.v[5]=.25f+std::fabs(rand());a.inverseInertia.v[10]=.25f+std::fabs(rand());
  if(n&1){a.sceneWorld100.v[0]=.6f;a.sceneWorld100.v[1]=.8f;a.sceneWorld100.v[4]=-.8f;a.sceneWorld100.v[5]=.6f;}
  ordered("deterministic normal finite grid",a,b,(n%5)!=0,vec(rand()*.5f,rand()*.5f,rand()*.5f,7),vec(rand()*.5f,rand()*.5f,rand()*.5f,11),.25f+std::fabs(rand())*4,(n%3)!=0,(n%7)==0);
 }
}
static void disabled(){current="disabled no-op preserves suppression";O o;J j=seeded();j.enabled38=0;j.suppressed40=7;j.bodyAPresent44=false;j.bodyBPresent48=false;putJoint(o,j);o.call(0x1b66d0);native([&]{return preparePodSphere49L(j,nullptr,nullptr);},"disabled prepare");compareJoint(o,j);float mag=123;o.call(0x1b6ba0);native([&]{return correctPodSphere49L(j,nullptr,nullptr,mag);},"disabled correct");same(mag,o.f[0],"disabled +0 magnitude");require(bits(mag)==0,"disabled +0");compareJoint(o,j);o.call(0x1b6ff0);native([&]{return applyPodSphere49L(j,nullptr);},"disabled apply");compareJoint(o,j);require(j.suppressed40==7,"disabled suppressed state changed");
 current="suppressed apply before A access";j.enabled38=1;j.suppressed40=1;putJoint(o,j);o.call(0x1b6ff0);native([&]{return applyPodSphere49L(j,nullptr);},"suppressed apply");compareJoint(o,j);collect(o);cases+=2;
}
static bool sameJoint(const J& a,const J& b){O o(false);putJoint(o,a);try{compareJoint(o,b);return true;}catch(...){return false;}}
static void boundary(){current="explicit native boundary";B a=body(),b=body();J j{};native([&]{return initializePodSphere49L(j);},"boundary init");native([&]{return setupPodSphereA49L(j,a,vec(0,0,0));},"boundary setup A");native([&]{return setupPodSphereB49L(j,&b,vec(0,0,0),1);},"boundary setup B");J old=j;
 require(!preparePodSphere49L(j,&a,&b),"coincident anchors must reject native boundary");require(sameJoint(old,j),"zero-distance rejection changed joint");
 a.sceneWorld100.v[12]=3;a.sceneFlags190=0;require(!preparePodSphere49L(j,&a,&b),"dirty scene must reject");require(sameJoint(old,j),"dirty scene rejection changed joint");
 a.sceneFlags190=1;a.linearVelocity.v[0]=fp(0x7fc00000);require(!preparePodSphere49L(j,&a,&b),"nonfinite consumed value must reject");require(sameJoint(old,j),"nonfinite rejection changed joint");
 // Source point(B) loads vf6.w from matrix lane11, later vsub.w self
 // reaches persisted C0.w. Nonfinite values are outside the native finite-
 // intermediate boundary. These are native admission tests, not nonfinite
 // EE/VU equivalence claims or unbounded word-execution fixtures.
 a.linearVelocity.v[0]=0;
 for(U bad:{0x7fc0491au,0x7f800000u,0xff800000u}){b.sceneWorld100.v[11]=fp(bad);require(!preparePodSphere49L(j,&a,&b),"nonfinite consumed B scene lane11 admitted");require(sameJoint(old,j),"B scene lane11 rejection changed joint");}
 b.sceneWorld100.v[11]=0;
 for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){require(std::fesetround(mode)==0,"rounding mode setup");require(!initializePodSphere49L(j),"unsupported rounding admitted");require(sameJoint(old,j),"rounding rejection changed joint");}require(std::fesetround(FE_TONEAREST)==0,"restore nearest");
}
static void orderWitness(){
 B a=body(3,0,0),b=body();a.linearVelocity=vec(.5f,.25f,0,17);a.worldForce=vec(1,2,3,23);a.localTorque=vec(.125f,.25f,.5f,29);const V anchor=vec(.5f,.25f,-.125f,7);
 auto correct=ordered("ordered C then F witness",a,b,true,anchor,vec(0,0,0),1,false);
 current="reversed-order counterexample";O o;J j;seed(o,j,a,b);setup(o,j,a,&b,anchor,vec(0,0,0),1);j.unilateral3C=0;o.write(O::joint+0x3c,0);o.call(0x1b66d0);o.call(0x1b6ff0);o.call(0x1b6ba0);
 bool different=false;for(unsigned i=0;i<3;++i)different|=bits(correct.a.worldForce.v[i])!=bits(o.load(O::bodyA+0x180+4*i));require(different,"fixture did not distinguish force order");collect(o);++cases;
}
static bool branch(U w){return (w>>26)==4||(w>>26)==5||(w>>26)==7||((w>>26)==17&&((w>>21)&31)==8);}
static const char* exclusion(U pc){
 if(pc>=0x1b6200&&pc<0x1b6214)return "inherited constructor prefix outside represented numeric fields";
 if((pc>=0x28b9e8&&pc<0x28ba60)||(pc>=0x28bae8&&pc<0x28bb60)||(pc>=0x28bbe8&&pc<0x28bc60)||(pc>=0x28bd88&&pc<0x28be00))return "dirty-scene hierarchy path excluded; initialized ready-scene inputs only";
 return "uncovered PC; no execution claim";
}
static void coverageRequirements(){
 std::size_t visited=0;std::uint64_t sum=0;
 for(const auto& b:sphere49l_words::blocks)for(std::size_t i=0;i<b.count;++i){U pc=b.start+U(i*4),w=b.words[i];auto ix=b.index+i;sum+=hits[ix];visited+=hits[ix]!=0;
  if(!hits[ix])require(std::string(exclusion(pc))!="uncovered PC; no execution claim","required original PC never executed");
  if(branch(w)&&hits[ix]&&(!taken[ix]||!notTaken[ix])){const bool unconditional=(w>>26)==4&&((w>>21)&31)==((w>>16)&31);const bool readyGate=pc==0x28b9e0||pc==0x28bae0||pc==0x28bbe0||pc==0x28bd80;const bool nullScene=pc==0x1b6eb4||pc==0x1b703c;require(unconditional||readyGate||nullScene,"required original branch outcome never executed");}
 }
 require(sum==instructions,"instruction and PC accounting diverged");require(visited==1514,"expected admitted source coverage changed");
}
static void coverage(const char* path){std::ofstream out(path);require(bool(out),"coverage file open");out<<"{\n  \"cases\": "<<cases<<",\n  \"checks\": "<<checks<<",\n  \"hot_calls\": "<<hotCalls<<",\n  \"native_hot_allocations\": 0,\n  \"original_instructions\": "<<instructions<<",\n  \"suppressed_cases\": "<<suppressedCases<<",\n  \"accepted_cases\": "<<acceptedCases<<",\n  \"blocks\": [\n";
 for(std::size_t bi=0;bi<std::size(sphere49l_words::blocks);++bi){const auto& b=sphere49l_words::blocks[bi];std::size_t visited=0;for(std::size_t i=0;i<b.count;++i)visited+=hits[b.index+i]!=0;out<<"    {\"name\":\""<<b.name<<"\",\"words\":"<<b.count<<",\"visited_words\":"<<visited<<",\"pcs\":[";
  for(std::size_t i=0;i<b.count;++i){U pc=b.start+U(i*4);auto ix=b.index+i;char p[16];std::snprintf(p,sizeof(p),"0x%08x",pc);out<<(i?",":"")<<"{\"pc\":\""<<p<<"\",\"hits\":"<<hits[ix];if(branch(b.words[i])){
 out<<",\"taken\":"<<taken[ix]<<",\"not_taken\":"<<notTaken[ix];
 if(((b.words[i]>>26)==4)&&(((b.words[i]>>21)&31)==((b.words[i]>>16)&31)))out<<",\"outcome_scope\":\"unconditional equal-register branch; not-taken impossible\"";
 else if(pc==0x1b6eb4||pc==0x1b703c)out<<",\"outcome_scope\":\"null A scene branch excluded by native initialized-ready-scene boundary\"";
 else if(pc==0x28b9e0||pc==0x28bae0||pc==0x28bbe0||pc==0x28bd80)out<<",\"outcome_scope\":\"dirty-scene outcome excluded by initialized-ready-scene boundary\"";
 }if(!hits[ix])out<<",\"exclusion\":\""<<exclusion(pc)<<"\"";out<<"}";}
  out<<"]}"<<(bi+1==std::size(sphere49l_words::blocks)?"\n":",\n");
 }out<<"  ]\n}\n";
}
int main(int argc,char** argv){try{require(std::fesetround(FE_TONEAREST)==0,"nearest rounding");fixtures();disabled();orderWitness();boundary();coverageRequirements();require(suppressedCases>0&&acceptedCases>0,"both unilateral outcomes");if(argc>1)coverage(argv[1]);std::cout<<"PASS PodSphere49L: "<<cases<<" cases, "<<checks<<" bit/state checks, "<<hotCalls<<" zero-allocation native calls, "<<instructions<<" original instructions\n";return 0;}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<"\n";return 1;}}
