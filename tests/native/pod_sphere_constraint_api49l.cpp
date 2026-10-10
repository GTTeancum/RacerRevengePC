#include "../../src/game/pod_sphere_constraint49l.h"
#include <cfenv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <initializer_list>
#include <new>
#include <type_traits>

static std::uint64_t allocations=0, checks=0, calls=0;
void* operator new(std::size_t n) { if(void* p=std::malloc(n?n:1)){++allocations;return p;} throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p,std::size_t) noexcept { std::free(p); }
void operator delete[](void* p,std::size_t) noexcept { std::free(p); }
using namespace rr::game;
using J=PodSphereJoint49L; using B=PodSphereBody49L; using V=PodSphereVector49L;
static_assert(std::is_trivially_copyable<J>::value && std::is_standard_layout<J>::value);
static_assert(std::is_trivially_copyable<B>::value && std::is_standard_layout<B>::value);
static void need(bool ok,const char* why) { ++checks; if(!ok){std::fprintf(stderr,"FAIL check %llu: %s\n",static_cast<unsigned long long>(checks),why);std::exit(1);} }
static std::uint32_t bits(float f){std::uint32_t u;std::memcpy(&u,&f,4);return u;}
static float fp(std::uint32_t u){float f;std::memcpy(&f,&u,4);return f;}
template<class T>static bool same(const T& a,const T& b){return std::memcmp(&a,&b,sizeof(T))==0;}
template<class F>static bool hot(F f){const auto before=allocations;const bool result=f();++calls;need(before==allocations,"production transition allocated");return result;}
static B body(){B b{};b.scenePresent=true;b.sceneFlags190=1;b.sceneWorld100.v[0]=b.sceneWorld100.v[5]=b.sceneWorld100.v[10]=b.sceneWorld100.v[15]=1;b.inverseMass=.25f;b.inverseInertia.v[0]=.1f;b.inverseInertia.v[5]=.2f;b.inverseInertia.v[10]=.3f;b.inverseInertia.v[15]=1;b.worldForce.v[3]=fp(0x7fc00123);b.localTorque.v[3]=fp(0xffc00456);return b;}
static J joint(const B& a,const B* b){J j{};need(hot([&]{return initializePodSphere49L(j);}),"constructor");need(hot([&]{return setupPodSphereA49L(j,a,V{{0,0,3,1}});}),"setup A");need(hot([&]{return setupPodSphereB49L(j,b,V{{0,0,0,1}},3);}),"setup B");return j;}
int main(){
    need(std::fesetround(FE_TONEAREST)==0,"nearest-even setup");
    B a=body(),b=body();a.sceneWorld100.v[14]=2;a.linearVelocity={{1,2,0,1}};
    J j=joint(a,&b);need(bits(j.stiffness30)==0x43c7ffffu,"constructor separate-rounding stiffness");need(bits(j.damping34)==0x42200000u,"constructor damping");
    const auto initialB=b;need(hot([&]{return preparePodSphere49L(j,&a,&b);}),"prepare fixture");need(bits(j.preparedForceF0.v[3])==0x40000000u,"prepared W tracks VU register increments");
    float magnitude=91;need(hot([&]{return correctPodSphere49L(j,&a,&b,magnitude);}),"correct fixture");need(hot([&]{return applyPodSphere49L(j,&a);}),"apply fixture");
    need(same(b,initialB),"B changed");need(bits(a.worldForce.v[3])==0x7fc00123&&bits(a.localTorque.v[3])==0xffc00456,"body fourth lanes changed");
    // A disabled source branch consumes none of these deliberately invalid data.
    {J x=j;x.enabled38=0;const auto before=x;float m=12;need(hot([&]{return preparePodSphere49L(x,nullptr,nullptr);}),"disabled prepare");need(hot([&]{return correctPodSphere49L(x,nullptr,nullptr,m);}),"disabled correction");need(hot([&]{return applyPodSphere49L(x,nullptr);}),"disabled application");need(same(x,before)&&bits(m)==0,"disabled state/return preservation");}
    {J x=j;x.suppressed40=9;const auto before=x;need(hot([&]{return applyPodSphere49L(x,nullptr);}),"suppressed apply gate");need(same(x,before),"suppressed apply mutation");}
    // Every public leaf rejects non-nearest rounding without changing outputs.
    for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){
        J x=j;B y=a;const J before=x;const B prior=y;float m=17;
        need(std::fesetround(mode)==0,"alternate mode");
        need(!hot([&]{return initializePodSphere49L(x);}),"constructor alternate mode accepted");
        need(!hot([&]{return setupPodSphereA49L(x,y,V{{0,0,1,1}});}),"setup A alternate mode accepted");
        need(!hot([&]{return setupPodSphereB49L(x,&b,V{{0,0,0,1}},1);}),"setup B alternate mode accepted");
        need(!hot([&]{return preparePodSphere49L(x,&y,&b);}),"prepare alternate mode accepted");
        need(!hot([&]{return correctPodSphere49L(x,&y,&b,m);}),"correction alternate mode accepted");
        need(!hot([&]{return applyPodSphere49L(x,&y);}),"apply alternate mode accepted");
        need(same(x,before)&&same(y,prior)&&bits(m)==bits(17.0f),"alternate mode changed outputs");
    }
    need(std::fesetround(FE_TONEAREST)==0,"restore nearest");
    // Coincident anchors are an explicit native boundary, not a source skip.
    {B y=body();J x=joint(y,&b);x.localAnchor60={{0,0,0,1}};const auto before=x;need(!hot([&]{return preparePodSphere49L(x,&y,&b);}),"coincident anchor admitted");need(same(x,before),"coincidence mutated joint");}
    // Missing/dirty scenes and pointer-presence mismatch are atomic failures.
    for(unsigned mode=0;mode<4;++mode){B y=a;J x=j;float m=19;if(mode==0)y.scenePresent=false;if(mode==1)y.sceneFlags190=0;if(mode==2)x.bodyAPresent44=false;if(mode==3)x.bodyBPresent48=false;const J before=x;const B prior=y;need(!hot([&]{return preparePodSphere49L(x,&y,&b);}),"invalid domain prepare");need(!hot([&]{return correctPodSphere49L(x,&y,&b,m);}),"invalid domain correction");need(same(x,before)&&same(y,prior)&&bits(m)==bits(19.0f),"invalid domain mutation");}
    // Overflow after computation remains atomic even with suppression disabled.
    {B y=body();J x=joint(y,&b);x.unilateral3C=0;x.speedSquared54=std::numeric_limits<float>::max();x.directionE0={{4,0,0,1}};x.separationD0={{1,0,0,1}};const J before=x;const B prior=y;float m=23;need(!hot([&]{return correctPodSphere49L(x,&y,&b,m);}),"overflow correction admitted");need(same(x,before)&&same(y,prior)&&bits(m)==bits(23.0f),"overflow correction mutation");}
    {B y=a;J x=j;x.suppressed40=0;x.preparedForceF0.v[0]=std::numeric_limits<float>::infinity();const B prior=y;need(!hot([&]{return applyPodSphere49L(x,&y);}),"nonfinite force admitted");need(same(y,prior),"nonfinite application mutation");}
    {J x=j;const J before=x;need(!hot([&]{return setupPodSphereB49L(x,&b,V{{1,2,3,4}},std::numeric_limits<float>::max());}),"overflow length admitted");need(same(x,before),"overflow setup mutation");}
    // Setup-cached values survive live mass/inertia changes and later prepare.
    {B a1=body(),a2=a1;J x=joint(a1,&b),y=x;a1.sceneWorld100.v[14]=a2.sceneWorld100.v[14]=1;a2.inverseMass=2;a2.inverseInertia.v[0]=4;need(hot([&]{return preparePodSphere49L(x,&a1,&b);}),"cache baseline prepare");need(hot([&]{return preparePodSphere49L(y,&a2,&b);}),"cache changed-live prepare");need(same(x,y),"prepare reread live setup mass/inertia");}
    // B angular velocity is not consumed, even with a nonzero B local anchor.
    {B a1=body(),b1=b,b2=b;J x=joint(a1,&b1);x.localAnchor70={{2,3,4,1}};J y=x;b2.angularVelocity={{1000,-2000,3000,fp(0x7fc00001)}};need(hot([&]{return preparePodSphere49L(x,&a1,&b1);}),"B angular baseline");need(hot([&]{return preparePodSphere49L(y,&a1,&b2);}),"B angular changed");need(same(x,y),"B angular velocity affected constraint");}
    // Loaded W lanes can still be consumed by source VU self-clears even
    // when the helper's visible arithmetic is XYZ-only. Reject just those
    // nonfinite paths, atomically; neighboring truly unused lanes stay valid.
    for(std::uint32_t poison:{0x7fc00321u,0x7f800000u,0xff800000u}){
        B a1=body(),b1=body();a1.sceneWorld100.v[14]=2;J x=joint(a1,&b1);
        b1.sceneWorld100.v[11]=fp(poison);const J prior=x;
        need(!hot([&]{return preparePodSphere49L(x,&a1,&b1);}),"nonfinite B point vf6.w admitted");
        need(same(x,prior),"B point vf6.w rejection mutated joint");
        x.enabled38=0;const J disabled=x;
        need(hot([&]{return preparePodSphere49L(x,&a1,&b1);}),"disabled prepare read B vf6.w");
        need(same(x,disabled),"disabled poisoned B changed joint");
        b1=body();x=joint(a1,&b1);need(hot([&]{return preparePodSphere49L(x,&a1,&b1);}),"prepare live inertia W case");
        a1.inverseInertia.v[11]=fp(poison);const J before=x;const B oldA=a1;float m=31;
        need(!hot([&]{return correctPodSphere49L(x,&a1,&b1,m);}),"nonfinite current inertia vf6.w admitted");
        need(same(x,before)&&same(a1,oldA)&&bits(m)==bits(31.0f),"current inertia W rejection mutated outputs");
        x.enabled38=0;const J off=x;
        need(hot([&]{return correctPodSphere49L(x,&a1,&b1,m);}),"disabled correction read current inertia W");
        need(same(x,off)&&same(a1,oldA)&&bits(m)==0,"disabled poisoned inertia changed state");
        for(unsigned lane:{3u,7u,15u}){
            B a2=body(),b2=body();a2.sceneWorld100.v[14]=2;J y=joint(a2,&b2);
            b2.sceneWorld100.v[lane]=fp(poison);
            need(hot([&]{return preparePodSphere49L(y,&a2,&b2);}),"unused B scene lane rejected");
            a2.inverseInertia.v[lane]=fp(poison);
            need(hot([&]{return correctPodSphere49L(y,&a2,&b2,m);}),"unused current inertia lane rejected");
            need(bits(a2.inverseInertia.v[lane])==poison,"unused current inertia lane changed");
        }
    }
    // Long repeated use exercises fixed state and preserved B/4th lanes.
    for(unsigned n=0;n<2000;++n){B y=body();y.sceneWorld100.v[14]=2;y.linearVelocity={{.2f,.1f,0,1}};J x=joint(y,&b);float m=0;need(hot([&]{return preparePodSphere49L(x,&y,&b);}),"hot prepare");need(hot([&]{return correctPodSphere49L(x,&y,&b,m);}),"hot correct");need(hot([&]{return applyPodSphere49L(x,&y);}),"hot apply");need(same(b,initialB),"hot B changed");}
    std::printf("%llu API checks passed; %llu zero-allocation transitions; atomic failures and stage gates verified\n",static_cast<unsigned long long>(checks),static_cast<unsigned long long>(calls));
}
