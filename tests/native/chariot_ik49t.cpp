#include <string>
#include "../../src/game/chariot_ik49t.h"
#include "../../evidence/chariotAnimation49T/ik/source/ik_fixture.h"
#include <array>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <limits>
#include <cfenv>
#include <cstdlib>
#include <new>
using namespace rr::game;
namespace src = ik_original;
namespace original = ik_fixture;
using U = std::uint32_t;
using I = SceneNodeIndex49P;
static bool nativeActive = false;
static std::uint64_t allocations = 0;
void* operator new(std::size_t n){if(nativeActive)++allocations;if(void*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void releaseAllocation(void*p) noexcept{std::free(p);}
void operator delete(void*p) noexcept{releaseAllocation(p);}
void operator delete[](void*p) noexcept{releaseAllocation(p);}
void operator delete(void*p,std::size_t) noexcept{releaseAllocation(p);}
void operator delete[](void*p,std::size_t) noexcept{releaseAllocation(p);}
#ifdef RR_CHARIOT_IK49T_TRACE
struct NativeFlagEvent {U node,before,after;};
static std::array<NativeFlagEvent,8192> nativeFlags{};
static std::size_t nativeFlagCount=0;
void rrChariotIk49TFlagStore(U i,std::uint8_t a,std::uint8_t b){if(nativeFlagCount>=nativeFlags.size())throw std::runtime_error("native flag trace capacity");nativeFlags[nativeFlagCount++]={i,a,b};}
#endif
namespace {
std::uint64_t checks=0,cases=0,initializers=0,originalInstructions=0,domainCalls=0,lowerClamps=0;
U mutationPc=0,mutationWord=0;
std::array<std::uint64_t,ik_words::total_words> hits{},taken{},notTaken{};
I ix(U i){return static_cast<I>(i);}
void need(bool b,const std::string&s){++checks;if(!b)throw std::runtime_error(s);}
void same(U a,U b,const std::string&s){++checks;if(a!=b){std::ostringstream o;o<<s<<" mismatch native="<<std::hex<<a<<" source="<<b;throw std::runtime_error(o.str());}}
ChariotIkBindings49T binding(){return {{ix(10),ix(12),ix(13),ix(16)},{ix(11),ix(14),ix(15),ix(17)}};}
void clear(src::Machine&m){m.hits.fill(0);m.taken.fill(0);m.notTaken.fill(0);m.trace.clear();m.stores.clear();m.reads.clear();m.calls.clear();m.acosEvents.clear();m.steps=0;}
void account(const src::Machine&m){originalInstructions+=m.steps;for(std::size_t i=0;i<hits.size();++i){hits[i]+=m.hits[i];taken[i]+=m.taken[i];notTaken[i]+=m.notTaken[i];}for(auto&s:m.stores)if(s.address==0x385160&&s.after==33)++domainCalls;for(auto&b:ik_words::blocks)if(b.start<=0x28f88c&&0x28f88c<b.start+b.count*4)lowerClamps+=m.notTaken[b.index+(0x28f88c-b.start)/4];}
struct Native {
 std::array<SceneNode49P,64> nodes{};U count=18;ChariotIkPair49T records{};SourceLibmState49T libm{};
 ChariotIkShared49T shared{};VisualShared49R visual{};ChariotIkWorkspace49T work{};ChariotIkDiagnostic49T diagnostic{};
 SceneGraph49P graph(){return {nodes.data(),count};}
 void scene(src::Machine&m,U n=18){count=n;for(U i=0;i<n;++i){U a=original::node(i);auto&x=nodes[i];for(U j=0;j<16;++j){x.localC0.v[j]=src::fp(m.read(a+0xc0+4*j));x.world100.v[j]=src::fp(m.read(a+0x100+4*j));}auto id=[](U p){return p?ix((p-src::Machine::nodeBase)/src::Machine::nodeStride):I::none;};x.parent180=id(m.read(a+0x180));x.firstChild184=id(m.read(a+0x184));x.nextSibling188=id(m.read(a+0x188));x.sceneFlags190=static_cast<std::uint8_t>(m.read(a+0x190));}}
 bool init(const ChariotIkBindings49T&b){auto g=graph();nativeActive=true;bool r=initializeChariotIk49T(b,g,records,libm,work,diagnostic);nativeActive=false;return r;}
 bool solve(const ChariotIkBindings49T&b){auto g=graph();nativeActive=true;bool r=solveChariotHands49T(b,records,g,libm,shared,visual,work,diagnostic);nativeActive=false;return r;}
};
void compareScene(Native&n,src::Machine&m,const char*label){for(U i=0;i<n.count;++i){const U a=original::node(i);auto&x=n.nodes[i];for(U j=0;j<16;++j){same(src::bits(x.localC0.v[j]),m.read(a+0xc0+4*j),std::string(label)+" local"+std::to_string(i)+":"+std::to_string(j));same(src::bits(x.world100.v[j]),m.read(a+0x100+4*j),std::string(label)+" world"+std::to_string(i)+":"+std::to_string(j));}auto ptr=[](I p){return p==I::none?0:original::node(static_cast<U>(p));};same(ptr(x.parent180),m.read(a+0x180),"parent");same(ptr(x.firstChild184),m.read(a+0x184),"firstChild");same(ptr(x.nextSibling188),m.read(a+0x188),"nextSibling");same(x.sceneFlags190,m.read(a+0x190)&255,"flags"+std::to_string(i));}}
void compareFlagTrace(const src::Machine&m){
#ifdef RR_CHARIOT_IK49T_TRACE
 std::size_t i=0;for(auto&s:m.stores){if(s.address<src::Machine::nodeBase||s.address>=src::Machine::nodeBase+64*src::Machine::nodeStride||(s.address-src::Machine::nodeBase)%src::Machine::nodeStride!=0x190)continue;need(i<nativeFlagCount,"missing native flag store");same(nativeFlags[i].node,(s.address-src::Machine::nodeBase)/src::Machine::nodeStride,"flag order node");same(nativeFlags[i].before,s.before&255,"flag order before");same(nativeFlags[i].after,s.after&255,"flag order after");++i;}same(U(i),U(nativeFlagCount),"native flag store count");
#else
 (void)m;
#endif
}
void clearFlagTrace(){
#ifdef RR_CHARIOT_IK49T_TRACE
 nativeFlagCount=0;
#endif
}
void compareRecord(const ChariotIkChain49T&r,src::Machine&m,U a){same(original::node(static_cast<U>(r.shoulder0)),m.read(a),"record00");same(original::node(static_cast<U>(r.elbow4)),m.read(a+4),"record04");same(original::node(static_cast<U>(r.wrist8)),m.read(a+8),"record08");same(r.mode10,m.read(a+0x10),"record10");const float c[]={r.lengthSquared14,r.inverseLengthProduct18,r.projectedSquared1C,r.inverseProjectedProduct20,r.restCosine24,r.restSine28,r.restAngle2C};for(U i=0;i<7;++i)same(src::bits(c[i]),m.read(a+0x14+i*4),"coefficient"+std::to_string(i));for(U i=0;i<4;++i){same(src::bits(r.savedElbowLocal30.v[i]),m.read(a+0x30+i*4),"savedVector");same(src::bits(r.axis50.v[i]),m.read(a+0x50+i*4),"axisVector");}same(r.debug40,m.read(a+0x40),"record40");}
void compareShared(Native&n,src::Machine&m){same(U(n.libm.errnoValue),m.read(0x385160),"errno");const VisualLazyWord49R* p[]={&n.shared.sentinel443450,&n.shared.sentinel443458,&n.visual.orientationSentinel4433D8};const U g[]={0x3d65b8,0x3d65bc,0x3d659c},w[]={0x443450,0x443458,0x4433d8};for(U i=0;i<3;++i){same(p[i]->guard,m.read(g[i])&255,"lazy guard");same(p[i]->word,m.read(w[i]),"lazy word");}}
void seedShared(Native&n,src::Machine&m,U mask,U err=77){n.libm.errnoValue=static_cast<std::int32_t>(err);m.init(0x385160,err);VisualLazyWord49R* p[]={&n.shared.sentinel443450,&n.shared.sentinel443458,&n.visual.orientationSentinel4433D8};const U g[]={0x3d65b8,0x3d65bc,0x3d659c},w[]={0x443450,0x443458,0x4433d8};for(U i=0;i<3;++i){*p[i]={static_cast<std::uint8_t>((mask>>i)&1),0x12345670+i};m.init(g[i],p[i]->guard);m.init(w[i],p[i]->word);}for(U i=0;i<9;++i)n.visual.globals[i]={static_cast<std::uint8_t>(i+2),0xcafe0000+i};n.visual.zero434630={{-0.0f,2,3,1}};}
void originalSolve(src::Machine&m,const ChariotIkBindings49T&b){if(b.left.target!=I::none)original::call(m,0x290a00,0x1000,original::node(static_cast<U>(b.left.target)));if(b.right.target!=I::none)original::call(m,0x290a00,0x1080,original::node(static_cast<U>(b.right.target)));}
void execute(Native&n,src::Machine&m,const ChariotIkBindings49T&b){auto record=n.records;auto visual=n.visual;clear(m);if(mutationPc)m.substitutions[mutationPc]=mutationWord;originalSolve(m,b);clearFlagTrace();need(n.solve(b),"native solve rejected stage"+std::to_string(n.diagnostic.sourceStage));compareScene(n,m,"runtime");compareFlagTrace(m);compareShared(n,m);need(std::memcmp(&record,&n.records,sizeof record)==0,"native runtime coefficient mutation");need(std::memcmp(visual.globals,n.visual.globals,sizeof visual.globals)==0&&std::memcmp(&visual.zero434630,&n.visual.zero434630,sizeof visual.zero434630)==0,"unrelated49R state changed");compareRecord(n.records.left,m,0x1000);compareRecord(n.records.right,m,0x1080);account(m);++cases;}
std::array<U,16> parent(U k){std::array<U,16>a{};a[0]=a[5]=a[10]=a[15]=0x3f800000u;if(k%3==1){a[0]=a[10]=0;a[2]=0xbf800000u;a[8]=0x3f800000u;}if(k%3==2){a[0]=a[5]=0xbf800000u;}a[12]=src::bits(float(k)*.125f);a[13]=src::bits(-4.0f);a[14]=src::bits(30.f-float(k));return a;}
void rejection(Native n,const ChariotIkBindings49T&b,ChariotIkError49T error){const auto nodes=n.nodes;const auto records=n.records;const auto libm=n.libm;const auto shared=n.shared;const auto visual=n.visual;need(!n.solve(b),"invalid context accepted");need(n.diagnostic.error==error,"wrong rejection");need(std::memcmp(nodes.data(),n.nodes.data(),sizeof nodes)==0&&std::memcmp(&records,&n.records,sizeof records)==0&&std::memcmp(&libm,&n.libm,sizeof libm)==0&&std::memcmp(&shared,&n.shared,sizeof shared)==0&&std::memcmp(&visual,&n.visual,sizeof visual)==0,"non-atomic rejection");}
}
int main(int argc,char**argv){try{const std::filesystem::path root=argc>1?argv[1]:".";if(argc==4){mutationPc=U(std::stoul(argv[2],nullptr,16));mutationWord=U(std::stoul(argv[3],nullptr,16));}
 auto b=binding();src::Machine uninitialized;original::initData(uninitialized);original::initScene(uninitialized,root);clear(uninitialized);Native seed;seed.scene(uninitialized);auto base=uninitialized;if(mutationPc)base.substitutions[mutationPc]=mutationWord;original::initializeRecords(base);clearFlagTrace();need(seed.init(b),"native initializer rejected");compareScene(seed,base,"constructor");compareFlagTrace(base);compareRecord(seed.records.left,base,0x1000);compareRecord(seed.records.right,base,0x1080);same(U(seed.libm.errnoValue),base.read(0x385160),"constructor errno");account(base);++initializers;
 for(U i=0;i<625;++i){auto m=base;original::loadPose(m,root,i);Native n=seed;n.scene(m);seedShared(n,m,i%8);execute(n,m,b);}
 // Independent target gates and warm/cold shared records.
 for(U mask=0;mask<8;++mask)for(U gates=0;gates<4;++gates){auto m=base;original::loadPose(m,root,mask*71);Native n=seed;n.scene(m);seedShared(n,m,mask);auto gate=b;if(!(gates&1))gate.left.target=I::none;if(!(gates&2))gate.right.target=I::none;execute(n,m,gate);}
 // Current parent pose changes preserve original persistent coefficients.
 for(U k=0;k<24;++k){auto m=base;original::loadPose(m,root,k*23);original::attachParentAndSibling(m,parent(k));Native n=seed;n.scene(m,20);seedShared(n,m,k%8);execute(n,m,b);}
 // Mixed ready/dirty cache state is supplied, including ready descendants
 // beneath dirty parents; no broad cache normalization is permitted.
 for(U pattern=0;pattern<40;++pattern){auto m=base;original::loadPose(m,root,(pattern*13)%625);for(U i=0;i<18;++i)m.init(original::node(i)+0x190,0x64u|((i*7+pattern*11)%3==0?1u:0u));Native n=seed;n.scene(m);seedShared(n,m,pattern%8);execute(n,m,b);}
 // Constructor coefficients derive from the separately supplied parent scene.
 for(U k=0;k<12;++k){auto m=uninitialized;original::attachParentAndSibling(m,parent(k));Native n;n.scene(m,20);clear(m);if(mutationPc)m.substitutions[mutationPc]=mutationWord;original::initializeRecords(m);clearFlagTrace();need(n.init(b),"parented initializer rejected");compareRecord(n.records.left,m,0x1000);compareRecord(n.records.right,m,0x1080);compareScene(n,m,"parented constructor");compareFlagTrace(m);account(m);++initializers;seedShared(n,m,k%8);execute(n,m,b);}
 // Shared errno and all scene/cache state persist across repeated frame calls.
 {auto m=base;Native n=seed;n.scene(m);seedShared(n,m,0,91);for(U frame=0;frame<72;++frame){if(frame%3!=0){original::loadPose(m,root,(frame*47)%625);for(U i=0;i<18;++i){for(U j=0;j<16;++j)n.nodes[i].localC0.v[j]=src::fp(m.read(original::node(i)+0xc0+4*j));n.nodes[i].sceneFlags190=0x24;}}execute(n,m,b);}}
 auto n=seed;auto m=base;seedShared(n,m,0);n.records.right.mode10=0;rejection(n,b,ChariotIkError49T::context);n=seed;n.records.left.debug40=1;rejection(n,b,ChariotIkError49T::context);n=seed;n.libm.mode=-1;rejection(n,b,ChariotIkError49T::context);n=seed;n.libm.coldNanRecord=false;rejection(n,b,ChariotIkError49T::context);n=seed;n.libm.defaultMatherr=false;rejection(n,b,ChariotIkError49T::context);n=seed;n.records.right.restAngle2C=std::numeric_limits<float>::infinity();rejection(n,b,ChariotIkError49T::context);n=seed;n.nodes[17].localC0.v[12]=std::numeric_limits<float>::max();rejection(n,b,ChariotIkError49T::arithmetic);n=seed;n.nodes[0].parent180=ix(0);rejection(n,b,ChariotIkError49T::graph);n=seed;auto bad=b;bad.right.wrist=bad.left.wrist;rejection(n,bad,ChariotIkError49T::bindings);
 need(std::fesetround(FE_DOWNWARD)==0,"round mode");rejection(seed,b,ChariotIkError49T::rounding_mode);need(std::fesetround(FE_TONEAREST)==0,"restore round mode");
 // No-op gates consume neither missing records nor unsupported libm context.
 {Native q=seed;auto gate=b;gate.left.target=gate.right.target=I::none;q.libm.mode=0;q.records.left.mode10=0;q.records.right.restAngle2C=std::numeric_limits<float>::quiet_NaN();auto nodes=q.nodes;need(q.solve(gate),"gated no-op rejected");need(std::memcmp(nodes.data(),q.nodes.data(),sizeof nodes)==0,"gated graph changed");}
 // A late right-constructor numerical failure cannot publish left outputs
 // or its cache/errno changes.
 {Native q=seed;for(U i:{14u,15u}){for(U j=12;j<15;++j)q.nodes[i].localC0.v[j]=0;q.nodes[i].sceneFlags190&=0xfe;}auto nodes=q.nodes;auto records=q.records;auto libm=q.libm;need(!q.init(b)&&q.diagnostic.error==ChariotIkError49T::arithmetic,"degenerate constructor accepted");need(std::memcmp(nodes.data(),q.nodes.data(),sizeof nodes)==0&&std::memcmp(&records,&q.records,sizeof records)==0&&std::memcmp(&libm,&q.libm,sizeof libm)==0,"constructor rejection non-atomic");}
 // Fixed caller-record overlap checks do not inspect heap ownership.
 {Native q=seed;auto g=q.graph();auto nodes=q.nodes;auto libm=q.libm;need(!initializeChariotIk49T(b,g,q.work.records,q.libm,q.work,q.diagnostic)&&q.diagnostic.error==ChariotIkError49T::records,"constructor scratch overlap");need(std::memcmp(nodes.data(),q.nodes.data(),sizeof nodes)==0&&std::memcmp(&libm,&q.libm,sizeof libm)==0,"overlap changed state");need(!solveChariotHands49T(b,q.records,g,q.libm,q.work.shared,q.visual,q.work,q.diagnostic)&&q.diagnostic.error==ChariotIkError49T::records,"runtime scratch overlap");}
 need(allocations==0,"native allocation");
 if(const char*path=std::getenv("RR_CHARIOT_IK49T_COVERAGE")){std::ofstream out(path);if(!out)throw std::runtime_error("coverage path");for(auto&block:ik_words::blocks)for(U i=0;i<block.count;++i){auto k=block.index+i;out<<std::hex<<block.start+4*i<<'\t'<<std::dec<<hits[k]<<'\t'<<taken[k]<<'\t'<<notTaken[k]<<'\n';}}
 std::cout<<"{\"runtime_cases\":"<<cases<<",\"initializers\":"<<initializers<<",\"checks\":"<<checks<<",\"original_instructions\":"<<originalInstructions<<",\"domain_error_calls\":"<<domainCalls<<",\"lower_clamps\":"<<lowerClamps<<",\"native_allocations\":"<<allocations<<",\"all_match\":true}\n";
 }catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
