#include <string>
#include "../../src/game/scene_graph49p.h"
#include "../../evidence/sceneProjection49P/source/scene_oracle.h"
#include "../../evidence/authoredGeometry49O/bounds/pod01_live_matrices.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <random>
#include <vector>
using namespace rr::game;
namespace original=scene49p_oracle;
using original::U;using original::bits;using original::fp;
using N=SceneNode49P;using M=BodyMatrix48G;using I=SceneNodeIndex49P;
static std::uint64_t allocations=0,checks=0,instructions=0,operations=0,fixtures=0,flagStores=0;
#ifndef RR_SCENE49P_TRACE_REQUIRED
#define RR_SCENE49P_TRACE_REQUIRED 0
#endif
static constexpr bool traceRequired=RR_SCENE49P_TRACE_REQUIRED!=0;
static std::array<std::uint64_t,scene49p_words::total_words> hits{},taken{},notTaken{};
struct Flag {U index;std::uint8_t before,after;};
static std::array<Flag,512> nativeFlags{};static unsigned nativeFlagCount=0;
void rrScene49PFlagStore(U index,std::uint8_t before,std::uint8_t after) noexcept {
 if(nativeFlagCount>=nativeFlags.size())std::abort();
 nativeFlags[nativeFlagCount++]={index,before,after};
}
[[gnu::noinline]] void* operator new(std::size_t n){void*p=std::malloc(n?n:1);if(!p)throw std::bad_alloc();++allocations;return p;}
[[gnu::noinline]] void* operator new[](std::size_t n){return ::operator new(n);}
[[gnu::noinline]] void operator delete(void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void*p,std::size_t)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete[](void*p,std::size_t)noexcept{std::free(p);}
static void need(bool value,const char*message){++checks;if(!value)throw std::runtime_error(message);}
static I ix(unsigned i){return static_cast<I>(i);}
static U pointer(I i){return i==I::none?0:0x1000+static_cast<U>(i)*0x200;}
static M matrix(float d=1){M m{};for(unsigned i=0;i<4;++i)m.v[5*i]=d;return m;}
static U mutationPc=0,mutationWord=0;
struct Fixture {
 std::array<N,64> nodes{},initial{};unsigned count;
 std::unique_ptr<original::Machine> o=std::make_unique<original::Machine>();
 explicit Fixture(unsigned n):count(n){
  std::memset(nodes.data(),0x5a,sizeof nodes);
  for(unsigned i=0;i<64;++i){nodes[i].localC0=matrix();nodes[i].world100=matrix(-3);nodes[i].parent180=nodes[i].firstChild184=nodes[i].nextSibling188=I::none;nodes[i].sceneFlags190=0x65;}
 }
 void link(unsigned child,unsigned parent){nodes[child].parent180=ix(parent);auto*p=&nodes[parent].firstChild184;while(*p!=I::none)p=&nodes[static_cast<unsigned>(*p)].nextSibling188;*p=ix(child);}
 void seed(){
  need(validateSceneGraph49P({nodes.data(),count}),"fixture topology");std::memcpy(initial.data(),nodes.data(),sizeof nodes);
  for(unsigned i=0;i<count;++i){U p=pointer(ix(i));for(unsigned j=0;j<16;++j){o->store(p+0xc0+j*4,nodes[i].localC0.v[j]);o->store(p+0x100+j*4,nodes[i].world100.v[j]);}
   o->write(p+0x180,pointer(nodes[i].parent180));o->write(p+0x184,pointer(nodes[i].firstChild184));o->write(p+0x188,pointer(nodes[i].nextSibling188));o->write(p+0x190,0xa5a5a500u|nodes[i].sceneFlags190);
  }
  if(mutationPc)o->substitutions[mutationPc]=mutationWord;
  ++fixtures;
 }
 void check(){
  for(unsigned i=0;i<count;++i){U p=pointer(ix(i));N expected;std::memcpy(&expected,&initial[i],sizeof(N));
   for(unsigned j=0;j<16;++j){expected.localC0.v[j]=fp(o->read(p+0xc0+j*4));expected.world100.v[j]=fp(o->read(p+0x100+j*4));}
   expected.sceneFlags190=o->read(p+0x190)&255;
   need(std::memcmp(&nodes[i],&expected,sizeof(N))==0,"native full node/local16/world16/flags/links/padding mismatch");
   for(unsigned at=0;at<0x200;at+=4){const U value=o->read(p+at);if((at>=0xc0&&at<0x140)||at==0x190)continue;
    const U expectedWord=at==0x180?pointer(initial[i].parent180):at==0x184?pointer(initial[i].firstChild184):at==0x188?pointer(initial[i].nextSibling188):0xa5a5a5a5u;
    need(value==expectedWord,"original wrote untouched scene field");
   }
   need((o->read(p+0x190)&0xffffff00u)==0xa5a5a500u,"original changed adjacent flag bytes");
  }
  for(unsigned i=count;i<64;++i)need(std::memcmp(&nodes[i],&initial[i],sizeof(N))==0,"native changed node outside graph");
  unsigned k=0;
  if(traceRequired)for(const auto&s:o->stores)if(s.address>=0x1000&&s.address<0x1000+count*0x200&&(s.address-0x1000)%0x200==0x190){
   need(k<nativeFlagCount,"native missing flag store");const auto&f=nativeFlags[k++];need(f.index==(s.address-0x1190)/0x200&&f.before==(s.oldWord&255)&&f.after==(s.newWord&255),"native flag store order/value mismatch");
  }
  if(traceRequired)need(k==nativeFlagCount,"native unexpected flag store");
  flagStores+=k;
 }
 enum Op {SetRoot,Translate,SetLocal,Ancestors,Invalidate,World};
 void op(Op type,unsigned index,const M& m=matrix(),const SceneDisplacement49P& d={{.25f,-.5f,1.f,-7.f}}){
  constexpr U input=0xa000;const U p=pointer(ix(index));for(unsigned j=0;j<16;++j)o->store(input+j*4,m.v[j]);if(type==Translate)for(unsigned j=0;j<4;++j)o->store(input+j*4,d.v[j]);
  o->trace.clear();o->stores.clear();nativeFlagCount=0;
  M suppliedMatrix;SceneDisplacement49P suppliedDisplacement;std::memcpy(&suppliedMatrix,&m,sizeof m);std::memcpy(&suppliedDisplacement,&d,sizeof d);
  switch(type){case SetRoot:o->call(0x289b20,p,0,input);break;case Translate:o->call(0x28af90,p,0,input);break;case SetLocal:o->call(0x28b490,p,input);break;case Ancestors:o->call(0x28c4f0,p);break;case Invalidate:o->call(0x289a20,p);break;case World:o->call(0x2897b0,p);break;}
  const SceneGraph49P graph{nodes.data(),count};const auto before=allocations;bool ok=false;
  switch(type){case SetRoot:ok=setParentlessWorldMatrix49P(graph,ix(index),m);break;case Translate:ok=addParentlessTranslation49P(graph,ix(index),d);break;case SetLocal:ok=setLocalMatrix49P(graph,ix(index),m);break;case Ancestors:ok=markSceneAncestors49P(graph,ix(index));break;case Invalidate:ok=invalidateSceneReady49P(graph,ix(index));break;case World:ok=refreshSceneWorld49P(graph,ix(index));break;}
  need(allocations==before,"native hot allocation");need(ok,"valid source fixture rejected");
  need(std::memcmp(&m,&suppliedMatrix,sizeof m)==0&&std::memcmp(&d,&suppliedDisplacement,sizeof d)==0,"native modified supplied argument");
  for(unsigned j=0;j<(type==Translate?4u:16u);++j)need(o->read(input+4*j)==bits(type==Translate?d.v[j]:m.v[j]),"original modified supplied argument");
  for(const auto&s:o->stores)need((s.address>=0x1000&&s.address<0x1000+count*0x200)||(s.address>=0xc000&&s.address<original::Machine::stack),"original store outside initialized nodes/stack");
  check();++operations;
 }
 ~Fixture(){instructions+=o->steps;for(unsigned i=0;i<hits.size();++i){hits[i]+=o->hits[i];taken[i]+=o->taken[i];notTaken[i]+=o->notTaken[i];}}
};
static M randomMatrix(std::mt19937&r){M m{};for(float&v:m.v)v=float(std::int32_t(r()%513)-256)/1024.f;return m;}
static void stateCases(){
 std::mt19937 rng(49050);
 // Every flag byte and all four main190/child194 ready combinations. Current
 // child locals are supplied nonidentity; no constructor-identity shortcut.
 for(unsigned mask=0;mask<256;++mask){Fixture f(2);f.link(1,0);f.nodes[0].sceneFlags190=mask;f.nodes[1].sceneFlags190=((mask<<1)|(mask>>7))&255;f.nodes[0].localC0=randomMatrix(rng);f.nodes[1].localC0=randomMatrix(rng);f.nodes[0].world100=randomMatrix(rng);f.nodes[1].world100=randomMatrix(rng);f.seed();
  const M a=randomMatrix(rng),b=randomMatrix(rng);f.op(Fixture::Translate,0);f.op(Fixture::World,1);f.op(Fixture::SetRoot,0,a);f.op(Fixture::Translate,0);f.op(Fixture::SetRoot,0,b);f.op(Fixture::Translate,0);f.op(Fixture::World,1);f.op(Fixture::World,1);f.op(Fixture::SetLocal,1,a);f.op(Fixture::World,0);f.op(Fixture::World,1);f.op(Fixture::Invalidate,1);f.op(Fixture::Ancestors,1);
 }
 // Wide/deep/mixed known-node forests, differing cache/local and flag states.
 for(unsigned trial=0;trial<256;++trial){unsigned count=1+trial%64;Fixture f(count);for(unsigned i=1;i<count;++i){if(trial%7==0&&i%5==0)continue;f.link(i,trial%3==0?i-1:trial%3==1?0:rng()%i);}
  for(unsigned i=0;i<count;++i){f.nodes[i].sceneFlags190=rng()&255;f.nodes[i].localC0=randomMatrix(rng);f.nodes[i].world100=randomMatrix(rng);}f.seed();
  f.op(Fixture::Translate,0);f.op(Fixture::Ancestors,count-1);f.op(Fixture::World,count-1);f.op(Fixture::World,count-1);f.op(Fixture::SetLocal,count/2,randomMatrix(rng));f.op(Fixture::Invalidate,0);f.op(Fixture::SetRoot,0,randomMatrix(rng));f.op(Fixture::Translate,0);for(unsigned i=0;i<count;++i)f.op(Fixture::World,i);
 }
 for(unsigned shape=0;shape<3;++shape){Fixture f(64);for(unsigned i=1;i<64;++i)f.link(i,shape==0?i-1:shape==1?0:(i-1)/2);f.seed();f.op(Fixture::Invalidate,0);f.op(Fixture::World,63);f.op(Fixture::Ancestors,63);f.op(Fixture::SetLocal,63);f.op(Fixture::World,63);}
 // Non-parent-first native indices. Roots are indices 7 and 3; child siblings
 // and parent chain are never inferred from array order.
 {Fixture f(8);f.link(6,7);f.link(0,7);f.link(2,6);f.link(5,2);f.link(1,3);f.link(4,3);f.nodes[2].sceneFlags190=0xfe;f.seed();f.op(Fixture::Invalidate,6);f.op(Fixture::SetLocal,5);f.op(Fixture::World,5);f.op(Fixture::SetRoot,7);f.op(Fixture::World,0);}
}
static void bitCases(){
 for(unsigned mask=0;mask<64;++mask){Fixture f(3);f.link(1,0);f.link(2,1);for(unsigned i=0;i<3;++i){f.nodes[i].sceneFlags190=0x64;for(unsigned j=0;j<16;++j)f.nodes[i].localC0.v[j]=(j%5==0?((mask&1)?-1.f:1.f):fp((mask&2)?0x80000000u:0u));}f.seed();f.op(Fixture::World,2);M m=f.nodes[0].localC0;m.v[3]=fp(mask&4?0x80000000:0);m.v[7]=.25f;m.v[11]=-.125f;m.v[15]=-.5f;f.op(Fixture::SetRoot,0,m);SceneDisplacement49P d{{fp(mask&8?0x80000000u:0),fp(mask&16?0x00800000u:0x00000001u),fp(mask&32?0x80000001u:0),fp(0x7fc01234)}};f.op(Fixture::Translate,0,m,d);f.op(Fixture::World,2);}
 // Copy-only and ready-cache operations retain even nonfinite payloads. Those
 // payloads are not admitted as arithmetic operands of a dirty child product.
 {Fixture f(1);for(unsigned j=0;j<16;++j){f.nodes[0].localC0.v[j]=fp(0x7fc00001u+j);f.nodes[0].world100.v[j]=fp(0xff800000u);}f.seed();f.op(Fixture::World,0);const M m=f.nodes[0].localC0;f.op(Fixture::SetRoot,0,m);f.op(Fixture::World,0);f.op(Fixture::SetLocal,0,m);f.op(Fixture::World,0);}
}
static void authored(const char*path){
 std::ifstream in(path,std::ios::binary);std::vector<char>bytes((std::istreambuf_iterator<char>(in)),{});need(bytes.size()>12+18*224,"Chariot PSG missing");auto read=[&](unsigned at){U w;std::memcpy(&w,bytes.data()+at,4);return w;};need(read(8)==18,"Chariot node count");
 for(unsigned variant=0;variant<4;++variant){Fixture f(19);for(unsigned i=0;i<18;++i){const auto parent=static_cast<std::int32_t>(read(12+224*i+220));need(parent>=-1&&parent<static_cast<std::int32_t>(i),"PSG parent");f.link(i+1,parent<0?0:static_cast<unsigned>(parent)+1);for(unsigned j=0;j<16;++j)f.nodes[i+1].localC0.v[j]=fp(variant&2?bounds49o_fixture::liveLocal[i][j]:read(12+224*i+128+j*4));f.nodes[i+1].sceneFlags190=(variant&1)?0xa5:0xa4;}
  f.nodes[0].sceneFlags190=(variant&1)?0x41:0x40;f.seed();M source=matrix();source.v[0]=0;source.v[1]=1;source.v[4]=-1;source.v[5]=0;source.v[12]=12.25f;source.v[13]=-8.5f;source.v[14]=31.75f;f.op(Fixture::SetLocal,0,source);for(unsigned i=0;i<19;++i)f.op(Fixture::World,i);f.op(Fixture::SetLocal,0,source);f.op(Fixture::World,18);for(unsigned i=0;i<19;++i)f.op(Fixture::World,i);
 }
}
static void admission(){
 Fixture f(3);f.link(1,0);f.link(2,1);f.seed();M m=matrix();SceneDisplacement49P d{{0,0,0,0}};
 auto reject=[&](unsigned operation,I index){auto snapshot=f.nodes;nativeFlagCount=0;const auto before=allocations;SceneGraph49P g{f.nodes.data(),f.count};bool ok=false;
  switch(operation){case 0:ok=setParentlessWorldMatrix49P(g,index,m);break;case 1:ok=addParentlessTranslation49P(g,index,d);break;case 2:ok=setLocalMatrix49P(g,index,m);break;case 3:ok=markSceneAncestors49P(g,index);break;case 4:ok=invalidateSceneReady49P(g,index);break;default:ok=refreshSceneWorld49P(g,index);break;}
  need(!ok,"invalid domain accepted");need(allocations==before,"rejection allocated");need(std::memcmp(snapshot.data(),f.nodes.data(),sizeof f.nodes)==0,"rejection changed graph");need(nativeFlagCount==0,"rejection flag stores");};
 for(unsigned op=0;op<6;++op){reject(op,I::none);reject(op,ix(3));}reject(0,ix(1));reject(1,ix(1));
 for(unsigned op=0;op<6;++op){need(std::fesetround(FE_DOWNWARD)==0,"set rounding");reject(op,ix(0));need(std::fesetround(FE_TONEAREST)==0,"restore rounding");}
 need(validateSceneGraph49P({nullptr,0}),"empty forest rejected");need(!validateSceneGraph49P({nullptr,1}),"null graph accepted");need(!validateSceneGraph49P({f.nodes.data(),65}),"65-node graph accepted");
 auto saved=f.nodes;f.nodes[0].parent180=ix(2);need(!validateSceneGraph49P({f.nodes.data(),3}),"parent cycle accepted");f.nodes=saved;f.nodes[1].nextSibling188=ix(1);need(!validateSceneGraph49P({f.nodes.data(),3}),"sibling cycle accepted");f.nodes=saved;f.nodes[0].firstChild184=I::none;need(!validateSceneGraph49P({f.nodes.data(),3}),"unlisted child accepted");f.nodes=saved;f.nodes[2].parent180=ix(0);need(!validateSceneGraph49P({f.nodes.data(),3}),"inconsistent child parent accepted");f.nodes=saved;f.nodes[0].nextSibling188=ix(1);need(!validateSceneGraph49P({f.nodes.data(),3}),"root sibling accepted");f.nodes=saved;f.nodes[2].parent180=static_cast<I>(-2);need(!validateSceneGraph49P({f.nodes.data(),3}),"invalid negative reference accepted");f.nodes=saved;
 d.v[0]=fp(0x7f800000);reject(1,ix(0));d.v[0]=fp(0x7fc01234);reject(1,ix(0));d.v[0]=std::numeric_limits<float>::max();f.nodes[0].localC0.v[12]=d.v[0];reject(1,ix(0));f.nodes=saved;
 for(unsigned i=0;i<3;++i)f.nodes[i].sceneFlags190&=~1u;
 f.nodes[0].localC0.v[0]=fp(0x7f800000);reject(5,ix(2));f.nodes[0].localC0.v[0]=std::numeric_limits<float>::max();f.nodes[1].localC0.v[0]=2;reject(5,ix(2));f.nodes=saved;
 // A dirty intermediate with a ready descendant remains inconsistent by source
 // design; admission must not silently normalize that supplied state.
 f.nodes[0].sceneFlags190=1;f.nodes[1].sceneFlags190=0;f.nodes[2].sceneFlags190=1;need(validateSceneGraph49P({f.nodes.data(),3}),"source dirty skip state rejected");
}
static void coverage(){const char*path=std::getenv("RR_SCENE49P_COVERAGE");if(!path)return;std::ofstream out(path);need(bool(out),"coverage file");out<<"{\"fixtures\":"<<fixtures<<",\"operations\":"<<operations<<",\"checks\":"<<checks<<",\"instructions\":"<<instructions<<",\"ordered_store_tracing\":"<<(traceRequired?"true":"false")<<",\"flag_stores_compared\":"<<flagStores<<",\"hot_allocations\":0,\"pcs\":[";bool first=true;for(const auto&b:scene49p_words::blocks)for(unsigned i=0;i<b.count;++i){if(!first)out<<',';first=false;const auto n=b.index+i;out<<"{\"pc\":"<<b.start+i*4<<",\"word\":"<<b.words[i]<<",\"hits\":"<<hits[n]<<",\"taken\":"<<taken[n]<<",\"not_taken\":"<<notTaken[n]<<'}';}out<<"]}\n";}
int main(int argc,char**argv){try{need(std::fesetround(FE_TONEAREST)==0,"rounding");if(argc==4){mutationPc=std::stoul(argv[2],nullptr,16);mutationWord=std::stoul(argv[3],nullptr,16);}stateCases();bitCases();authored(argc>1?argv[1]:"evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg");admission();coverage();std::cout<<"PASS SceneGraph49P: "<<fixtures<<" fixtures, "<<operations<<" staged operations, "<<checks<<" checks, "<<instructions<<" original instructions, "<<flagStores<<(traceRequired?" exact ordered flag stores":" ordered flag stores (tracing off in uninstrumented aggregate build)")<<", zero native hot allocations\n";return 0;}catch(const std::exception&e){std::cerr<<"FAIL SceneGraph49P: "<<e.what()<<'\n';return 1;}}
