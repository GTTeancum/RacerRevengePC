#include <string>
#include "../../src/game/vehicle_visual49r_internal.h"
#include "../../evidence/vehicleVisual49R/container/source/container_oracle.h"
#include <array>
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <new>
#include <random>
#include <sstream>
using namespace rr::game;
namespace original=container49r_oracle;
using original::U;using original::bits;using original::fp;
using N=SceneNode49P;using M=BodyMatrix48G;using I=SceneNodeIndex49P;
static std::uint64_t allocations=0,checks=0,instructions=0,operations=0,fixtures=0,flagStores=0;
static std::array<std::uint64_t,container49r_words::total_words> hits{},taken{},notTaken{};
#ifndef RR_SCENE49P_TRACE_REQUIRED
#define RR_SCENE49P_TRACE_REQUIRED 0
#endif
static constexpr bool traceRequired=RR_SCENE49P_TRACE_REQUIRED!=0;
struct Flag {U index;std::uint8_t before,after;};
static std::array<Flag,2048> nativeFlags{};static unsigned nativeFlagCount=0;
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
static std::string label;
static void need(bool value,const char*message){++checks;if(!value)throw std::runtime_error(label+": "+message);}
static I ix(unsigned i){return static_cast<I>(i);}
static U pointer(I i){return i==I::none?0:0x1000+static_cast<U>(i)*0x200;}
static M matrix(float d=1){M m{};for(unsigned i=0;i<4;++i)m.v[5*i]=d;return m;}
static U mutationPc=0,mutationWord=0;
static constexpr U vehicle=0xa000,probe=0xb000,gp=0x440000,baseSp=original::Machine::stack-0x9c0;
static constexpr U globalValues[9]={0x434760,0x434768,0x434770,0x434778,0x434780,0x434788,0x434790,0x434798,0x4347a0};
static constexpr U globalGuards[9]={gp-0x7870,gp-0x786c,gp-0x7868,gp-0x7864,gp-0x7860,gp-0x785c,gp-0x7858,gp-0x7854,gp-0x7850};
static constexpr U sentinelGuard=gp-0x73d4;
static void sameWords(const void*a,const void*b,unsigned n,const char*name){
 const auto*x=static_cast<const unsigned char*>(a),*y=static_cast<const unsigned char*>(b);
 for(unsigned i=0;i<n/4;++i){U l,r;std::memcpy(&l,x+i*4,4);std::memcpy(&r,y+i*4,4);++checks;if(l!=r){std::ostringstream s;s<<label<<": "<<name<<" word "<<i<<" mismatch native="<<std::hex<<l<<" original="<<r;throw std::runtime_error(s.str());}}
}
struct Fixture {
 std::array<N,64> nodes{},initial{};unsigned count;
 VehicleVisualState49R state{},initialState{};VisualShared49R shared{},initialShared{};detail::VisualFrame49R frame{};
 VisualSceneBindings49R bindings{ix(0),ix(1),ix(2),ix(3),false,false,0};
 std::unique_ptr<original::Machine> o=std::make_unique<original::Machine>();
 explicit Fixture(unsigned n):count(n){
  std::memset(nodes.data(),0x5a,sizeof nodes);std::memset(&state,0x5a,sizeof state);std::memset(&shared,0x5a,sizeof shared);
  for(unsigned i=0;i<64;++i){nodes[i].localC0=matrix();nodes[i].world100=matrix(-3);nodes[i].parent180=nodes[i].firstChild184=nodes[i].nextSibling188=I::none;nodes[i].sceneFlags190=0x65;}
  state.upC20={{.2f,1.f,-.3f,fp(0x7fc01234)}};
  for(unsigned i=0;i<9;++i){shared.globals[i].guard=0;shared.globals[i].word=0x7fc02345+i;}
  shared.orientationSentinel4433D8.guard=0;shared.orientationSentinel4433D8.word=0xabcdef01;
  shared.zero434630={{0,0,0,1}};
 }
 void link(unsigned child,unsigned parent){nodes[child].parent180=ix(parent);auto*p=&nodes[parent].firstChild184;while(*p!=I::none)p=&nodes[static_cast<unsigned>(*p)].nextSibling188;*p=ix(child);}
 void byte(U at,std::uint8_t b){o->write(at&~3u,(o->read(at&~3u)&~(255u<<((at&3)*8)))|(U(b)<<((at&3)*8)));}
 void seed(){
  need(validateSceneGraph49P({nodes.data(),count}),"fixture topology");std::memcpy(initial.data(),nodes.data(),sizeof nodes);initialState=state;std::memcpy(&initialShared,&shared,sizeof shared);
  for(unsigned i=0;i<count;++i){U p=pointer(ix(i));for(unsigned j=0;j<16;++j){o->store(p+0xc0+j*4,nodes[i].localC0.v[j]);o->store(p+0x100+j*4,nodes[i].world100.v[j]);}
   o->write(p+0x180,pointer(nodes[i].parent180));o->write(p+0x184,pointer(nodes[i].firstChild184));o->write(p+0x188,pointer(nodes[i].nextSibling188));o->write(p+0x190,0xa5a5a500u|nodes[i].sceneFlags190);
  }
  o->write(vehicle+0x190,pointer(bindings.main190));o->write(vehicle+0x194,pointer(bindings.container194));o->write(vehicle+0x348,probe);
  for(unsigned j=0;j<4;++j){o->store(vehicle+0xc20+j*4,state.upC20.v[j]);o->store(vehicle+0xaf0+j*4,state.publishedUpAF0.v[j]);o->store(0x434630+j*4,shared.zero434630.v[j]);}
  for(unsigned i=0;i<9;++i){byte(globalGuards[i],shared.globals[i].guard);o->write(globalValues[i],shared.globals[i].word);}
  byte(sentinelGuard,shared.orientationSentinel4433D8.guard);o->write(0x4433d8,shared.orientationSentinel4433D8.word);
  if(mutationPc)o->substitutions[mutationPc]=mutationWord;
  ++fixtures;
 }
 void check(){
  for(unsigned i=0;i<count;++i){U p=pointer(ix(i));N expected;std::memcpy(&expected,&initial[i],sizeof(N));
   for(unsigned j=0;j<16;++j){expected.localC0.v[j]=fp(o->read(p+0xc0+j*4));expected.world100.v[j]=fp(o->read(p+0x100+j*4));}
   expected.sceneFlags190=o->read(p+0x190)&255;
   sameWords(&nodes[i],&expected,sizeof(N),"complete scene node");
   for(unsigned at=0;at<0x200;at+=4){const U value=o->read(p+at);if((at>=0xc0&&at<0x140)||at==0x190)continue;
    const U expectedWord=at==0x180?pointer(initial[i].parent180):at==0x184?pointer(initial[i].firstChild184):at==0x188?pointer(initial[i].nextSibling188):0xa5a5a5a5u;
    need(value==expectedWord,"original wrote untouched scene field");
   }
   need((o->read(p+0x190)&0xffffff00u)==0xa5a5a500u,"original changed adjacent flag bytes");
  }
  for(unsigned i=count;i<64;++i)need(std::memcmp(&nodes[i],&initial[i],sizeof(N))==0,"native changed node outside graph");
  auto expectedState=initialState;
  for(unsigned j=0;j<4;++j){expectedState.upC20.v[j]=fp(o->read(vehicle+0xc20+j*4));expectedState.publishedUpAF0.v[j]=fp(o->read(vehicle+0xaf0+j*4));}
  sameWords(&state,&expectedState,sizeof state,"persistent C20/AF0 and untouched responses/clocks");
  VisualShared49R expectedShared;std::memcpy(&expectedShared,&initialShared,sizeof expectedShared);
  for(unsigned i=0;i<9;++i){expectedShared.globals[i].guard=(o->read(globalGuards[i]&~3u)>>((globalGuards[i]&3)*8))&255;expectedShared.globals[i].word=o->read(globalValues[i]);need((o->read(globalGuards[i])&0xffffff00u)==0xa5a5a500u,"original changed neighboring lazy guard bytes");}
  expectedShared.orientationSentinel4433D8.guard=o->read(sentinelGuard)&255;expectedShared.orientationSentinel4433D8.word=o->read(0x4433d8);
  sameWords(&shared,&expectedShared,sizeof shared,"all shared guards, words, zero vector and padding");
  detail::VisualFrame49R expectedFrame{};const U offsets[6]={0xa0,0xb0,0xc0,0x70,0x80,0x90};
  VisualVector49R* vectors[6]={&expectedFrame.mainPosition,&expectedFrame.correctedForward,&expectedFrame.mainUp,&expectedFrame.containerPosition,&expectedFrame.containerForward,&expectedFrame.containerUp};
  for(unsigned i=0;i<6;++i)for(unsigned j=0;j<4;++j)vectors[i]->v[j]=fp(o->read(baseSp+offsets[i]+j*4));
  sameWords(&frame,&expectedFrame,sizeof frame,"six final source frame vectors");
  unsigned k=0;
  if(traceRequired)for(const auto&s:o->stores)if(s.address>=0x1000&&s.address<0x1000+count*0x200&&(s.address-0x1000)%0x200==0x190){
   need(k<nativeFlagCount,"native missing flag store");const auto&f=nativeFlags[k++];need(f.index==(s.address-0x1190)/0x200&&f.before==(s.oldWord&255)&&f.after==(s.newWord&255),"native flag store order/value mismatch");
  }
  if(traceRequired)need(k==nativeFlagCount,"native unexpected flag store");
  flagStores+=k;
 }
 void step(U probe108){
  o->write(probe+0x108,probe108);o->trace.clear();o->stores.clear();nativeFlagCount=0;
  o->f[12]=fp(operations%3==0?0x7fc07777u:operations%3==1?0x80000000u:0x3f100001u);
  o->call(0x21c120,vehicle);
  VehicleVisualDiagnostic49R diagnostic{};const auto before=allocations;
  bool ok=detail::prepareVisualContainer49R({nodes.data(),count},bindings,probe108,state,shared,frame,diagnostic);
  const auto after=allocations;need(after==before,"native hot allocation");if(!ok){std::ostringstream s;s<<label<<": valid original fixture rejected native stage="<<std::hex<<diagnostic.sourceStage<<" error="<<unsigned(diagnostic.error);throw std::runtime_error(s.str());}
  for(const auto&s:o->stores){bool allowed=(s.address>=0x1000&&s.address<0x1000+count*0x200)||(s.address>=0xc000&&s.address<original::Machine::stack)||(s.address>=vehicle+0xc20&&s.address<vehicle+0xc30)||(s.address>=vehicle+0xaf0&&s.address<vehicle+0xb00)||s.address==sentinelGuard||s.address==0x4433d8;
   for(unsigned i=0;i<6;++i)allowed=allowed||s.address==globalValues[i]||s.address==globalGuards[i];
   need(allowed,"original write outside bounded initialized output/storage");
  }
  check();++operations;
 }
 ~Fixture(){instructions+=o->steps;for(unsigned i=0;i<hits.size();++i){hits[i]+=o->hits[i];taken[i]+=o->taken[i];notTaken[i]+=o->notTaken[i];}}
};
static M arbitrary(std::mt19937&r){M m{};for(float&v:m.v)v=float(std::int32_t(r()%257)-128)/256.f;return m;}
static void cases(){
 std::mt19937 rng(490621);
 // Every unrelated flag bit, every cache-ready combination, both probe arms,
 // cold and noncanonical warm guard bytes, all nonzero W lanes in parents.
 for(unsigned trial=0;trial<256;++trial){label="flags/persistence "+std::to_string(trial);Fixture f(12);f.link(1,0);f.link(4,1);f.link(5,1);f.link(6,4);f.link(7,4);f.link(8,5);f.link(9,5);f.link(10,8);f.link(11,0);
  for(unsigned i=0;i<f.count;++i){f.nodes[i].localC0=arbitrary(rng);f.nodes[i].world100=arbitrary(rng);f.nodes[i].sceneFlags190=(trial+i*37)&255;}
  for(unsigned i=0;i<9;++i){f.shared.globals[i].guard=trial%3==0?0:trial%3==1?1:255;f.shared.globals[i].word=i>=2?(0x7f800000u+i*0x1111u):bits(i==0?.35f:.7f);}
  f.shared.orientationSentinel4433D8.guard=trial%2?128:0;f.shared.orientationSentinel4433D8.word=0x12340000+trial;
  f.state.upC20={{float(int(rng()%101)-50)/71,1.f,float(int(rng()%101)-50)/73,fp(0x7fc00000u+trial)}};
  f.seed();for(unsigned frame=0;frame<6;++frame){label="flags/persistence "+std::to_string(trial)+" frame "+std::to_string(frame);f.step(frame%3==0?0:frame%3==1?0x80000000u:7);}
 }
 // Mixed cold/warm guards: every subset of the six lazy slots. Only the
 // selected alpha is arithmetic; unselected alpha and warm store-only words
 // may be opaque. Ready main consumes its cache despite poisoned local words.
 for(unsigned trial=0;trial<64;++trial){label="mixedguards/opaque "+std::to_string(trial);Fixture f(4);f.link(1,0);f.nodes[0].sceneFlags190=1;f.nodes[0].world100=matrix();for(float&v:f.nodes[0].localC0.v)v=fp(0x7fc01234);for(float&v:f.nodes[1].world100.v)v=fp(0xff800000);f.nodes[1].sceneFlags190=trial;
  const unsigned selected=trial&1;for(unsigned i=0;i<6;++i){f.shared.globals[i].guard=(trial&(1u<<i))?std::uint8_t(128+i):0;f.shared.globals[i].word=i==selected?bits(.375f):(i==3?0xff800000u:0x7fc12340u+i);}
  f.seed();f.step(selected);f.step(selected);
 }
 // Signed zero enters arithmetic alongside a positive norm. Child's obsolete
 // basis and old C20.W are intentionally opaque; only old local XYZ survives.
 for(unsigned trial=0;trial<64;++trial){label="signedzero/W "+std::to_string(trial);Fixture f(4);f.link(1,0);M m=matrix();for(unsigned j=0;j<16;++j)if(j%5)m.v[j]=fp((trial&(1u<<(j%6)))?0x80000000u:0u);m.v[12]=3.5f;m.v[13]=-2.f;m.v[14]=1.25f;m.v[3]=.25f;m.v[7]=-.5f;m.v[11]=.75f;m.v[15]=1.125f;f.nodes[0].localC0=m;f.nodes[0].world100=m;f.nodes[0].sceneFlags190=trial;f.nodes[1].sceneFlags190=255-trial;
  for(unsigned j=0;j<16;++j)f.nodes[1].localC0.v[j]=fp(0x7fc12340+j);
  f.nodes[1].localC0.v[12]=fp(trial&1?0x80000000u:0);f.nodes[1].localC0.v[13]=-3.f;f.nodes[1].localC0.v[14]=.25f;
  f.state.upC20={{fp(trial&2?0x80000000u:0),1,fp(trial&4?0x80000000u:0),fp(0xff800000)}};f.seed();f.step(trial&1);f.step(!(trial&1));
 }
 // Source-proved Q saturation at zero norm, and parallel forward/up whose
 // exact crosses collapse the requested forward. No fallback axis is added.
 for(unsigned trial=0;trial<16;++trial){label="zero/parallel "+std::to_string(trial);Fixture f(6);f.link(1,0);f.link(4,1);f.link(5,1);f.nodes[0].localC0=matrix();f.nodes[0].world100=matrix();f.nodes[0].sceneFlags190=trial;
  if(trial&1){f.nodes[0].localC0.v[8]=0;f.nodes[0].localC0.v[9]=1;f.nodes[0].localC0.v[10]=0;f.nodes[0].world100=f.nodes[0].localC0;}
  f.shared.globals[0]={1,bits(.5f)};f.shared.globals[1]={128,bits(.5f)};f.state.upC20={{fp(trial&2?0x80000000u:0),(trial&4)?1.f:-1.f,fp(trial&8?0x80000000u:0),fp(0x7fc05555)}};f.seed();f.step(trial&1);f.step(!(trial&1));
 }
 // Non-parent-first indices and larger descendant forests exercise source
 // siblings-before-current invalidation and dirty nodes skipping descendants.
 for(unsigned shape=0;shape<3;++shape){label="forest "+std::to_string(shape);Fixture f(64);f.bindings={ix(63),ix(20),ix(0),ix(1),false,false,0};f.link(20,63);for(unsigned i=2;i<63;++i)if(i!=20)f.link(i,shape==0?20:shape==1?(i>21?i-1:20):((i%2&&i>3&&i!=21)?i-1:20));
  for(unsigned i=0;i<64;++i){f.nodes[i].sceneFlags190=(i%5==0)?0x64:0x65;f.nodes[i].localC0=arbitrary(rng);f.nodes[i].world100=arbitrary(rng);}f.nodes[20].sceneFlags190=shape==2?0x64:0x65;f.seed();f.step(shape);f.step(0);
 }
 // Two explicitly initialized vehicles consume one warm shared graphics state
 // in sequence. The second keeps effective non-default first-consumer values.
 {label="shared vehicles";Fixture a(4),b(4);a.link(1,0);b.link(1,0);a.shared.globals[0]={255,bits(.15f)};a.shared.globals[1]={128,bits(.95f)};a.seed();a.step(0);b.shared=a.shared;b.state.upC20={{.7f,.9f,.1f,1}};b.nodes[0].localC0.v[6]=.3f;b.nodes[0].world100=b.nodes[0].localC0;b.seed();b.step(1);b.step(0);}
}
static void admission(){
 label="bounded rejection";Fixture f(4);f.link(1,0);f.seed();VehicleVisualDiagnostic49R d{};
 auto rejected=[&](VehicleVisualError49R expected){const auto before=allocations;const bool ok=detail::prepareVisualContainer49R({f.nodes.data(),f.count},f.bindings,0,f.state,f.shared,f.frame,d);const auto after=allocations;need(!ok&&d.error==expected,"invalid arithmetic/graph/rounding accepted");need(before==after,"rejection allocated");};
 need(std::fesetround(FE_DOWNWARD)==0,"set alternate rounding");rejected(VehicleVisualError49R::rounding_mode);need(std::fesetround(FE_TONEAREST)==0,"restore rounding");
 f.bindings.container194=ix(0);rejected(VehicleVisualError49R::graph);f.bindings.container194=ix(1);
 f.shared.globals[0]={1,bits(1.f)};f.state.upC20={{std::ldexp(1.f,-70),0,0,1}};rejected(VehicleVisualError49R::arithmetic);
 f.state.upC20={{fp(0x7fc05555),1,0,1}};rejected(VehicleVisualError49R::arithmetic);
}
static void coverage(){const char*path=std::getenv("RR_CONTAINER49R_COVERAGE");if(!path)return;std::ofstream out(path);need(bool(out),"coverage file");out<<"{\"fixtures\":"<<fixtures<<",\"operations\":"<<operations<<",\"checks\":"<<checks<<",\"instructions\":"<<instructions<<",\"ordered_flag_store_tracing\":"<<(traceRequired?"true":"false")<<",\"flag_stores_compared\":"<<flagStores<<",\"hot_allocations\":0,\"pcs\":[";bool first=true;for(const auto&b:container49r_words::blocks)for(unsigned i=0;i<b.count;++i){if(!first)out<<',';first=false;const auto n=b.index+i;out<<"{\"pc\":"<<b.start+i*4<<",\"word\":"<<b.words[i]<<",\"hits\":"<<hits[n]<<",\"taken\":"<<taken[n]<<",\"not_taken\":"<<notTaken[n]<<'}';}out<<"]}\n";}
int main(int argc,char**argv){try{need(std::fesetround(FE_TONEAREST)==0,"rounding");if(argc==3){mutationPc=std::stoul(argv[1],nullptr,16);mutationWord=std::stoul(argv[2],nullptr,16);}cases();admission();coverage();std::cout<<"PASS Container49R: "<<fixtures<<" fixtures, "<<operations<<" persistent calls, "<<checks<<" exact checks, "<<instructions<<" original instructions, "<<flagStores<<" ordered flag stores, zero native hot allocations\n";return 0;}catch(const std::exception&e){std::cerr<<"FAIL Container49R: "<<e.what()<<'\n';return 1;}}
