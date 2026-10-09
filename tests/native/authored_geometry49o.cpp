#include "../../src/game/authored_geometry49o.h"
#include "../../evidence/authoredGeometry49O/initialization/geometry_oracle_bridge49o.h"
#include "../../tools/gameplay/pod_sphere_constraint_oracle49l.h"
#include <cfenv>
#include "../../evidence/authoredGeometry49O/initialization/preanimation_fixture49o.h"
#include <cstdio>
#include <limits>
#include <stdexcept>
#include <string>
#include <fstream>
#include <cstdlib>
#include <new>
using namespace rr::game;
using Input=AuthoredGeometryInputs49O;using Binding=AuthoredGeometryBinding49O;
namespace detail {static float fp(unsigned u){float f;std::memcpy(&f,&u,4);return f;}static unsigned bits(float f){unsigned u;std::memcpy(&u,&f,4);return u;}}
using O=initialization49o_oracle::Machine;
using S=sphere49l_oracle::Machine;
using U=std::uint32_t;
static std::uint64_t allocations=0;
void* operator new(std::size_t n){++allocations;if(void*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void*p)noexcept{std::free(p);}void operator delete[](void*p)noexcept{std::free(p);}
void operator delete(void*p,std::size_t)noexcept{std::free(p);}void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
static std::array<std::uint64_t,initialization49o_words::total_words> hits{},taken{},notTaken{};
static unsigned checks=0,cases=0,reciprocalDiff=0;static std::uint64_t instructions=0;
static void require(bool b,const char*m){++checks;if(!b)throw std::runtime_error(m);}
static void word(float a,U w,const char*m){require(detail::bits(a)==w,m);}
static void same(const void*a,const void*b,unsigned n,const char*m){require(!std::memcmp(a,b,n),m);}
static Binding original(Input in){
 const auto source=initialization49o_fixture::execute(in);
 for(auto limit:source.squaredLimits)require(limit==0x501502f9u,"limits source");
 instructions+=source.instructions;for(unsigned i=0;i<hits.size();++i){hits[i]+=source.hits[i];taken[i]+=source.taken[i];notTaken[i]+=source.notTaken[i];}return source.binding;
}

static void jointCase(const Binding&b){
 PodSphereJoint49L actual{};PodSphereBody49L main{};require(setupAuthoredPodSphere49O(b,main,actual,actual),"setup accepted");
 S o;o.constructor();o.store(S::bodyA+0x270,b.inverseMass270);for(unsigned i=0;i<16;++i)o.store(S::bodyA+0x140+4*i,b.inverseInertia140.v[i]);
 for(unsigned i=0;i<4;++i)o.store(S::input+4*i,b.secondaryAnchor300.v[i]);o.call(0x1b64d0,S::joint,S::bodyA,S::input);
 for(unsigned i=0;i<4;++i)o.store(S::input+4*i,b.mainAnchor.v[i]);o.f[12]=b.unclampedLength858;o.call(0x1b6670,S::joint,S::bodyB,S::input);
 word(actual.stiffness30,o.read(S::joint+0x30),"stiffness");word(actual.damping34,o.read(S::joint+0x34),"damping");require(actual.enabled38==o.read(S::joint+0x38)&&actual.unilateral3C==o.read(S::joint+0x3c)&&actual.suppressed40==o.read(S::joint+0x40),"flags");require(actual.bodyAPresent44&&actual.bodyBPresent48,"presence");
 constexpr unsigned count=(sizeof(PodSphereJoint49L)-offsetof(PodSphereJoint49L,length4C))/4;std::array<float,count> tail{};std::memcpy(tail.data(),&actual.length4C,4*count);
 for(unsigned i=0;i<count;++i)word(tail[i],o.read(S::joint+(i<3?0x4c+4*i:0x60+4*(i-3))),"joint all numeric lanes");instructions+=o.steps;
}
int main(int argc,char**argv){try{
 std::fesetround(FE_TONEAREST);
 Input pod{detail::fp(0x40d082c6),{{detail::fp(preanimation49o_fixture::chariotLocalXYZ[0]),detail::fp(preanimation49o_fixture::chariotLocalXYZ[1]),detail::fp(preanimation49o_fixture::chariotLocalXYZ[2])}},detail::fp(preanimation49o_fixture::rightEngineWorldZ)};
 Binding p{};require(bindAuthoredGeometry49O(pod,p),"pod bind");const auto po=original(pod);same(&p,&po,sizeof p,"pod original all words");jointCase(p);word(p.length854,0x41a64687,"rest854");word(p.unclampedLength858,0x418e4687,"rest858");
 const auto noAlloc=allocations;Binding pb{};require(bindAuthoredGeometry49O(pod,pb),"hot bind");CoupledBodyParameters49M pp{};SecondaryInputs49M psi{};require(projectAuthoredGeometry49O(pb,pp,psi,pp,psi),"hot project");PodSphereJoint49L pj{};PodSphereBody49L main{};require(setupAuthoredPodSphere49O(pb,main,pj,pj),"hot joint");require(allocations==noAlloc,"fixed-storage allocation-free");
 std::printf("{\"pod01_with_explicit_bind_only_candidate_radius_not_live_claim\":{\"radius\":\"%08x\",\"mass\":\"%08x\",\"inverseMass\":\"%08x\",\"inertia\":\"%08x\",\"inverseInertia\":\"%08x\",\"rest854\":\"%08x\",\"raw858\":\"%08x\",\"inverse_all_lanes\":[",detail::bits(p.radius198),detail::bits(p.mass26C),detail::bits(p.inverseMass270),detail::bits(p.inertia100.v[0]),detail::bits(p.inverseInertia140.v[0]),detail::bits(p.length854),detail::bits(p.unclampedLength858));for(unsigned i=0;i<16;++i)std::printf("%s\"%08x\"",i?",":"",detail::bits(p.inverseInertia140.v[i]));std::printf("]},");
 for(unsigned i=0;i<512;++i){Input in=pod;in.postMotionRadius198=i==0?0.f:float(i)*.03125f;in.preAnimationChariotNullCgLocal={{float(int(i%13)-6)*.125f,float(int(i%7)-3)*.125f,-float(i%19)}};in.preAnimationRightEngineNullCgWorldZ=float(i%23)*.25f;Binding b{};require(bindAuthoredGeometry49O(in,b),"case bind");const auto expected=original(in);same(&b,&expected,sizeof b,"numeric source exact");jointCase(b);if(b.inertia100.v[0]>0){volatile float naive=1.f/b.inertia100.v[0];if(detail::bits(naive)!=detail::bits(b.inverseInertia140.v[0]))++reciprocalDiff;}++cases;}
 CoupledBodyParameters49M cp{};SecondaryInputs49M si{};cp.main.body.mass=71;cp.secondary.centerOffset={{3,4,5}};cp.secondary.centerOffsetW=.25f;cp.secondary.body.translationEnabled[1]=true;cp.secondary.body.rotationEnabled[2]=true;si.gravity378=9.75f;si.effective4346D0=72;
 const auto prev=cp;const auto prior=si;require(projectAuthoredGeometry49O(p,cp,si,cp,si),"projection alias");same(&cp.main,&prev.main,sizeof cp.main,"main preserved");same(&cp.secondary.centerOffset,&prev.secondary.centerOffset,sizeof cp.secondary.centerOffset,"center preserved");word(cp.secondary.centerOffsetW,detail::bits(prev.secondary.centerOffsetW),"center W preserved");require(cp.secondary.body.translationEnabled[1]&&cp.secondary.body.rotationEnabled[2],"axes preserved");word(si.gravity378,detail::bits(prior.gravity378),"gravity preserved");word(si.effective4346D0,detail::bits(prior.effective4346D0),"context preserved");for(unsigned i=0;i<7;++i)word(cp.secondary.inverseInertiaExtra[i],0,"full inverse extras zero");
 Binding guard=p;for(float bad:{-1.f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN(),1e30f}){auto in=pod;in.postMotionRadius198=bad;require(!bindAuthoredGeometry49O(in,guard),"bad rejected");same(&guard,&p,sizeof p,"false atomic");}
 std::fesetround(FE_UPWARD);require(!bindAuthoredGeometry49O(pod,guard),"round mode rejected");std::fesetround(FE_TONEAREST);same(&guard,&p,sizeof p,"round failure atomic");

 // Independent original-word setup/projection bridge, used by connected49O.
 const auto bridge=initialization49o_fixture::execute(pod);auto bridgeP=prev;auto bridgeI=prior;initialization49o_fixture::project(bridge,prev,prior,bridgeP,bridgeI);same(&bridgeP,&cp,sizeof cp,"independent projected parameters");same(&bridgeI,&si,sizeof si,"independent projected context");
 std::uint64_t setupInstructions=0;const auto bridgeJoint=initialization49o_fixture::setupJoint(bridge,&setupInstructions);PodSphereJoint49L nativeBridgeJoint{};PodSphereBody49L bridgeMain{};require(setupAuthoredPodSphere49O(p,bridgeMain,nativeBridgeJoint,nativeBridgeJoint),"bridge native setup");same(&bridgeJoint,&nativeBridgeJoint,sizeof bridgeJoint,"independent complete joint");require(setupInstructions>0,"original setup count");instructions+=bridge.instructions+setupInstructions;for(unsigned i=0;i<hits.size();++i){hits[i]+=bridge.hits[i];taken[i]+=bridge.taken[i];notTaken[i]+=bridge.notTaken[i];}
 // API admission/preservation: canonical inverse140, never stale inertia100.
 auto dead=p;for(float&x:dead.inertia100.v)x=std::numeric_limits<float>::quiet_NaN();dead.radius198=std::numeric_limits<float>::quiet_NaN();dead.length854=std::numeric_limits<float>::quiet_NaN();
 auto projected=prev;auto projectedInputs=prior;require(projectAuthoredGeometry49O(dead,prev,prior,projected,projectedInputs),"unused geometry fields ignored by projection");same(&projected,&cp,sizeof cp,"canonical projection");same(&projectedInputs,&si,sizeof si,"canonical input projection");
 PodSphereBody49L mainForSetup{};PodSphereJoint49L goodJoint{};require(setupAuthoredPodSphere49O(p,mainForSetup,goodJoint,goodJoint),"alias setup");auto sameJoint=goodJoint;require(setupAuthoredPodSphere49O(dead,mainForSetup,sameJoint,sameJoint),"unused geometry fields ignored by setup");same(&sameJoint,&goodJoint,sizeof goodJoint,"canonical cache");
 for(unsigned lane=0;lane<16;++lane){auto bad=p;bad.inverseInertia140.v[lane]=std::numeric_limits<float>::infinity();auto oldP=cp;auto oldI=si;auto oldJ=goodJoint;
  require(!projectAuthoredGeometry49O(bad,cp,si,oldP,oldI),"nonfinite inverse projection rejected");same(&oldP,&cp,sizeof cp,"projection parameter failure atomic");same(&oldI,&si,sizeof si,"projection context failure atomic");
  require(!setupAuthoredPodSphere49O(bad,mainForSetup,goodJoint,oldJ),"nonfinite inverse setup rejected");same(&oldJ,&goodJoint,sizeof goodJoint,"setup failure atomic");
 }
 for(unsigned axis=0;axis<4;++axis){auto bad=pod;if(axis<3)bad.preAnimationChariotNullCgLocal.v[axis]=std::numeric_limits<float>::quiet_NaN();else bad.preAnimationRightEngineNullCgWorldZ=std::numeric_limits<float>::infinity();auto old=p;require(!bindAuthoredGeometry49O(bad,old),"nonfinite nullcg rejected");same(&old,&p,sizeof p,"nullcg failure atomic");
  auto badBinding=p;badBinding.targetRelative2F0.v[axis]=std::numeric_limits<float>::quiet_NaN();auto oldP=cp;auto oldI=si;require(!projectAuthoredGeometry49O(badBinding,oldP,oldI,oldP,oldI),"target lane rejected");same(&oldP,&cp,sizeof cp,"target parameter atomic");same(&oldI,&si,sizeof si,"target context atomic");
  for(unsigned anchor=0;anchor<2;++anchor){badBinding=p;(anchor?badBinding.mainAnchor:badBinding.secondaryAnchor300).v[axis]=std::numeric_limits<float>::infinity();auto oldJ=goodJoint;require(!setupAuthoredPodSphere49O(badBinding,mainForSetup,oldJ,oldJ),"anchor lane rejected");same(&oldJ,&goodJoint,sizeof goodJoint,"anchor failure atomic");}
 }
 for(int mode:{FE_UPWARD,FE_DOWNWARD,FE_TOWARDZERO}){std::fesetround(mode);auto oldP=cp;auto oldI=si;auto oldJ=goodJoint;auto old=p;require(!bindAuthoredGeometry49O(pod,old),"all wrong rounding bind rejected");require(!projectAuthoredGeometry49O(p,oldP,oldI,oldP,oldI),"all wrong rounding projection rejected");require(!setupAuthoredPodSphere49O(p,mainForSetup,oldJ,oldJ),"all wrong rounding setup rejected");std::fesetround(FE_TONEAREST);same(&old,&p,sizeof p,"rounding binding atomic");same(&oldP,&cp,sizeof cp,"rounding parameters atomic");same(&oldI,&si,sizeof si,"rounding context atomic");same(&oldJ,&goodJoint,sizeof goodJoint,"rounding joint atomic");}
 for(float radius:{-0.0f,1e-15f,1e-10f,1e-5f,1e-2f,1000.f}){auto in=pod;in.postMotionRadius198=radius;Binding value{};require(bindAuthoredGeometry49O(in,value),"finite degenerate or large radius");auto expected=original(in);same(&value,&expected,sizeof value,"edge radius source");jointCase(value);++cases;}
 if(argc>1){unsigned covered=0;std::ofstream coverage(argv[1]);coverage<<"{\"words\":[";bool first=true;for(const auto&block:initialization49o_words::blocks)for(unsigned i=0;i<block.count;++i){const auto ix=block.index+i;if(hits[ix])++covered;if(!first)coverage<<',';first=false;coverage<<"{\"pc\":"<<(block.start+4*i)<<",\"hits\":"<<hits[ix]<<",\"taken\":"<<taken[ix]<<",\"not_taken\":"<<notTaken[ix]<<'}';}coverage<<"],\"covered_words\":"<<covered<<",\"total_words\":"<<hits.size()<<"}\n";}
 std::printf("\"random_cases\":%u,\"checks\":%u,\"original_instructions\":%llu,\"cofactor_vs_naive_reciprocal_differences\":%u,\"status\":\"pass\"}\n",cases,checks,(unsigned long long)instructions,reciprocalDiff);return 0;
 }catch(const std::exception&e){std::fprintf(stderr,"%s\n",e.what());return 1;}}
