#include "../../src/game/graph_body49w.h"
#include "../../evidence/frameBridge49W/body/source/body_fixture.h"
#include <cfenv>
#include <algorithm>
#include <cstdlib>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <random>
#include <string>
using namespace rr::game;
using namespace rr::game::detail;
namespace original=graph49w_original;
using U=std::uint32_t;using I=SceneNodeIndex49P;using M=BodyMatrix48G;
using original::bits;using original::fp;
constexpr U B=0x5000, VT=0x7000, NB=original::Machine::nodeBase, NS=original::Machine::nodeStride;
constexpr I none=I::none;constexpr U N=8;
static bool nativeActive=false;static std::uint64_t allocations=0,cases=0,checks=0,rejections=0,leafCases=0,stageComparisons=0,flagComparisons=0;
void* operator new(std::size_t n){if(nativeActive)++allocations;if(void*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}[[gnu::noinline]]void release(void*p)noexcept{std::free(p);}
void operator delete(void*p)noexcept{release(p);}void operator delete[](void*p)noexcept{release(p);}void operator delete(void*p,std::size_t)noexcept{release(p);}void operator delete[](void*p,std::size_t)noexcept{release(p);}
static void need(bool b,const char*s){++checks;if(!b)throw std::runtime_error(std::string("mismatch ")+s+" case="+std::to_string(cases));}
static void same(U a,U b,const char*s){++checks;if(a!=b){char t[240];std::snprintf(t,sizeof t,"mismatch %s actual=%08x expected=%08x case=%llu",s,a,b,(unsigned long long)cases);throw std::runtime_error(t);}}
static void bytes(const void*a,const void*b,std::size_t n,const char*s){for(std::size_t i=0;i<n;i+=4){U x=0,y=0;std::memcpy(&x,(const char*)a+i,std::min(std::size_t(4),n-i));std::memcpy(&y,(const char*)b+i,std::min(std::size_t(4),n-i));if(x!=y){char t[200];std::snprintf(t,sizeof t,"%s word=%zu",s,i/4);same(x,y,t);}else ++checks;}}
struct Flag{U index;unsigned char before,after;};static std::array<Flag,1024> nativeFlags{};static U nativeFlagCount=0;
void rrGraphBody49WFlag(I i,unsigned char a,unsigned char b)noexcept{if(nativeFlagCount==nativeFlags.size())std::abort();nativeFlags[nativeFlagCount++]={U(i),a,b};}
struct Stage{U pc;RootBodyState48H body;SecondaryActivationSupplement49V supplement;GraphBodyExtraLanes49W extra;std::array<SceneNode49P,N> nodes;};
static std::array<Stage,8> nativeStages{};static U nativeStageCount=0;
void rrGraphBody49WStage(U pc,const RootBodyState48H&b,const SecondaryActivationSupplement49V&s,const GraphBodyExtraLanes49W&e,const SceneGraph49P&g)noexcept{
 if(nativeStageCount==nativeStages.size()||g.count!=N)std::abort();auto&o=nativeStages[nativeStageCount++];o.pc=pc;o.body=b;o.supplement=s;o.extra=e;std::memcpy(o.nodes.data(),g.nodes,N*sizeof(SceneNode49P));}
struct Fixture{
 float cachedSpeed34;Vector3_48C cachedAnglesE0;U body20C;
 RootBodyState48H body;SecondaryActivationSupplement49V supplement;GraphBodyExtraLanes49W extra;
 float velocityW,angularW,forceW,torqueW;U flags;BodyParameters48C parameters;Vector3_48C center;
 GraphBodyDomain49W domain;std::array<SceneNode49P,N> nodes;I binding;bool enabled;float dt;
};
static U address(I i){return i==none?0:NB+U(i)*NS;}
static M pose(float shift){M m{{1,0,0,0,0,1,0,0,0,0,1,0,shift,shift*.5f,-shift*.25f,1}};return m;}
static Fixture fixture(U seed){Fixture f{};f.cachedSpeed34=23.5f;f.cachedAnglesE0={{.3f,-.7f,1.2f}};f.body20C=0x753124e9;
 f.body.body.position={{121,122,123}};f.body.body.velocity={{2.5f,-1.25f,.75f}};f.body.body.angularVelocity={{.125f,-.0625f,.25f}};
 f.body.body.orientation={{.2f,.3f,.4f,.5f}};f.body.body.previousPositionDerivative={{.75f,-.5f,.125f}};f.body.body.previousLinearAcceleration={{.0625f,-.125f,.25f}};
 f.body.body.previousAngularAcceleration={{-.015625f,.03125f,.0625f}};f.body.body.previousOrientationDerivative={{.03125f,-.0625f,.09375f,-.125f}};
 f.body.body.linearAcceleration={{91,92,93}};f.body.body.angularAcceleration={{94,95,96}};f.body.body.orientationDerivative={{97,98,99,100}};
 f.body.previousVelocity90={{21,22,23}};f.body.localVelocity80={{31,32,33}};f.body.worldAngularVelocityA0={{41,42,43}};
 f.body.loads={{{.75f,-.5f,.25f}},{{.03125f,.015625f,-.0625f}}};f.body.positionCorrection1a0={{.125f,-.25f,.0625f}};
 f.body.rotationCorrection1b0={{seed%3?.125f:0,seed%3?-.0625f:0,seed%3?.03125f:0}};
 f.body.accumulator1c0={{71,72,73}};f.body.accumulator1d0={{81,82,83}};f.body.callbackState238=0x71234567;
 f.supplement={-11,-12,-13,-14,{{-15,-16,-17}},{{-18,-19,-20}},{{-21,-22,-23}}};f.extra={17,18,19,20,21,22};
 f.velocityW=-3.5f;f.angularW=4.25f;f.forceW=-5.75f;f.torqueW=6.5f;
 f.parameters={1,1,{{{1,.125f,0}},{{0,1,-.0625f}},{{.03125f,0,1}}},1e10f,1e10f,1e10f,1e10f,{true,true,true},{true,true,true}};
 f.center={{.125f,-.25f,.375f}};f.domain={seed&2,false,false,GraphBodyIntegrator49W::OriginalAdamsBashforth2,GraphBodyCallback49W::Clear238};f.binding=I(5);f.enabled=true;f.dt=seed%4?.015625f:0;
 for(U i=0;i<N;++i){auto&n=f.nodes[i];n.localC0=pose(float(i+1));n.world100=pose(float(15+i));n.parent180=n.firstChild184=n.nextSibling188=none;n.sceneFlags190=static_cast<unsigned char>(0x54|(i+seed)%2);}
 // Physical190 lives at index5; graphics194 at2. Siblings are2 ->0 ->6,
 // and the ready grandchild at7 is below potentially dirty graphics194.
 f.nodes[5].firstChild184=I(2);f.nodes[2].parent180=I(5);f.nodes[2].firstChild184=I(7);f.nodes[2].nextSibling188=I(0);
 f.nodes[7].parent180=I(2);f.nodes[7].sceneFlags190|=1;f.nodes[0].parent180=I(5);f.nodes[0].nextSibling188=I(6);f.nodes[6].parent180=I(5);
 f.nodes[0].firstChild184=I(3);f.nodes[3].parent180=I(0);f.nodes[4].firstChild184=I(1);f.nodes[1].parent180=I(4);
 f.nodes[5].sceneFlags190=static_cast<unsigned char>(0x36|(seed&1));f.nodes[2].sceneFlags190=static_cast<unsigned char>(0x64|((seed>>1)&1));
 f.flags=0xc35a7e00u|f.nodes[5].sceneFlags190;f.body.scene=f.nodes[5].world100;return f;}
struct Field{U source;std::size_t native;U count;};
#define BF(member,offset,count) {offset,offsetof(RootBodyState48H,body)+offsetof(BodyState48C,member),count}
#define RF(member,offset,count) {offset,offsetof(RootBodyState48H,member),count}
static constexpr Field fields[]={BF(position,0x60,3),BF(velocity,0x70,3),BF(angularVelocity,0xb0,3),BF(orientation,0x1f0,4),BF(previousPositionDerivative,0x2c4,3),BF(previousLinearAcceleration,0x2dc,3),BF(previousAngularAcceleration,0x314,3),BF(previousOrientationDerivative,0x2f8,4),BF(linearAcceleration,0xc0,3),BF(angularAcceleration,0xd0,3),BF(orientationDerivative,0x2e8,4),RF(previousVelocity90,0x90,3),RF(localVelocity80,0x80,3),RF(worldAngularVelocityA0,0xa0,3),RF(loads,0x180,3),{0x190,offsetof(RootBodyState48H,loads)+sizeof(Vector3_48C),3},RF(positionCorrection1a0,0x1a0,3),RF(rotationCorrection1b0,0x1b0,3),RF(accumulator1c0,0x1c0,3),RF(accumulator1d0,0x1d0,3),RF(callbackState238,0x238,1)};
static constexpr U so[]={0x8c,0x9c,0xcc,0xdc,0x2b8,0x2bc,0x2c0,0x2d0,0x2d4,0x2d8,0x308,0x30c,0x310};
static constexpr U eo[]={0x6c,0xac,0x1ac,0x1bc,0x1cc,0x1dc};
static void putWords(original::Machine&m,U at,const void*p,U n){for(U i=0;i<n;++i){U w;std::memcpy(&w,(const char*)p+4*i,4);m.init(at+4*i,w);}}
static void getWords(original::Machine&m,U at,void*p,U n){for(U i=0;i<n;++i){U w=m.read(at+4*i);std::memcpy((char*)p+4*i,&w,4);}}
static void put(original::Machine&m,const Fixture&f,U seed){
 for(U i=1;i<32;++i)for(U j=0;j<4;++j)m.v[i][j]=float(int(i*4+j+seed%9)-51)/8;m.v[0]={0,0,0,1};
 graph49w_fixture::putConstants(m,seed);const SceneGraph49P g{const_cast<SceneNode49P*>(f.nodes.data()),N};graph49w_fixture::putGraph(m,g);
 graph49w_fixture::putBody(m,B,VT,f.body,f.supplement,f.extra,f.velocityW,f.angularW,f.forceW,f.torqueW,f.parameters,f.center,f.binding,f.enabled,f.domain);
 m.store(B+0x34,f.cachedSpeed34);putWords(m,B+0xe0,&f.cachedAnglesE0,3);m.init(B+0x20c,f.body20C);
 for(U i=0;i<N;++i)if(I(i)!=f.binding)m.init(NB+i*NS+0x19c,0x12340000+i);
 m.stores.clear();m.trace.clear();m.reads.clear();m.calls.clear();}
static Stage get(original::Machine&m,const Fixture&seed,U pc){Stage s{};s.pc=pc;s.body=seed.body;
 for(const auto&x:fields)getWords(m,B+x.source,(char*)&s.body+x.native,x.count);
 for(U i=0;i<13;++i)getWords(m,B+so[i],(char*)&s.supplement+4*i,1);
 for(U i=0;i<6;++i)getWords(m,B+eo[i],(char*)&s.extra+4*i,1);
 s.nodes=seed.nodes;for(U i=0;i<N;++i){const U a=NB+i*NS;getWords(m,a+0xc0,&s.nodes[i].localC0,16);getWords(m,a+0x100,&s.nodes[i].world100,16);s.nodes[i].sceneFlags190=static_cast<unsigned char>(m.read(a+0x190));}
 if(seed.binding!=none)s.body.scene=s.nodes[U(seed.binding)].world100;return s;}
static void compareStage(const Stage&a,const Stage&b){same(a.pc,b.pc,"stage PC");bytes(&a.body,&b.body,sizeof a.body,"all root body fields");bytes(&a.supplement,&b.supplement,sizeof a.supplement,"all52 supplement bytes");bytes(&a.extra,&b.extra,sizeof a.extra,"six full lanes");
 for(U i=0;i<N;++i){bytes(&a.nodes[i].localC0,&b.nodes[i].localC0,sizeof(M),"local16");bytes(&a.nodes[i].world100,&b.nodes[i].world100,sizeof(M),"cached world16");same(U(a.nodes[i].parent180),U(b.nodes[i].parent180),"parent link");same(U(a.nodes[i].firstChild184),U(b.nodes[i].firstChild184),"child link");same(U(a.nodes[i].nextSibling188),U(b.nodes[i].nextSibling188),"sibling link");same(a.nodes[i].sceneFlags190,b.nodes[i].sceneFlags190,"flags");}}
static bool runNative(Fixture&f,GraphBodyWorkspace49W&w,GraphBodyDiagnostic49W&d){const SceneGraph49P g{f.nodes.data(),N};nativeFlagCount=nativeStageCount=0;nativeActive=true;const bool ok=advanceGraphBody49W(f.body,f.supplement,f.extra,{f.velocityW,f.angularW,f.forceW,f.torqueW},f.flags,f.parameters,f.center,f.binding,f.enabled,f.domain,f.dt,g,w,d);nativeActive=false;return ok;}
struct Proof{std::unique_ptr<original::Machine> machine=std::make_unique<original::Machine>();U mutationPc=0,mutationWord=0;std::uint64_t instructions=0,rotationCalls=0,scalarZeroSkips=0;std::array<std::uint64_t,7> gates{};
 Fixture compare(Fixture f,bool seedMachine=true){++cases;auto&m=*machine;if(seedMachine)put(m,f,U(cases));if(mutationPc)m.substitutions[mutationPc]=mutationWord;
 m.put64(4,B);m.f[12]=f.dt;m.put64(31,original::Machine::stop);m.put64(29,original::Machine::stack);m.callSteps=0;
 const U gate[]={0x167ca0,0x168128,0x168234,0x168244,0x1682c4,0x1682fc,0x168354};std::array<Stage,8> stages{};U count=0;
 const auto oldSteps=m.steps;const auto oldRotate=m.calls[0x28b840];const auto oldScalarZero=m.taken[m.index(0x1681f0)];U pc=0x167c40;
 while(pc!=original::Machine::stop){for(U i=0;i<7;++i)if(pc==gate[i]&&(i!=5||(f.enabled&&f.binding!=none))){need(count<stages.size(),"stage capacity");stages[count++]=get(m,f,pc);++gates[i];}
 U next=0,unused=0;if(m.execute(pc,next)){const bool delay=m.delay;if(delay&&m.execute(pc+4,unused))throw std::runtime_error("branch in delay slot");pc=next;}else pc+=4;}
 instructions+=m.steps-oldSteps;rotationCalls+=m.calls[0x28b840]-oldRotate;scalarZeroSkips+=m.taken[m.index(0x1681f0)]-oldScalarZero;
 same(m.g[2][0],1,"source realreturn v0");same(m.g[29][0],original::Machine::stack,"source realreturn SP");
 const Fixture before=f;GraphBodyWorkspace49W w{};GraphBodyDiagnostic49W d{};need(runNative(f,w,d),"valid graph body rejected");same(U(d),0,"success diagnostic");
 const Stage expected=get(m,before,0x168354);
 RootBodyState48H marshalled=before.body;SecondaryActivationSupplement49V ms{};GraphBodyExtraLanes49W me{};float vw,aw,fw,tw;graph49w_fixture::readBody(m,B,marshalled,ms,me,vw,aw,fw,tw,before.binding);bytes(&marshalled,&expected.body,sizeof marshalled,"helper body marshalling");bytes(&ms,&expected.supplement,sizeof ms,"helper supplement marshalling");bytes(&me,&expected.extra,sizeof me,"helper extra marshalling");
const Stage actual{0x168354,f.body,f.supplement,f.extra,f.nodes};compareStage(actual,expected);
 for(U offset:{0x34u,0xe0u,0xe4u,0xe8u,0x20cu}){U x=offset==0x34?bits(before.cachedSpeed34):offset==0x20c?before.body20C:bits(before.cachedAnglesE0.v[(offset-0xe0)/4]);same(m.read(B+offset),x,"original retained caches20C");}
 bytes(&f.cachedSpeed34,&before.cachedSpeed34,sizeof f.cachedSpeed34,"native speed cache");bytes(&f.cachedAnglesE0,&before.cachedAnglesE0,sizeof f.cachedAnglesE0,"native angle cache");same(f.body20C,before.body20C,"native20C unchanged");
 same(bits(f.velocityW),m.read(B+0x7c),"velocity70W");same(bits(f.angularW),m.read(B+0xbc),"angularB0W");same(bits(f.forceW),m.read(B+0x18c),"force180W");same(bits(f.torqueW),m.read(B+0x19c),"torque190W");same(f.flags,(before.flags&0xffffff00u)|(before.binding==none?before.flags&255:m.read(address(before.binding)+0x190)&255),"projection high24 retained");
#ifdef RR_GRAPH_BODY49W_TRACED
 same(nativeStageCount,count,"stage count");for(U i=0;i<count;++i){compareStage(nativeStages[i],stages[i]);++stageComparisons;}
 U flag=0;for(const auto&s:m.stores)if(s.address>=NB&&s.address<NB+N*NS&&(s.address-NB)%NS==0x190){need(flag<nativeFlagCount,"missing native flag store");same(nativeFlags[flag].index,(s.address-NB)/NS,"ordered flag node");same(nativeFlags[flag].before,s.before&255,"ordered flag before");same(nativeFlags[flag].after,s.after&255,"ordered flag after");same(s.after&0xffffff00u,0x731a9500,"original flag high24 retained");++flag;++flagComparisons;}same(flag,nativeFlagCount,"flag store count");
#endif
 return f;}
 void coverage(const char*path){if(!path)return;std::ofstream out(path);for(const auto&b:graph49w_words::blocks)for(U i=0;i<b.count;++i){U ix=b.index+i;out<<std::hex<<b.start+4*i<<std::dec<<' '<<machine->hits[ix]<<' '<<machine->taken[ix]<<' '<<machine->notTaken[ix]<<'\n';}if(!out)throw std::runtime_error("coverage output");}
};
static void accepted(Proof&p){for(U mask=0;mask<8;++mask)for(float dt:{0.f,.015625f}){auto f=fixture(mask);f.dt=dt;p.compare(f);f.enabled=false;p.compare(f);f.binding=none;p.compare(f);}

 for(bool attached:{false,true}){auto f=fixture(3);f.enabled=false;if(!attached)f.binding=none;f.dt=fp(0x7fc01234);f.parameters.mass=fp(0x7fc02222);f.domain.rootObserverPresent19C=true;f.domain.integrationMethod290=GraphBodyIntegrator49W::Unsupported;f.body.body.velocity.v[0]=fp(0x7fc05555);f.supplement.localVelocity80W=fp(0x7fc04321);f.supplement.positionDerivative2B8.v[2]=fp(0xff800000);f.extra.position60W=fp(0x7fc02468);f.body.rotationCorrection1b0.v[0]=fp(0x7fc03579);p.compare(f);}
 {auto f=fixture(3);for(U i=0;i<N;++i){if(i!=5)for(float&value:f.nodes[i].localC0.v)value=fp(0x7fc0babe);if(i!=5)for(float&value:f.nodes[i].world100.v)value=fp(0xff800000);}for(U j:{3u,7u,11u,15u})f.nodes[5].world100.v[j]=fp(0x7fc01234+j);f.body.scene=f.nodes[5].world100;p.compare(f);}
 {auto f=fixture(3);f.velocityW=fp(0x7fc01111);f.angularW=fp(0xff800000);f.forceW=fp(0x7fc02222);f.torqueW=fp(0x7fc03333);p.compare(f);}
 for(float correction:{-4.f,-2.f,-.5f,0.f,.5f,2.f,4.f,fp(1),fp(0x80000001),fp(0x10000000),fp(0x1c000000)}){auto f=fixture(1);f.body.rotationCorrection1b0={{correction,0,0}};p.compare(f);}

 for(U branch=0;branch<4;++branch)for(U dirty=0;dirty<2;++dirty){auto f=fixture(dirty);M basis=pose(3);if(branch<3){basis.v[0]=basis.v[5]=basis.v[10]=-1;basis.v[5*branch]=1;}else{basis={{0,1,0,0,0,0,1,0,1,0,0,0,3,4,5,1}};}if(dirty)f.nodes[5].world100=basis;else f.nodes[5].localC0=basis;f.body.scene=f.nodes[5].world100;p.compare(f);}
 std::mt19937 rng(0x49b0d1u);auto random=[&](float d){return float(int(rng()%4097)-2048)/d;};
 for(U n=0;n<384;++n){auto f=fixture(n);for(U j=0;j<3;++j){f.body.body.velocity.v[j]=random(256);f.body.body.angularVelocity.v[j]=random(4096);f.body.loads.force.v[j]=random(512);f.body.loads.torque.v[j]=random(8192);f.body.positionCorrection1a0.v[j]=random(8192);f.body.rotationCorrection1b0.v[j]=n%3?random(16384):0;f.center.v[j]=random(1024);f.parameters.translationEnabled[j]=rng()%3;f.parameters.rotationEnabled[j]=rng()%3;}
 if(n%7==0){f.parameters.linearAccelerationLimitSquared=.125f;f.parameters.angularAccelerationLimitSquared=.015625f;f.parameters.linearVelocityLimitSquared=1;f.parameters.angularVelocityLimitSquared=.125f;}
 if(n%5==0)f.parameters.mass=0;if(n%11==0)f.parameters.mass=-1;p.compare(f);}
 // Persistent independent oracle: original registers, all histories, graph and
 // cached matrices survive; only externally refreshed loads are reseeded.
 auto f=fixture(3);for(U n=0;n<96;++n){f.body.loads={{{.125f,-.0625f,.03125f}},{{.001f,.002f,-.003f}}};if(n){putWords(*p.machine,B+0x180,&f.body.loads.force,3);putWords(*p.machine,B+0x190,&f.body.loads.torque,3);p.machine->stores.clear();}f=p.compare(f,n==0);}
}

static void compareLeafGraph(original::Machine&m,const Fixture&f,const std::array<SceneNode49P,N>&before){
 for(U i=0;i<N;++i){M local{},world{};getWords(m,NB+i*NS+0xc0,&local,16);getWords(m,NB+i*NS+0x100,&world,16);bytes(&f.nodes[i].localC0,&local,sizeof(M),"leaf local");bytes(&f.nodes[i].world100,&world,sizeof(M),"leaf world");same(f.nodes[i].sceneFlags190,m.read(NB+i*NS+0x190)&255,"leaf flags");same(U(f.nodes[i].parent180),U(before[i].parent180),"leaf parent unchanged");same(U(f.nodes[i].firstChild184),U(before[i].firstChild184),"leaf child unchanged");same(U(f.nodes[i].nextSibling188),U(before[i].nextSibling188),"leaf sibling unchanged");}
#ifdef RR_GRAPH_BODY49W_TRACED
 U flag=0;for(const auto&s:m.stores)if(s.address>=NB&&s.address<NB+N*NS&&(s.address-NB)%NS==0x190){need(flag<nativeFlagCount,"leaf missing flag");same(nativeFlags[flag].index,(s.address-NB)/NS,"leaf orderedflag node");same(nativeFlags[flag].before,s.before&255,"leaf orderedflag before");same(nativeFlags[flag].after,s.after&255,"leaf orderedflag after");++flag;++flagComparisons;}same(flag,nativeFlagCount,"leaf flag count");
#endif
}
static void leaves(Proof&p){auto&m=*p.machine;std::mt19937 rng(0x49babeu);auto random=[&](){return float(int(rng()%2049)-1024)/128;};
 for(U n=0;n<192;++n){auto f=fixture(n);I target=I(n%N);for(U i=0;i<N;++i)for(U j=0;j<16;++j){f.nodes[i].localC0.v[j]=random();f.nodes[i].world100.v[j]=random();}
  const auto before=f.nodes;put(m,f,n);VisualVector49R in{{random(),random(),random(),fp(0x7fc01234)}},out{{91,92,93,94}};SceneVectorContext49U context{fp(0x7fc04321)};
  putWords(m,0x8000,&out,4);putWords(m,0x8040,&in,4);m.v[9][3]=fp(0x7fc04321);m.put64(4,0x8000);m.put64(5,address(target));m.put64(6,0x8040);m.run(0x28bab0);
  const SceneGraph49P graph{f.nodes.data(),N};nativeFlagCount=0;nativeActive=true;bool ok=pointGraph49W(graph,target,in,context,out);nativeActive=false;need(ok,"point leaf rejected");VisualVector49R expected{};getWords(m,0x8000,&expected,4);bytes(&out,&expected,sizeof out,"point full output");same(bits(context.vf9W),bits(m.v[9][3]),"point scratch W");compareLeafGraph(m,f,before);++leafCases;
 }
 for(U n=0;n<192;++n){auto f=fixture(n);I target=I(n%N);for(U i=0;i<N;++i)for(U j=0;j<16;++j){f.nodes[i].localC0.v[j]=random();f.nodes[i].world100.v[j]=random();}
  if(n%11==0)for(U lane=12;lane<16;++lane)f.nodes[U(target)].localC0.v[lane]=fp(0x80000000);
  const auto before=f.nodes;put(m,f,n);VisualVector49R axis{{n%13?random():0,n%13?random():0,n%13?random():0,fp(0x7fc01234)}};
  float angle=random();putWords(m,0x8040,&axis,4);m.put64(4,address(target));m.put64(5,0x8040);m.f[12]=angle;m.run(0x28b840);
  const SceneGraph49P graph{f.nodes.data(),N};nativeFlagCount=0;nativeActive=true;bool ok=rotateAxisGraph49W(graph,target,axis,angle);nativeActive=false;need(ok,"axis leaf rejected");compareLeafGraph(m,f,before);++leafCases;
  auto cos=std::find(m.trace.begin(),m.trace.end(),0x13bba0),sin=std::find(m.trace.begin(),m.trace.end(),0x13bad0);need(cos<sin,"source cosine before sine");
 }
}

[[gnu::noinline]] static GraphBodyDiagnostic49W& diagnosticAt(void*p){return *static_cast<GraphBodyDiagnostic49W*>(p);}
static void rejected(){auto reject=[](Fixture f,GraphBodyDiagnostic49W reason){const auto before=f;GraphBodyWorkspace49W w{};GraphBodyDiagnostic49W d{};need(!runNative(f,w,d),"invalid body accepted");same(U(d),U(reason),"reject reason");bytes(&f,&before,sizeof f,"atomic rejection");++rejections;};
 using D=GraphBodyDiagnostic49W;auto f=fixture(1);f.domain.rootObserverPresent19C=0x1234;reject(f,D::Observer);f=fixture(1);f.domain.componentFlags24=2;f.domain.firstComponentChildPresent18=1;reject(f,D::ComponentRoute);f=fixture(1);f.domain.callbackMethod9C=GraphBodyCallback49W::Unsupported;reject(f,D::CallbackMethod);f=fixture(1);f.domain.integrationMethod290=GraphBodyIntegrator49W::Unsupported;reject(f,D::IntegrationMethod);f=fixture(1);f.binding=I(2);reject(f,D::SceneBinding);f=fixture(1);f.nodes[0].nextSibling188=I(2);reject(f,D::GraphTopology);f=fixture(1);f.body.scene.v[0]+=1;reject(f,D::SceneProjection);f=fixture(1);f.flags^=1;reject(f,D::SceneProjection);
 for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}){need(std::fesetround(mode)==0,"set rounding");reject(fixture(0),D::RoundingMode);}need(std::fesetround(FE_TONEAREST)==0,"restore rounding");
 for(float dt:{-1.f,fp(0x7fc01234),std::numeric_limits<float>::infinity()}){f=fixture(1);f.dt=dt;reject(f,D::Arithmetic);}f=fixture(1);f.parameters.inverseMass=std::numeric_limits<float>::max();f.body.loads.force.v[0]=std::numeric_limits<float>::max();reject(f,D::Arithmetic);f=fixture(1);f.body.rotationCorrection1b0.v[0]=fp(0x7fc01234);reject(f,D::Arithmetic);f=fixture(1);f.body.positionCorrection1a0.v[0]=std::numeric_limits<float>::max();f.nodes[5].world100.v[12]=std::numeric_limits<float>::max();f.body.scene=f.nodes[5].world100;reject(f,D::Arithmetic);
 f=fixture(1);const auto before=f;GraphBodyWorkspace49W w{};GraphBodyDiagnostic49W d{};const SceneGraph49P g{f.nodes.data(),N};nativeActive=true;bool ok=advanceGraphBody49W(f.body,f.supplement,f.extra,{f.velocityW,f.angularW,f.forceW,f.forceW},f.flags,f.parameters,f.center,f.binding,f.enabled,f.domain,f.dt,g,w,d);nativeActive=false;need(!ok,"aliased lanes accepted");same(U(d),U(D::StorageAlias),"lane alias reason");bytes(&f,&before,sizeof f,"alias atomic");++rejections;
 auto aliasCall=[&](Vector3_48C&center,GraphBodyDiagnostic49W&diagnostic,RootBodyState48H&body){nativeActive=true;bool result=advanceGraphBody49W(body,f.supplement,f.extra,{f.velocityW,f.angularW,f.forceW,f.torqueW},f.flags,f.parameters,center,f.binding,f.enabled,f.domain,f.dt,g,w,diagnostic);nativeActive=false;need(!result,"storage alias accepted");bytes(&f,&before,sizeof f,"storage alias atomic");++rejections;};
 aliasCall(f.body.body.position,d,f.body);
 aliasCall(f.center,diagnosticAt(&f.flags),f.body);
 w.body=f.body;const auto stagedBefore=w.body;aliasCall(f.center,d,w.body);bytes(&w.body,&stagedBefore,sizeof w.body,"workspace body alias atomic");

}
int main(int argc,char**argv){try{need(std::fesetround(FE_TONEAREST)==0,"rounding");Proof p;if(argc==3){p.mutationPc=U(std::stoul(argv[1],nullptr,16));p.mutationWord=U(std::stoul(argv[2],nullptr,16));}accepted(p);leaves(p);rejected();p.coverage(std::getenv("RR_GRAPH_BODY49W_COVERAGE"));need(allocations==0,"native allocation");std::cout<<"{\"passed\":true,\"cases\":"<<cases<<",\"checks\":"<<checks<<",\"rejections\":"<<rejections<<",\"leaf_cases\":"<<leafCases<<",\"stage_comparisons\":"<<stageComparisons<<",\"ordered_flag_stores\":"<<flagComparisons<<",\"original_instructions\":"<<p.instructions<<",\"rotation_calls\":"<<p.rotationCalls<<",\"scalar_zero_skips\":"<<p.scalarZeroSkips<<",\"native_allocations\":"<<allocations<<"}\n";return 0;}catch(const std::exception&e){nativeActive=false;std::fesetround(FE_TONEAREST);std::cerr<<e.what()<<'\n';return 1;}}
