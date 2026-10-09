#include <string>
#include "../../src/game/model_bounds49o.h"
#include "../../evidence/authoredGeometry49O/bounds/bounds_oracle.h"
#include "../../evidence/authoredGeometry49O/bounds/pod01_live_matrices.h"
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <new>
#include <random>
#include <set>
#include <vector>
using namespace rr::game;
namespace original=bounds49o_oracle;
using original::bits;using original::fp;using original::U;
using N=ModelBoundsNode49O;using M=BodyMatrix48G;using V=Vector3_48C;
static std::uint64_t allocations=0,instructions=0,checks=0;static unsigned cases=0;
static std::set<U> hit,taken,notTaken;
static std::array<std::uint64_t,bounds49o_words::total_words> hitCounts{},takenCounts{},notTakenCounts{};
[[gnu::noinline]] void* operator new(std::size_t n){void* p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}[[gnu::noinline]] void operator delete(void*p)noexcept{std::free(p);}[[gnu::noinline]] void operator delete[](void*p)noexcept{std::free(p);}[[gnu::noinline]] void operator delete(void*p,std::size_t)noexcept{std::free(p);}[[gnu::noinline]] void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
static void require(bool b,const char* s){++checks;if(!b)throw std::runtime_error(s);}
static void same(float a,U b,const char* s){++checks;if(bits(a)!=b)throw std::runtime_error(s);}
static M identity(){M m{};for(unsigned j=0;j<4;++j)m.v[j*5]=1;return m;}
static void collect(const original::Machine&o){instructions+=o.steps;for(const auto&b:bounds49o_words::blocks)for(unsigned i=0;i<b.count;++i){unsigned ix=b.index+i;U pc=b.start+i*4;hitCounts[ix]+=o.hits[ix];takenCounts[ix]+=o.taken[ix];notTakenCounts[ix]+=o.notTaken[ix];if(o.hits[ix])hit.insert(pc);if(o.taken[ix])taken.insert(pc);if(o.notTaken[ix])notTaken.insert(pc);}}
static ModelBoundsResult49O fixture(const N*nodes,unsigned count,const ModelBoundsContext49O&ctx,U mutationPc=0,U mutationWord=0){
 auto o=std::make_unique<original::Machine>();constexpr U root=0x1000,base=0x2000,stride=0x200,ptrs=0xb000;
 for(unsigned j=0;j<16;++j)o->store(root+0xc0+j*4,ctx.rootWorld100.v[j]);
 o->write(root+0x190,ctx.rootSceneFlags190);o->write(root+0x1a0,count);o->write(root+0x1b8,ptrs);
 for(unsigned j=0;j<3;++j){o->store(root+0x140+j*4,ctx.rootCenter140.v[j]);o->store(root+0x150+j*4,ctx.rootHalfExtent150.v[j]);}
 for(unsigned i=0;i<count;++i){const auto&n=nodes[i];const U p=base+stride*i;o->write(ptrs+4*i,p);o->write(p+0x180,n.parent<0?root:base+stride*n.parent);o->write(p+0x190,n.sceneFlags190&~1u);o->write(p+0x194,0x7fc01234);for(unsigned j=0;j<16;++j)o->store(p+0xc0+4*j,n.local.v[j]);for(unsigned j=0;j<3;++j){o->store(p+0x140+4*j,n.center140.v[j]);o->store(p+0x150+4*j,n.halfExtent150.v[j]);}}
 if(mutationPc)o->substitutions[mutationPc]=mutationWord;
 o->call(0x26f4b0,root);
 ModelBoundsWorkspace49O workspace{};ModelBoundsResult49O actual{};const auto before=allocations;const bool ok=computeModelBounds49O(nodes,count,ctx,workspace,actual);const auto after=allocations;require(ok,"valid fixture rejected");require(before==after,"hot allocation");
 for(unsigned j=0;j<4;++j){same(actual.minimum[j],o->read(original::Machine::stack-0xf0+0x40+j*4),"minimum mismatch");same(actual.maximum[j],o->read(original::Machine::stack-0xf0+0x50+j*4),"maximum mismatch");same(actual.center160[j],o->read(root+0x160+j*4),"center mismatch");same(actual.halfExtent170[j],o->read(root+0x170+j*4),"extent mismatch");}same(actual.radius198,o->read(root+0x198),"radius mismatch");require(actual.rootSceneFlags190==o->read(root+0x190),"root flags mismatch");require(actual.seeded==bool(o->read(original::Machine::stack-0xf0+0xe8)),"seed flag mismatch");
 for(unsigned i=0;i<count;++i){const U p=base+stride*i;require(!o->reads[(p+0x194)/4],"cached sphere read");if(!(o->read(p+0x190)&1u))continue;for(unsigned j=0;j<16;++j)same(workspace.world[i].v[j],o->read(p+0x100+j*4),"world mismatch");}
 collect(*o);++cases;return actual;
}
static void sourceOrientation(){
 auto o=std::make_unique<original::Machine>();constexpr U root=0x1000;M m=identity();for(unsigned j=0;j<16;++j)o->store(root+0xc0+4*j,m.v[j]);o->write(root+0x190,0x80);
 o->g[29][0]=original::Machine::stack;o->staticData[0x36adb8]=0;for(U pc=0x3c9f70;pc<0x3c9ff8;pc+=4){U unused=0;require(!o->execute(pc,unused),"basis initializer branch");}{U unused=0;o->execute(0x3c9ffc,unused);}
 o->g[4][0]=root;o->g[5][0]=0;o->g[6][0]=0x434660;o->g[7][0]=0x434650;o->g[8][0]=0;o->g[9][0]=1;o->g[10][0]=0;o->run(0x28a090);for(unsigned j=0;j<16;++j)same(m.v[j],o->read(root+0xc0+4*j),"source orientation is not identity");collect(*o);
}
static void synthetic(U mutationPc,U mutationWord){
 std::mt19937 rnd(490);auto random=[&](){return float(std::int32_t(rnd()%200001)-100000)/16384.f;};
 for(unsigned trial=0;trial<384;++trial){unsigned count=trial%65;std::vector<N>nodes(count);ModelBoundsContext49O c{};c.rootWorld100=identity();
  if(trial%4){for(unsigned r=0;r<3;++r)for(unsigned j=0;j<3;++j)c.rootWorld100.v[r*4+j]=random();for(unsigned j=0;j<3;++j)c.rootWorld100.v[12+j]=random();}c.rootSceneFlags190=trial%3?0x80:0x84;
  for(unsigned j=0;j<3;++j){c.rootCenter140.v[j]=random();c.rootHalfExtent150.v[j]=std::fabs(random());}
  for(unsigned i=0;i<count;++i){auto&n=nodes[i];n.parent=i==0?-1:std::int32_t(rnd()%(i+1))-1;n.sceneFlags190=trial%5==0?0x80:(rnd()%4==0?0x80:0xa4);n.local.v[15]=1;for(unsigned r=0;r<4;++r)for(unsigned j=0;j<3;++j)n.local.v[r*4+j]=(trial%4==0?(r==j?1.f:0.f):random());for(unsigned j=0;j<3;++j){n.center140.v[j]=random();n.halfExtent150.v[j]=trial%7==0?0:std::fabs(random());}}
  fixture(nodes.data(),count,c,mutationPc,mutationWord);
 }
}
static void reflectedZeroFixtures(){
 for(unsigned mask=0;mask<128;++mask){ModelBoundsContext49O c{};c.rootWorld100=identity();c.rootSceneFlags190=0x80;
  for(unsigned r=0;r<4;++r)for(unsigned j=0;j<3;++j)c.rootWorld100.v[r*4+j]=r==j?((mask&(1u<<j))?-1.f:1.f):fp((mask&8)?0x80000000u:0u);
  N node{};node.local=identity();node.parent=-1;node.sceneFlags190=0xa4;
  for(unsigned j=0;j<3;++j){node.center140.v[j]=fp((mask&(1u<<(4+j)))?0x80000000u:0u);node.halfExtent150.v[j]=0;}
  fixture(&node,1,c);
 }
}
static void edgeFixtures(){
 for(unsigned mask=0;mask<16;++mask){ModelBoundsContext49O c{};c.rootWorld100=identity();c.rootSceneFlags190=0xc4;for(unsigned j=0;j<3;++j){c.rootCenter140.v[j]=(mask&1)?fp(0x80000000):0;c.rootHalfExtent150.v[j]=(mask&2)?fp(0x80000000):0;}if(mask&4)c.rootHalfExtent150.v[mask%3]=fp((mask&8)?0x20000000:1);fixture(nullptr,0,c);}
}
static void authored(const char*path){
 std::ifstream in(path,std::ios::binary);std::vector<char>b((std::istreambuf_iterator<char>(in)),{});require(b.size()>12+18*224,"PSG fixture missing");auto rd=[&](unsigned offset){U w;std::memcpy(&w,b.data()+offset,4);return w;};require(rd(8)==18,"PSG fixture nodes");N nodes[18]{};
 for(unsigned i=0;i<18;++i){const unsigned at=12+224*i;auto&n=nodes[i];for(unsigned j=0;j<16;++j)n.local.v[j]=fp(rd(at+128+4*j));for(unsigned j=0;j<3;++j){n.center140.v[j]=fp(rd(at+192+j*4));n.halfExtent150.v[j]=fp(rd(at+204+j*4));}n.parent=static_cast<std::int32_t>(rd(at+220));n.sceneFlags190=0xa4;}
 ModelBoundsContext49O c{};c.rootWorld100=identity();c.rootSceneFlags190=0x80;const auto bind=fixture(nodes,18,c);
 for(unsigned i=0;i<18;++i)for(unsigned j=0;j<16;++j)nodes[i].local.v[j]=fp(bounds49o_fixture::liveLocal[i][j]);
 const auto live=fixture(nodes,18,c);
 const U center[3]={0x3d07d900,0xbd4a6b80,0xc1413b28},extent[3]={0x40448f5c,0x3fbd5750,0x40b1b31f};for(unsigned j=0;j<3;++j){same(live.center160[j],center[j],"live center golden");same(live.halfExtent170[j],extent[j],"live extent golden");}same(live.radius198,0x40d082c6,"live radius golden");same(bind.radius198,0x40d082c6,"bind radius golden");
}
static void admission(){
 N node{};node.local=identity();node.parent=-1;node.sceneFlags190=4;ModelBoundsContext49O ctx{};ctx.rootWorld100=identity();ModelBoundsWorkspace49O w{};ModelBoundsResult49O out{},sentinel{};std::memset(&sentinel,0xa5,sizeof sentinel);
 auto reject=[&](const N*p,std::size_t n){out=sentinel;require(!computeModelBounds49O(p,n,ctx,w,out),"invalid input accepted");require(std::memcmp(&out,&sentinel,sizeof out)==0,"failure output modified");};
 reject(nullptr,1);reject(&node,65);ctx.rootSceneFlags190=0x10;reject(&node,1);ctx.rootSceneFlags190=0;node.parent=0;reject(&node,1);node.parent=-2;reject(&node,1);node.parent=-1;node.local.v[0]=fp(0x7fc01234);reject(&node,1);node.local=identity();node.local.v[3]=1;reject(&node,1);node.local=identity();node.halfExtent150.v[0]=-1;reject(&node,1);node.halfExtent150.v[0]=fp(0x7f800000);reject(&node,1);node.halfExtent150.v[0]=fp(0x7f7fffff);node.local.v[0]=4;reject(&node,1);node.local=identity();node.halfExtent150.v[0]=0;
 require(std::fesetround(FE_DOWNWARD)==0,"set rounding");reject(&node,1);require(std::fesetround(FE_TONEAREST)==0,"restore rounding");node.sceneFlags190=0;node.halfExtent150.v[0]=fp(0x7fc01234);require(computeModelBounds49O(&node,1,ctx,w,out),"unused filtered bounds rejected");
}
static void coverage(){
 const char* path=std::getenv("RR_BOUNDS49O_COVERAGE");if(!path)return;std::ofstream out(path);require(bool(out),"coverage output");out<<"{\"cases\":"<<cases<<",\"instructions\":"<<instructions<<",\"pcs\":[";bool first=true;
 for(const auto&b:bounds49o_words::blocks)for(unsigned i=0;i<b.count;++i){const U pc=b.start+i*4;const auto ix=b.index+i;if(!first)out<<',';first=false;out<<"{\"pc\":"<<pc<<",\"hits\":"<<hitCounts[ix]<<",\"taken\":"<<takenCounts[ix]<<",\"not_taken\":"<<notTakenCounts[ix];
 if(!hitCounts[ix]){const char*reason="outside the recovered fresh model box/unit-basis orientation path";if(pc>=0x26f4d4&&pc<0x26f5e4)reason="sphere route excluded by root bit10 clear";else if(pc>=0x28e078&&pc<0x28e0b0)reason="recursive leaf traversal excluded; model flat traversal passes recurse0";else if(pc>=0x28a090&&pc<0x28a528)reason="orientation fixture is fixed unit basis, relative scene null and unparented constructor root";else if(pc>=0x289a20&&pc<0x289b18)reason="orientation fixture enters with dirty root; recursive ready-cache invalidation is outside this numeric fixture";else if(pc==0x3c9ff8)reason="unrelated post-initializer registration call is excluded; its basis-store delay slot is executed";out<<",\"scope\":\""<<reason<<"\"";}out<<'}';}out<<"]}\n";
}
int main(int argc,char**argv){try{require(std::fesetround(FE_TONEAREST)==0,"rounding");const U pc=argc==4?std::stoul(argv[2],nullptr,16):0,word=argc==4?std::stoul(argv[3],nullptr,16):0;sourceOrientation();synthetic(pc,word);edgeFixtures();reflectedZeroFixtures();authored(argc>1?argv[1]:"evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg");admission();coverage();std::cout<<"PASS ModelBounds49O: "<<cases<<" cases, "<<checks<<" checks, "<<instructions<<" original instructions, "<<hit.size()<<" reached PCs, zero hot allocations\n";return 0;}catch(const std::exception&e){std::cerr<<"FAIL ModelBounds49O: "<<e.what()<<'\n';return 1;}}
